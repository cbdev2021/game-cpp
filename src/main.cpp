#include "raylib.h"
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
};

struct Enemy {
    Vector2D position;
    float leftLimit;
    float rightLimit;
    float speed;
    int health;
    bool alive;
};

static constexpr int INTERNAL_WIDTH = 480;
static constexpr int INTERNAL_HEIGHT = 270;
static constexpr int WINDOW_WIDTH = INTERNAL_WIDTH * 2;
static constexpr int WINDOW_HEIGHT = INTERNAL_HEIGHT * 2;
static constexpr float WORLD_WIDTH = 1200.0f;
static constexpr float PLAYER_WIDTH = 14.0f;
static constexpr float PLAYER_HEIGHT = 25.0f;
static constexpr float PLAYER_SPEED = 105.0f;
static constexpr float JUMP_SPEED = -205.0f;
static constexpr float GRAVITY = 560.0f;

static RenderTexture2D internalTarget;
static Camera2D camera = {{0.0f, 0.0f}, {0.0f, 0.0f}, 0.0f, 1.0f};
static bool levelComplete = false;

static Player warrior = {
    1, "Warrior", 100, 100, 0, 1,
    {48.0f, 170.0f}, {0.0f, 0.0f}, 1, false, 0.0f
};

static Enemy enemies[] = {
    {{430.0f, 190.0f}, 390.0f, 500.0f, 28.0f, 2, true},
    {{760.0f, 135.0f}, 720.0f, 850.0f, 22.0f, 2, true}
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

static Rectangle GetPlayerBounds(const Vector2D& position) {
    return {position.x - PLAYER_WIDTH * 0.5f, position.y - PLAYER_HEIGHT, PLAYER_WIDTH, PLAYER_HEIGHT};
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
    enemies[0] = {{430.0f, 190.0f}, 390.0f, 500.0f, 28.0f, 2, true};
    enemies[1] = {{760.0f, 135.0f}, 720.0f, 850.0f, 22.0f, 2, true};
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
            warrior.position.y = platform.y + platform.height + PLAYER_HEIGHT;
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
        if (CheckCollisionRecs(GetPlayerBounds(warrior.position), GetEnemyBounds(enemy)) && warrior.attackTimer <= 0.0f) {
            warrior.health -= 8;
            warrior.position.x -= enemy.speed * deltaTime * 2.0f;
        }
    }
}

static void UpdateGame() {
    const float deltaTime = fmaxf(0.0f, fminf(GetFrameTime(), 0.033f));
    if (levelComplete || warrior.health <= 0) {
        if (IsKeyPressed(KEY_R)) ResetLevel();
        return;
    }

    float horizontal = 0.0f;
    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) horizontal -= 1.0f;
    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) horizontal += 1.0f;
    if (horizontal != 0.0f) warrior.facing = horizontal > 0.0f ? 1 : -1;
    MovePlayerHorizontally(horizontal * PLAYER_SPEED * deltaTime);

    if ((IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) && warrior.grounded) {
        warrior.velocity.y = JUMP_SPEED;
        warrior.grounded = false;
    }
    warrior.velocity.y += GRAVITY * deltaTime;
    MovePlayerVertically(warrior.velocity.y * deltaTime);

    if (IsKeyPressed(KEY_J) || IsKeyPressed(KEY_X)) {
        warrior.attackTimer = 0.18f;
        DamageEnemies();
    }
    if (warrior.attackTimer > 0.0f) warrior.attackTimer -= deltaTime;
    UpdateEnemies(deltaTime);

    if (warrior.position.x > 1123.0f && IsKeyPressed(KEY_E)) levelComplete = true;
    camera.target = {(float)fmaxf(INTERNAL_WIDTH * 0.5f, fminf(warrior.position.x, WORLD_WIDTH - INTERNAL_WIDTH * 0.5f)), (float)(INTERNAL_HEIGHT * 0.5f)};
}

static void DrawWarrior() {
    const Rectangle bounds = GetPlayerBounds(warrior.position);
    const float x = warrior.position.x;
    const float y = warrior.position.y;
    const Color tunic = warrior.attackTimer > 0.0f ? ORANGE : BLUE;
    DrawRectangle(static_cast<int>(x - 6.0f), static_cast<int>(y - 17.0f), 12, 14, tunic);
    DrawRectangle(static_cast<int>(x - 5.0f), static_cast<int>(y - 24.0f), 10, 7, BEIGE);
    DrawRectangle(static_cast<int>(x - 8.0f), static_cast<int>(y - 26.0f), 16, 3, GOLD);
    DrawRectangle(static_cast<int>(x - 5.0f), static_cast<int>(y - 3.0f), 4, 3, BROWN);
    DrawRectangle(static_cast<int>(x + 1.0f), static_cast<int>(y - 3.0f), 4, 3, BROWN);
    DrawRectangleLinesEx(bounds, 1.0f, RAYWHITE);
    const float handX = x + warrior.facing * 7.0f;
    DrawCircle(static_cast<int>(handX), static_cast<int>(y - 12.0f), 2.0f, GOLD);
    if (warrior.attackTimer > 0.0f) DrawLineEx({handX, y - 12.0f}, {x + warrior.facing * 24.0f, y - 12.0f}, 3.0f, LIGHTGRAY);
}

static void DrawEnemy(const Enemy& enemy) {
    if (!enemy.alive) return;
    const float x = enemy.position.x;
    const float y = enemy.position.y;
    DrawRectangle(static_cast<int>(x - 7.0f), static_cast<int>(y - 13.0f), 14, 13, DARKPURPLE);
    DrawCircle(static_cast<int>(x), static_cast<int>(y - 16.0f), 6.0f, MAROON);
    DrawRectangle(static_cast<int>(x - 3.0f), static_cast<int>(y - 17.0f), 2, 2, YELLOW);
    DrawRectangle(static_cast<int>(x + 1.0f), static_cast<int>(y - 17.0f), 2, 2, YELLOW);
    DrawRectangle(static_cast<int>(x - 8.0f), static_cast<int>(y - 24.0f), 16, 2, BLACK);
    DrawRectangle(static_cast<int>(x - 8.0f), static_cast<int>(y - 24.0f), 8 * enemy.health, 2, RED);
}

static void DrawWorld() {
    DrawRectangle(0, 0, static_cast<int>(WORLD_WIDTH), INTERNAL_HEIGHT, Color{21, 27, 52, 255});
    DrawCircle(930, 65, 24.0f, Color{58, 67, 105, 255});
    DrawCircle(930, 65, 18.0f, Color{77, 87, 130, 255});
    for (const Rectangle& platform : platforms) {
        DrawRectangleRec(platform, Color{89, 57, 45, 255});
        DrawRectangle(static_cast<int>(platform.x), static_cast<int>(platform.y), static_cast<int>(platform.width), 4, Color{56, 117, 63, 255});
        DrawRectangleLinesEx(platform, 1.0f, Color{143, 92, 57, 255});
    }
    DrawRectangle(1135, 170, 30, 50, BROWN);
    DrawRectangle(1141, 177, 18, 43, GOLD);
    DrawCircle(1154, 198, 2.0f, YELLOW);
    for (const Enemy& enemy : enemies) DrawEnemy(enemy);
    DrawWarrior();
}

static void DrawHud() {
    DrawRectangle(0, 0, INTERNAL_WIDTH, 34, Fade(BLACK, 0.8f));
    DrawText("WARRIOR", 10, 7, 14, WHITE);
    DrawText(TextFormat("HP %d/%d", warrior.health, warrior.maxHealth), 105, 7, 14, RED);
    DrawText(TextFormat("GOLD %d", warrior.gold), 210, 7, 14, GOLD);
    DrawText(TextFormat("LV %d", warrior.level), 300, 7, 14, SKYBLUE);
    DrawText("J/X ATTACK", 350, 7, 12, RAYWHITE);
    DrawText("A/D MOVE  SPACE JUMP  E EXIT", 10, 250, 12, RAYWHITE);
    if (levelComplete || warrior.health <= 0) {
        DrawRectangle(115, 96, 250, 70, Fade(BLACK, 0.9f));
        DrawRectangleLines(115, 96, 250, 70, GOLD);
        DrawText(levelComplete ? "STAGE CLEAR" : "YOU FALL", 181, 108, 22, GOLD);
        DrawText("Press R to restart", 170, 140, 14, WHITE);
    }
}

static void DrawGameFrame() {
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

int main() {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Cadash Remake - Side Scroller");
    if (!IsWindowReady()) return 1;
    SetTargetFPS(60);
    internalTarget = LoadRenderTexture(INTERNAL_WIDTH, INTERNAL_HEIGHT);
    if (!IsRenderTextureValid(internalTarget)) {
        CloseWindow();
        return 1;
    }
    camera.offset = {INTERNAL_WIDTH * 0.5f, INTERNAL_HEIGHT * 0.5f};
    camera.target = {INTERNAL_WIDTH * 0.5f, INTERNAL_HEIGHT * 0.5f};
    camera.zoom = 1.0f;

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(DrawGameFrame, 0, 1);
#else
    while (!WindowShouldClose()) DrawGameFrame();
    UnloadRenderTexture(internalTarget);
    CloseWindow();
#endif
    return 0;
}