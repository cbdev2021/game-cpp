---
name: cadash-build-pipeline
description: Usar al compilar/ejecutar/desplegar el juego: make, g++, mingw32-make, em++, emcc, emsdk, wasm, index.html, index.js, index.wasm, --embed-file, --preload-file, python http.server, serve_web, node, localhost, vercel, heroku, deploy, previsualizar, PNG no carga, MIME, "compilalo", "hacelo andar", "no veo cambios en el navegador", web no funciona, cache. Es la unica fuente de verdad para comandos de build/run/deploy.
---

# Cadash Build Pipeline

Comandos EXACTOS para compilar y servir (desktop + web). No improvises: el
PATH/toolchain es frágil en esta máquina y un error de MIME o de PATH rompe
días de trabajo.

## Desktop (game.exe) — ventana nativa

**IMPORTANTE**: el `g++` de PowerShell no funciona directamente (no encuentra
`cc1plus`). Hay que pasar SIEMPRE el PATH de UCRT64 dentro de un solo `cmd /c`
y comprobar `$env:ERRORLEVEL`. Usá este bloque exacto:

```powershell
cmd /c "set PATH=C:\msys64\ucrt64\bin;%PATH% & g++ -std=c++17 -Wall -Wextra -I. -IC:/msys64/ucrt64/include -o game.exe main.cpp -LC:/msys64/ucrt64/lib -lraylib > compile_out.txt 2>&1 & echo BUILD=%ERRORLEVEL%"
Get-Content compile_out.txt
```
Ejecutar también el preview/export de sprites ANTES de compilar (ver abajo) si
cambiaste arte; los PNG se leen de `assets/sprites/`.

OJO con `-I.`: desde `src/` el `main.cpp` incluye `"raylib.h"` (include de
ucrt64) y `"sprites.h"` (local). Incluir SIEMPRE `-I.` cuando compiles algo
que referencie `sprites.h` (p. ej. `tools/gen_sprites` usa `#include "sprites.h"`).

## Sprites → PNG + preview

```powershell
mingw32-make sprites   # en src/: compila tools/gen_sprites + exporta assets/sprites/*.png
# Iteración rápida de arte (ASCII preview por frames):
tools/gen_sprites_preview.exe --preview
```
`gen_sprites_preview.exe` también exporta los PNG (modo export) — ver su
`--help`. Nunca editar los PNG a mano (son artefactos; ver skill
`cadash-pixel-art`).

## Web (wasm) — build + servir

### Build web (genera `web/index.{html,js,wasm}`)

Usar EMSDK + raylib web. Ruta del emsdk: `C:\Users\usuario\emsdk\emsdk_env.bat`.
Ruta del raylib web/include: `C:\Users\usuario\raylib\src` (headers fuente de
raylib 5.5.0 — NO usar el header de UCRT64, da error `__declspec` con wasm).

```powershell
cmd /c "call C:\Users\usuario\emsdk\emsdk_env.bat >nul 2>&1 & em++ -std=c++17 -Wall -Wextra -DPLATFORM_WEB -IC:/Users/usuario/raylib/src main.cpp -o ../web/index.html -LC:/Users/usuario/Documents/proyectos/opencode/game-cpp/lib/web -lraylib -sUSE_GLFW=3 -sASYNCIFY -sEXPORTED_RUNTIME_METHODS=ccall,cwrap -sALLOW_MEMORY_GROWTH=1 -sFORCE_FILESYSTEM=1 --embed-file assets --shell-file ../web/shell.html > ../web_build.txt 2>&1 & echo WEBBUILD=%ERRORLEVEL%"
```

### Servir web local con MIME correcto

- **`python -m http.server` NO sirve `.wasm`** (manda `application/octet-stream`
  → Chrome lo rechaza → "no se ve"). 
- Usar un servidor node simple con MIME types correctos. Guardado en
  `src/tools/serve_web.js` (recibe [carpeta] [puerto]; por defecto sirve
  `web/` en `8080`):

```powershell
node src/tools/serve_web.js
```
y abrir `http://localhost:8080` — **siempre Ctrl+F5** (el header el `Cache-Control`
es `no-store` para que no cachee; si igual ves lo viejo, forzar recarga dura).

## Deploy (Vercel)

- El hosting es **Vercel** (`vercel.json`: `outputDirectory: "web"`). NO hay
  buildCommand en vercel.json: sirve estáticamente lo que hay en `web/` del
  commit.
- Por tanto: tras editar arte/código hay que **commitear + pushear** para que
  Vercel redeploye (~1 min). El commit lo hace SOLO el usuario (regla en
  `AGENTS.md`). Yo solo dejo los cambios en el working tree.
- Si el cambio no se ve tras el push: probar el `web/` local (server node) para
  descartar cache del navegador vs. build stale.

## Checklist pre-build

1. ¿Cambiaste `src/sprites.h` o el arte? → correr `mingw32-make sprites`
   (regenera PNG) ANTES del build desktop/web.
2. ¿Cambiaste solo `main.cpp`? → compilar directo (`game.exe` desktop y `em++`
   web en paralelo si se toca la lógica).
3. Verificar `BUILD=0` / `WEBBUILD=0` (leer el txt, no fiarse del &&).
4. Warnings: `-Wall -Wextra`; si algo warnnea, arreglarlo, no silenciarlo.
