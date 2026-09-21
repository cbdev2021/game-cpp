# Cadash Remake

Remake 2D de bajo nivel en C++ y Raylib, preparado para escritorio y WebAssembly.

## Resolucion

El juego usa un lienzo logico fijo de `480x270`. La ventana de escritorio y el canvas web se escalan conservando la relacion 16:9 y el pixel art.

## Dependencias

- C++17
- Raylib
- GNU Make o `mingw32-make`
- Emscripten SDK (`em++`) para la version web

En Windows, `g++`, `make` y `pkg-config` deben estar disponibles en `PATH`. Una instalacion habitual es MSYS2 con los paquetes de MinGW correspondientes. Raylib debe estar compilada para el mismo compilador.

Desde una terminal UCRT64 de MSYS2:

```sh
pacman -Syu
pacman -S --needed mingw-w64-ucrt-x86_64-gcc \
	mingw-w64-ucrt-x86_64-make \
	mingw-w64-ucrt-x86_64-pkgconf \
	mingw-w64-ucrt-x86_64-raylib
```

Abre una nueva terminal UCRT64 despues de actualizar. Si el paquete de Raylib no existe en tu mirror, compilala desde sus fuentes con el mismo compilador y usa `RAYLIB_DIR`.

## Compilacion de escritorio

Desde `src/`:

```sh
make desktop
```

En MSYS2 UCRT64, usa `mingw32-make` si `make` no esta disponible:

```sh
mingw32-make desktop
```

Si Raylib no ofrece `pkg-config`, indica su carpeta manualmente:

```sh
make desktop RAYLIB_DIR=C:/ruta/a/raylib
```

La carpeta debe contener `include/raylib.h` y la biblioteca dentro de `lib/`.

## Compilacion web

Primero instala el SDK oficial de Emscripten, activa su entorno y compila Raylib para Emscripten. Deja `libraylib.a` en `lib/web/` y sus cabeceras en `lib/web/include/`, o indica ambas ubicaciones:

```sh
make web RAYLIB_WEB_DIR=C:/ruta/a/raylib-web RAYLIB_WEB_INCLUDE=C:/ruta/a/raylib/src
```

En MSYS2 UCRT64, ejecuta primero `emsdk_env.bat` o configura `EMSDK`, `EMSDK_PYTHON`, `EMSDK_NODE` y `PATH` para que `em++` este disponible.

La salida se genera en `web/`:

- `index.html`
- `index.js`
- `index.wasm`

El HTML puede servirse estaticamente. `vercel.json` configura `web/` como directorio de salida para Vercel.

## Limpieza

```sh
make clean
```

## Controles actuales

La primera sala jugable incluye un Guerrero procedural, paredes, obstaculos, una puerta de salida y HUD.

## Controles actuales

El prototipo incluye un tramo lateral original con plataformas, camara, Guerrero procedural, enemigos y una salida.

- `A/D` o flechas: mover.
- `Espacio`, `W` o flecha arriba: saltar.
- `J` o `X`: atacar.
- `E`: abrir la salida al llegar al final.
- `R`: reiniciar despues de ganar o perder.

El arte es procedural y original. Los sprites, audio, mas enemigos y nuevas salas se incorporaran en iteraciones posteriores.

El arte es procedural y original. En iteraciones posteriores se pueden incorporar sprites, enemigos, combate, audio y mas salas.
# game-cpp
