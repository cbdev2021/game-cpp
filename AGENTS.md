# AGENTS.md

## Git
- Prohibido hacer stage, commit, push o cualquier operación de git.
- Solo el usuario realiza los commits.

## Entorno y build
- Cambios en arte → `mingw32-make sprites`; PNG en `src/assets/sprites/` son
  artefactos (generados), no editar a mano.
- Desktop: `cmd /c "set PATH=C:\msys64\ucrt64\bin;%PATH% & g++ ..."` (ver skill).
- Web: `em++` con emsdk + raylib web (`C:/Users/usuario/raylib/src`).
- Probar web local con `node src/tools/serve_web.js` (NUNCA `python -m
  http.server`, no sirve `.wasm`).
- Detalles exactos de comandos en la skill `cadash-build-pipeline`.
  Gameplay/física en `cadash-gameplay`; arte/pixel-art en `cadash-pixel-art`.