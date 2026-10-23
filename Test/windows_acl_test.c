// SPDX-License-Identifier: MPL-2.0

// Exercise private assessment helpers without adding test hooks to the public ABI.
#include "framework/test_framework.h"
#include "windows_secure_file.c"
#include <winioctl.h>

#define TEST_CAPABILITY_SID                                                                                            \
    "S-1-15-3-65536-599108337-2355189375-1353122160-3480128286-3345335107-485756383-4087318168-230526575"
#define TEST_USER_SID              "S-1-5-21-100-200-300-1001"
#define TEST_TRUSTED_INSTALLER_SID "S-1-5-80-956008885-3418522649-1831038044-1853292631-2271478464"

static void check(bool condition, const char* message)
{
    TEST_ASSERT(condition, message);
}

static void require(bool condition, const char* message)
{
    check(condition, message);
    if (!condition)
    {
        fprintf(stderr, "%s (Windows error %lu)\n", message, GetLastError());
        exit(EXIT_FAILURE);
    }
}

static void expect_descriptor(const char* sddl, bool leaf, bool expected, const char* description)
{
    PSECURITY_DESCRIPTOR descriptor = NULL;
    PSID                 user       = NULL;
    PSID                 installer  = NULL;
    char*                error      = NULL;
    require(ConvertStringSecurityDescriptorToSecurityDescriptorA(sddl, SDDL_REVISION, &descriptor, NULL) != 0,
            "parse synthetic security descriptor");
    require(ConvertStringSidToSidA(TEST_USER_SID, &user) != 0 &&
                ConvertStringSidToSidA(TEST_TRUSTED_INSTALLER_SID, &installer) != 0,
            "create synthetic trusted identities");
    bool secure = is_Folder_Secure(descriptor, "C:\\synthetic", user, installer, leaf, &error);
    check(secure == expected, description);
    check(secure ? error == NULL : error != NULL, "assessment returns an explicit diagnostic on rejection");
    if (secure != expected && error != NULL)
    {
        fprintf(stderr, "%s\n", error);
    }
    free(error);
    LocalFree(descriptor);
    LocalFree(user);
    LocalFree(installer);
}

static void test_acl_semantics(void)
{
    expect_descriptor("O:SYD:P(A;;0x20;;;" TEST_CAPABILITY_SID ")", true, true,
                      "customer capability SID with traverse-only folder access is harmless");
    expect_descriptor("O:SYD:P(A;;FR;;;" TEST_CAPABILITY_SID ")", true, true,
                      "composite FILE_GENERIC_READ is not a write grant");
    expect_descriptor("O:SYD:P(A;;GRGX;;;" TEST_CAPABILITY_SID ")", true, true,
                      "generic read/execute rights are mapped without treating synchronization as mutation");
    expect_descriptor("O:SYD:P(A;;GW;;;" TEST_CAPABILITY_SID ")", true, false,
                      "generic write from the same capability SID is not trusted");
    expect_descriptor("O:SYD:P(A;;FA;;;WD)", true, false, "Everyone full control is rejected");
    expect_descriptor("O:SYD:P(A;;WD;;;BU)", true, false, "Users cannot change output DACL");
    expect_descriptor("O:SYD:P(A;;WO;;;AU)", true, false, "Authenticated Users cannot change output ownership");
    expect_descriptor("O:SYD:P(A;;0x40;;;WD)", false, false, "parent delete-child is rejected despite protected child");
    expect_descriptor("O:SYD:P(A;;SD;;;WD)", false, false, "untrusted deletion of a selected ancestor is rejected");
    expect_descriptor("O:SYD:P(A;;0x100;;;WD)", false, false, "untrusted write-attributes is rejected");
    expect_descriptor("O:SYD:P(A;;0x10;;;WD)", false, false, "untrusted write-EA is rejected");
    expect_descriptor("O:SYD:P(A;;0x6;;;BU)", false, true,
                      "ancestor may allow sibling file/directory creation without child replacement");
    expect_descriptor("O:SYD:P(A;;0x6;;;BU)", true, false,
                      "the same creation grant is rejected on the leaf output directory");
    expect_descriptor("O:SYD:P(A;OICIIO;FA;;;WD)(A;;FA;;;SY)", true, true,
                      "inherit-only grant is not effective on the current directory");
    expect_descriptor("O:SYD:P(A;OICI;FA;;;WD)", true, false,
                      "inheritable but effective grant still applies to current directory");
    expect_descriptor("O:SYD:P(A;ID;FA;;;WD)", true, false, "inherited effective grant still applies");
    expect_descriptor("O:SYD:P(D;;FA;;;WD)(A;;FA;;;WD)", true, false,
                      "deny/allow is conservatively rejected rather than claiming effective-access calculation");
    expect_descriptor("O:SYD:P(D;;FA;;;WD)", true, true, "deny alone grants no mutation authority");
    expect_descriptor("O:SYD:P", true, true, "empty DACL grants no untrusted mutation");
    expect_descriptor("O:SY", true, false, "absent DACL is rejected");
    expect_descriptor("O:SYD:NO_ACCESS_CONTROL", true, false, "null DACL is rejected");
    expect_descriptor("O:" TEST_USER_SID "D:P(A;;FA;;;" TEST_USER_SID ")", true, true,
                      "explicitly trusted current user may own and modify the directory");
    expect_descriptor("O:" TEST_TRUSTED_INSTALLER_SID "D:P(A;;FA;;;" TEST_TRUSTED_INSTALLER_SID ")", true, true,
                      "exact TrustedInstaller SID is accepted without path-specific exceptions");
    expect_descriptor("O:BAD:P(A;;FA;;;BA)(A;;FA;;;SY)", true, true, "administrators and SYSTEM remain trusted");
    expect_descriptor("O:BUD:P(A;;FA;;;SY)", true, false, "broad Users group is not a trusted owner");
    expect_descriptor("O:S-1-5-21-100-200-300-1002D:P(A;;FA;;;SY)", true, false, "unrelated user owner is rejected");

    PSECURITY_DESCRIPTOR descriptor = NULL;
    require(ConvertStringSecurityDescriptorToSecurityDescriptorA("O:SYD:P(A;;FR;;;WD)", SDDL_REVISION, &descriptor,
                                                                 NULL) != 0,
            "create ACE-type fixture");
    BOOL        present   = FALSE;
    BOOL        defaulted = FALSE;
    PACL        dacl      = NULL;
    PACE_HEADER header    = NULL;
    require(GetSecurityDescriptorDacl(descriptor, &present, &dacl, &defaulted) != 0 &&
                GetAce(dacl, 0, (void**)&header) != 0,
            "obtain test ACE");
    header->AceType = ACCESS_ALLOWED_CALLBACK_ACE_TYPE;
    PSID user       = NULL;
    PSID installer  = NULL;
    require(ConvertStringSidToSidA(TEST_USER_SID, &user) != 0 &&
                ConvertStringSidToSidA(TEST_TRUSTED_INSTALLER_SID, &installer) != 0,
            "create ACE-type test identities");
    char* error  = NULL;
    bool  secure = is_Folder_Secure(descriptor, "C:\\synthetic", user, installer, true, &error);
    check(!secure && error != NULL, "unknown effective grant-capable ACE semantics do not silently pass");
    free(error);
    LocalFree(user);
    LocalFree(installer);
    LocalFree(descriptor);
}

static void test_paths_and_handles(void)
{
    static const char* const rejected[] = {
        "",
        "C",
        "C:",
        "C:relative",
        "\\\\server\\share",
        "\\\\?\\C:\\path",
        "C:\\path:stream",
        "C:\\..\\path",
        "C:\\path.\\child",
        "C:\\path \\child",
        "C:\\path\\\\child",
        "C:/path",
    };
    for (size_t i = 0; i < sizeof(rejected) / sizeof(rejected[0]); ++i)
    {
        char* error  = NULL;
        bool  secure = os_Is_Directory_Secure(rejected[i], &error);
        check(!secure && error != NULL, "unsupported or ambiguous path is rejected explicitly");
        free(error);
    }

    char* userString = get_Current_User_SID();
    PSID  user       = NULL;
    PSID  installer  = NULL;
    require(userString != NULL && ConvertStringSidToSidA(userString, &user) != 0 &&
                ConvertStringSidToSidA(TEST_TRUSTED_INSTALLER_SID, &installer) != 0,
            "query effective Windows user");
    char security[2048];
    require(
        snprintf(security, sizeof(security), "O:%sD:P(A;;FA;;;%s)(A;;FA;;;SY)(A;;FA;;;BA)", userString, userString) > 0,
        "build private test directory DACL");
    PSECURITY_DESCRIPTOR descriptor = NULL;
    require(ConvertStringSecurityDescriptorToSecurityDescriptorA(security, SDDL_REVISION, &descriptor, NULL) != 0,
            "create protected test-directory descriptor");
    SECURITY_ATTRIBUTES attrs = {sizeof(attrs), descriptor, FALSE};
    char                temp[MAX_PATH];
    require(GetTempPathA(sizeof(temp), temp) > 0, "find temporary test location");
    char directory[MAX_PATH];
    require(GetTempFileNameA(temp, "acl", 0, directory) != 0 && DeleteFileA(directory) != 0,
            "reserve unique test directory name");
    require(CreateDirectoryA(directory, &attrs) != 0, "create protected disposable directory");
    char* error  = NULL;
    bool  secure = win_Inspect_Directory(directory, user, installer, true, &error);
    check(secure && error == NULL, "inspect a real private directory through its opened handle");
    free(error);
    error         = NULL;
    size_t length = strlen(directory);
    require(length + 2 < sizeof(directory), "reserve trailing separator");
    directory[length]     = '\\';
    directory[length + 1] = '\0';
    secure                = os_Is_Directory_Secure(directory, &error);
    check(secure && error == NULL, "normal local user path including its leaf passes with a trailing separator");
    if (!secure)
    {
        printf("Host ancestor assessment: %s\n", error);
    }
    free(error);
    error             = NULL;
    directory[length] = '\0';
    char child[MAX_PATH];
    require(snprintf(child, sizeof(child), "%s\\file", directory) > 0, "construct file fixture");
    HANDLE file = CreateFileA(child, GENERIC_WRITE, 0, &attrs, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
    require(file != INVALID_HANDLE_VALUE && CloseHandle(file) != 0, "create regular-file fixture");
    secure = win_Inspect_Directory(child, user, installer, true, &error);
    check(!secure && error != NULL, "regular file cannot pass as a directory");
    free(error);
    error = NULL;
    require(DeleteFileA(child) != 0, "remove regular-file fixture");
    secure = win_Inspect_Directory(child, user, installer, true, &error);
    check(!secure && error != NULL, "missing directory fails without null dereference");
    free(error);
    error = NULL;

    // A mount-point junction needs no symlink-creation privilege and must not be decoded as a symlink.
    require(snprintf(child, sizeof(child), "%s\\junction", directory) > 0, "construct junction fixture");
    require(CreateDirectoryA(child, &attrs) != 0, "create disposable junction directory");
    struct
    {
        DWORD tag;
        WORD  length;
        WORD  reserved;
        WORD  substituteOffset;
        WORD  substituteLength;
        WORD  printOffset;
        WORD  printLength;
        WCHAR names[MAX_PATH * 2 + 8];
    } junction = {0};
    WCHAR target[MAX_PATH];
    int   converted = MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, directory, -1, target, MAX_PATH);
    require(converted > 0, "convert junction target");
    size_t targetlen       = (size_t)converted - 1;
    size_t substituteChars = targetlen + 4;
    require(substituteChars + 1 + targetlen + 1 < sizeof(junction.names) / sizeof(junction.names[0]),
            "bound junction fixture");
    memcpy(junction.names, L"\\??\\", 4 * sizeof(WCHAR));
    memcpy(junction.names + 4, target, (targetlen + 1) * sizeof(WCHAR));
    memcpy(junction.names + substituteChars + 1, target, (targetlen + 1) * sizeof(WCHAR));
    junction.tag              = IO_REPARSE_TAG_MOUNT_POINT;
    junction.substituteLength = (WORD)(substituteChars * sizeof(WCHAR));
    junction.printOffset      = (WORD)((substituteChars + 1) * sizeof(WCHAR));
    junction.printLength      = (WORD)(targetlen * sizeof(WCHAR));
    junction.length           = (WORD)(8 + (substituteChars + 1 + targetlen + 1) * sizeof(WCHAR));
    HANDLE junctionHandle     = CreateFileA(child, GENERIC_WRITE, 0, NULL, OPEN_EXISTING,
                                            FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OPEN_REPARSE_POINT, NULL);
    require(junctionHandle != INVALID_HANDLE_VALUE, "open disposable junction");
    DWORD returned = 0;
    require(DeviceIoControl(junctionHandle, FSCTL_SET_REPARSE_POINT, &junction, junction.length + 8, NULL, 0, &returned,
                            NULL) != 0,
            "set junction tag on disposable test directory");
    require(CloseHandle(junctionHandle) != 0, "close junction fixture");
    secure = win_Inspect_Directory(child, user, installer, true, &error);
    check(!secure && error != NULL && strstr(error, "Reparse") != NULL,
          "junction returns explicit unsupported result without following or misdecoding its target");
    free(error);
    error = NULL;
    require(RemoveDirectoryA(child) != 0, "remove junction only, never its target");

    PSECURITY_DESCRIPTOR unsafeDescriptor = NULL;
    require(ConvertStringSecurityDescriptorToSecurityDescriptorA("D:P(A;;FA;;;WD)", SDDL_REVISION, &unsafeDescriptor,
                                                                 NULL) != 0,
            "construct unsafe disposable leaf DACL");
    BOOL present    = FALSE;
    BOOL defaulted  = FALSE;
    PACL unsafeDacl = NULL;
    require(GetSecurityDescriptorDacl(unsafeDescriptor, &present, &unsafeDacl, &defaulted) != 0 &&
                SetNamedSecurityInfoA(directory, SE_FILE_OBJECT,
                                      DACL_SECURITY_INFORMATION | PROTECTED_DACL_SECURITY_INFORMATION, NULL, NULL,
                                      unsafeDacl, NULL) == ERROR_SUCCESS,
            "make only disposable leaf directory unsafe");
    directory[length]     = '\\';
    directory[length + 1] = '\0';
    secure                = os_Is_Directory_Secure(directory, &error);
    check(!secure && error != NULL && strstr(error, "mutation") != NULL,
          "trailing separator cannot skip unsafe leaf evaluation");
    free(error);
    error             = NULL;
    directory[length] = '\0';
    LocalFree(unsafeDescriptor);

    HANDLE token         = NULL;
    HANDLE impersonation = NULL;
    require(OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY | TOKEN_DUPLICATE, &token) != 0 &&
                DuplicateToken(token, SecurityImpersonation, &impersonation) != 0 &&
                SetThreadToken(NULL, impersonation) != 0,
            "set an impersonating thread token");
    char* effectiveUser = get_Current_User_SID();
    require(RevertToSelf() != 0, "restore thread token");
    check(effectiveUser != NULL && strcmp(userString, effectiveUser) == 0,
          "identity helper supports thread impersonation");
    free(effectiveUser);
    CloseHandle(impersonation);
    CloseHandle(token);
    require(RemoveDirectoryA(directory) != 0, "remove disposable test directory");
    LocalFree(descriptor);
    LocalFree(user);
    LocalFree(installer);
    free(userString);
}

int main(void)
{
    test_acl_semantics();
    test_paths_and_handles();
    print_test_summary();
    return failed_tests == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
