---
post_title: 'Diagnostic output path security'
author1: 'OpenSea maintainers'
post_slug: 'diagnostic-output-path-security'
microsoft_alias: ''
featured_image: ''
categories: []
tags: ['security', 'linux', 'configuration']
ai_note: 'Created with AI assistance'
summary: 'Administrator-controlled exceptions for exclusive diagnostic output.'
post_date: '2026-10-22'
---

## Scope

Linux diagnostic output created through `create_And_Open_Secure_Log_File`
supports manually configured directory exceptions. No policy file is created
by the library or installed automatically. Ordinary file opens, including
exclusive opens through `secure_Open_File`, do not use these exceptions.
Inputs, firmware, configuration data, append, overwrite, and deletion remain
subject to their existing strict path checks.

The implementation creates only new files. It traverses canonical directory
components using retained descriptors and `openat`, checks the opened objects
using `fstat`, and creates the final basename atomically with
`O_CREAT | O_EXCL | O_NOFOLLOW | O_CLOEXEC`. The stream uses that descriptor;
there is no second pathname open.

Files are created with requested permissions `0600`; umask may remove further
permissions. Group and other access are checked before returning the stream,
including the effective Linux POSIX ACL mask. Existing files, hardlinks,
symlinks, dangling symlinks, FIFOs, and directories are never opened as output.

## System policy

The only configuration location is `/etc/opensea/path-security.conf`.
There is no per-user configuration or environment-selected location.

An administrator must deliberately create the directory and policy. The
policy and every containing directory must be root-owned and must not permit
group or other write. For example, a policy directory mode of `0755` and a
regular policy file mode of `0644` allow ordinary users to read the policy
without sudo. This does not grant them permission to modify it.

Symlinks in policy storage are rejected. The policy loader walks from a
verified root descriptor independently of the configurable output checks.
A missing policy or missing policy directory uses the defaults, after
checking existing ancestors. Wrong types, unsafe permissions, unreadable
files, malformed content, and other errors fail the output operation.

Replace policy atomically when editing it. Do not modify an active policy
in place. The loader bounds reads and checks for observable concurrent file
changes, but administrators remain trusted.

## Example configuration

This example accepts a root-owned sticky shared ancestor. It does not
change its permissions or create any directories.

```ini
[directory "/usr/tmp"]
allow_group_write = true
allow_other_write = true
require_sticky_bit = true
```

All options default to false. A rule granting either write exception must
explicitly set `require_sticky_bit=true`. Writable exceptions require both
root ownership and the sticky bit on the matching directory.

The customer's private child directory, for example
`/usr/tmp/tmp.OEG2GTXllJ`, is still checked normally. The final output
directory cannot be group- or other-writable, even if it has a matching rule.
The existing effective-user/root/sudo-user ownership allowance is retained
for non-root output components.

Rules match one complete canonical absolute directory component. They do
not match prefixes or implicitly apply to children. Paths must already
exist. Specify the physical canonical target if a site uses a symlink alias;
aliases, repeated separators, dot components, trailing separators, and `/`
rules are rejected. No globbing, variable expansion, includes, or relative
paths are supported.

The parser accepts printable ASCII, tabs, LF or CRLF line endings, blank
lines, and whole-line `#` or `;` comments. Booleans are exactly `true` or
`false`. Unknown sections/settings, duplicate directories/settings,
embedded NULs, and invalid values are errors. Limits are 32 rules, 65536
file bytes, directory names shorter than `OPENSEA_PATH_MAX`, and lines
shorter than `OPENSEA_PATH_MAX + 64` bytes.

There is no global bypass. A successful creation that actually uses an
exception emits a warning to standard error identifying the policy and
output path.

## Immutable root checks

On Linux, both strict directory validation and policy-enabled output
require the opened `/` directory to have UID 0 and GID 0 ownership and no
group/other write permission. Linux POSIX ACL write grants are bounded by
the group mask reflected in those mode bits. This check is not subject to
the sudo-user allowance or any policy exception.

This is a deliberately strict deployment baseline, not a universal POSIX
requirement. Do not automatically change a customer's system permissions
to satisfy it.

## Security boundaries

- Disk-device authorization is separate from output-path policy. Reading
  system policy never requires elevating a tool that otherwise runs without
  sudo.
- An exception accepts a specific shared-ancestor risk; it does not mark an
  entire subtree trustworthy.
- Sticky directories restrict other users' entry deletion/renaming. They do
  not prevent a directory owner from changing their own entries.
- Descriptor-relative creation prevents a replacement pathname from
  redirecting the final open after the parent descriptor is retained. It
  does not guarantee that the file stays reachable by its original name.
- The accepted sudo caller may own the private output directory. This
  allowance is not a security boundary against that owner. Restricted
  sudoers deployments must consider whether privileged output creation is
  authorized for the specified destinations.
- `allow_group_write` also accepts effective named-user/group POSIX ACL
  writers covered by the group ACL mask. It does not mean only the owning
  group can write. Other-write permission requires its separate option.
- The implementation assumes trusted Linux filesystem permission, ACL, and
  exclusive-creation semantics and a trusted mount namespace. It does not
  defend against an administrator, hostile filesystem server, or mount
  authority. Other ACL models or incompatible filesystems are not covered
  by this policy feature.
- A creation failure after atomic creation can leave an empty file. It is
  not removed automatically through an attacker-mutable pathname. Diagnose
  the failure before retrying with the same name.
- Warnings and displayed canonical paths are diagnostic names, not a
  guarantee that the namespace remains unchanged after opening.

## Compatibility and migration

`DISABLE_SECURE_FILE_PATH_CHECK` no longer disables directory checks.
Meson's existing `directorysecurity` option is retained, but configuring
it as disabled is an explicit error. Migrate eligible Linux diagnostic
output workflows to the administrator-created policy.

Policy support is Linux-only. Windows directory assessment is repaired as
described below, but its existing automatic opening-path bypass remains.
This feature does not implement Windows configuration-policy exceptions or
immutable Windows volume-root checks.

## Windows directory assessment

`os_Is_Directory_Secure` performs a conservative Windows ACL assessment.
It is not an effective-access calculator for every possible Windows token.
Automatic enforcement during ordinary Windows file opening remains disabled:
assessing directories alone does not secure subsequent pathname traversal,
creation, or a newly created file's inherited DACL.

### Rights and identities

- Trusted owners and writers are the effective thread user (or process user
  when no impersonation token exists), SYSTEM, built-in Administrators, and
  the exact TrustedInstaller service SID. Other service or capability SIDs
  are not automatically trusted.
- Unknown identities may have read or traverse access. The example capability
  SID `S-1-15-3-65536-599108337-2355189375-1353122160-3480128286-3345335107-485756383-4087318168-230526575`
  is accepted with traverse-only access, not with mutation rights.
- Generic rights are mapped to file rights before testing. Composite
  `FILE_GENERIC_WRITE` is not used as a bitwise mutation mask: it shares
  `READ_CONTROL` and `SYNCHRONIZE` with read-only grants.
- Ancestors may allow creating siblings, provided untrusted identities cannot
  delete the selected directory or its children, change security/ownership,
  or modify attributes/extended attributes. In particular, parent
  `FILE_DELETE_CHILD` is assessed independently of a child's protected DACL.
- The final output directory additionally rejects untrusted
  `FILE_ADD_FILE` and `FILE_ADD_SUBDIRECTORY` grants. Broad `Users`,
  `Authenticated Users`, and `Everyone` grants receive no location-based
  trust exception.
- Inherit-only ACEs do not grant access to the directory itself. Their
  implications for future children must be handled separately at creation.
- Deny ACE reductions are conservatively ignored. Consequently, an allow
  grant can cause rejection even when an earlier deny would block it.
  Unsupported effective ACE forms and unknown access-mask bits are rejected
  with diagnostics rather than silently accepted.
- Missing/null DACLs are rejected. An empty DACL grants no untrusted access;
  accepting its security assessment does not guarantee the caller can open
  or create anything there.

### Filesystem and race boundaries

Each component is opened with `CreateFileW` and
`FILE_FLAG_OPEN_REPARSE_POINT`; attributes and security are read from that
same handle. The narrow public path API retains Windows ACP encoding.
Only absolute local drive-letter paths on filesystems reporting persistent
ACL support are assessed. UNC/device paths, ambiguous components, and all
reparse-point directories (including junctions) are explicitly unsupported.
The validator does not follow or reinterpret unknown reparse buffers.

Component handles are closed after assessment. This is a snapshot, not a
retained-handle traversal or a race-free guarantee about a later open.
Before enabling automatic enforcement, Windows output creation needs a
separately validated binding between assessment and use, atomic `CREATE_NEW`,
and a restrictive creation DACL independent of unsafe inheritance.

Focused x64 and x86 tests cover synthetic ACLs, the supplied capability SID,
real private directories, a disposable junction, trailing-separator leaf
checks, missing/non-directory paths, and thread impersonation. These tests
do not reproduce every enterprise security product or Windows container.

Relevant Microsoft documentation:

- [File access rights](https://learn.microsoft.com/en-us/windows/win32/fileio/file-access-rights-constants)
- [Generic-rights mapping](https://learn.microsoft.com/en-us/windows/win32/api/securitybaseapi/nf-securitybaseapi-mapgenericmask)
- [DACL access control](https://learn.microsoft.com/en-us/windows/win32/secauthz/how-dacls-control-access-to-an-object)
- [ACE inheritance](https://learn.microsoft.com/en-us/windows/win32/secauthz/ace-inheritance-rules)
- [Reparse points](https://learn.microsoft.com/en-us/windows/win32/fileio/reparse-points)

## CERT C review

These are relevant alignment points, not a certification or a claim that
the whole secure-file API satisfies every CERT rule.

| Guidance | Implementation |
| --- | --- |
| FIO01-C / FIO02-C | Canonical component selection, followed by descriptor-bound validation/use; canonicalization alone is not treated as race prevention. |
| FIO03-C / FIO21-C | Atomic exclusive creation instead of existence-check emulation or temporary-name generation. |
| FIO05-C | Attributes and identity obtained from the opened file descriptor. |
| FIO06-C | Restrictive creation permissions and post-create permission checks. |
| FIO15-C | Strict private output leaf; explicit root-owned sticky shared-ancestor exceptions. |
| FIO30-C | Constant format strings; paths are data arguments, not formats. |
| FIO32-C | Policy must be regular; newly created output must be regular; existing special entries are rejected atomically. |
| FIO42-C | Internal descriptors are closed on all exits. Callers own the returned stream and must check write/flush/close results. |
| FIO45-C | Final creation is relative to the retained, inspected parent descriptor; existing entries are never opened. |

FIO15-C normally recommends secure directories. Shared-ancestor exceptions
are a deliberate, documented relaxation with additional safeguards, not
unqualified adherence to that recommendation. Existing general file-open
and deletion operations still have their original pathname-based behavior;
the new output mechanism does not establish their race freedom.

See the official [FIO recommendations][recommendations] and
[FIO rules][rules]. Recommendations and rules have different status within
CERT C; review their platform assumptions and exceptions individually.

[recommendations]: https://cmu-sei.github.io/secure-coding-standards/sei-cert-c-coding-standard/recommendations/input-output-fio/
[rules]: https://cmu-sei.github.io/secure-coding-standards/sei-cert-c-coding-standard/rules/input-output-fio/

## Validation

Configure the existing Meson `tests` option as enabled, build
`path-security-tests`, and run `path-security-unit-tests` and
`path-security-isolated-tests`.

The isolated tests require root solely to enter a disposable chroot. They
create policy, permissions, ACLs, links, and files only within that
filesystem, not the host configuration. Without root they return Meson's
skip code. They also verify consuming the same policy after dropping
privileges. Run both tests before shipping; a skipped integration test
does not validate the privileged workflows.
