# awesome-os/webui (`sectium_pipe`)

Hard-fork of MIT-licensed [`webui-dev/webui`](https://github.com/webui-dev/webui) adding a zero-HTTP, zero-dependency C11 **`sectium_pipe`** transport (`B8G_ABI_MAGIC_V3 = 0x33564742`) over the Sectium Host Device Boundary:

- **Windows:** `\\.\pipe\sectium-dev` (default binary: `D:\Sectium\sectium.exe`)
- **POSIX (Linux / macOS):** `/tmp/sectium-dev.sock` (default binary: `/usr/local/bin/sectium`)
- **Zero `127.0.0.1` HTTP listeners** — routes pure-art `display` and `command` (`ttyS0` / DevTools) NDJSON frames directly over local OS pipes.

## Quick 1-Line Install & Use

### Windows (PowerShell)

```powershell
irm https://github.com/awesome-os/webui/releases/latest/download/libwebui-sectium-windows-x64.dll -OutFile libwebui-sectium.dll; irm https://github.com/awesome-os/webui/releases/latest/download/b8g_webui_sectium.h -OutFile b8g_webui_sectium.h
```

### Linux (x64 / arm64) & macOS (arm64)

```sh
# Linux x64 (replace linux-x64.so with linux-arm64.so or darwin-arm64.dylib as needed)
curl -fsSLO https://github.com/awesome-os/webui/releases/latest/download/libwebui-sectium-linux-x64.so && curl -fsSLO https://github.com/awesome-os/webui/releases/latest/download/b8g_webui_sectium.h
```

### C11 Usage

```c
#include "b8g_webui_sectium.h"
#include <stdio.h>

int main(void) {
    char frame[256];
    b8g_webui_encode_display_frame("sectium://canvas/app", frame, sizeof(frame));
    printf("magic=0x%08X pipe=%s frame=%s",
           b8g_webui_abi_magic(),
           b8g_webui_sectium_pipe_path(),
           frame);
    return 0;
}
```

## Build & Test Locally (`zig cc`, zero WSL)

```sh
zig cc -std=c11 -Os -s -Wall -Wextra -Werror -Iinclude \
  src/b8g_webui_sectium.c src/b8g_webui_sectium_test.c -o b8g_webui_sectium_test && ./b8g_webui_sectium_test
```

## Exported C11 API (`include/b8g_webui_sectium.h`)

| Symbol | Description |
| :--- | :--- |
| `B8G_WEBUI_ABI_MAGIC_V3` | `0x33564742u` (`"BGV3"`) |
| `b8g_webui_abi_magic()` | Returns `B8G_WEBUI_ABI_MAGIC_V3` |
| `b8g_webui_sectium_pipe_path()` | `"\\\\.\\pipe\\sectium-dev"` (Win32) / `"/tmp/sectium-dev.sock"` (POSIX) |
| `b8g_webui_default_sectium_bin()` | `"D:\\Sectium\\sectium.exe"` (Win32) / `"/usr/local/bin/sectium"` (POSIX) |
| `b8g_webui_encode_display_frame(url, out, cap)` | Encodes pure-art `channel: "display"` NDJSON frame |
| `b8g_webui_encode_command_frame(op, payload, out, cap)` | Encodes `channel: "command"` NDJSON frame |

## License

MIT License — see [`LICENSE`](LICENSE). Upstream WebUI copyright (c) Hassan Draga; `sectium_pipe` extensions copyright (c) awesome-os.

## Quick Install & Run (Windows / Linux / macOS)

```sh
# Install / hydrate from v11 CAS via holo-pm
holo-pm install repository://github.com/awesome-os/webui@main

# Or project & run directly via holo-project (0-byte hardlink)
holo-project "project://cpp/webui?from=repository://github.com/awesome-os/webui@main"
```
