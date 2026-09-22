---
name: cadash-pixel-art
description: Usa esta skill cuando toques pixel-art del juego: sprites, warrior, slime, bat, paletas, frames, animación, espada, assets/sprites/*.png, sprites.h, gen_sprites, pixel art, dibujar personaje, guardar hay sprite. Define el estilo visual Cadash arcade 90s y el pipeline de arte.
---

# Cadash Pixel-Art (estilo arcade 90s)

Refuerza que el arte siga el pipeline y el canon "Cadash 90s": pixel art nítido,
silueta clara y movimientos reales por frame.

## Paletas oficiales

- `WARRIOR_PAL` — bárbaro (24 colores): casco dorado, cota 'k' azul, cinturón
  'B', faldilla 'b', contorno 'O', espada 'w'/'W'.
- `SLIME_PAL` — slime (11): cuerpo 'g' verde, ojos 'e', boca 'C'/'c'.
- `BAT_PAL` — murciélago (11): cuerpo 'M'/'m' púrpura, ojos 'e', colmillos 'w'.

## Cómo se compone el arte

- Cada frame se arma con `Join(...)` (apilar) + overlays `MergeOverlay`
  (pintar por encima sin borrar). Un frame NUNCA se dibuja completo: se
  reutilizan piezas (cabeza, torso con/sin espada, cadera, piernas).
- Piensa en "profundidad": cabeza encima, luego torso, cadera, piernas.
  El que esté más abajo en `Join` se pinta primero.
- Frames del warrior (13):
  - 0-1 idle (respiración), 2-5 run con piernas en contrafase y brazos
    balanceándose (usa `WAR_LEG_RUN0-3` + torsos `STRIDE_OUT/IN` y `PASS`),
  - 6 salto, 7 caída,
  - 8-10 ataque con espada SOBRE el torso (`MergeOverlay` del overlay de
    espada, frames `ATK0/1/2` con barrido windup→swing→followthrough),
  - 11 idle agachado, 12 estocada agachado.

## Reglas de oro del sprite

1. **Silueta primero**: un frame debe leerse como silueta contra el fondo
   antes de fijarse en el detalle. El héroe debe distinguirse del slime/bat a
   1 vistazo.
2. **Contorno**: los bordes exteriores llevan 'O' (o color de contorno del
   personaje) para que no se fundan con el fondo. El interior usa los acentos.
3. **Paleta limitada** por sprite (no inventar colores fuera de la paleta).
4. **Anclaje**: los PIES van abajo del todo; el sprite se dibuja desde la
   posición de los pies (`y - h`). Al agacharse, el cuerpo baja (frame 11/12
   con relleno arriba `EmptyRows(2)` + torso/cadera/piernas recogidas).
5. **Profundidad de espada por capa**: en idle la espada cuelga en la cadera
   ('w' en `WAR_SWORD_HIP`), en ataque pasa a adelante sobre el torso.

## Cómo evaluar el arte (SAY "bien/sigue" o señalá el detalle a cambiar)

Cuando muestre un preview, evaluá:
- ¿Lee como silueta? ¿Piezas bien proporcionadas (cabeza/tronco/piernas)?
- ¿El movimiento se entiende por frames (piernas en fases, brazos en
  contrafase, espada con barrido)?
- ¿El agachado se ve más bajo y el ataque agachado golpea bajo?
- Decime "más realista" y ajusto brazos/piernas/velocidad de animación.

## Pipeline de arte (no romper)

- `src/tools/gen_sprites.cpp` es la única fuente de sprite sheets → regenera
  `src/assets/sprites/*.png` con paletas y frame layout. NUNCA editar los PNG
  a mano (son artefactos).
- Flujo rápido de iteración: editar `src/sprites.h` → compilar
  `gen_sprites_preview.exe` → `--preview` ASCII del warrior → iterar →
  exportar PNG → recompilar juego/web.
- Después de cambiar sprites hay que recompilar `.exe` (desktop) y la web
  (wasm + `--embed-file assets`), porque los PNG se embeben.
