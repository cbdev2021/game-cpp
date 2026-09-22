---
name: cadash-gameplay
description: Usar al tocar la logica del juego en main.cpp: fisica, movimiento, salto, agacharse, ataque/hitbox, spawn de enemigos, nivel/plataformas, spread/ritmo, "movimientos mas reales", "que se pueda agachar", "el golpe se vea", hitbox, damage, dificultad. Tambien cuando se pidan ajustes de control (teclas) o de ritmo del gameplay.
---

# Cadash Gameplay

Reglas de comportamiento de los sistemas del juego (main.cpp). Consultar esto
antes de "tunear" cualquier numero de gameplay para mantener el feel Cadash.

## Filosofia del movimiento

- **Hitbox generoso a favor del jugador**: el area de colision del warrior es
  mas pequena que el sprite (figura 24x34 pero hitbox 14x~25). Contacto
  "raspando" cuenta como esquive, no como golpe. NUNCA encoger el hitbox del
  jugador mas de lo que ya esta sin pedir permiso.
- **El golpe debe "leerse"**: cuando el warrior ataca (J/X) la espada y el
  brazo tienen que llenar el arco de 3 frames (ATK0/1/2) de forma que a 60fps
  se vea el barrido completo: windup -> swing -> follow-through. Si solo se ve
  "un instante" de espada, es un error de arte (ver skill cadash-pixel-art), no
  de timing.
- **Piernas/brazos reales**: correr usa UN CICLO de 4 fases de piernas con los
  brazos en contrafase (los brazos se mueven opuestos a las piernas). Salto:
  piernas recogidas (frame 6) / piernas estiradas en caida (frame 7).

## Controles (fijos, NO cambiar sin preguntar)

| Tecla | Accion |
|---|---|
| `A`/`D` o `Izq`/`Der` | Moverse (fija `facing`) |
| `Space`/`W`/`Arriba` | Saltar (solo en suelo) |
| `S`/`Abajo` | Agacharse (solo en suelo) |
| `J` o `X` | Ataque con espada (3-frame arc) |
| `R` | Reiniciar tras ganar/perder |
| `E` | Abrir salida al final |

## Constantes (fuente de verdad en main.cpp)

| Constante | Valor | Uso |
|---|---|---|
| `PLAYER_SPEED` | 105.0f | velocidad horizontal |
| `JUMP_SPEED` | -205.0f | impulso de salto (arriba) |
| `GRAVITY` | 560.0f | gravedad |
| `PLAYER_WIDTH` | 14.0f | ancho hitbox warrior |
| `PLAYER_HEIGHT` | 25.0f | alto hitbox de pie |
| `CROUCH_HEIGHT` | 20.0f | alto hitbox agachado |
| `ATTACK_DURATION` | 0.18f | duracion total del ataque |
| `PLAYER_HEALTH` | 100 | vida maxima |

Nota: los SPRITES son 24x34 (son mas "altos" que la hitbox a proposito:
`sprite.h = 34, PLAYER_HEIGHT = 25`). El sobrante superior es la punta del
casco/plumas — el cuerpo util es 25px. NO igualar la hitbox al sprite.

## Agacharse (crouching)

- Estado `warrior.crouching` (bool, en `Player`).
- Se activa con `S` o flecha abajo SOLO en suelo (`warrior.grounded`).
- Efectos:
  - Reduce `PLAYER_HEIGHT` a `CROUCH_HEIGHT` (20) para colisiones
    (`GetPlayerBounds`, `MovePlayerVertically`, `GetPlayerHeight`).
  - Bloquea movimiento horizontal y salto mientras se agacha
    (`horizontal = 0`, `IsKeyDown(KEY_S/DOWN)` → no correr/saltar).
  - Permite atacar agachado (frames 11/12) y esquivarle proyectiles bajos.
  - Al soltar la tecla vuelve a la altura normal.
- `CROUCH` se dibuja como el frame 11/12 de `WarriorArt` (cuerpo bajo con
  espada adelante en horizontal).

## Enemigos

- **Slime**: 4 frames (F0-F3) squash-and-stretch; salta de plataforma; rebota
  con `baseY` fija. `facing` lo voltea. Damage = 8.
- **Bat**: 3 frames de alitas (BAT WINGS up/mid/down) sobre cuerpo; vuela en
  sinus (`offset 9` px). `facing` +/-.
- Hitbox enemigo: `{x-8, y-20, 16, 20}` (mas generoso, favorece al jugador).
- Al morir sueltan `+10` gold y se marcan `alive=false`.

## Nivel

- `platforms[]` (arreglo fijo): base segmentada + 5 plataformas flotantes con
  `ACID` en el suelo. `worldHeight/worldWidth` definen el area; la camara
  sigue al warrior.
- Puerta final: `x > 1123` + `E` abre → `levelComplete`. `R` reinicia.
- `holes`: los gaps en el suelo se detectan por falta de plataforma y hay que
  saltarlos (no caer al acid = muere).

## Physics (valores de Cadash)

- `Gravity 560`, `Jump -205`, `Speed 105` → salto alcanza ~37px de altura y
  ~0.63s de aire: lo justo para saltar un gap de 2-3 tiles. No recortar
  salto/speed o los saltos exactos dejan de ser posibles.
- `DeltaTime` clamped a `1/30` para evitar "tunelado" en saltos rapidos.
- Vuelo (bat) no usa gravedad; flota con `velocity.y` senoidal. Al agacharse
  bajo el bat, el player puede esquivarlo sin entrar en su hitbox.

## Luces/sombras de escenario

- Sombra elip (procedural) bajo warrior y enemigos, ancla en `position.y`.
- Deborah: no "half bright" — sombras con `Fade(BLACK, 0.5)` o color directo
  oscuro; se ven legibles a 2x.

## Regla de oro

**Mantener el feel = los numeros exactos arriba. Cualquier cambio de
velocidad/dano/altura de salto requiere P: preguntar primero al usuario.**
