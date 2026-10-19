# Platform metadata boundary

`hps/platform/detection.h` includes `os.h`, `libc.h` and `posix.h`. These are
optional preprocessing-only headers, independent of Base's language/compiler/CPU
information. None includes a libc, SDK or operating-system header, calls an OS
service, creates runtime storage or changes feature-test macros. No runtime
platform adapter or supported deployment target is implied.

`os.h` reports overlapping `HPS_PLATFORM_IS_*` flags for Windows, Linux, Apple,
iOS, macOS, Android, Unix and BSD, plus UNKNOWN. Android may also be Linux.
`HPS_PLATFORM_IS_POSIX_FAMILY` is only a family hint; it is not API availability.
An unrecognized target is not automatically embedded or freestanding. Use Base's
explicit hosted-environment evidence for that separate question.

Apple subtype identification uses already-visible SDK `TARGET_OS_*` values.
An application needing that detail includes its selected `TargetConditionals.h`
before the first HPS platform inclusion. Without it, Apple can be known while
iOS/macOS remain unidentified. Other Apple device families are not mislabeled
macOS, and known TV/watch/vision/bridge targets are excluded from the older
`TARGET_OS_IPHONE` fallback.

`libc.h` records visible implementation macros as `HPS_LIBC_DETECTED`, then
exposes the selected `HPS_LIBC` ID and `HPS_LIBC_IS_*` predicates. Families are
unknown, glibc, musl, uClibc, Newlib, Bionic, MSVCRT, Apple and UCRT. Detection
tests specific implementations before compatibility markers. A Linux target
without markers stays unknown; compiler identity does not identify its runtime.
Musl deliberately has no official identification macro. See the
[musl FAQ](https://wiki.musl-libc.org/faq.html).

For automatic libc detection, include a header from the consumer's chosen
library before the first HPS libc inclusion. Alternatively define
`HPS_CONFIG_LIBC` to one known `HPS_LIBC_*` family before inclusion, including
`HPS_LIBC_MUSL` for an explicitly selected musl toolchain. Invalid selections,
UNKNOWN overrides and disagreement with observed evidence produce an error.
The override is metadata only; it changes no declarations, linkage or ABI and
does not make an unavailable library/API exist. Keep the choice consistent across
an integration. Detection never refreshes after its guarded first inclusion.

`posix.h` reports `HPS_POSIX_AVAILABLE` only when `_POSIX_VERSION` is already
visible, and copies that value to `HPS_POSIX_VERSION`. `_XOPEN_VERSION` is
independently exposed as `HPS_XOPEN_VERSION`; absent evidence yields zero.
Feature requests `_POSIX_C_SOURCE`/`_XOPEN_SOURCE` are not implementation evidence.
An application can include its selected platform's `unistd.h` before HPS when
it needs those advertised levels. Even a known level does not establish support
for an arbitrary optional POSIX interface. The
[POSIX unistd contract](https://pubs.opengroup.org/onlinepubs/9799919799/basedefs/unistd.h.html)
defines the implementation version macros.

These flags are trust-boundary metadata supplied by the selected compiler,
headers and explicit configuration. They neither qualify ABI/resources nor
authorize a prohibited core dependency. Applications still select and qualify
their actual bare-metal/RTOS/hosted profile. See the
[migration and behavior corrections](../migration/detection-environment.md).
