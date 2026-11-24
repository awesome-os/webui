/*
 * awesome-os/webui — Zero-HTTP Sectium Named-Pipe Transport (sectium_pipe)
 *
 * Pure C11, zero external dependencies, zero 127.0.0.1 HTTP listeners.
 * Routes display & command NDJSON frames over the Sectium Host Device Boundary:
 *   - Windows: \\.\pipe\sectium-dev
 *   - POSIX:   /tmp/sectium-dev.sock
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef B8G_WEBUI_SECTIUM_H
#define B8G_WEBUI_SECTIUM_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Canonical B8G ABI v3 magic ("BGV3" little-endian). */
#define B8G_WEBUI_ABI_MAGIC_V3 0x33564742u

#if defined(_WIN32)
#  if defined(B8G_WEBUI_BUILD_DLL)
#    define B8G_WEBUI_API __declspec(dllexport)
#  elif defined(B8G_WEBUI_USE_DLL)
#    define B8G_WEBUI_API __declspec(dllimport)
#  else
#    define B8G_WEBUI_API
#  endif
#elif defined(__GNUC__) || defined(__clang__)
#  define B8G_WEBUI_API __attribute__((visibility("default")))
#else
#  define B8G_WEBUI_API
#endif

/**
 * Returns B8G_WEBUI_ABI_MAGIC_V3 (0x33564742u).
 */
B8G_WEBUI_API uint32_t b8g_webui_abi_magic(void);

/**
 * Returns the canonical zero-HTTP Sectium Host Device Boundary IPC path:
 *   - Windows: "\\\\.\\pipe\\sectium-dev"
 *   - POSIX:   "/tmp/sectium-dev.sock"
 */
B8G_WEBUI_API const char *b8g_webui_sectium_pipe_path(void);

/**
 * Returns the default Sectium display appliance binary path:
 *   - Windows: "D:\\Sectium\\sectium.exe"
 *   - POSIX:   "/usr/local/bin/sectium"
 */
B8G_WEBUI_API const char *b8g_webui_default_sectium_bin(void);

/**
 * Encodes an NDJSON frame for the pure-art "display" channel:
 *   {"magic":861292354,"channel":"display","url":"<escaped>"}\n
 *
 * Returns the number of bytes written (excluding the trailing NUL byte),
 * or 0 on invalid arguments or insufficient buffer capacity.
 */
B8G_WEBUI_API size_t b8g_webui_encode_display_frame(
    const char *url,
    char *out,
    size_t cap);

/**
 * Encodes an NDJSON frame for the "command" (ttyS0 / DevTools) channel:
 *   {"magic":861292354,"channel":"command","op":"<escaped>","payload":"<escaped>"}\n
 * If payload is NULL, encodes "payload":null.
 *
 * Returns the number of bytes written (excluding the trailing NUL byte),
 * or 0 on invalid arguments or insufficient buffer capacity.
 */
B8G_WEBUI_API size_t b8g_webui_encode_command_frame(
    const char *op,
    const char *payload,
    char *out,
    size_t cap);

#ifdef __cplusplus
}
#endif

#endif /* B8G_WEBUI_SECTIUM_H */
