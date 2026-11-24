/*
 * awesome-os/webui — Zero-HTTP Sectium Named-Pipe Transport (sectium_pipe)
 *
 * Strictly -std=c11 -Os -s -Wall -Wextra -Werror, zero external dependencies.
 * SPDX-License-Identifier: MIT
 */

#include "b8g_webui_sectium.h"

#include <stddef.h>
#include <stdint.h>

typedef struct {
    char *buf;
    size_t cap;
    size_t len;
    int overflow;
} b8g_buf_t;

static void b8g_buf_putc(b8g_buf_t *b, char c) {
    if (b->overflow || b->len + 1u >= b->cap) {
        b->overflow = 1;
        return;
    }
    b->buf[b->len++] = c;
}

static void b8g_buf_puts(b8g_buf_t *b, const char *s) {
    while (*s != '\0') {
        b8g_buf_putc(b, *s++);
    }
}

static void b8g_buf_put_json_str(b8g_buf_t *b, const char *s) {
    static const char hex[] = "0123456789abcdef";
    b8g_buf_putc(b, '"');
    while (*s != '\0') {
        unsigned char c = (unsigned char)*s++;
        switch (c) {
            case '"':  b8g_buf_puts(b, "\\\""); break;
            case '\\': b8g_buf_puts(b, "\\\\"); break;
            case '\b': b8g_buf_puts(b, "\\b");  break;
            case '\f': b8g_buf_puts(b, "\\f");  break;
            case '\n': b8g_buf_puts(b, "\\n");  break;
            case '\r': b8g_buf_puts(b, "\\r");  break;
            case '\t': b8g_buf_puts(b, "\\t");  break;
            default:
                if (c < 0x20u) {
                    b8g_buf_puts(b, "\\u00");
                    b8g_buf_putc(b, hex[(c >> 4) & 0x0Fu]);
                    b8g_buf_putc(b, hex[c & 0x0Fu]);
                } else {
                    b8g_buf_putc(b, (char)c);
                }
                break;
        }
    }
    b8g_buf_putc(b, '"');
}

B8G_WEBUI_API uint32_t b8g_webui_abi_magic(void) {
    return B8G_WEBUI_ABI_MAGIC_V3;
}

B8G_WEBUI_API const char *b8g_webui_sectium_pipe_path(void) {
#if defined(_WIN32)
    return "\\\\.\\pipe\\sectium-dev";
#else
    return "/tmp/sectium-dev.sock";
#endif
}

B8G_WEBUI_API const char *b8g_webui_default_sectium_bin(void) {
#if defined(_WIN32)
    return "D:\\Sectium\\sectium.exe";
#else
    return "/usr/local/bin/sectium";
#endif
}

B8G_WEBUI_API size_t b8g_webui_encode_display_frame(
    const char *url,
    char *out,
    size_t cap) {
    if (out == NULL || cap == 0u) {
        return 0u;
    }
    out[0] = '\0';
    if (url == NULL) {
        return 0u;
    }

    b8g_buf_t b = { out, cap, 0u, 0 };
    b8g_buf_puts(&b, "{\"magic\":861292354,\"channel\":\"display\",\"url\":");
    b8g_buf_put_json_str(&b, url);
    b8g_buf_puts(&b, "}\n");

    if (b.overflow) {
        out[0] = '\0';
        return 0u;
    }
    out[b.len] = '\0';
    return b.len;
}

B8G_WEBUI_API size_t b8g_webui_encode_command_frame(
    const char *op,
    const char *payload,
    char *out,
    size_t cap) {
    if (out == NULL || cap == 0u) {
        return 0u;
    }
    out[0] = '\0';
    if (op == NULL) {
        return 0u;
    }

    b8g_buf_t b = { out, cap, 0u, 0 };
    b8g_buf_puts(&b, "{\"magic\":861292354,\"channel\":\"command\",\"op\":");
    b8g_buf_put_json_str(&b, op);
    b8g_buf_puts(&b, ",\"payload\":");
    if (payload == NULL) {
        b8g_buf_puts(&b, "null");
    } else {
        b8g_buf_put_json_str(&b, payload);
    }
    b8g_buf_puts(&b, "}\n");

    if (b.overflow) {
        out[0] = '\0';
        return 0u;
    }
    out[b.len] = '\0';
    return b.len;
}
