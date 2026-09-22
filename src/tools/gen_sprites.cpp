#include "raylib.h"
#include "sprites.h"

#include <cstdio>
#include <cstring>
#include <string>

#ifdef _WIN32
#include <direct.h>
#define MKDIR(p) _mkdir(p)
#else
#include <sys/stat.h>
#define MKDIR(p) mkdir(p, 0755)
#endif

static void Preview(const char* name, const sprites::ArtDef& def) {
    printf("== %s : %dx%d x %d frames ==\n", name, def.w, def.h, (int)def.frames.size());
    for (size_t f = 0; f < def.frames.size(); ++f) {
        printf("-- frame %zu (%d rows) --\n", f, (int)def.frames[f].size());
        for (size_t y = 0; y < def.frames[f].size(); ++y) {
            const std::string& row = def.frames[f][y];
            printf("|%s|%d\n", row.c_str(), (int)row.size());
        }
    }
    printf("\n");
}

static bool MakeDirs() {
    if (MKDIR("assets") != 0 && !DirectoryExists("assets")) return false;
    if (MKDIR("assets/sprites") != 0 && !DirectoryExists("assets/sprites")) return false;
    return true;
}

int main(int argc, char** argv) {
    bool preview = argc > 1 && strcmp(argv[1], "--preview") == 0;

    if (preview) {
        Preview("WARRIOR", sprites::WarriorArt());
        Preview("SLIME", sprites::SlimeArt());
        Preview("BAT", sprites::BatArt());
        sprites::ValidateLength(sprites::WarriorArt(), "warrior");
        sprites::ValidateLength(sprites::SlimeArt(), "slime");
        sprites::ValidateLength(sprites::BatArt(), "bat");
        return 0;
    }

    if (!MakeDirs()) {
        printf("ERROR: cannot create assets/sprites\n");
        return 1;
    }

    bool ok = true;
    ok &= sprites::ExportSheet("assets/sprites/warrior.png", sprites::WarriorArt(), sprites::WARRIOR_PAL);
    ok &= sprites::ExportSheet("assets/sprites/slime.png", sprites::SlimeArt(), sprites::SLIME_PAL);
    ok &= sprites::ExportSheet("assets/sprites/bat.png", sprites::BatArt(), sprites::BAT_PAL);
    printf(ok ? "OK: PNG exportado en assets/sprites/\n" : "ERROR al exportar PNG\n");
    return ok ? 0 : 1;
}