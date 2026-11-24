/*
 * Unit tests for awesome-os/webui zero-HTTP Sectium named-pipe transport.
 *
 * SPDX-License-Identifier: MIT
 */

#include "b8g_webui_sectium.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define B8G_CHECK(cond)                                                     \
    do {                                                                    \
        if (!(cond)) {                                                      \
            fprintf(stderr, "FAIL: %s:%d: %s\n", __FILE__, __LINE__, #cond);\
            return 1;                                                       \
        }                                                                   \
    } while (0)

int main(void) {
    /* 1. ABI magic */
    B8G_CHECK(B8G_WEBUI_ABI_MAGIC_V3 == 0x33564742u);
    B8G_CHECK(b8g_webui_abi_magic() == 0x33564742u);

    /* 2. Zero-HTTP pipe path & default Sectium binary */
    const char *pipe_path = b8g_webui_sectium_pipe_path();
    const char *bin_path = b8g_webui_default_sectium_bin();
    B8G_CHECK(pipe_path != NULL);
    B8G_CHECK(bin_path != NULL);
    B8G_CHECK(strstr(pipe_path, "127.0.0.1") == NULL);
    B8G_CHECK(strstr(pipe_path, "http://") == NULL);
#if defined(_WIN32)
    B8G_CHECK(strcmp(pipe_path, "\\\\.\\pipe\\sectium-dev") == 0);
    B8G_CHECK(strcmp(bin_path, "D:\\Sectium\\sectium.exe") == 0);
#else
    B8G_CHECK(strcmp(pipe_path, "/tmp/sectium-dev.sock") == 0);
    B8G_CHECK(strcmp(bin_path, "/usr/local/bin/sectium") == 0);
#endif

    /* 3. Display frame encoding + JSON escaping */
    char buf[512];
    size_t n = b8g_webui_encode_display_frame("sectium://canvas/app?q=\"a\\b\"\n", buf, sizeof(buf));
    B8G_CHECK(n > 0u);
    B8G_CHECK(n == strlen(buf));
    B8G_CHECK(strcmp(
        buf,
        "{\"magic\":861292354,\"channel\":\"display\",\"url\":\"sectium://canvas/app?q=\\\"a\\\\b\\\"\\n\"}\n") == 0);

    /* 4. Command frame encoding (string payload & NULL payload) */
    n = b8g_webui_encode_command_frame("eval", "window.b8g(\"ok\")", buf, sizeof(buf));
    B8G_CHECK(n > 0u);
    B8G_CHECK(strcmp(
        buf,
        "{\"magic\":861292354,\"channel\":\"command\",\"op\":\"eval\",\"payload\":\"window.b8g(\\\"ok\\\")\"}\n") == 0);

    n = b8g_webui_encode_command_frame("ping", NULL, buf, sizeof(buf));
    B8G_CHECK(n > 0u);
    B8G_CHECK(strcmp(
        buf,
        "{\"magic\":861292354,\"channel\":\"command\",\"op\":\"ping\",\"payload\":null}\n") == 0);

    /* 5. Error & overflow handling */
    B8G_CHECK(b8g_webui_encode_display_frame(NULL, buf, sizeof(buf)) == 0u);
    B8G_CHECK(b8g_webui_encode_display_frame("sectium://ok", NULL, 128u) == 0u);
    B8G_CHECK(b8g_webui_encode_display_frame("sectium://ok", buf, 16u) == 0u);
    B8G_CHECK(buf[0] == '\0');

    B8G_CHECK(b8g_webui_encode_command_frame(NULL, "x", buf, sizeof(buf)) == 0u);
    B8G_CHECK(b8g_webui_encode_command_frame("op", "x", buf, 10u) == 0u);
    B8G_CHECK(buf[0] == '\0');

    printf("PASS: b8g_webui_sectium (magic=0x%08X, pipe=%s, bin=%s)\n",
           b8g_webui_abi_magic(), pipe_path, bin_path);
    return 0;
}
