// SPDX-License-Identifier: MPL-2.0

// Include the implementation to exercise private policy helpers without production test hooks.
#include "../src/posix_secure_file.c"
#include "framework/test_framework.h"
#include <endian.h>
#include <grp.h>
#include <linux/posix_acl.h>
#include <linux/posix_acl_xattr.h>
#include <sys/wait.h>
#include <sys/xattr.h>

static void check(bool condition, const char* message)
{
    TEST_ASSERT(condition, message);
}

static void require(bool condition, const char* message)
{
    check(condition, message);
    if (!condition)
    {
        perror(message);
        exit(EXIT_FAILURE);
    }
}

static void test_parser(void)
{
    static const char* const rejected[] = {
        "[directory \"/\"]\n",
        "[directory \"//\"]\n",
        "[directory \"/.\"]\n",
        "[directory \"/x/..\"]\n",
        "[directory \"relative\"]\n",
        "[directory \"/shared/\"]\n",
        "[directory \"/shared//child\"]\n",
        "[directory \"/shared\"]\nunknown=true\n",
        "[directory \"/shared\"]\nallow_group_write=1\n",
        "[directory \"/shared\"]\nallow_group_write=true\n",
        "[directory \"/shared\"]\nallow_other_write=true\nrequire_sticky_bit=false\n",
        "[directory \"/shared\"]\nrequire_sticky_bit=true\nrequire_sticky_bit=true\n",
        "[directory \"/shared\"]\n[directory \"/shared\"]\n",
        "[policy]\ndisable_directory_checks=true\n",
        "allow_group_write=true\n",
        "[directory \"/shared\"] garbage\n",
    };
    pathOutputPolicy* policy = calloc(1, sizeof(*policy));
    require(policy != NULL, "allocate parser test policy");
    for (size_t i = 0; i < sizeof(rejected) / sizeof(rejected[0]); ++i)
    {
        char* text = strdup(rejected[i]);
        require(text != NULL, "allocate parser input");
        char* error = NULL;
        bool  valid = path_policy_parse(text, strlen(text), policy, &error);
        check(!valid && error != NULL, rejected[i]);
        free(error);
        free(text);
    }
    char  good[] = "# policy\n[directory \"/shared\"]\r\nallow_group_write = true\n"
                   "allow_other_write=false\nrequire_sticky_bit=true\n";
    char* error  = NULL;
    bool  valid  = path_policy_parse(good, sizeof(good) - 1, policy, &error);
    check(valid && error == NULL && policy->count == 1, "valid policy parses");
    check(policy->rules[0].allowGroupWrite && !policy->rules[0].allowOtherWrite && policy->rules[0].requireStickyBit,
          "settings are independent");
    char nul[] = "[directory \"/shared\"]\n\0require_sticky_bit=true\n";
    valid      = path_policy_parse(nul, sizeof(nul) - 1, policy, &error);
    check(!valid && error != NULL, "embedded NUL is rejected");
    free(error);
    error           = NULL;
    char* oversized = calloc(PATH_POLICY_MAX_BYTES + 2, 1);
    require(oversized != NULL, "allocate oversized policy");
    memset(oversized, '#', PATH_POLICY_MAX_BYTES + 1);
    valid = path_policy_parse(oversized, PATH_POLICY_MAX_BYTES + 1, policy, &error);
    check(!valid && error != NULL, "oversized policy is rejected before parsing");
    free(error);
    free(oversized);
    error                   = NULL;
    char   many_rules[4096] = {0};
    size_t used_bytes       = 0;
    for (size_t i = 0; i <= PATH_POLICY_MAX_RULES; ++i)
    {
        int count =
            snprintf(many_rules + used_bytes, sizeof(many_rules) - used_bytes, "[directory \"/directory%zu\"]\n", i);
        require(count > 0 && (size_t)count < sizeof(many_rules) - used_bytes, "build rule-count test");
        used_bytes += (size_t)count;
    }
    valid = path_policy_parse(many_rules, used_bytes, policy, &error);
    check(!valid && error != NULL, "excessive rule count is rejected");
    free(error);
    error = NULL;

    memset(policy, 0, sizeof(*policy));
    policy->count = 1;
    strcpy(policy->rules[0].path, "/shared");
    policy->rules[0].allowGroupWrite  = true;
    policy->rules[0].requireStickyBit = true;
    struct stat attrs                 = {0};
    attrs.st_mode                     = S_IFDIR | 01770;
    attrs.st_uid                      = 0;
    bool used                         = false;
    valid                             = path_output_component_secure("/shared", &attrs, false, policy, &used, &error);
    check(valid && used, "root-owned sticky group-writable ancestor is permitted");
    used  = false;
    valid = path_output_component_secure("/shared2", &attrs, false, policy, &used, &error);
    check(!valid && !used, "textual prefixes do not match");
    free(error);
    error = NULL;
    valid = path_output_component_secure("/shared/child", &attrs, false, policy, &used, &error);
    check(!valid, "rules do not inherit into children");
    free(error);
    error = NULL;
    valid = path_output_component_secure("/shared", &attrs, true, policy, &used, &error);
    check(!valid, "shared leaf output directory is never exempted");
    free(error);
    error         = NULL;
    attrs.st_mode = S_IFDIR | 01777;
    valid         = path_output_component_secure("/shared", &attrs, false, policy, &used, &error);
    check(!valid, "group permission does not grant other write");
    free(error);
    error         = NULL;
    attrs.st_mode = S_IFDIR | 0770;
    valid         = path_output_component_secure("/shared", &attrs, false, policy, &used, &error);
    check(!valid, "sticky bit is mandatory for writable exceptions");
    free(error);
    free(policy);

    attrs.st_mode = S_IFDIR | 0755;
    attrs.st_uid  = 0;
    attrs.st_gid  = 0;
    check(path_root_attributes_secure(&attrs), "normal root baseline is accepted");
    attrs.st_uid = 1000;
    check(!path_root_attributes_secure(&attrs), "non-root UID on root is rejected");
    attrs.st_uid = 0;
    attrs.st_gid = 1000;
    check(!path_root_attributes_secure(&attrs), "non-root GID on root is rejected");
    attrs.st_gid = 0;
    attrs.st_mode |= S_IWGRP;
    check(!path_root_attributes_secure(&attrs), "group-writable root is rejected");
    attrs.st_mode = S_IFDIR | 0757;
    check(!path_root_attributes_secure(&attrs), "other-writable root is rejected");
}

static void set_test_acl(const char* path, const char* name)
{
    struct
    {
        struct posix_acl_xattr_header header;
        struct posix_acl_xattr_entry  entries[5];
    } acl                        = {0};
    static const uint16_t tags[] = {ACL_USER_OBJ, ACL_USER, ACL_GROUP_OBJ, ACL_MASK, ACL_OTHER};
    acl.header.a_version         = htole32(POSIX_ACL_XATTR_VERSION);
    for (size_t i = 0; i < 5; ++i)
    {
        acl.entries[i].e_tag  = htole16(tags[i]);
        acl.entries[i].e_perm = htole16(7);
        acl.entries[i].e_id   = htole32(i == 1 ? 12346U : UINT32_MAX);
    }
    require(setxattr(path, name, &acl, sizeof(acl), 0) == 0, "set isolated POSIX ACL");
}

static void write_policy(const char* text)
{
    FILE* file = fopen("/etc/opensea/path-security.conf", "w");
    require(file != NULL, "create isolated policy");
    require(fputs(text, file) >= 0, "write isolated policy");
    require(fclose(file) == 0, "close isolated policy");
    require(chmod("/etc/opensea/path-security.conf", 0644) == 0, "set isolated policy mode");
}

static void expect_output(const char* path, bool success, eSecureFileError expected)
{
    secureFileInfo* info = os_Create_Exclusive_Secure_Log(path);
    require(info != NULL, "output returns file information");
    check(info->isValid == success && info->error == expected, path);
    if (success)
    {
        struct stat attrs;
        check(info->file != NULL && fstat(info->fileno, &attrs) == 0 && (attrs.st_mode & (S_IRWXG | S_IRWXO)) == 0,
              "new output denies all group and other access");
        check((fcntl(info->fileno, F_GETFD) & FD_CLOEXEC) != 0, "output descriptor is close-on-exec");
        require(fputs("diagnostic data", info->file) >= 0, "write diagnostic output");
        require(fclose(info->file) == 0, "close diagnostic output");
        info->file = NULL;
    }
    else
    {
        check(info->file == NULL && info->errorString != NULL, "failure is explicit and has no open stream");
    }
    free_Secure_File_Info(&info);
}

static void test_isolated_filesystem(void)
{
    require(mkdir("/etc", 0755) == 0, "create isolated etc");
    require(mkdir("/etc/opensea", 0755) == 0, "create isolated policy directory");
    require(mkdir("/safe", 0700) == 0, "create private output directory");
    require(mkdir("/shared", 0700) == 0 && chmod("/shared", 01777) == 0, "create shared sticky ancestor");
    require(mkdir("/shared/private", 0700) == 0, "create private child");
    expect_output("/safe/default.log", true, SEC_FILE_SUCCESS);
    expect_output("/shared/private/default.log", false, SEC_FILE_INSECURE_PATH);

    write_policy("[directory \"/shared\"]\nallow_group_write=true\n"
                 "allow_other_write=true\nrequire_sticky_bit=true\n");
    char random_directory[] = "/shared/tmp.XXXXXX";
    require(mkdtemp(random_directory) != NULL, "create randomly named customer output directory");
    char random_output[sizeof(random_directory) + 16];
    require(snprintf(random_output, sizeof(random_output), "%s/output.log", random_directory) > 0,
            "construct random output path");
    expect_output(random_output, true, SEC_FILE_SUCCESS);
    require(unlink(random_output) == 0 && rmdir(random_directory) == 0, "remove random temporary output");
    mode_t savedmask = umask(0);
    expect_output("/shared/private/output.log", true, SEC_FILE_SUCCESS);
    umask(savedmask);
    expect_output("/shared/private/output.log", false, SEC_FILE_FILE_ALREADY_EXISTS);
    expect_output("/shared/direct.log", false, SEC_FILE_INSECURE_PATH);
    secureFileInfo* generated = NULL;
    eReturnValues   generated_result =
        create_And_Open_Secure_Log_File("disk", 4, &generated, NAMING_BYUSER, "/shared/private", 15, NULL, 0, "log", 3);
    require(generated_result == SUCCESS && generated != NULL && generated->isValid,
            "log helper uses the descriptor-bound policy path");
    require(fclose(generated->file) == 0, "close generated log");
    generated->file = NULL;
    free_Secure_File_Info(&generated);

    set_test_acl("/safe", "system.posix_acl_default");
    expect_output("/safe/acl.log", true, SEC_FILE_SUCCESS);
    require(removexattr("/safe", "system.posix_acl_default") == 0, "remove default ACL");

    secureFileInfo* ordinary = secure_Open_File("/shared/private/output.log", "rb", NULL, NULL, NULL);
    require(ordinary != NULL, "strict open returns file information");
    check(ordinary->error == SEC_FILE_INSECURE_PATH && ordinary->file == NULL,
          "output policy never relaxes ordinary input");
    free_Secure_File_Info(&ordinary);
    ordinary = secure_Open_File("/shared/private/append.log", "ab", NULL, NULL, NULL);
    require(ordinary != NULL, "strict append returns file information");
    check(ordinary->error == SEC_FILE_INSECURE_PATH && ordinary->file == NULL, "output policy never relaxes append");
    free_Secure_File_Info(&ordinary);
    ordinary = secure_Open_File("/shared/private/output.log", "wb", NULL, NULL, NULL);
    require(ordinary != NULL, "strict overwrite returns file information");
    check(ordinary->error == SEC_FILE_INSECURE_PATH && ordinary->file == NULL, "output policy never relaxes overwrite");
    free_Secure_File_Info(&ordinary);
    eSecureFileError deletion = secure_Delete_File_By_Name("/shared/private/output.log", SEC_DELETE_NAME_FAIL_IF_OPEN);
    check(deletion == SEC_FILE_INSECURE_PATH, "output policy never relaxes deletion");

    require(symlink("/safe/default.log", "/shared/private/link.log") == 0, "create final symlink");
    expect_output("/shared/private/link.log", false, SEC_FILE_FILE_ALREADY_EXISTS);
    require(symlink("/safe/absent.log", "/shared/private/dangling.log") == 0, "create dangling final symlink");
    expect_output("/shared/private/dangling.log", false, SEC_FILE_FILE_ALREADY_EXISTS);
    require(link("/safe/default.log", "/shared/private/hard.log") == 0, "create hardlink");
    expect_output("/shared/private/hard.log", false, SEC_FILE_FILE_ALREADY_EXISTS);
    require(mkfifo("/shared/private/fifo.log", 0600) == 0, "create FIFO");
    expect_output("/shared/private/fifo.log", false, SEC_FILE_FILE_ALREADY_EXISTS);
    struct stat untouched;
    require(stat("/safe/default.log", &untouched) == 0, "inspect protected target");
    check(untouched.st_size == 15, "existing link target contents remain unchanged");

    require(chmod("/shared", 0777) == 0, "remove ancestor sticky bit");
    expect_output("/shared/private/nonsticky.log", false, SEC_FILE_INSECURE_PATH);
    require(chmod("/shared", 01777) == 0, "restore sticky bit");
    require(chmod("/shared/private", 0777) == 0, "make child unsafe");
    expect_output("/shared/private/child.log", false, SEC_FILE_INSECURE_PATH);
    require(chmod("/shared/private", 0700) == 0, "restore child permissions");

    require(chmod("/etc/opensea/path-security.conf", 0666) == 0, "make policy unsafe");
    expect_output("/safe/insecure-policy.log", false, SEC_FILE_INSECURE_PATH);
    require(chmod("/etc/opensea/path-security.conf", 0644) == 0, "restore policy");
    require(chown("/etc/opensea/path-security.conf", 12345, 12345) == 0, "make policy user-owned");
    expect_output("/safe/user-policy.log", false, SEC_FILE_INSECURE_PATH);
    require(chown("/etc/opensea/path-security.conf", 0, 0) == 0, "restore policy ownership");
    require(chmod("/etc/opensea", 0777) == 0, "make policy parent unsafe");
    expect_output("/safe/insecure-parent.log", false, SEC_FILE_INSECURE_PATH);
    require(chmod("/etc/opensea", 0755) == 0, "restore policy parent");
    require(rename("/etc/opensea/path-security.conf", "/etc/opensea/original.conf") == 0, "move policy");
    require(symlink("original.conf", "/etc/opensea/path-security.conf") == 0, "redirect policy");
    expect_output("/safe/symlink-policy.log", false, SEC_FILE_INSECURE_PATH);
    require(unlink("/etc/opensea/path-security.conf") == 0, "remove policy symlink");
    require(rename("/etc/opensea/original.conf", "/etc/opensea/path-security.conf") == 0, "restore policy file");
    write_policy("[policy]\ndisable_directory_checks=true\n");
    expect_output("/safe/global.log", false, SEC_FILE_INSECURE_PATH);
    write_policy("[directory \"/safe/default.log\"]\nrequire_sticky_bit=true\n");
    expect_output("/safe/file-rule.log", false, SEC_FILE_INSECURE_PATH);
    write_policy("[directory \"/\"]\nallow_group_write=true\nrequire_sticky_bit=true\n");
    expect_output("/safe/root-rule.log", false, SEC_FILE_INSECURE_PATH);
    require(symlink("/", "/root-alias") == 0, "create root alias");
    write_policy("[directory \"/root-alias\"]\nallow_group_write=true\nrequire_sticky_bit=true\n");
    expect_output("/safe/root-alias.log", false, SEC_FILE_INSECURE_PATH);
    require(unlink("/root-alias") == 0, "remove root alias");
    require(symlink("/shared", "/shared-alias") == 0, "create directory alias");
    write_policy("[directory \"/shared-alias\"]\nallow_group_write=true\n"
                 "allow_other_write=true\nrequire_sticky_bit=true\n");
    expect_output("/shared/private/alias.log", false, SEC_FILE_INSECURE_PATH);
    require(unlink("/shared-alias") == 0, "remove directory alias");
    write_policy("[directory \"/shared\"]\nallow_group_write=true\n"
                 "allow_other_write=true\nrequire_sticky_bit=true\n");

    pathOutputPolicy* policy = calloc(1, sizeof(*policy));
    require(policy != NULL, "allocate descriptor binding test policy");
    int rootfd = open("/", O_RDONLY | O_DIRECTORY | O_CLOEXEC);
    require(rootfd >= 0, "open isolated root");
    char* error = NULL;
    require(path_policy_load(rootfd, policy, &error), "load descriptor binding policy");
    require(chown("/shared/private", 12345, 12345) == 0, "give sudo caller ownership of private child");
    require(setenv("SUDO_UID", "12345", 1) == 0, "model existing sudo-user ownership allowance");
    bool used     = false;
    int  parentfd = path_output_parent(rootfd, "/shared/private", policy, &used, &error);
    require(parentfd >= 0, "bind output directory before substitution");
    pid_t attacker = fork();
    require(attacker >= 0, "fork unrelated local user");
    if (attacker == 0)
    {
        if (setgroups(0, NULL) != 0 || setgid(12346) != 0 || setuid(12346) != 0)
        {
            _exit(EXIT_FAILURE);
        }
        int replaced = rename("/shared/private", "/shared/moved");
        _exit(replaced < 0 && errno == EPERM ? EXIT_SUCCESS : EXIT_FAILURE);
    }
    int attackstatus = 0;
    require(waitpid(attacker, &attackstatus, 0) == attacker, "wait for unrelated local user");
    check(WIFEXITED(attackstatus) && WEXITSTATUS(attackstatus) == 0,
          "sticky ancestor prevents unrelated user replacing private child");
    attacker = fork();
    require(attacker >= 0, "fork directory-owning sudo caller");
    if (attacker == 0)
    {
        if (setgroups(0, NULL) != 0 || setgid(12345) != 0 || setuid(12345) != 0 ||
            rename("/shared/private", "/shared/moved") != 0 || symlink("/safe", "/shared/private") != 0)
        {
            _exit(EXIT_FAILURE);
        }
        _exit(EXIT_SUCCESS);
    }
    require(waitpid(attacker, &attackstatus, 0) == attacker, "wait for directory owner");
    check(WIFEXITED(attackstatus) && WEXITSTATUS(attackstatus) == 0,
          "directory owner can replace child despite sticky ancestor");
    int fd = openat(parentfd, "bound.log", O_WRONLY | O_CREAT | O_EXCL | O_NOFOLLOW | O_CLOEXEC, 0600);
    require(fd >= 0 && close(fd) == 0, "create through retained descriptor");
    check(access("/shared/moved/bound.log", F_OK) == 0 && access("/safe/bound.log", F_OK) != 0,
          "replacement cannot redirect descriptor-bound creation");
    close(parentfd);
    close(rootfd);
    free(error);
    free(policy);
    require(unlink("/shared/private") == 0 && rename("/shared/moved", "/shared/private") == 0,
            "restore replaced directory");

    require(chmod("/", 0775) == 0, "make isolated root unsafe");
    expect_output("/safe/unsafe-root.log", false, SEC_FILE_INSECURE_PATH);
    require(chmod("/", 0755) == 0, "restore isolated root");
    set_test_acl("/", "system.posix_acl_access");
    expect_output("/safe/root-acl.log", false, SEC_FILE_INSECURE_PATH);
    require(removexattr("/", "system.posix_acl_access") == 0 && chmod("/", 0755) == 0,
            "restore isolated root ACL and mode");
    require(chown("/", 0, 1000) == 0, "change isolated root group");
    expect_output("/safe/root-group.log", false, SEC_FILE_INSECURE_PATH);
    require(chown("/", 0, 0) == 0, "restore isolated root group");
    require(chown("/", 12345, 0) == 0, "make root owned by accepted sudo caller");
    expect_output("/safe/root-owner.log", false, SEC_FILE_INSECURE_PATH);
    require(chown("/", 0, 0) == 0, "restore isolated root owner");

    // Reuse the same policy without root execution.
    require(chown("/safe", 12345, 12345) == 0, "give ordinary user a private output directory");
    pid_t ordinary_user = fork();
    require(ordinary_user >= 0, "fork unprivileged policy consumer");
    if (ordinary_user == 0)
    {
        require(setgroups(0, NULL) == 0 && setgid(12345) == 0 && setuid(12345) == 0, "drop privileges");
        int before = failed_tests;
        expect_output("/safe/unprivileged.log", true, SEC_FILE_SUCCESS);
        fflush(NULL);
        _exit(failed_tests == before ? EXIT_SUCCESS : EXIT_FAILURE);
    }
    int status = 0;
    require(waitpid(ordinary_user, &status, 0) == ordinary_user, "wait for unprivileged consumer");
    check(WIFEXITED(status) && WEXITSTATUS(status) == 0, "system policy can be consumed without sudo");

    static const char* const files[] = {
        "/safe/default.log",
        "/safe/unprivileged.log",
        "/shared/private/output.log",
        "/shared/private/link.log",
        "/shared/private/dangling.log",
        "/shared/private/hard.log",
        "/shared/private/fifo.log",
        "/shared/private/bound.log",
        "/etc/opensea/path-security.conf",
        "/shared/private/disk.log",
        "/safe/acl.log",
    };
    for (size_t i = 0; i < sizeof(files) / sizeof(files[0]); ++i)
    {
        require(unlink(files[i]) == 0, "remove isolated test file");
    }
    require(rmdir("/shared/private") == 0 && rmdir("/shared") == 0 && rmdir("/safe") == 0 &&
                rmdir("/etc/opensea") == 0 && rmdir("/etc") == 0,
            "remove isolated test directories");
}

int main(int argc, char** argv)
{
    if (argc == 2 && strcmp(argv[1], "--integration") == 0)
    {
        if (geteuid() != 0)
        {
            puts("SKIP: isolated integration tests require root for a disposable chroot");
            return 77;
        }
        char sandbox[] = "/tmp/opensea-path-policy-XXXXXX";
        require(mkdtemp(sandbox) != NULL, "create disposable sandbox");
        require(chmod(sandbox, 0755) == 0, "set sandbox root mode");
        fflush(NULL);
        pid_t child = fork();
        require(child >= 0, "fork isolated filesystem test");
        if (child == 0)
        {
            require(chroot(sandbox) == 0 && chdir("/") == 0, "enter disposable filesystem");
            test_isolated_filesystem();
            print_test_summary();
            fflush(NULL);
            _exit(failed_tests == 0 ? EXIT_SUCCESS : EXIT_FAILURE);
        }
        int status = 0;
        require(waitpid(child, &status, 0) == child, "wait for isolated tests");
        check(WIFEXITED(status) && WEXITSTATUS(status) == 0, "isolated integration tests pass");
        require(rmdir(sandbox) == 0, "remove disposable sandbox");
    }
    else
    {
        test_parser();
    }
    print_test_summary();
    return failed_tests == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
