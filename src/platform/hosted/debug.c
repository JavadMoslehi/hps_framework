#include "hps/platform/hosted/debug.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

/* Keep NULL fallback and exact-length writes identical across all diagnostics.
 * A failed write is deliberately ignored: panic must still terminate and
 * reporting has no result domain of its own. */
static void hps_hosted_write_text(const char *text, const size_t length)
{
    if (text == NULL) {
        (void)fputs("?", stderr);
    } else if (length != 0U) {
        (void)fwrite(text, 1U, length, stderr);
    } else {
        /* An empty non-NULL input contributes no bytes. */
    }
}

HPS_NORETURN void hps_panic(const char *message, const size_t length)
{
    (void)fputs("PANIC: ", stderr);
    hps_hosted_write_text(message, length);
    (void)fputs("\n", stderr);
    (void)fflush(stderr);
    abort();
}

HPS_NORETURN void hps_debug_assert_fail(const char *expression,
                                      const size_t expression_length,
                                      const char *file,
                                      const size_t file_length,
                                      const uint32_t line)
{
    (void)fputs("ASSERTION FAILED: ", stderr);
    hps_hosted_write_text(expression, expression_length);
    (void)fputs("\nFile: ", stderr);
    hps_hosted_write_text(file, file_length);
    (void)fprintf(stderr, "\nLine: %" PRIu32 "\n", line);
    (void)fflush(stderr);
    abort();
}

void hps_result_report(const int32_t result, const char *context,
                       const size_t context_length, const char *message,
                       const size_t message_length)
{
    if (result != INT32_C(0)) {
        (void)fputs("[ERROR] (", stderr);
        hps_hosted_write_text(context, context_length);
        (void)fputs("): ", stderr);
        hps_hosted_write_text(message, message_length);
        (void)fprintf(stderr, " [result=%" PRId32 "]\n", result);
        (void)fflush(stderr);
    }
}

void hps_panic_on_result(const int32_t result, const char *context,
                         const size_t context_length, const char *message,
                         const size_t message_length)
{
    if (result != INT32_C(0)) {
        hps_result_report(result, context, context_length, message,
                          message_length);
        abort();
    }
}
