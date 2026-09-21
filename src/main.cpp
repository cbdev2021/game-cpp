#include "raylib.h"
#include <cmath>

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

struct Vector2D {
    float x;
    float y;
};

struct Player {
    int id;
    const char* name;
    int health;
    int maxHealth;
    int gold;
    int level;
    Vector2D position;
};

static constexpr int INTERNAL_WIDTH = 480;
static constexpr int INTERNAL_HEIGHT = 270;
static constexpr int WINDOW_WIDTH = INTERNAL_WIDTH * 2;
static constexpr int WINDOW_HEIGHT = INTERNAL_HEIGHT * 2;
static constexpr float PLAYER_SIZE = 14.0f;
static constexpr float PLAYER_SPEED = 110.0f;

static RenderTexture2D internalTarget;
static Player warrior = {
    1,
    "Warrior",
    100,
    100,
    0,
    1,
    {INTERNAL_WIDTH / 2.0f, INTERNAL_HEIGHT / 2.0f}
};

static const Rectangle walls[] = {
    {0.0f, 0.0f, static_cast<float>(INTERNAL_WIDTH), 8.0f},
    {0.0f, static_cast<float>(INTERNAL_HEIGHT - 8), static_cast<float>(INTERNAL_WIDTH), 8.0f},
    {0.0f, 0.0f, 8.0f, static_cast<float>(INTERNAL_HEIGHT)},
    {static_cast<float>(INTERNAL_WIDTH - 8), 0.0f, 8.0f, static_cast<float>(INTERNAL_HEIGHT)},
    {120.0f, 92.0f, 180.0f, 12.0f},
    {120.0f, 166.0f, 180.0f, 12.0f},
    {76.0f, 92.0f, 12.0f, 86.0f},
    {332.0f, 92.0f, 12.0f, 86.0f}
};

static Rectangle GetPlayerBounds(const Vector2D& position) {
    return {
        position.x - PLAYER_SIZE * 0.5f,
        position.y - PLAYER_SIZE * 0.5f,
        PLAYER_SIZE,
        PLAYER_SIZE
    };
}

static bool IsPositionValid(const Vector2D& position) {
    const Rectangle playerBounds = GetPlayerBounds(position);

    for (const Rectangle& wall : walls) {
        if (CheckCollisionRecs(playerBounds, wall)) {
            return false;
        }
    }

    return true;
}

static void UpdateGame() {
    Vector2D direction = {0.0f, 0.0f};

    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) direction.x += 1.0f;
    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) direction.x -= 1.0f;
    if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) direction.y += 1.0f;
    if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) direction.y -= 1.0f;

    if (direction.x != 0.0f || direction.y != 0.0f) {
        const float length = sqrtf(direction.x * direction.x + direction.y * direction.y);
        direction.x /= length;
        direction.y /= length;
    }

    const float distance = PLAYER_SPEED * GetFrameTime();
    Vector2D nextPosition = warrior.position;
    nextPosition.x += direction.x * distance;
    if (IsPositionValid(nextPosition)) warrior.position.x = nextPosition.x;

    nextPosition = warrior.position;
    nextPosition.y += direction.y * distance;
    if (IsPositionValid(nextPosition)) warrior.position.y = nextPosition.y;
}

static void DrawGameFrame() {
    UpdateGame();

    BeginTextureMode(internalTarget);
    ClearBackground(DARKGREEN);

    for (const Rectangle& wall : walls) {
        DrawRectangleRec(wall, DARKGRAY);
        DrawRectangleLinesEx(wall, 1.0f, GRAY);
    }

    DrawText(warrior.name, 16, 16, 20, WHITE);
    DrawText(TextFormat("HP: %d/%d", warrior.health, warrior.maxHealth), 16, 42, 16, RED);
    DrawText(TextFormat("Gold: %d  Level: %d", warrior.gold, warrior.level), 16, 62, 16, GOLD);
    DrawRectangleRec(GetPlayerBounds(warrior.position), MAROON);
    DrawRectangleLinesEx(GetPlayerBounds(warrior.position), 1.0f, RAYWHITE);
    DrawText("Move: WASD / Arrows", 16, INTERNAL_HEIGHT - 24, 14, RAYWHITE);

    EndTextureMode();

    BeginDrawing();
    ClearBackground(DARKGRAY);

    const float screenWidth = static_cast<float>(GetScreenWidth());
    const float screenHeight = static_cast<float>(GetScreenHeight());
    const float scale = (screenWidth / INTERNAL_WIDTH < screenHeight / INTERNAL_HEIGHT)
        ? screenWidth / INTERNAL_WIDTH
        : screenHeight / INTERNAL_HEIGHT;
    const float destinationWidth = INTERNAL_WIDTH * scale;
    const float destinationHeight = INTERNAL_HEIGHT * scale;
    const float destinationX = (screenWidth - destinationWidth) * 0.5f;
    const float destinationY = (screenHeight - destinationHeight) * 0.5f;

    DrawTexturePro(
        internalTarget.texture,
        {0.0f, 0.0f, static_cast<float>(INTERNAL_WIDTH), -static_cast<float>(INTERNAL_HEIGHT)},
        {destinationX, destinationY, destinationWidth, destinationHeight},
        {0.0f, 0.0f},
        0.0f,
        WHITE
    );
    EndDrawing();
}

int main() {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Cadash Remake");

    if (!IsWindowReady()) {
        return 1;
    }

    SetTargetFPS(60);

    internalTarget = LoadRenderTexture(INTERNAL_WIDTH, INTERNAL_HEIGHT);

    if (!IsRenderTextureValid(internalTarget)) {
        CloseWindow();
        return 1;
    }

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(DrawGameFrame, 0, 1);
#else
    while (!WindowShouldClose()) {
        DrawGameFrame();
    }

    UnloadRenderTexture(internalTarget);
    CloseWindow();
#endif

    return 0;
}
