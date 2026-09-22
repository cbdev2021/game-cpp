#ifndef SPRITES_H
#define SPRITES_H

#include "raylib.h"
#include <map>
#include <string>
#include <vector>

namespace sprites {

using Palette = std::map<char, Color>;

struct Sheet {
    Texture2D tex;
    int frameW = 0;
    int frameH = 0;
    int frameCount = 0;

    bool valid() const {
        return frameCount > 0 && IsTextureValid(tex);
    }
};

struct ArtDef {
    int w = 0;
    int h = 0;
    std::vector<std::vector<std::string>> frames;
};

static const Palette WARRIOR_PAL = {
    {'.', {0, 0, 0, 0}},
    {'O', {36, 24, 18, 255}},
    {'k', {238, 195, 154, 255}},
    {'K', {213, 154, 104, 255}},
    {'D', {168, 110, 66, 255}},
    {'h', {74, 46, 22, 255}},
    {'H', {107, 69, 36, 255}},
    {'s', {200, 210, 220, 255}},
    {'S', {138, 148, 160, 255}},
    {'w', {232, 240, 248, 255}},
    {'W', {180, 188, 200, 255}},
    {'g', {216, 168, 56, 255}},
    {'G', {156, 110, 28, 255}},
    {'r', {192, 58, 46, 255}},
    {'R', {124, 32, 24, 255}},
    {'p', {122, 82, 38, 255}},
    {'P', {84, 54, 26, 255}},
    {'b', {74, 50, 32, 255}},
    {'B', {216, 192, 96, 255}},
    {'l', {106, 78, 46, 255}},
    {'L', {80, 52, 28, 255}},
    {'e', {30, 26, 42, 255}}
};

static const Palette SLIME_PAL = {
    {'.', {0, 0, 0, 0}},
    {'O', {20, 44, 22, 255}},
    {'g', {58, 158, 66, 255}},
    {'G', {82, 188, 88, 255}},
    {'U', {42, 118, 48, 255}},
    {'w', {220, 235, 190, 255}},
    {'e', {20, 40, 22, 255}},
    {'C', {40, 92, 44, 255}}
};

static const Palette BAT_PAL = {
    {'.', {0, 0, 0, 0}},
    {'O', {22, 16, 34, 255}},
    {'m', {74, 58, 98, 255}},
    {'M', {100, 80, 130, 255}},
    {'U', {52, 40, 70, 255}},
    {'d', {36, 26, 48, 255}},
    {'e', {232, 48, 40, 255}},
    {'E', {255, 100, 70, 255}},
    {'w', {236, 226, 214, 255}}
};

static std::vector<std::string> Join(const std::vector<std::string>& a,
                                     const std::vector<std::string>& b) {
    std::vector<std::string> out = a;
    out.insert(out.end(), b.begin(), b.end());
    return out;
}

static const std::vector<std::string> WAR_HEAD = {
    ".......OHHHHHO.......",
    "......OhhhhhhoO......",
    ".....OhhhhhhhhoO.....",
    "....OOhhhhhhhhoOO....",
    "....OHgGGGGGGgoO.....",
    "....OHOkkkkkkoOO.....",
    ".......OkkkkeOkO.......",
    "......OOkkkkkkoO......"
};

static const std::vector<std::string> WAR_TORSO = {
    ".......Okkkkkko.......",
    "........OkkO..........",
    "...OOOkkkkkkkkkOOO...",
    "...OkOkkkkkkkkOssO....",
    "..OkOpkkkkkkkpOsssO...",
    "..OkOkpkkkkkkOssssO...",
    "..OkOkOkkkkkkOsssO....",
    "..OkOkOkkkkkkOssO.....",
    "..OOOOkkkkkkOOO.......",
    "......OkkkkkO.........",
    "......ObbbbbboO.......",
    "......ObbBbbbO........"
};

static const std::vector<std::string> WAR_TORSO_STRIDE_OUT = {
    ".......Okkkkkko.......",
    "........OkkO..........",
    "...OOOkkkkkkkkkOOO...",
    "..OkOkOkkkkkkkOsssO...",
    "..OkOkpkkkkkkkpOwwsO..",
    "..OkOkpkkkkkkOwwWSsO..",
    "..OkOkOkkkkkkOwwWWo...",
    "..OkOkOkkkkkkOwWWWO...",
    "..OOOOkkkkkkOOOWWWW...",
    "......OkkkkkOwwW......",
    "......ObbbbbOwwWW.....",
    "......ObbBbbbOwwWW...."
};

static const std::vector<std::string> WAR_TORSO_STRIDE_IN = {
    ".......Okkkkkko.......",
    "........OkkO..........",
    "...OOOkkkkkkkkkOOO...",
    "..OkOkOkkkkkkkOsssO...",
    "..OkOkpkkkkkkkpOsssO..",
    "..OkOkpkkkkkkpOswwwO..",
    "..OkOkOkkkkkkOswwwWO..",
    "..OkOkOkkkkkkOswWWo...",
    "..OOOOkkkkkkOOwwWW....",
    "......OkkkkkOwwWW.....",
    "......ObbbbbOwwWW.....",
    "......ObbBbbbOwwWW...."
};

static const std::vector<std::string> WAR_TORSO_PASS = {
    ".......Okkkkkko.......",
    "........OkkO..........",
    "...OOOkkkkkkkkkOOO...",
    "..OkO.OkOkkkkkkOssO...",
    "..OkO.OkpkkkkkkOssO...",
    "..OOO.OkpkkkkkkOssO...",
    "......OkOkkkkkkOssO...",
    "......OkOkkkkkkOssO...",
    "......OkOkkkkkkOOO....",
    "......OkkkkkkO........",
    "......ObbbbbbO........",
    "......ObbBbbbO........"
};

static const std::vector<std::string> WAR_HIP = {
    "......OllllllO........",
    "......OllllllO........",
    "......OlllrllO........",
    "........OOOOO........."
};

static const std::vector<std::string> WAR_LEG_IDLE = {
    "......OkkO.OkoO.......",
    "......OkkO.OkoO.......",
    "......OkkO.OkoO.......",
    "......OllO.OloO.......",
    "......OllO.OloO.......",
    "....OllllO.OllllO.....",
    "....OLLLLO.OLLLLO.....",
    "....OLLLLO.OLLLLO.....",
    "....OLLLLO.OLLLLlO....",
    "....OOOOO...OOOOOO...."
};

static const std::vector<std::string> WAR_LEG_RUN0 = {
    ".......OkO..OkkO.......",
    ".......OkO..OHkkO......",
    "......OllO..OllO.......",
    "......OllO..OlloO......",
    ".....OllllO.OlloO......",
    "....OllllO..OlloO......",
    "...OLLLLlO..OllloO.....",
    "..OLLLLlO....OLLLLO....",
    ".OLLLO.........OLLLLlO..",
    ".OOOO...........OOOOOO.."
};

static const std::vector<std::string> WAR_LEG_RUN1 = {
    "......OkkO..OkkO.......",
    "......OllO..OllO.......",
    ".....OlllO..OlllO......",
    ".....OlllO..OlllO......",
    "....OllllO..OllllO.....",
    "....OLLLLO..OLLLLO.....",
    "...OLLLLlO..OLLLLlO....",
    "..OLLLLlO....OLLLLlO...",
    ".OLLL............OLLLLO.",
    ".OOOO...........OOOOOO."
};

static const std::vector<std::string> WAR_LEG_RUN2 = {
    ".......OkkO..OkkO.......",
    ".......OkkO..OkkO.......",
    ".......OllO..OlloO......",
    ".......OllO..OlloO......",
    "......OlllO.OllllO......",
    ".....OllllO.OllllO......",
    ".....OLLLLO.OLLLLlO.....",
    "....OLLLLlO..OLLLLlO....",
    "...OLLLLlO....OLLLLlO...",
    "..OOOOO........OOOOOOO.."
};

static const std::vector<std::string> WAR_LEG_RUN3 = {
    "......OkkO...OkkO.......",
    "......OllO...OllO.......",
    "......OlllO..OlllO......",
    ".....OllllO..OllllO.....",
    ".....OllllO..OllllO.....",
    "....OLLLLO...OLLLLO.....",
    "...OLLLLlO...OLLLLlO....",
    "....OLLLLlO...OLLLLlO...",
    "....OLLLLlO....OLLLLlO..",
    "....OOOOOO.....OOOOOOO.."
};

static const std::vector<std::string> WAR_LEG_JUMP = {
    "......OkkkO.OkkkO.....",
    "......OkkkO.OkkkO.....",
    ".....OllllO.OllllO....",
    ".....OlllO..OlllO.....",
    ".....OOkO...OOkO......",
    ".....OLLO...OLLO......",
    ".....OLLL....OLLL.....",
    "....OLLLL....OLLLL....",
    "....OOOOO....OOOOO....",
    "........................"
};

static const std::vector<std::string> WAR_LEG_FALL = {
    "......OkkO..OkkO.......",
    "......OkkO..OkkO.......",
    "......Ollo..OlloO......",
    "......Ollo..OlloO......",
    ".....OlllO..OlllO......",
    ".....OlllO..OlllO......",
    "....OLLLLlO.OLLLLlO....",
    "....OLLLLlO.OLLLLlO....",
    "....OLLLLlO.OLLLLlO....",
    "....OOOOOO..OOOOOO....."
};

static const std::vector<std::string> WAR_ATK0 = {
    ".......Okkkkkko....w....",
    "........OkkO......wW....",
    "...OOOkkkkkkkkkOOOwwW...",
    "...OkOkkkkkkkkOssOWwW...",
    "..OkOpkkpkkkkkpOss.....",
    "..OkO..OkpkkkkkOssssO..",
    "..OkO..OkOkkkkkOsssO...",
    "..OkO..OkOkkkkkOssO....",
    "..OOOOOkOkkkkkOOO......",
    "......OkOkkkkkO........",
    "......OOObbbbbO........",
    "..........ObbBbbO......"
};

static const std::vector<std::string> WAR_ATK1 = {
    ".......Okkkkkko.......",
    "........OkkO......w...",
    "...OOOkkkkkkkkkOOO.wW.",
    "...OkOkkkkkkkkOssOwwW.",
    "..OkOpkkpkkkkkpOssO...",
    "..OkO.kkpkkkkkkOssO...",
    "..OkO..kOkkkkkkOssO...",
    "..OkO..kOkkkkkkOssO...",
    "..OOO..kOkkkkkOOO.....",
    "......OkOkkkkkO.......",
    "......OObbbbbO........",
    "........ObbBbbbO......"
};

static const std::vector<std::string> WAR_ATK2 = {
    ".......Okkkkkko.......",
    "........OkkO..........",
    "...OOOkkkkkkkkkOOO...",
    "...OkOkkkkkkkkOssO....",
    "..OkOpkkpkkkkkpOssO...",
    "..OkO.kkpkkkkkpOss.w..",
    "..OkO..kOkkkkkOsss.wW.",
    "..OkO..kOkkkkkOss.wWW.",
    "..OOO..kOkkkkkOO..wwW.",
    "......kkOkkkkkO...wwwW",
    "......OObbbbbbO...wwW.",
    "........ObbBbbbO...wW."
};

static const std::vector<std::string> WAR_CT = {
    ".......Okkkkkko.......",
    "........OkkO..........",
    "...OOOkkkkkkkkkOOO...",
    "...OkOkkkkkkkkOssO....",
    "..OkOkkpkkkkkkpOssO...",
    "..OkOkkpkkkkkkOwwsO...",
    "..OkO.OkkkkkkOwwWO....",
    "..OkO.OkkkkkkOwWWo....",
    "..OOO.kkkkkkkOwwW.....",
    "......OkkkkkkOwwW.....",
    "......ObbbbbbOwwW.....",
    "......ObbBBbbOwwW....."
};

static const std::vector<std::string> WAR_CTA = {
    ".......Okkkkkko.......",
    "........OkkO..........",
    "...OOOkkkkkkkkkOOO...",
    "...OkOkkkkkkkkOssO....",
    "..OkOkpkkkkkkpOsswwwW.",
    "..OkOkpkkkkkkpOsswwWW.",
    "..OkO.OkkkkkkOss.wW...",
    "..OkO.OkkkkkkOss..W...",
    "..OOO.kkkkkkkOOO......",
    "......OkkkkkkO........",
    "......ObbbbbbO........",
    "......ObbBBbbO........"
};

static const std::vector<std::string> WAR_CL = {
    ".......OkO..OkoO.......",
    "......OlloO.OlloO......",
    "......OlloO.OlloO......",
    ".....OLLLLO.OLLLLO.....",
    ".....OLLLLO.OLLLLO.....",
    "....OLLLLLO.OLLLLLO....",
    "....OOOOOOO.OOOOOOO...."
};

static const std::vector<std::string> WAR_SWORD_TOP = {
    "...............k........",
    "..............OkkO......",
    "..............GkcG......",
    "...............kww......",
    "...............OwwW.....",
    "..............OwwWW....."
};

static const std::vector<std::string> WAR_SWORD_HIP = {
    "..............wwwwW.....",
    "...............wwW......",
    "..................W......",
    "........................"
};

static void MergeOverlay(std::vector<std::string>& target, size_t startRow,
                         const std::vector<std::string>& overlay) {
    for (size_t i = 0; i < overlay.size() && startRow + i < target.size(); ++i) {
        std::string& dst = target[startRow + i];
        const std::string& src = overlay[i];
        for (size_t x = 0; x < src.size() && x < dst.size(); ++x) {
            if (src[x] != '.') dst[x] = src[x];
        }
    }
}

static std::vector<std::string> WarTorso(bool swordOverlay) {
    if (!swordOverlay) return WAR_TORSO;
    std::vector<std::string> t = WAR_TORSO;
    MergeOverlay(t, 6, WAR_SWORD_TOP);
    return t;
}

static std::vector<std::string> WarHip(bool swordOverlay) {
    if (!swordOverlay) return WAR_HIP;
    std::vector<std::string> h = WAR_HIP;
    MergeOverlay(h, 0, WAR_SWORD_HIP);
    return h;
}

static std::vector<std::string> EmptyRows(int n) {
    std::vector<std::string> out;
    for (int i = 0; i < n; ++i) out.emplace_back(24, '.');
    return out;
}

static ArtDef WarriorArt() {
    ArtDef def;
    def.w = 24;
    def.h = 34;
    const std::vector<std::string> idleT = WarTorso(true);
    const std::vector<std::string> idleH = WarHip(true);
    def.frames.push_back(Join(Join(WAR_HEAD, idleT), Join(idleH, WAR_LEG_IDLE)));
    def.frames.push_back(Join(Join(WAR_HEAD, idleT), Join(idleH, WAR_LEG_RUN1)));
    def.frames.push_back(Join(Join(WAR_HEAD, WAR_TORSO_STRIDE_OUT), Join(WAR_HIP, WAR_LEG_RUN0)));
    def.frames.push_back(Join(Join(WAR_HEAD, WAR_TORSO_PASS), Join(WAR_HIP, WAR_LEG_RUN1)));
    def.frames.push_back(Join(Join(WAR_HEAD, WAR_TORSO_STRIDE_IN), Join(WAR_HIP, WAR_LEG_RUN2)));
    def.frames.push_back(Join(Join(WAR_HEAD, WAR_TORSO_PASS), Join(WAR_HIP, WAR_LEG_RUN3)));
    def.frames.push_back(Join(Join(WAR_HEAD, WAR_TORSO_PASS), Join(WAR_HIP, WAR_LEG_JUMP)));
    def.frames.push_back(Join(Join(WAR_HEAD, WAR_TORSO_PASS), Join(WAR_HIP, WAR_LEG_FALL)));
    def.frames.push_back(Join(Join(WAR_HEAD, WAR_ATK0), Join(WAR_HIP, WAR_LEG_IDLE)));
    def.frames.push_back(Join(Join(WAR_HEAD, WAR_ATK1), Join(WAR_HIP, WAR_LEG_IDLE)));
    def.frames.push_back(Join(Join(WAR_HEAD, WAR_ATK2), Join(WAR_HIP, WAR_LEG_IDLE)));
    def.frames.push_back(Join(EmptyRows(3), Join(WAR_HEAD, Join(WAR_CT, Join(WAR_HIP, WAR_CL)))));
    def.frames.push_back(Join(EmptyRows(3), Join(WAR_HEAD, Join(WAR_CTA, Join(WAR_HIP, WAR_CL)))));
    return def;
}

static const std::vector<std::string> SLIME_F0 = {
    ".........OOO...........",
    "......OggggggggO.......",
    ".....OggggggggggO......",
    "....OggwwggggggggO.....",
    "....OgggggGGgggggO.....",
    "...OggggOeeOOgggggO....",
    "...OggggOeeOggggggO....",
    "...OgggggOOgggggggO....",
    "....OggggOCCgggggO.....",
    "....OggggOCCCgggggO....",
    "....OgggggCCCgggggO....",
    ".....OggggCCgggggO.....",
    ".....OgggggggggggO.....",
    "......OgggggggggO......",
    "......OgggggggggO......",
    ".......OgggggggO.......",
    ".......OgggggggO.......",
    "........OOOOOOO........",
    "........................",
    "........................"
};

static const std::vector<std::string> SLIME_F1 = {
    ".........OOO...........",
    "......OggggggggO.......",
    ".....OggggggggggO......",
    "....OggwwggggggggO.....",
    "....OggggOgggggggO.....",
    "...OggggOeeOOgggggO....",
    "...OggggOeeOggggggO....",
    "...OgggggOggggggggO....",
    "....OggggOCCggggggO....",
    "....OggggOCCCgggggO....",
    ".....OgggggCCCggggO....",
    ".....OggggCCgggggO.....",
    "......OggggggggggO.....",
    "......OggggggggggO.....",
    ".......OggggggggO......",
    ".......OggggggggO......",
    "........OOOOOOOOO......",
    "........................",
    "........................",
    "........................"
};

static const std::vector<std::string> SLIME_F2 = {
    ".........OOO...........",
    "......OggggggggO.......",
    ".....OggggggggggO......",
    "....OggwwggggggggO.....",
    "....OgggggGGgggggO.....",
    "...OggggOeeOOgggggO....",
    "...OggggOeeOggggggO....",
    "...OgggggOOgggggggO....",
    "....OggggOCCgggggO.....",
    "....OggggOCCCgggggO....",
    "....OgggggCCCgggggO....",
    ".....OggggCCgggggO.....",
    ".....OgggggggggggO.....",
    "......OgggggggggO......",
    "......OgggggggggO......",
    ".......OgggggggO.......",
    ".......OgggggggO.......",
    "........OOOOOOO........",
    "........................",
    "........................"
};

static const std::vector<std::string> SLIME_F3 = {
    ".........OOO...........",
    "......OggggggggO.......",
    ".....OggggggggggO......",
    "....OggwwggggggggO.....",
    "....OgggggGGgggggO.....",
    "...OggggOeeOOgggggO....",
    "...OggggOeeOggggggO....",
    "...OgggggOOgggggggO....",
    "....OggggOCCgggggO.....",
    "....OggggOCCCgggggO....",
    "....OgggggCCCgggggO....",
    ".....OggggCCgggggO.....",
    "....OggggggggggggO.....",
    "....OggggggggggggO.....",
    ".....OgggggggggggO.....",
    ".....OgggggggggggO.....",
    "......OggggggggggO.....",
    "......OOOOOOOOOOO......",
    "........................",
    "........................"
};

static ArtDef SlimeArt() {
    ArtDef def;
    def.w = 24;
    def.h = 20;
    def.frames.push_back(SLIME_F0);
    def.frames.push_back(SLIME_F1);
    def.frames.push_back(SLIME_F2);
    def.frames.push_back(SLIME_F3);
    return def;
}

static std::vector<std::string> MergeRows(std::vector<std::string> base,
                                          const std::vector<std::string>& top) {
    for (size_t y = 0; y < top.size() && y < base.size(); ++y) {
        for (size_t x = 0; x < top[y].size() && x < base[y].size(); ++x) {
            if (top[y][x] != '.') base[y][x] = top[y][x];
        }
    }
    return base;
}

static const std::vector<std::string> BAT_BODY = {
    "........................",
    "........................",
    "........................",
    "........................",
    "........................",
    ".......OMMMMMMO.........",
    "......OMMMMMMMO.........",
    "......OMMeeMMMO.........",
    "......OMMeeMMMO.........",
    "......OMMMMMMMO.........",
    ".......OMMwwMO..........",
    ".......OOMMMMO..........",
    "........OMMMmO..........",
    ".........OMMmO..........",
    "...........OO...........",
    "........................",
    "........................",
    "........................",
    "........................",
    "........................"
};

static const std::vector<std::string> BAT_WINGS_UP = {
    "O..................OO....",
    "OO................OOOO...",
    "OmmO............OOOOOO...",
    ".OmmmO........OOOOOOOO...",
    "..OmmmmO.....OOOOOOOO....",
    "...OmmmmmO..OOOOOOOO.....",
    "....OmmmmmmmOOOOOOO......",
    ".....OmmmmmmmmmmmmmO.....",
    "......OmmmmmmmmmmmmO.....",
    ".......OmmmmmmmmmmO......",
    "........OmmmmmmmmO.......",
    ".........OmmmmmmO........",
    "..........OmmmmO.........",
    "...........OmmO..........",
    "............OO...........",
    "..........................",
    "..........................",
    "..........................",
    "..........................",
    ".........................."
};

static const std::vector<std::string> BAT_WINGS_MID = {
    "..........................",
    "..O....................OO.",
    ".OO..................OOOO.",
    ".OmO..............OOOOOO..",
    "OmmO............OOOOOO....",
    "OmmmO.........OOOOOOO.....",
    ".OmmmO......OOOOOOOO......",
    "..OmmmO...OOOOOOOO........",
    "...OmmmOO.OOOOOOO..........",
    "....OmmmmmmmmmmmmmO........",
    ".....OmmmmmmmmmmmO.........",
    "......OmmmmmmmmmO..........",
    ".......OmmmmmmmO...........",
    "........OmmmmmO............",
    ".........OmmmO.............",
    "..........OmO..............",
    "..........................",
    "..........................",
    "..........................",
    ".........................."
};

static const std::vector<std::string> BAT_WINGS_DOWN = {
    "..........................",
    "..........................",
    "..........................",
    "O......................OO.",
    "OO....................OOOO.",
    "OmO.................OOOOOO.",
    "OmmO...............OOOOOO..",
    ".OmmO.............OOOOOO...",
    "..OmmmO.........OOOOOOO....",
    "...OmmmO.......OOOOOOO.....",
    "....OmmmO.....OOOOOOO......",
    ".....OmmmO...OOOOOOO.......",
    "......OmmmO.OOOOOOO........",
    ".......OmmmmmmmmmmO........",
    "........OmmmmmmmmmO........",
    ".........OmmmmmmmO.........",
    "..........OmmOmmO..........",
    "...........OO..OO..........",
    "..........................",
    ".........................."
};

static ArtDef BatArt() {
    ArtDef def;
    def.w = 24;
    def.h = 20;
    def.frames.push_back(MergeRows(BAT_WINGS_UP, BAT_BODY));
    def.frames.push_back(MergeRows(BAT_WINGS_MID, BAT_BODY));
    def.frames.push_back(MergeRows(BAT_WINGS_DOWN, BAT_BODY));
    return def;
}

[[maybe_unused]] static void ValidateLength(const ArtDef& def, const char* name) {
    for (size_t f = 0; f < def.frames.size(); ++f) {
        for (size_t y = 0; y < def.frames[f].size(); ++y) {
            if (def.frames[f][y].size() != (size_t)def.w) {
                TraceLog(LOG_WARNING, "sprites: %s frame %d row %d has %d cols (want %d)",
                         name, (int)f, (int)y, (int)def.frames[f][y].size(), def.w);
            }
        }
        if (def.frames[f].size() != (size_t)def.h) {
            TraceLog(LOG_WARNING, "sprites: %s frame %d has %d rows (want %d)",
                     name, (int)f, (int)def.frames[f].size(), def.h);
        }
    }
}

static Image MakeSheetImage(const ArtDef& def, const Palette& pal) {
    const int count = (int)def.frames.size();
    Image img = GenImageColor(def.w * count, def.h, (Color){0, 0, 0, 0});
    for (int f = 0; f < count; ++f) {
        for (int y = 0; y < def.h; ++y) {
            const std::string& row = def.frames[f][y];
            for (int x = 0; x < def.w; ++x) {
                const char c = x < (int)row.size() ? row[x] : '.';
                if (c == '.') continue;
                auto it = pal.find(c);
                if (it != pal.end()) ImageDrawPixel(&img, f * def.w + x, y, it->second);
            }
        }
    }
    return img;
}

[[maybe_unused]] static Sheet LoadSheetFromImage(const ArtDef& def, const Palette& pal, bool pointFilter) {
    Sheet sheet;
    Image img = MakeSheetImage(def, pal);
    sheet.tex = LoadTextureFromImage(img);
    sheet.frameW = def.w;
    sheet.frameH = def.h;
    sheet.frameCount = (int)def.frames.size();
    UnloadImage(img);
    if (pointFilter) SetTextureFilter(sheet.tex, TEXTURE_FILTER_POINT);
    return sheet;
}

[[maybe_unused]] static bool ExportSheet(const char* path, const ArtDef& def, const Palette& pal) {
    Image img = MakeSheetImage(def, pal);
    bool ok = ExportImage(img, path);
    UnloadImage(img);
    return ok;
}

} // namespace sprites

#endif