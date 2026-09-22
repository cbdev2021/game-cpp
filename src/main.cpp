#include "raylib.h"
#include "sprites.h"
#include <cmath>

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

struct Vector2D { float x; float y; };

struct Player {
    int id;
    const char* name;
    int health;
    int maxHealth;
    int gold;
    int level;
    Vector2D position;
    Vector2D velocity;
    int facing;
    bool grounded;
    float attackTimer;
    bool crouching;
};

enum class EnemyType { SLIME, BAT };

struct Enemy {
    EnemyType type;
    Vector2D position;
    float baseY;
    float leftLimit;
    float rightLimit;
    float speed;
    int facing;
    int health;
    bool alive;
    float phase;
};

static constexpr int INTERNAL_WIDTH = 480;
static constexpr int INTERNAL_HEIGHT = 270;
static constexpr int WINDOW_WIDTH = INTERNAL_WIDTH * 2;
static constexpr int WINDOW_HEIGHT = INTERNAL_HEIGHT * 2;
static constexpr float WORLD_WIDTH = 1200.0f;
static constexpr float PLAYER_WIDTH = 14.0f;
static constexpr float PLAYER_HEIGHT = 25.0f;
static constexpr float CROUCH_HEIGHT = 20.0f;
static constexpr float PLAYER_SPEED = 105.0f;
static constexpr float JUMP_SPEED = -205.0f;
static constexpr float GRAVITY = 560.0f;
static constexpr float ATTACK_DURATION = 0.18f;

static RenderTexture2D internalTarget;
static Camera2D camera = {{0.0f, 0.0f}, {0.0f, 0.0f}, 0.0f, 1.0f};
static bool levelComplete = false;
static float animTime = 0.0f;
static bool warriorMoving = false;

static Player warrior = {
    1, "WARRIOR", 100, 100, 0, 1,
    {48.0f, 170.0f}, {0.0f, 0.0f}, 1, false, 0.0f, false
};

static Enemy enemies[] = {
    {EnemyType::SLIME, {430.0f, 190.0f}, 190.0f, 390.0f, 500.0f, 24.0f, -1, 2, true, 0.0f},
    {EnemyType::BAT, {760.0f, 120.0f}, 120.0f, 720.0f, 850.0f, 46.0f, 1, 2, true, 1.3f}
};

static const Rectangle platforms[] = {
    {0.0f, 220.0f, WORLD_WIDTH, 50.0f},
    {120.0f, 174.0f, 116.0f, 12.0f},
    {292.0f, 140.0f, 118.0f, 12.0f},
    {470.0f, 178.0f, 145.0f, 12.0f},
    {670.0f, 128.0f, 135.0f, 12.0f},
    {870.0f, 170.0f, 130.0f, 12.0f},
    {1040.0f, 120.0f, 105.0f, 12.0f}
};

static const Color SOIL = {122, 92, 52, 255};
static const Color SOIL_DARK = {94, 70, 40, 255};
static const Color GRASS = {88, 168, 80, 255};
static const Color GRASS_DARK = {60, 132, 60, 255};
static const Color GOLD_DARK = {176, 128, 42, 255};
static const Color SKIN = {232, 185, 140, 255};
static const Color PLUM = {200, 45, 45, 255};

static sprites::Sheet warriorSheet;
static sprites::Sheet slimeSheet;
static sprites::Sheet batSheet;

static void DR(float x, float y, float w, float h, Color c) {
    DrawRectangle(static_cast<int>(x + 0.5f), static_cast<int>(y + 0.5f),
                  static_cast<int>(w + 0.5f), static_cast<int>(h + 0.5f), c);
}

static float PlayerHeight() {
    return warrior.crouching ? CROUCH_HEIGHT : PLAYER_HEIGHT;
}

static Rectangle GetPlayerBounds(const Vector2D& position) {
    const float h = PlayerHeight();
    return {position.x - PLAYER_WIDTH * 0.5f, position.y - h, PLAYER_WIDTH, h};
}

static Rectangle GetEnemyBounds(const Enemy& enemy) {
    return {enemy.position.x - 8.0f, enemy.position.y - 20.0f, 16.0f, 20.0f};
}

static Rectangle GetAttackBounds() {
    const Rectangle bounds = GetPlayerBounds(warrior.position);
    return {warrior.facing > 0 ? bounds.x + bounds.width : bounds.x - 20.0f, bounds.y + 5.0f, 20.0f, 14.0f};
}

static void ResetLevel() {
    warrior.position = {48.0f, 170.0f};
    warrior.velocity = {0.0f, 0.0f};
    warrior.health = warrior.maxHealth;
    warrior.gold = 0;
    warrior.facing = 1;
    warrior.grounded = false;
    warrior.attackTimer = 0.0f;
    warrior.crouching = false;
    enemies[0] = {EnemyType::SLIME, {430.0f, 190.0f}, 190.0f, 390.0f, 500.0f, 24.0f, -1, 2, true, 0.0f};
    enemies[1] = {EnemyType::BAT, {760.0f, 120.0f}, 120.0f, 720.0f, 850.0f, 46.0f, 1, 2, true, 1.3f};
    levelComplete = false;
}

static void MovePlayerHorizontally(float amount) {
    warrior.position.x += amount;
    Rectangle bounds = GetPlayerBounds(warrior.position);
    for (const Rectangle& platform : platforms) {
        if (!CheckCollisionRecs(bounds, platform)) continue;
        if (amount > 0.0f) warrior.position.x = platform.x - PLAYER_WIDTH * 0.5f;
        if (amount < 0.0f) warrior.position.x = platform.x + platform.width + PLAYER_WIDTH * 0.5f;
        bounds = GetPlayerBounds(warrior.position);
    }
    warrior.position.x = fmaxf(PLAYER_WIDTH * 0.5f, fminf(warrior.position.x, WORLD_WIDTH - PLAYER_WIDTH * 0.5f));
}

static void MovePlayerVertically(float amount) {
    const float previousBottom = warrior.position.y;
    warrior.position.y += amount;
    warrior.grounded = false;
    Rectangle bounds = GetPlayerBounds(warrior.position);
    for (const Rectangle& platform : platforms) {
        if (!CheckCollisionRecs(bounds, platform)) continue;
        if (amount >= 0.0f && previousBottom <= platform.y + 1.0f) {
            warrior.position.y = platform.y;
            warrior.velocity.y = 0.0f;
            warrior.grounded = true;
        } else if (amount < 0.0f) {
            warrior.position.y = platform.y + platform.height + PlayerHeight();
            warrior.velocity.y = 0.0f;
        }
        bounds = GetPlayerBounds(warrior.position);
    }
    if (warrior.position.y > INTERNAL_HEIGHT + PLAYER_HEIGHT) warrior.health = 0;
}

static void DamageEnemies() {
    const Rectangle attackBounds = GetAttackBounds();
    for (Enemy& enemy : enemies) {
        if (!enemy.alive || !CheckCollisionRecs(attackBounds, GetEnemyBounds(enemy))) continue;
        enemy.health -= 1;
        enemy.position.x += warrior.facing * 8.0f;
        if (enemy.health <= 0) {
            enemy.alive = false;
            warrior.gold += 10;
        }
    }
}

static void UpdateEnemies(float deltaTime) {
    for (Enemy& enemy : enemies) {
        if (!enemy.alive) continue;
        enemy.position.x += enemy.speed * deltaTime;
        if (enemy.position.x <= enemy.leftLimit || enemy.position.x >= enemy.rightLimit) enemy.speed *= -1.0f;
        enemy.facing = enemy.speed >= 0.0f ? 1 : -1;
        enemy.position.y = enemy.baseY;
        if (enemy.type == EnemyType::BAT) enemy.position.y += sinf(animTime * 2.6f + enemy.phase) * 9.0f;
        if (CheckCollisionRecs(GetPlayerBounds(warrior.position), GetEnemyBounds(enemy)) && warrior.attackTimer <= 0.0f) {
            warrior.health -= 8;
            warrior.position.x -= enemy.speed * deltaTime * 2.0f;
        }
    }
}

static void UpdateGame() {
    const float deltaTime = fmaxf(0.0f, fminf(GetFrameTime(), 0.033f));
    if (levelComplete || warrior.health <= 0) {
        warriorMoving = false;
        if (IsKeyPressed(KEY_R)) ResetLevel();
        return;
    }

    warrior.crouching = warrior.grounded && (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S));

    float horizontal = 0.0f;
    if (!warrior.crouching) {
        if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) horizontal -= 1.0f;
        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) horizontal += 1.0f;
    }
    warriorMoving = horizontal != 0.0f && !warrior.crouching;
    if (horizontal != 0.0f) warrior.facing = horizontal > 0.0f ? 1 : -1;
    MovePlayerHorizontally(horizontal * PLAYER_SPEED * deltaTime);

    if ((IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) && warrior.grounded && !warrior.crouching) {
        warrior.velocity.y = JUMP_SPEED;
        warrior.grounded = false;
    }
    warrior.velocity.y += GRAVITY * deltaTime;
    MovePlayerVertically(warrior.velocity.y * deltaTime);

    if (IsKeyPressed(KEY_J) || IsKeyPressed(KEY_X)) {
        warrior.attackTimer = ATTACK_DURATION;
        DamageEnemies();
    }
    if (warrior.attackTimer > 0.0f) warrior.attackTimer -= deltaTime;
    UpdateEnemies(deltaTime);

    if (warrior.position.x > 1123.0f && IsKeyPressed(KEY_E)) levelComplete = true;
    camera.target = {(float)fmaxf(INTERNAL_WIDTH * 0.5f, fminf(warrior.position.x, WORLD_WIDTH - INTERNAL_WIDTH * 0.5f)), (float)(INTERNAL_HEIGHT * 0.5f)};
}

static void DrawWarriorShadow() {
    const float squish = 2.0f + fmaxf(0.0f, 1.0f - warrior.velocity.y * 0.004f) * 1.5f;
    DrawEllipse(static_cast<int>(warrior.position.x), static_cast<int>(warrior.position.y),
                8.0f, squish, Fade(BLACK, 0.35f));
}

static void DrawSheetFrame(const sprites::Sheet& sheet, int frame, float x, float y, int facing) {
    if (!sheet.valid()) return;
    const float w = static_cast<float>(sheet.frameW);
    const float h = static_cast<float>(sheet.frameH);
    const Rectangle src = facing >= 0
        ? Rectangle{frame * w, 0.0f, w, h}
        : Rectangle{(frame + 1) * w, 0.0f, -w, h};
    const Rectangle dst = Rectangle{x - w * 0.5f, y - h, w, h};
    DrawTexturePro(sheet.tex, src, dst, Vector2{0.0f, 0.0f}, 0.0f, WHITE);
}

static int WarriorFrame() {
    if (warrior.crouching) return warrior.attackTimer > 0.0f ? 12 : 11;
    if (warrior.attackTimer > 0.0f) {
        const float p = 1.0f - fmaxf(0.0f, warrior.attackTimer) / ATTACK_DURATION;
        return p < 0.34f ? 8 : (p < 0.67f ? 9 : 10);
    }
    if (!warrior.grounded) return warrior.velocity.y < 0.0f ? 6 : 7;
    if (warriorMoving) return 2 + ((static_cast<int>(animTime * 9.0f) & 3));
    return static_cast<int>(sinf(animTime * 3.0f) * 0.5f + 0.5f);
}

static void DrawWarrior() {
    DrawWarriorShadow();
    DrawSheetFrame(warriorSheet, WarriorFrame(), warrior.position.x, warrior.position.y, warrior.facing);
}

static void DrawSlime(const Enemy& enemy) {
    const float x = enemy.position.x;
    const float y = enemy.position.y;
    DrawEllipse(static_cast<int>(x), static_cast<int>(y), 9.0f, 2.0f, Fade(BLACK, 0.35f));
    const int frame = (static_cast<int>((animTime * 3.0f + enemy.phase) * 3.0f)) & 3;
    DrawSheetFrame(slimeSheet, frame, x, y, enemy.facing);
}

static void DrawBat(const Enemy& enemy) {
    const float x = enemy.position.x;
    const float y = enemy.position.y;
    const int frame = (static_cast<int>((animTime * 11.0f + enemy.phase * 4.0f))) % 3;
    DrawSheetFrame(batSheet, frame, x, y, enemy.facing);
}

static void DrawEnemy(const Enemy& enemy) {
    if (!enemy.alive) return;
    if (enemy.type == EnemyType::SLIME) DrawSlime(enemy);
    else DrawBat(enemy);
}

static void DrawSky() {
    DrawRectangleGradientV(0, 0, static_cast<int>(WORLD_WIDTH), INTERNAL_HEIGHT,
                           Color{96, 152, 216, 255}, Color{214, 234, 246, 255});
    DrawCircleGradient(150, 44, 42.0f, Color{255, 250, 210, 255}, Color{255, 214, 120, 255});
    DrawCircle(150, 44, 13.0f, Color{255, 230, 150, 255});
    struct Float2 { float x, y, r; };
    const Float2 clouds[] = {
        {300.0f, 42.0f, 16.0f}, {420.0f, 60.0f, 12.0f}, {560.0f, 34.0f, 18.0f},
        {760.0f, 55.0f, 14.0f}, {980.0f, 40.0f, 16.0f}, {860.0f, 66.0f, 10.0f},
        {1090.0f, 52.0f, 14.0f}
    };
    for (const Float2& c : clouds) {
        DrawEllipse(static_cast<int>(c.x), static_cast<int>(c.y), c.r, c.r * 0.42f, Color{255, 255, 255, 200});
        DrawEllipse(static_cast<int>(c.x) + 8, static_cast<int>(c.y) + 2, c.r * 0.7f, c.r * 0.32f, Color{255, 255, 255, 170});
    }
    DrawRectangle(0, 150, static_cast<int>(WORLD_WIDTH), 70, Fade(Color{74, 118, 92, 255}, 0.55f));
}

static void DrawCastle() {
    DrawRectangle(700, 128, 12, 60, Color{74, 84, 120, 255});
    DrawRectangle(700, 118, 12, 10, Color{74, 84, 120, 255});
    DrawRectangle(700, 128, 12, 2, Color{90, 102, 142, 255});
    DrawRectangle(702, 142, 2, 3, Color{30, 36, 58, 255});
    DrawRectangle(706, 150, 2, 3, Color{30, 36, 58, 255});

    DrawRectangle(712, 138, 34, 50, Color{66, 76, 110, 255});
    for (int i = 0; i < 3; ++i) DrawRectangle(712 + i * 11, 134, 6, 4, Color{66, 76, 110, 255});
    DrawRectangle(714, 148, 30, 2, Color{88, 100, 140, 255});
    DrawRectangle(716, 164, 6, 24, Color{40, 48, 76, 255});
    DrawRectangle(736, 164, 6, 24, Color{40, 48, 76, 255});

    DrawRectangle(746, 128, 12, 60, Color{74, 84, 120, 255});
    DrawRectangle(746, 118, 12, 10, Color{74, 84, 120, 255});
    DrawRectangle(746, 128, 12, 2, Color{90, 102, 142, 255});
    DrawRectangle(748, 142, 2, 3, Color{30, 36, 58, 255});
    DrawRectangle(752, 150, 2, 3, Color{30, 36, 58, 255});

    DrawRectangle(650, 168, 24, 20, Color{58, 92, 72, 255});
    DrawRectangle(830, 160, 30, 28, Color{58, 92, 72, 255});
    DrawCircle(662, 150, 14, Color{52, 70, 90, 255});
    DrawCircle(845, 142, 18, Color{56, 76, 96, 255});
}

static void DrawGroundTexture() {
    DrawRectangle(0, 220, static_cast<int>(WORLD_WIDTH), 50, SOIL);
    DrawRectangle(0, 220, static_cast<int>(WORLD_WIDTH), 3, Color{106, 188, 92, 255});
    DrawRectangle(0, 223, static_cast<int>(WORLD_WIDTH), 2, GRASS);
    DrawRectangle(0, 225, static_cast<int>(WORLD_WIDTH), 2, GRASS_DARK);
    for (int i = 0; i < 52; ++i) {
        const float px = static_cast<float>((i * 37 + 11) % 1200);
        const float py = 228.0f + static_cast<float>((i * 53 + 3) % 38);
        DR(px, py, 3, 2, SOIL_DARK);
        DR(px + 5, py + 3, 2, 2, Color{138, 104, 60, 255});
    }
    for (int i = 0; i < 40; ++i) {
        const float px = static_cast<float>((i * 29) % 1200);
        DR(px, 220, 2, 5, Color{140, 210, 120, 255});
    }
}

static void DrawPlatforms() {
    for (const Rectangle& platform : platforms) {
        if (platform.y >= 220.0f) continue;
        DR(platform.x, platform.y, platform.width, platform.height, SOIL);
        DR(platform.x, platform.y, platform.width, 3, Color{106, 188, 92, 255});
        DR(platform.x, platform.y + 3, platform.width, 2, GRASS);
        DR(platform.x, platform.y + 5, platform.width, 1, GRASS_DARK);
        for (float px = platform.x; px < platform.x + platform.width; px += 16.0f) {
            DR(px + 1.0f, platform.y + 6, 2, platform.height - 6, SOIL_DARK);
        }
        for (float px = platform.x; px < platform.x + platform.width; px += 8.0f) {
            const float grassSway = sinf(animTime * 3.0f + px * 0.5f) * 1.0f;
            DR(px + 1.0f, platform.y + platform.height - 1, 2, 3.0f + grassSway, GRASS_DARK);
            DR(px, platform.y - 1, 3, 2, Color{140, 210, 120, 255});
        }
        DrawRectangleLinesEx(platform, 1.0f, Color{70, 48, 30, 255});
    }
}

static void DrawTreeEx(float x) {
    const float trunkW = 7.0f;
    DR(x - trunkW * 0.5f, 197.0f, trunkW, 23.0f, Color{92, 62, 38, 255});
    DR(x - trunkW * 0.5f, 197.0f, trunkW, 2.0f, Color{60, 38, 22, 255});
    DrawCircle(static_cast<int>(x), 170, 17, Color{44, 118, 62, 255});
    DrawCircle(static_cast<int>(x) - 13, 177, 12, Color{44, 118, 62, 255});
    DrawCircle(static_cast<int>(x) + 13, 177, 12, Color{44, 118, 62, 255});
    DrawCircle(static_cast<int>(x), 160, 13, Color{52, 136, 74, 255});
    DrawCircle(static_cast<int>(x) - 6, 178, 6, Color{60, 148, 84, 255});
    DR(x - 2.0f, 190.0f, 4.0f, 4.0f, Color{90, 160, 90, 255});
}

static void DrawDecorations() {
    DrawTreeEx(210.0f);
    DrawTreeEx(600.0f);
    DrawTreeEx(1030.0f);
    DrawTreeEx(160.0f);

    const float bushXs[] = {80.0f, 350.0f, 515.0f, 920.0f, 1120.0f};
    for (float bx : bushXs) {
        DrawCircle(static_cast<int>(bx), 217, 8, Color{56, 140, 74, 255});
        DrawCircle(static_cast<int>(bx) + 7, 218, 6, Color{46, 120, 62, 255});
        DrawCircle(static_cast<int>(bx) - 6, 216, 5, GRASS_DARK);
    }
    DR(120, 212, 10, 8, Color{110, 106, 104, 255});
    DR(118, 210, 14, 3, Color{128, 124, 122, 255});
    DR(356, 214, 8, 6, Color{110, 106, 104, 255});
    DR(702, 213, 11, 7, Color{110, 106, 104, 255});
    DR(962, 210, 9, 10, Color{110, 106, 104, 255});
}

static void DrawGate() {
    const float gx = 1136.0f;
    DrawCircleGradient(static_cast<int>(gx), 205, 55.0f, Color{255, 226, 150, 255}, Fade(Color{255, 200, 100, 255}, 0.0f));
    DrawRectangle(1122, 148, 30, 72, Color{116, 100, 82, 255});
    DrawRectangle(1158, 148, 24, 72, Color{116, 100, 82, 255});
    for (int i = 0; i < 3; ++i) {
        DR(1122 + i * 10, 142, 7, 6, Color{116, 100, 82, 255});
        DR(1158 + i * 8, 142, 7, 6, Color{116, 100, 82, 255});
    }
    DrawRectangle(1140, 158, 24, 62, Color{40, 32, 24, 255});
    DrawRectangle(1146, 162, 12, 58, Color{66, 44, 26, 255});
    for (int i = 0; i < 4; ++i) DR(1148, 162 + i * 15, 8, 1, Color{48, 32, 18, 255});
    DR(1152, 180, 2, 2, GOLD);
    DR(1156, 178, 2, 2, Color{150, 210, 120, 255});
    DR(1144, 158, 16, 3, GOLD_DARK);
}

static void DrawWorld() {
    DrawSky();
    DrawCastle();
    DrawGroundTexture();
    DrawDecorations();
    DrawPlatforms();
    DrawGate();
    for (const Enemy& enemy : enemies) DrawEnemy(enemy);
    DrawWarrior();
}

static void DrawHud() {
    DrawRectangle(0, 0, INTERNAL_WIDTH, 44, Fade(Color{18, 22, 38, 255}, 0.94f));
    DrawRectangle(0, 44, INTERNAL_WIDTH, 2, Color{120, 90, 50, 255});
    DrawRectangle(0, 0, INTERNAL_WIDTH, 2, Color{90, 64, 40, 255});

    DrawRectangle(5, 6, 34, 34, Color{24, 28, 46, 255});
    DrawRectangleLines(5, 6, 34, 34, GOLD_DARK);
    DrawRectangle(7, 8, 30, 30, Color{40, 46, 74, 255});
    DR(19, 17, 6, 6, SKIN);
    DR(18, 14, 8, 3, GOLD);
    DR(18, 14, 8, 1, GOLD_DARK);
    DR(17, 15, 3, 2, PLUM);
    DR(23, 15, 3, 2, PLUM);
    DR(20, 19, 1, 1, Color{30, 24, 40, 255});

    DrawText("WARRIOR", 45, 10, 12, WHITE);
    DrawText("1P", 45, 25, 9, Color{180, 200, 230, 255});

    DrawRectangle(95, 9, 120, 11, Color{30, 30, 40, 255});
    DrawRectangleLines(95, 9, 120, 11, Color{110, 90, 50, 255});
    const float hpRatio = (float)warrior.health / (float)warrior.maxHealth;
    DrawRectangle(97, 11, static_cast<int>(116 * hpRatio), 7, hpRatio > 0.35f ? Color{200, 48, 48, 255} : Color{240, 160, 40, 255});
    DrawRectangle(97, 11, 3, 7, Color{255, 120, 120, 255});
    DrawText(TextFormat("%d/%d", warrior.health, warrior.maxHealth), 99, 22, 9, WHITE);

    DrawCircle(233, 14, 5, GOLD);
    DrawRectangle(230, 13, 7, 2, GOLD_DARK);
    DrawText(TextFormat("GOLD %d", warrior.gold), 228, 24, 9, Color{230, 200, 120, 255});

    DrawText(TextFormat("LV %d", warrior.level), 310, 10, 12, Color{150, 210, 240, 255});
    DrawText("FIGHT BIG MONSTERS", 310, 28, 8, Color{120, 140, 170, 255});

    DrawText("J/X ATK", 390, 10, 9, RAYWHITE);
    DrawText("A/D MOVE", 390, 22, 9, RAYWHITE);
    DrawText("SPACE JUMP", 390, 34, 9, RAYWHITE);

    DrawText("E EXIT", 10, 256, 9, Color{200, 215, 235, 255});
    DrawText("R RESTART", 62, 256, 9, Color{160, 170, 190, 255});

    if (levelComplete || warrior.health <= 0) {
        DrawRectangle(115, 96, 250, 74, Fade(Color{16, 20, 34, 255}, 0.94f));
        DrawRectangleLines(115, 96, 250, 74, GOLD);
        DrawRectangle(119, 100, 242, 66, GOLD_DARK);
        const char* title = levelComplete ? "STAGE CLEAR" : "GAME OVER";
        DrawText(title, levelComplete ? 160 : 168, 112, 22, levelComplete ? GOLD : Color{220, 60, 50, 255});
        DrawText("Press R to restart", 170, 146, 13, WHITE);
    }
}

static void DrawGameFrame() {
    animTime += GetFrameTime();
    UpdateGame();
    BeginTextureMode(internalTarget);
    ClearBackground(BLACK);
    BeginMode2D(camera);
    DrawWorld();
    EndMode2D();
    DrawHud();
    EndTextureMode();

    BeginDrawing();
    ClearBackground(DARKGRAY);
    const float screenWidth = static_cast<float>(GetScreenWidth());
    const float screenHeight = static_cast<float>(GetScreenHeight());
    const float scale = fminf(screenWidth / INTERNAL_WIDTH, screenHeight / INTERNAL_HEIGHT);
    const float destinationWidth = INTERNAL_WIDTH * scale;
    const float destinationHeight = INTERNAL_HEIGHT * scale;
    const float destinationX = (screenWidth - destinationWidth) * 0.5f;
    const float destinationY = (screenHeight - destinationHeight) * 0.5f;
    DrawTexturePro(internalTarget.texture, {0.0f, 0.0f, static_cast<float>(INTERNAL_WIDTH), -static_cast<float>(INTERNAL_HEIGHT)}, {destinationX, destinationY, destinationWidth, destinationHeight}, {0.0f, 0.0f}, 0.0f, WHITE);
    EndDrawing();
}

static bool LoadSpriteSheet(sprites::Sheet* sheet, const char* path,
                            const sprites::ArtDef& def, const sprites::Palette& pal) {
    Texture2D tex = LoadTexture(path);
    if (IsTextureValid(tex)) {
        sheet->tex = tex;
        sheet->frameW = def.w;
        sheet->frameH = def.h;
        sheet->frameCount = static_cast<int>(def.frames.size());
        SetTextureFilter(tex, TEXTURE_FILTER_POINT);
        return true;
    }
    TraceLog(LOG_WARNING, "No se encontro %s, usando arte embebido", path);
    *sheet = sprites::LoadSheetFromImage(def, pal, true);
    return sheet->valid();
}

static void loadSprites() {
    LoadSpriteSheet(&warriorSheet, "assets/sprites/warrior.png", sprites::WarriorArt(), sprites::WARRIOR_PAL);
    LoadSpriteSheet(&slimeSheet, "assets/sprites/slime.png", sprites::SlimeArt(), sprites::SLIME_PAL);
    LoadSpriteSheet(&batSheet, "assets/sprites/bat.png", sprites::BatArt(), sprites::BAT_PAL);
}

int main() {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Cadash Remake - Side Scroller");
    if (!IsWindowReady()) return 1;
    SetTargetFPS(60);
    internalTarget = LoadRenderTexture(INTERNAL_WIDTH, INTERNAL_HEIGHT);
    if (!IsRenderTextureValid(internalTarget)) {
        CloseWindow();
        return 1;
    }
    loadSprites();
    camera.offset = {INTERNAL_WIDTH * 0.5f, INTERNAL_HEIGHT * 0.5f};
    camera.target = {INTERNAL_WIDTH * 0.5f, INTERNAL_HEIGHT * 0.5f};
    camera.zoom = 1.0f;

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(DrawGameFrame, 0, 1);
#else
    while (!WindowShouldClose()) DrawGameFrame();
    if (IsTextureValid(warriorSheet.tex)) UnloadTexture(warriorSheet.tex);
    if (IsTextureValid(slimeSheet.tex)) UnloadTexture(slimeSheet.tex);
    if (IsTextureValid(batSheet.tex)) UnloadTexture(batSheet.tex);
    UnloadRenderTexture(internalTarget);
    CloseWindow();
#endif
    return 0;
}