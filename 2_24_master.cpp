#include <graphics.h>
#include <conio.h>
#include <windows.h>
#include <mmsystem.h>
#include <ctime>
#include <cstdlib>
#include <string>
#include <cmath>

#pragma execution_character_set("utf-8")
#pragma comment(lib, "winmm.lib")

using std::wstring;

int SCREEN_W = 1600;
int SCREEN_H = 900;

int ARENA_L = 90;
int ARENA_T = 130;
int ARENA_R = 1510;
int ARENA_B = 760;

const float PI_F = 3.1415926f;
const wchar_t* FONT_NAME = L"Microsoft YaHei";

const int CARD_W = 230;
const int CARD_H = 360;
int CARD_X[3] = { 35, 385, 735 };
int CARD_Y = 150;

enum SceneType {
    SCENE_MENU,
    SCENE_MAP_SELECT,
    SCENE_SELECT_EGG,
    SCENE_INSPECT,
    SCENE_BATTLE,
    SCENE_RESULT
};

enum ArenaMode {
    ARENA_BASIC = 0,
    ARENA_ADVANCED = 1,
    ARENA_NIGHTMARE = 2
};

enum ContactRegion {
    REGION_SHARP = 0,
    REGION_BLUNT = 1,
    REGION_SIDE = 2
};

struct Button {
    int left;
    int top;
    int right;
    int bottom;
    const wchar_t* text;
};

struct Vec2 {
    float x;
    float y;
};

struct EggRuntimeState {
    int shapeType;
    int toneType;
    int surfaceType;
    int endBias;
    int thicknessType;
    int symmetryType;

    bool hiddenCrack;
    int hiddenCrackSide;

    int sharpAttack;
    int sharpDefense;
    int sharpHP;
    int sharpMaxHP;

    int bluntAttack;
    int bluntDefense;
    int bluntHP;
    int bluntMaxHP;

    wstring clues[6];
    int clueCount;
};

struct EggEntity {
    EggRuntimeState draft;

    Vec2 pos;
    Vec2 vel;

    float angle;
    float radius;

    float stamina;
    float maxStamina;
    bool boosting;

    int sharpHP;
    int sharpMaxHP;

    int bluntHP;
    int bluntMaxHP;

    int sideHP;
    int sideMaxHP;

    int totalIntegrity;
    int totalMaxIntegrity;

    int collisionCooldown;
    int wallCooldown;
    int hitFlashFrames;
};

struct ArenaObstacle {
    bool active;
    Vec2 pos;
    float radius;
};


enum SpikeState {
    SPIKE_IDLE = 0,
    SPIKE_WARNING = 1,
    SPIKE_FALLING = 2,
    SPIKE_LINGER = 3
};

struct FallingSpike {
    SpikeState state;
    float x;
    float y;
    float speed;
    int framesLeft;
    int damage;
    bool hitLeft;
    bool hitRight;
    float halfWidth;
    float height;
};

struct HealthPack {
    bool active;
    bool tiedToSpike;
    int tiedSpikeIndex;
    Vec2 pos;
    float radius;
    int healPoints;
};

struct SkillPack {
    bool active;
    Vec2 pos;
    float radius;
};

struct YolkProjectile {
    bool active;
    int owner; // 0 left, 1 right
    Vec2 pos;
    Vec2 vel;
    float radiusX;
    float radiusY;
    int damage;
    int lifeFrames;
};

const int MAX_OBSTACLES = 4;
const int MAX_SPIKES = 2;
const int MAX_HEALTH_PACKS = 3;

ArenaMode gArenaMode = ARENA_BASIC;
ArenaObstacle gObstacles[MAX_OBSTACLES]{};
int gObstacleCount = 0;

FallingSpike gSpikes[MAX_SPIKES]{};
int gSpikeCount = 0;
int gNextSpikeFrames = 0;
int gSpikeDamage = 2;

HealthPack gHealthPacks[MAX_HEALTH_PACKS]{};
int gNextHealthPackFrames = 0;

SkillPack gSkillPack{};
int gNextSkillPackFrames = 0;
bool gLeftHasSkillShot = false;
bool gRightHasSkillShot = false;

YolkProjectile gYolkShots[2]{};
bool gLeftSkillKeyHeld = false;
bool gRightSkillKeyHeld = false;

enum AIState {
    AI_PATROL = 0,
    AI_ATTACK = 1,
    AI_RECOVER = 2,
    AI_AVOID = 3
};

AIState gAIState = AI_PATROL;
int gAINextAttackFrames = 420;
int gAIAttackFrames = 0;
int gAIRecoverFrames = 0;
int gAIAvoidFrames = 0;
int gAIOrbitFrames = 180;
float gAIOrbitBias = 1.0f;


int randInt(int l, int r) {
    return l + rand() % (r - l + 1);
}

float clampFloat(float x, float l, float r) {
    if (x < l) return l;
    if (x > r) return r;
    return x;
}

int clampMin(int x, int mn) {
    return x < mn ? mn : x;
}

wstring toW(int x) {
    return std::to_wstring(x);
}

Vec2 makeVec(float x, float y) {
    Vec2 v;
    v.x = x;
    v.y = y;
    return v;
}

Vec2 addVec(Vec2 a, Vec2 b) {
    return makeVec(a.x + b.x, a.y + b.y);
}

Vec2 subVec(Vec2 a, Vec2 b) {
    return makeVec(a.x - b.x, a.y - b.y);
}

Vec2 mulVec(Vec2 a, float k) {
    return makeVec(a.x * k, a.y * k);
}

float dotVec(Vec2 a, Vec2 b) {
    return a.x * b.x + a.y * b.y;
}

float lenVec(Vec2 a) {
    return sqrtf(a.x * a.x + a.y * a.y);
}

Vec2 normVec(Vec2 a) {
    float l = lenVec(a);
    if (l < 0.0001f) return makeVec(0.0f, 0.0f);
    return makeVec(a.x / l, a.y / l);
}

float normalizeAngle(float a) {
    while (a > PI_F) a -= 2.0f * PI_F;
    while (a < -PI_F) a += 2.0f * PI_F;
    return a;
}

float angleDiff(float target, float current) {
    return normalizeAngle(target - current);
}

Vec2 rotateLocal(Vec2 p, float ang) {
    float c = cosf(ang);
    float s = sinf(ang);
    return makeVec(p.x * c - p.y * s, p.x * s + p.y * c);
}

Vec2 worldFromLocalScaled(const EggEntity& e, Vec2 local, float scale) {
    return addVec(e.pos, rotateLocal(mulVec(local, scale), e.angle));
}

bool isInside(const Button& btn, int x, int y) {
    return x >= btn.left && x <= btn.right &&
        y >= btn.top && y <= btn.bottom;
}

void setUIFont(int size) {
    settextstyle(size, 0, FONT_NAME);
}

void setUIFontAndColor(int size, COLORREF color) {
    setbkmode(TRANSPARENT);
    settextcolor(color);
    setUIFont(size);
}

wstring fitTextToWidth(const wstring& s, int maxWidth) {
    if (textwidth(s.c_str()) <= maxWidth) return s;

    wstring dots = L"...";
    int dotsW = textwidth(dots.c_str());
    wstring res;

    for (size_t i = 0; i < s.size(); ++i) {
        wstring tmp = res + s[i];
        if (textwidth(tmp.c_str()) + dotsW > maxWidth) break;
        res += s[i];
    }
    return res + dots;
}


void updateLayoutMetrics() {
    SCREEN_W = GetSystemMetrics(SM_CXSCREEN);
    SCREEN_H = GetSystemMetrics(SM_CYSCREEN);

    ARENA_L = 90;
    ARENA_T = 140;
    ARENA_R = SCREEN_W - 90;
    ARENA_B = SCREEN_H - 150;

    int totalCardsW = CARD_W * 3;
    int gap = (SCREEN_W - totalCardsW) / 4;
    if (gap < 30) gap = 30;
    CARD_X[0] = gap;
    CARD_X[1] = CARD_X[0] + CARD_W + gap;
    CARD_X[2] = CARD_X[1] + CARD_W + gap;
    CARD_Y = 160;
}

void makeFullscreenWindow() {
    HWND hwnd = GetHWnd();
    LONG style = GetWindowLong(hwnd, GWL_STYLE);
    style &= ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU);
    SetWindowLong(hwnd, GWL_STYLE, style);
    SetWindowPos(hwnd, HWND_TOP, 0, 0, SCREEN_W, SCREEN_H, SWP_FRAMECHANGED | SWP_SHOWWINDOW);
}

const wchar_t* getArenaModeText(ArenaMode mode) {
    if (mode == ARENA_ADVANCED) return L"进阶模式";
    if (mode == ARENA_NIGHTMARE) return L"噩梦模式";
    return L"基础模式";
}

bool isNightmareMode() {
    return gArenaMode == ARENA_NIGHTMARE;
}

bool isAdvancedLikeMode() {
    return gArenaMode == ARENA_ADVANCED || gArenaMode == ARENA_NIGHTMARE;
}


int getTargetObstacleCount() {
    if (gArenaMode == ARENA_BASIC) return 2;
    if (gArenaMode == ARENA_ADVANCED) return 4;
    return 4;
}

int getTargetSpikeCount() {
    if (gArenaMode == ARENA_ADVANCED) return 1;
    if (gArenaMode == ARENA_NIGHTMARE) return 2;
    return 0;
}

int getSpikeIntervalMinFrames() {
    if (gArenaMode == ARENA_NIGHTMARE) return 160;
    if (gArenaMode == ARENA_ADVANCED) return 240;
    return 999999;
}

int getSpikeIntervalMaxFrames() {
    if (gArenaMode == ARENA_NIGHTMARE) return 320;
    if (gArenaMode == ARENA_ADVANCED) return 480;
    return 999999;
}

float getSpikeScale() {
    if (gArenaMode == ARENA_NIGHTMARE) return 3.0f;
    if (gArenaMode == ARENA_ADVANCED) return 2.0f;
    return 1.0f;
}

float getAIStrengthFactor() {
    if (gArenaMode == ARENA_NIGHTMARE) return 2.0f;
    if (gArenaMode == ARENA_ADVANCED) return 1.0f;
    return 0.0f;
}

bool isSpikeMode() {
    return getTargetSpikeCount() > 0;
}


int computeSpikeDamageFromDrafts(const EggRuntimeState& a, const EggRuntimeState& b) {
    int maxAtk = a.sharpAttack;
    if (a.bluntAttack > maxAtk) maxAtk = a.bluntAttack;
    if (b.sharpAttack > maxAtk) maxAtk = b.sharpAttack;
    if (b.bluntAttack > maxAtk) maxAtk = b.bluntAttack;

    if (maxAtk >= 5) return 3;
    if (maxAtk >= 3) return 2;
    return 1;
}

bool fileExistsW(const wchar_t* path) {
    DWORD attr = GetFileAttributesW(path);
    return attr != INVALID_FILE_ATTRIBUTES && !(attr & FILE_ATTRIBUTE_DIRECTORY);
}

void playEffect(const wchar_t* path) {
    if (fileExistsW(path)) {
        PlaySoundW(path, NULL, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);
    }
}

void stopBGM() {
    mciSendStringW(L"stop bgm", NULL, 0, NULL);
    mciSendStringW(L"close bgm", NULL, 0, NULL);
}

void startBGM() {
    stopBGM();
    if (fileExistsW(L"bgm.wav")) {
        mciSendStringW(L"open \"bgm.wav\" type waveaudio alias bgm", NULL, 0, NULL);
        mciSendStringW(L"play bgm repeat", NULL, 0, NULL);
    }
}

float nowTimeSec() {
    return GetTickCount() * 0.001f;
}

float idleBob(float phase, float amp) {
    return sinf(nowTimeSec() * 2.2f + phase) * amp;
}

void resetAIState() {
    gAIState = AI_PATROL;

    if (gArenaMode == ARENA_NIGHTMARE) gAINextAttackFrames = randInt(150, 240);
    else if (gArenaMode == ARENA_ADVANCED) gAINextAttackFrames = randInt(240, 360);
    else gAINextAttackFrames = randInt(360, 540);

    gAIAttackFrames = 0;
    gAIRecoverFrames = 0;
    gAIAvoidFrames = 0;
    gAIOrbitFrames = randInt(120, 220);
    gAIOrbitBias = randInt(0, 1) == 0 ? -1.0f : 1.0f;
}

void drawMouseFocus(int x, int y) {
    if (x < 0 || y < 0 || x >= SCREEN_W || y >= SCREEN_H) return;

    setlinecolor(BLACK);
    circle(x, y, 11);
    circle(x, y, 20);

    setlinecolor(WHITE);
    circle(x, y, 10);
    circle(x, y, 19);

    setlinecolor(BLACK);
    line(x - 26, y, x - 10, y);
    line(x + 10, y, x + 26, y);
    line(x, y - 26, x, y - 10);
    line(x, y + 10, x, y + 26);

    setlinecolor(WHITE);
    line(x - 24, y, x - 12, y);
    line(x + 12, y, x + 24, y);
    line(x, y - 24, x, y - 12);
    line(x, y + 12, x, y + 24);
}

void drawCornerOrnaments(int l, int t, int r, int b, COLORREF color) {
    setlinecolor(color);

    int d1 = 24;
    int d2 = 12;

    line(l, t, l + d1, t);
    line(l, t, l, t + d1);
    line(l + d2, t + d2, l + d1, t + d2);
    line(l + d2, t + d2, l + d2, t + d1);

    line(r, t, r - d1, t);
    line(r, t, r, t + d1);
    line(r - d2, t + d2, r - d1, t + d2);
    line(r - d2, t + d2, r - d2, t + d1);

    line(l, b, l + d1, b);
    line(l, b, l, b - d1);
    line(l + d2, b - d2, l + d1, b - d2);
    line(l + d2, b - d2, l + d2, b - d1);

    line(r, b, r - d1, b);
    line(r, b, r, b - d1);
    line(r - d2, b - d2, r - d1, b - d2);
    line(r - d2, b - d2, r - d2, b - d1);
}

void drawDiamond(int cx, int cy, int r, COLORREF lineColor, COLORREF fillColor) {
    POINT p[4];
    p[0] = { cx, cy - r };
    p[1] = { cx + r, cy };
    p[2] = { cx, cy + r };
    p[3] = { cx - r, cy };

    setlinecolor(lineColor);
    setfillcolor(fillColor);
    solidpolygon(p, 4);
    polygon(p, 4);
}

void drawSceneBackground(COLORREF bg, COLORREF band1, COLORREF band2, COLORREF accent) {
    cleardevice();

    setfillcolor(bg);
    solidrectangle(0, 0, SCREEN_W, SCREEN_H);

    setfillcolor(band1);
    solidrectangle(0, 0, SCREEN_W, 112);

    COLORREF mid = RGB(
        (GetRValue(bg) + GetRValue(band1) + 210) / 3,
        (GetGValue(bg) + GetGValue(band1) + 218) / 3,
        (GetBValue(bg) + GetBValue(band1) + 228) / 3
    );
    setfillcolor(mid);
    solidrectangle(0, 112, SCREEN_W, SCREEN_H - 92);

    for (int i = 0; i < 7; ++i) {
        int inset = 90 + i * 40;
        int top = 150 + i * 18;
        int bottom = SCREEN_H - 140 - i * 18;
        if (top >= bottom) break;
        setlinecolor(RGB(196 - i * 4, 204 - i * 4, 214 - i * 3));
        rectangle(inset, top, SCREEN_W - inset, bottom);
    }

    setfillcolor(band2);
    solidrectangle(0, SCREEN_H - 92, SCREEN_W, SCREEN_H);

    setlinecolor(accent);
    line(0, 112, SCREEN_W, 112);
    line(0, SCREEN_H - 92, SCREEN_W, SCREEN_H - 92);
    line(0, 118, SCREEN_W, 118);
    line(0, SCREEN_H - 98, SCREEN_W, SCREEN_H - 98);

    drawCornerOrnaments(18, 18, SCREEN_W - 18, SCREEN_H - 18, accent);

    for (int x = 70; x < SCREEN_W - 40; x += 105) {
        drawDiamond(x, 56, 8, accent, RGB(243, 238, 224));
        drawDiamond(x + 28, SCREEN_H - 46, 6, accent, RGB(243, 238, 224));
    }
}

void drawPanel(int l, int t, int r, int b, COLORREF fill, COLORREF border, COLORREF accent) {
    setlinecolor(RGB(200, 202, 206));
    setfillcolor(RGB(212, 214, 218));
    solidrectangle(l + 6, t + 6, r + 6, b + 6);

    setlinecolor(border);
    setfillcolor(fill);
    solidrectangle(l, t, r, b);
    rectangle(l, t, r, b);

    setfillcolor(RGB(
        (GetRValue(fill) + 250) / 2,
        (GetGValue(fill) + 248) / 2,
        (GetBValue(fill) + 244) / 2));
    solidrectangle(l + 2, t + 2, r - 2, t + 28);

    setlinecolor(accent);
    rectangle(l + 6, t + 6, r - 6, b - 6);
    rectangle(l + 12, t + 12, r - 12, b - 12);
    line(l + 24, t + 36, r - 24, t + 36);
    line(l + 24, b - 20, r - 24, b - 20);
    drawCornerOrnaments(l + 7, t + 7, r - 7, b - 7, accent);
}

void drawCenteredTextRect(int l, int t, int r, int b, const wchar_t* text, int fontSize, COLORREF color, int yOffset = 0) {
    setUIFontAndColor(fontSize, color);
    int tw = textwidth(text);
    int th = textheight(text);
    int tx = (l + r - tw) / 2;
    int ty = (t + b - th) / 2 + yOffset;
    outtextxy(tx, ty, text);
}

void drawCenteredTextAtY(int centerX, int y, const wchar_t* text, int fontSize, COLORREF color) {
    setUIFontAndColor(fontSize, color);
    int tw = textwidth(text);
    outtextxy(centerX - tw / 2, y, text);
}

void drawSectionCaption(int centerX, int y, int halfWidth, const wchar_t* text, int fontSize, COLORREF color, COLORREF lineColor) {
    setUIFontAndColor(fontSize, color);
    int tw = textwidth(text);
    int th = textheight(text);
    int tx = centerX - tw / 2;
    outtextxy(tx, y, text);
    int midY = y + th / 2 + 2;
    setlinecolor(lineColor);
    line(centerX - halfWidth, midY, tx - 24, midY);
    line(tx + tw + 24, midY, centerX + halfWidth, midY);
    drawDiamond(centerX - halfWidth - 10, midY, 5, lineColor, RGB(236, 230, 216));
    drawDiamond(centerX + halfWidth + 10, midY, 5, lineColor, RGB(236, 230, 216));
}

void drawTextCenteredInButton(const Button& btn, const wchar_t* text, int fontSize, COLORREF color) {
    drawCenteredTextRect(btn.left, btn.top, btn.right, btn.bottom, text, fontSize, color, -1);
}

void drawButton(const Button& btn, bool enabled) {
    COLORREF shadow = enabled ? RGB(112, 126, 142) : RGB(180, 180, 180);
    COLORREF fillTop = enabled ? RGB(245, 248, 251) : RGB(244, 244, 244);
    COLORREF fillBody = enabled ? RGB(226, 234, 242) : RGB(236, 236, 236);
    COLORREF border = enabled ? RGB(64, 85, 108) : RGB(150, 150, 150);
    COLORREF accent = enabled ? RGB(142, 162, 180) : RGB(190, 190, 190);

    setlinecolor(shadow);
    setfillcolor(shadow);
    solidrectangle(btn.left + 4, btn.top + 5, btn.right + 4, btn.bottom + 5);

    setlinecolor(border);
    setfillcolor(fillBody);
    solidrectangle(btn.left, btn.top, btn.right, btn.bottom);
    rectangle(btn.left, btn.top, btn.right, btn.bottom);

    setfillcolor(fillTop);
    solidrectangle(btn.left + 2, btn.top + 2, btn.right - 2, btn.top + (btn.bottom - btn.top) / 2 + 4);

    setlinecolor(accent);
    rectangle(btn.left + 6, btn.top + 6, btn.right - 6, btn.bottom - 6);
    line(btn.left + 18, btn.top + 16, btn.right - 18, btn.top + 16);
    line(btn.left + 18, btn.bottom - 16, btn.right - 18, btn.bottom - 16);

    int cy = (btn.top + btn.bottom) / 2;
    setlinecolor(accent);
    line(btn.left + 22, cy, btn.left + 70, cy);
    line(btn.right - 70, cy, btn.right - 22, cy);
    drawDiamond(btn.left + 82, cy, 4, accent, enabled ? RGB(231, 236, 242) : RGB(239, 239, 239));
    drawDiamond(btn.right - 82, cy, 4, accent, enabled ? RGB(231, 236, 242) : RGB(239, 239, 239));

    drawTextCenteredInButton(btn, btn.text, 24, enabled ? BLACK : RGB(130, 130, 130));
}

void shuffleClues(wstring arr[], int n) {
    for (int i = n - 1; i > 0; --i) {
        int j = randInt(0, i);
        wstring t = arr[i];
        arr[i] = arr[j];
        arr[j] = t;
    }
}

EggRuntimeState generateRandomEgg() {
    EggRuntimeState egg{};

    egg.shapeType = randInt(0, 2);
    egg.toneType = randInt(0, 2);
    egg.surfaceType = randInt(0, 2);
    egg.endBias = randInt(0, 2);
    egg.thicknessType = randInt(0, 2);
    egg.symmetryType = randInt(0, 2);

    egg.hiddenCrack = (randInt(1, 100) <= 18);
    egg.hiddenCrackSide = egg.hiddenCrack ? randInt(0, 1) : -1;

    int sharpAtk = 2;
    int sharpDef = 1;
    int bluntAtk = 1;
    int bluntDef = 2;
    int sharpHP = 4;
    int bluntHP = 5;

    if (egg.shapeType == 2) sharpAtk += 1;
    if (egg.shapeType == 0) {
        bluntDef += 1;
        bluntHP += 1;
    }
    if (egg.shapeType == 1) bluntAtk += 1;

    if (egg.endBias == 0) sharpAtk += 1;
    if (egg.endBias == 1) bluntDef += 1;
    if (egg.endBias == 2) bluntAtk += 1;

    if (egg.thicknessType == 2) {
        sharpDef += 1;
        bluntDef += 1;
        sharpHP += 1;
        bluntHP += 1;
    }
    else if (egg.thicknessType == 0) {
        sharpAtk += 1;
    }

    if (egg.symmetryType == 0) {
        sharpHP += 1;
        bluntHP += 1;
    }
    else if (egg.symmetryType == 2) {
        sharpDef -= 1;
        bluntDef -= 1;
    }

    if (randInt(1, 100) <= 35) sharpAtk += 1;
    if (randInt(1, 100) <= 25) bluntDef += 1;
    if (randInt(1, 100) <= 20) sharpDef += 1;
    if (randInt(1, 100) <= 20) bluntAtk += 1;

    if (egg.hiddenCrack) {
        if (egg.hiddenCrackSide == 0) {
            sharpHP -= 1;
            sharpDef -= 1;
            if (randInt(1, 100) <= 50) sharpAtk -= 1;
        }
        else {
            bluntHP -= 1;
            bluntDef -= 1;
            if (randInt(1, 100) <= 35) bluntAtk -= 1;
        }
    }

    egg.sharpAttack = clampMin(sharpAtk, 1);
    egg.sharpDefense = clampMin(sharpDef, 1);
    egg.bluntAttack = clampMin(bluntAtk, 1);
    egg.bluntDefense = clampMin(bluntDef, 1);

    egg.sharpMaxHP = clampMin(sharpHP, 2) * 2;
    egg.bluntMaxHP = clampMin(bluntHP, 2) * 2;
    egg.sharpHP = egg.sharpMaxHP;
    egg.bluntHP = egg.bluntMaxHP;

    wstring temp[6];

    if (egg.shapeType == 0) temp[0] = L"\u6574\u4f53\u770b\u8d77\u6765\u504f\u5706\u6da6";
    else if (egg.shapeType == 1) temp[0] = L"\u6574\u4f53\u5f62\u72b6\u6bd4\u8f83\u5747\u8861";
    else temp[0] = L"\u6574\u4f53\u770b\u8d77\u6765\u7a0d\u5fae\u4fee\u957f";

    if (egg.endBias == 0) temp[1] = L"\u5c16\u7aef\u770b\u8d77\u6765\u66f4\u7a81\u51fa\u4e00\u4e9b";
    else if (egg.endBias == 1) temp[1] = L"\u949d\u7aef\u770b\u8d77\u6765\u66f4\u9971\u6ee1\u4e00\u4e9b";
    else temp[1] = L"\u4e24\u7aef\u5dee\u522b\u4e0d\u7b97\u7279\u522b\u660e\u663e";

    if (egg.surfaceType == 0) temp[2] = L"\u8868\u9762\u770b\u8d77\u6765\u6bd4\u8f83\u5149\u6ed1";
    else if (egg.surfaceType == 1) temp[2] = L"\u8868\u9762\u80fd\u770b\u89c1\u7ec6\u5c0f\u7eb9\u8def";
    else temp[2] = L"\u8868\u9762\u7565\u663e\u7c97\u7cd9";

    if (egg.toneType == 0) temp[3] = L"\u58f3\u8272\u770b\u8d77\u6765\u504f\u6d45";
    else if (egg.toneType == 1) temp[3] = L"\u58f3\u8272\u6df1\u6d45\u6bd4\u8f83\u666e\u901a";
    else temp[3] = L"\u58f3\u8272\u770b\u8d77\u6765\u7565\u6df1\u4e00\u4e9b";

    if (egg.thicknessType == 0) temp[4] = L"\u62ff\u8d77\u6765\u611f\u89c9\u53ef\u80fd\u504f\u8f7b";
    else if (egg.thicknessType == 1) temp[4] = L"\u770b\u8d77\u6765\u539a\u8584\u6bd4\u8f83\u666e\u901a";
    else temp[4] = L"\u770b\u8d77\u6765\u4f3c\u4e4e\u6bd4\u8f83\u539a\u5b9e";

    if (egg.hiddenCrack && randInt(1, 100) <= 55) {
        temp[5] = L"\u4e00\u4fa7\u4f3c\u4e4e\u6709\u5f88\u6d45\u7684\u6697\u7eb9";
    }
    else {
        if (egg.symmetryType == 0) temp[5] = L"\u5916\u5f62\u770b\u8d77\u6765\u6bd4\u8f83\u5bf9\u79f0";
        else if (egg.symmetryType == 1) temp[5] = L"\u5de6\u53f3\u770b\u8d77\u6765\u6709\u4e00\u70b9\u8f7b\u5fae\u504f\u659c";
        else temp[5] = L"\u5916\u5f62\u770b\u8d77\u6765\u4e0d\u7b97\u7279\u522b\u89c4\u6574";
    }

    shuffleClues(temp, 6);
    egg.clueCount = 6;
    for (int i = 0; i < 6; ++i) egg.clues[i] = temp[i];

    return egg;
}

void generateCandidateBatch(EggRuntimeState eggs[3]) {
    for (int i = 0; i < 3; ++i) eggs[i] = generateRandomEgg();
}

wstring getEggSummary(const EggRuntimeState& egg) {
    wstring a, b, c;

    if (egg.shapeType == 0) a = L"\u504f\u5706\u6da6";
    else if (egg.shapeType == 1) a = L"\u8f83\u5747\u8861";
    else a = L"\u7a0d\u4fee\u957f";

    if (egg.endBias == 0) b = L"\u5c16\u7aef\u7a81\u51fa";
    else if (egg.endBias == 1) b = L"\u949d\u7aef\u9971\u6ee1";
    else b = L"\u4e24\u7aef\u666e\u901a";

    if (egg.toneType == 0) c = L"\u58f3\u8272\u504f\u6d45";
    else if (egg.toneType == 1) c = L"\u58f3\u8272\u666e\u901a";
    else c = L"\u58f3\u8272\u7565\u6df1";

    return a + L" / " + b + L" / " + c;
}

wstring getEggRevealLine1(const EggRuntimeState& egg) {
    wstring a = egg.hiddenCrack ? L"\u7591\u4f3c\u5b58\u5728\u6697\u4f24" : L"\u65e0\u660e\u663e\u6697\u4f24";
    wstring b;
    if (egg.thicknessType == 2) b = L"\u58f3\u504f\u539a\u5b9e";
    else if (egg.thicknessType == 0) b = L"\u58f3\u504f\u8f7b\u8584";
    else b = L"\u58f3\u539a\u8584\u666e\u901a";
    return a + L" / " + b;
}

wstring getEggRevealLine2(const EggRuntimeState& egg) {
    if (egg.endBias == 0) return L"\u6574\u4f53\u66f4\u504f\u5411\u5c16\u7aef\u53d1\u529b";
    if (egg.endBias == 1) return L"\u6574\u4f53\u66f4\u504f\u5411\u949d\u7aef\u627f\u538b";
    return L"\u4e24\u7aef\u7684\u503e\u5411\u5dee\u5f02\u8f83\u5c0f";
}

wstring getTapFeedback(const EggRuntimeState& egg) {
    if (egg.hiddenCrack) {
        if (egg.thicknessType == 2) return L"\u8f7b\u6572\u58f0\u97f3\u7565\u95f7\uff0c\u4f46\u4f3c\u4e4e\u5e26\u4e00\u70b9\u53d1\u865a";
        return L"\u8f7b\u6572\u58f0\u97f3\u4e0d\u592a\u7a33\uff0c\u50cf\u662f\u67d0\u4e00\u4fa7\u5b58\u5728\u9690\u60a3";
    }
    if (egg.thicknessType == 2) return L"\u8f7b\u6572\u58f0\u97f3\u504f\u95f7\uff0c\u611f\u89c9\u6bd4\u8f83\u539a\u5b9e";
    if (egg.thicknessType == 0) return L"\u8f7b\u6572\u58f0\u97f3\u504f\u6e05\uff0c\u611f\u89c9\u6bd4\u8f83\u8f7b\u8584";
    return L"\u8f7b\u6572\u58f0\u97f3\u666e\u901a\uff0c\u4e0d\u7b97\u7279\u522b\u51fa\u6311";
}

EggEntity makeBattleEgg(const EggRuntimeState& draft, float x, float y, float angle) {
    EggEntity e{};

    e.draft = draft;
    e.pos = makeVec(x, y);
    e.vel = makeVec(0.0f, 0.0f);
    e.angle = angle;
    e.radius = 49.0f;

    e.maxStamina = 100.0f;
    e.stamina = 100.0f;
    e.boosting = false;

    e.sharpMaxHP = draft.sharpMaxHP;
    e.sharpHP = e.sharpMaxHP;

    e.bluntMaxHP = draft.bluntMaxHP;
    e.bluntHP = e.bluntMaxHP;

    e.sideMaxHP = (clampMin(4 + draft.thicknessType + (draft.symmetryType == 0 ? 1 : 0), 3) * 2) - 1;
    e.sideHP = e.sideMaxHP;

    e.totalMaxIntegrity = e.sharpMaxHP + e.bluntMaxHP + e.sideMaxHP + 6;
    e.totalIntegrity = e.totalMaxIntegrity;

    e.collisionCooldown = 0;
    e.wallCooldown = 0;
    e.hitFlashFrames = 0;

    return e;
}

int regionStateLevel(int hp, int maxHP) {
    if (hp <= 0) return 3;
    if (hp * 100 / maxHP <= 30) return 2;
    if (hp * 100 / maxHP <= 70) return 1;
    return 0;
}

const wchar_t* getRegionStateText(int hp, int maxHP) {
    int level = regionStateLevel(hp, maxHP);
    if (level == 3) return L"\u7834\u88c2";
    if (level == 2) return L"\u660e\u663e\u88c2\u7eb9";
    if (level == 1) return L"\u8f7b\u5fae\u88c2\u7eb9";
    return L"\u5b8c\u6574";
}

Vec2 eggForward(const EggEntity& e) {
    return makeVec(cosf(e.angle), sinf(e.angle));
}

ContactRegion getContactRegion(const EggEntity& e, Vec2 dirFromCenter) {
    Vec2 f = eggForward(e);
    Vec2 d = normVec(dirFromCenter);
    float k = dotVec(f, d);

    if (k > 0.62f) return REGION_SHARP;
    if (k < -0.62f) return REGION_BLUNT;
    return REGION_SIDE;
}

const wchar_t* getRegionName(ContactRegion r) {
    if (r == REGION_SHARP) return L"\u5c16\u7aef";
    if (r == REGION_BLUNT) return L"\u949d\u7aef";
    return L"\u4fa7\u9762";
}

int getAttackByRegion(const EggEntity& e, ContactRegion r) {
    if (r == REGION_SHARP) return e.draft.sharpAttack;
    if (r == REGION_BLUNT) return e.draft.bluntAttack;
    return clampMin((e.draft.sharpAttack + e.draft.bluntAttack) / 2, 1);
}

int getDefenseByRegion(const EggEntity& e, ContactRegion r) {
    if (r == REGION_SHARP) return e.draft.sharpDefense;
    if (r == REGION_BLUNT) return e.draft.bluntDefense;
    return clampMin((e.draft.sharpDefense + e.draft.bluntDefense) / 2, 1);
}

void applyDamage(EggEntity& e, ContactRegion r, int dmg) {
    if (dmg <= 0) return;

    int realDmg = dmg;

    if (r == REGION_SHARP) {
        if (e.sharpHP > 0) {
            e.sharpHP -= dmg;
            if (e.sharpHP < 0) e.sharpHP = 0;
        }
        else {
            realDmg += 1;
        }
    }
    else if (r == REGION_BLUNT) {
        if (e.bluntHP > 0) {
            e.bluntHP -= dmg;
            if (e.bluntHP < 0) e.bluntHP = 0;
        }
        else {
            realDmg += 1;
        }
    }
    else {
        if (e.sideHP > 0) {
            e.sideHP -= dmg;
            if (e.sideHP < 0) e.sideHP = 0;
        }
        else {
            realDmg += 1;
        }
    }

    e.totalIntegrity -= realDmg;
    if (e.totalIntegrity < 0) e.totalIntegrity = 0;
    e.hitFlashFrames = 8;
}

int calcDamageValue(float offense, float defense) {
    float score = offense - defense;
    if (score < 1.0f) return 0;
    if (score < 2.7f) return 1;
    if (score < 4.9f) return 2;
    return 3;
}

bool isEggDead(const EggEntity& e) {
    return e.totalIntegrity <= 0;
}

void drawLocalLineScaled(const EggEntity& e, float x1, float y1, float x2, float y2, float scale) {
    Vec2 a = worldFromLocalScaled(e, makeVec(x1, y1), scale);
    Vec2 b = worldFromLocalScaled(e, makeVec(x2, y2), scale);
    line((int)a.x, (int)a.y, (int)b.x, (int)b.y);
}

void drawEggShadow(const EggEntity& e, float scale) {
    int w = (int)(58.0f * scale);
    int h = (int)(14.0f * scale);
    int cx = (int)e.pos.x;
    int cy = (int)(e.pos.y + 48.0f * scale);

    setlinecolor(RGB(176, 162, 136));
    setfillcolor(RGB(176, 162, 136));
    solidellipse(cx - w, cy - h, cx + w, cy + h);
}

void buildEggPolygon(const EggEntity& e, float scale, POINT pts[], int& count) {
    const int SAMPLE = 120;

    float a = 56.0f;
    float b = 40.0f;

    if (e.draft.shapeType == 0) {
        a = 54.0f;
        b = 41.0f;
    }
    else if (e.draft.shapeType == 1) {
        a = 56.0f;
        b = 40.0f;
    }
    else {
        a = 59.0f;
        b = 39.0f;
    }

    float asym = 0.10f;
    if (e.draft.endBias == 0) asym = 0.13f;
    else if (e.draft.endBias == 1) asym = 0.07f;

    count = 0;

    for (int i = 0; i < SAMPLE; ++i) {
        float u = -1.0f + 2.0f * i / (SAMPLE - 1);
        float coreSq = 1.0f - u * u;
        if (coreSq < 0.0f) coreSq = 0.0f;
        float core = sqrtf(coreSq);

        float sideScale = 1.0f - asym * u;
        float belly = 1.0f + 0.018f * (1.0f - fabsf(u));
        float x = a * u;
        float y = -b * core * sideScale * belly;

        Vec2 w = worldFromLocalScaled(e, makeVec(x, y), scale);
        pts[count].x = (LONG)w.x;
        pts[count].y = (LONG)w.y;
        count++;
    }

    for (int i = SAMPLE - 1; i >= 0; --i) {
        float u = -1.0f + 2.0f * i / (SAMPLE - 1);
        float coreSq = 1.0f - u * u;
        if (coreSq < 0.0f) coreSq = 0.0f;
        float core = sqrtf(coreSq);

        float sideScale = 1.0f - asym * u;
        float belly = 1.0f + 0.018f * (1.0f - fabsf(u));
        float x = a * u;
        float y = b * core * sideScale * belly;

        Vec2 w = worldFromLocalScaled(e, makeVec(x, y), scale);
        pts[count].x = (LONG)w.x;
        pts[count].y = (LONG)w.y;
        count++;
    }
}

void drawSharpCracks(const EggEntity& e, int level, float scale) {
    setlinecolor(RGB(105, 82, 82));

    drawLocalLineScaled(e, 34, 0, 24, -4, scale);
    drawLocalLineScaled(e, 24, -4, 12, 2, scale);

    if (level >= 1) {
        drawLocalLineScaled(e, 24, -4, 16, -12, scale);
        drawLocalLineScaled(e, 12, 2, 4, 10, scale);
    }
    if (level >= 2) {
        drawLocalLineScaled(e, 12, 2, 2, -2, scale);
        drawLocalLineScaled(e, 2, -2, -8, 6, scale);
        drawLocalLineScaled(e, 2, -2, -6, -8, scale);
        drawLocalLineScaled(e, 18, -1, 9, -14, scale);
    }
    if (level >= 3) {
        drawLocalLineScaled(e, -8, 6, -18, 1, scale);
        drawLocalLineScaled(e, -18, 1, -24, 10, scale);
        drawLocalLineScaled(e, -6, -8, -16, -14, scale);
        drawLocalLineScaled(e, 0, 5, -10, 16, scale);
        drawLocalLineScaled(e, 6, -6, -4, -18, scale);
    }
}

void drawBluntCracks(const EggEntity& e, int level, float scale) {
    setlinecolor(RGB(105, 82, 82));

    drawLocalLineScaled(e, -34, 0, -22, 4, scale);
    drawLocalLineScaled(e, -22, 4, -10, -2, scale);

    if (level >= 1) {
        drawLocalLineScaled(e, -22, 4, -16, 14, scale);
        drawLocalLineScaled(e, -10, -2, -2, -10, scale);
    }
    if (level >= 2) {
        drawLocalLineScaled(e, -10, -2, 0, 2, scale);
        drawLocalLineScaled(e, 0, 2, 10, -6, scale);
        drawLocalLineScaled(e, 0, 2, 8, 10, scale);
        drawLocalLineScaled(e, -18, 1, -10, -12, scale);
    }
    if (level >= 3) {
        drawLocalLineScaled(e, 10, -6, 18, -1, scale);
        drawLocalLineScaled(e, 18, -1, 24, -12, scale);
        drawLocalLineScaled(e, 8, 10, 18, 16, scale);
        drawLocalLineScaled(e, -2, -5, 10, -18, scale);
        drawLocalLineScaled(e, 2, 8, 14, 20, scale);
    }
}

void drawSideCracks(const EggEntity& e, int level, float scale) {
    setlinecolor(RGB(110, 85, 85));

    drawLocalLineScaled(e, -4, -18, 5, -8, scale);
    drawLocalLineScaled(e, 5, -8, -2, 4, scale);

    if (level >= 1) {
        drawLocalLineScaled(e, 5, -8, 15, -12, scale);
        drawLocalLineScaled(e, -2, 4, -10, 12, scale);
    }
    if (level >= 2) {
        drawLocalLineScaled(e, -2, 4, 6, 14, scale);
        drawLocalLineScaled(e, 0, -2, -12, -4, scale);
        drawLocalLineScaled(e, 3, 2, 14, 6, scale);
    }
    if (level >= 3) {
        drawLocalLineScaled(e, 6, 14, 0, 24, scale);
        drawLocalLineScaled(e, -10, 12, -20, 18, scale);
        drawLocalLineScaled(e, -12, -4, -22, -8, scale);
        drawLocalLineScaled(e, 14, 6, 24, 10, scale);
    }
}

void drawEggShape(const EggEntity& e, float scale = 1.0f) {
    POINT pts[260];
    int count = 0;
    buildEggPolygon(e, scale, pts, count);

    setlinecolor(RGB(92, 92, 92));

    if (e.draft.toneType == 0) {
        setfillcolor(e.hitFlashFrames > 0 ? RGB(255, 228, 228) : RGB(248, 244, 234));
    }
    else if (e.draft.toneType == 1) {
        setfillcolor(e.hitFlashFrames > 0 ? RGB(255, 222, 218) : RGB(241, 232, 207));
    }
    else {
        setfillcolor(e.hitFlashFrames > 0 ? RGB(255, 214, 208) : RGB(228, 210, 176));
    }

    solidpolygon(pts, count);
    polygon(pts, count);

    setlinecolor(RGB(255, 255, 255));
    drawLocalLineScaled(e, 8, -26, 15, -10, scale);
    drawLocalLineScaled(e, 15, -10, 10, 10, scale);

    if (e.draft.surfaceType == 0) {
        setlinecolor(RGB(220, 220, 220));
        drawLocalLineScaled(e, -4, -10, 2, -9, scale);
        drawLocalLineScaled(e, -2, 10, 4, 9, scale);
    }
    else if (e.draft.surfaceType == 1) {
        setlinecolor(RGB(192, 192, 192));
        drawLocalLineScaled(e, -9, -6, -3, -5, scale);
        drawLocalLineScaled(e, -6, 10, 1, 9, scale);
    }
    else {
        setlinecolor(RGB(188, 181, 165));
        drawLocalLineScaled(e, -12, -5, -5, -4, scale);
        drawLocalLineScaled(e, -10, 12, -2, 11, scale);
    }
}

void drawEggCracks(const EggEntity& e, float scale = 1.0f) {
    int sharpLevel = regionStateLevel(e.sharpHP, e.sharpMaxHP);
    int bluntLevel = regionStateLevel(e.bluntHP, e.bluntMaxHP);
    int sideLevel = regionStateLevel(e.sideHP, e.sideMaxHP);

    if (sharpLevel > 0) drawSharpCracks(e, sharpLevel, scale);
    if (bluntLevel > 0) drawBluntCracks(e, bluntLevel, scale);
    if (sideLevel > 0) drawSideCracks(e, sideLevel, scale);
}

void drawEggPreviewCard(int x, int y, const EggRuntimeState& egg, const wchar_t* title, float phase) {
    drawPanel(x, y, x + 230, y + 360, RGB(247, 245, 240), RGB(132, 122, 104), RGB(170, 152, 120));

    drawCenteredTextRect(x, y + 10, x + 230, y + 54, title, 28, BLACK);

    EggEntity preview = makeBattleEgg(egg, (float)(x + 115), (float)(y + 128 + idleBob(phase, 3.0f)), -PI_F / 2.0f);
    drawEggShadow(preview, 0.92f);
    drawEggShape(preview, 0.92f);

    setUIFontAndColor(19, BLACK);
    for (int i = 0; i < egg.clueCount; ++i) {
        outtextxy(x + 14, y + 212 + i * 23, egg.clues[i].c_str());
    }
}

void drawBattleEggPanel(int x, int y, const wchar_t* title, const EggEntity& egg, float scaleMul) {
    drawPanel(x, y, x + 270, y + 320, RGB(247, 245, 240), RGB(132, 122, 104), RGB(170, 152, 120));

    drawCenteredTextRect(x, y + 8, x + 270, y + 48, title, 28, BLACK);

    EggEntity preview = egg;
    preview.pos = makeVec((float)(x + 135), (float)(y + 125 + idleBob((float)x * 0.01f, 2.0f)));
    drawEggShadow(preview, 0.96f * scaleMul);
    drawEggShape(preview, 0.96f * scaleMul);
    drawEggCracks(preview, 0.96f * scaleMul);

    wstring s1 = wstring(L"\u5c16\u7aef\uff1a") + getRegionStateText(egg.sharpHP, egg.sharpMaxHP);
    wstring s2 = wstring(L"\u949d\u7aef\uff1a") + getRegionStateText(egg.bluntHP, egg.bluntMaxHP);
    wstring s3 = wstring(L"\u603b\u5b8c\u6574\u5ea6\uff1a") + toW(egg.totalIntegrity) + L"/" + toW(egg.totalMaxIntegrity);

    setUIFontAndColor(20, BLACK);
    outtextxy(x + 18, y + 214, s1.c_str());
    outtextxy(x + 18, y + 242, s2.c_str());
    outtextxy(x + 18, y + 270, s3.c_str());
    outtextxy(x + 18, y + 298, fitTextToWidth(getEggRevealLine1(egg.draft), 234).c_str());
}

void drawBattleLogBox(int x, int y, int w, int h, const wstring& line1, const wstring& line2, const wstring& line3) {
    drawPanel(x, y, x + w, y + h, RGB(247, 245, 240), RGB(132, 122, 104), RGB(170, 152, 120));

    setUIFontAndColor(24, BLACK);
    outtextxy(x + 18, y + 10, L"\u5bf9\u6218\u65e5\u5fd7");

    setUIFontAndColor(20, BLACK);
    outtextxy(x + 18, y + 48, fitTextToWidth(line1, w - 36).c_str());
    outtextxy(x + 18, y + 78, fitTextToWidth(line2, w - 36).c_str());
    outtextxy(x + 18, y + 108, fitTextToWidth(line3, w - 36).c_str());
}

void drawRomanBarFrame(int x, int y, int w, int h, COLORREF border, COLORREF fill) {
    POINT leftCap[3];
    POINT rightCap[3];

    leftCap[0] = { x - 12, y + h / 2 };
    leftCap[1] = { x, y };
    leftCap[2] = { x, y + h };

    rightCap[0] = { x + w + 12, y + h / 2 };
    rightCap[1] = { x + w, y };
    rightCap[2] = { x + w, y + h };

    setlinecolor(border);
    setfillcolor(fill);

    solidrectangle(x, y, x + w, y + h);
    rectangle(x, y, x + w, y + h);
    solidpolygon(leftCap, 3);
    polygon(leftCap, 3);
    solidpolygon(rightCap, 3);
    polygon(rightCap, 3);

    line(x + 12, y + 3, x + 12, y + h - 3);
    line(x + w - 12, y + 3, x + w - 12, y + h - 3);
}

void drawStaminaBar(int x, int y, int w, int h, float ratio) {
    ratio = clampFloat(ratio, 0.0f, 1.0f);

    drawRomanBarFrame(x, y, w, h, RGB(116, 88, 28), RGB(232, 221, 187));

    int innerL = x + 4;
    int innerT = y + 4;
    int innerW = w - 8;
    int innerH = h - 8;

    setfillcolor(RGB(244, 237, 214));
    solidrectangle(innerL, innerT, innerL + innerW, innerT + innerH);

    int fillW = (int)(innerW * ratio);
    if (fillW > 0) {
        if (ratio > 0.55f) setfillcolor(RGB(214, 164, 52));
        else if (ratio > 0.25f) setfillcolor(RGB(201, 121, 44));
        else setfillcolor(RGB(173, 63, 38));

        solidrectangle(innerL, innerT, innerL + fillW, innerT + innerH);

        setlinecolor(RGB(160, 120, 40));
        for (int i = innerL + 10; i < innerL + fillW; i += 14) {
            line(i, innerT + 1, i, innerT + innerH - 1);
        }
    }
}

void drawIntegrityBar(int x, int y, int w, int h, float ratio) {
    ratio = clampFloat(ratio, 0.0f, 1.0f);

    setlinecolor(RGB(90, 90, 90));
    setfillcolor(RGB(236, 236, 236));
    solidrectangle(x, y, x + w, y + h);
    rectangle(x, y, x + w, y + h);

    int innerL = x + 3;
    int innerT = y + 3;
    int innerW = w - 6;
    int innerH = h - 6;

    setfillcolor(RGB(245, 245, 245));
    solidrectangle(innerL, innerT, innerL + innerW, innerT + innerH);

    int fillW = (int)(innerW * ratio);
    if (fillW > 0) {
        if (ratio > 0.6f) setfillcolor(RGB(112, 171, 107));
        else if (ratio > 0.3f) setfillcolor(RGB(202, 161, 62));
        else setfillcolor(RGB(191, 88, 69));
        solidrectangle(innerL, innerT, innerL + fillW, innerT + innerH);
    }
}


float distanceVec(Vec2 a, Vec2 b) {
    return lenVec(subVec(a, b));
}

bool isObstaclePosValid(Vec2 p, float r, Vec2 leftSpawn, Vec2 rightSpawn, int placedCount) {
    if (p.x < ARENA_L + 120 || p.x > ARENA_R - 120) return false;
    if (p.y < ARENA_T + 90 || p.y > ARENA_B - 90) return false;
    if (distanceVec(p, leftSpawn) < 220.0f) return false;
    if (distanceVec(p, rightSpawn) < 220.0f) return false;
    for (int i = 0; i < placedCount; ++i) {
        if (gObstacles[i].active && distanceVec(p, gObstacles[i].pos) < gObstacles[i].radius + r + 120.0f) return false;
    }
    return true;
}

void initArenaHazards(const EggEntity& leftEgg, const EggEntity& rightEgg) {
    gObstacleCount = 0;
    for (int i = 0; i < MAX_OBSTACLES; ++i) {
        gObstacles[i].active = false;
        gObstacles[i].pos = makeVec(0.0f, 0.0f);
        gObstacles[i].radius = 28.0f;
    }

    gSpikeCount = getTargetSpikeCount();
    for (int i = 0; i < MAX_SPIKES; ++i) {
        gSpikes[i].state = SPIKE_IDLE;
        gSpikes[i].x = 0.0f;
        gSpikes[i].y = 0.0f;
        gSpikes[i].framesLeft = 0;
        gSpikes[i].damage = 0;
        gSpikes[i].hitLeft = false;
        gSpikes[i].hitRight = false;
        gSpikes[i].speed = 14.0f;
        gSpikes[i].halfWidth = 12.0f;
        gSpikes[i].height = 42.0f;
    }
    gNextSpikeFrames = isSpikeMode() ? randInt(getSpikeIntervalMinFrames(), getSpikeIntervalMaxFrames()) : 999999;

    for (int i = 0; i < MAX_HEALTH_PACKS; ++i) {
        gHealthPacks[i].active = false;
        gHealthPacks[i].tiedToSpike = false;
        gHealthPacks[i].tiedSpikeIndex = -1;
        gHealthPacks[i].pos = makeVec(0.0f, 0.0f);
        gHealthPacks[i].radius = 16.0f;
        gHealthPacks[i].healPoints = 5;
    }
    gNextHealthPackFrames = randInt(480, 720);

    gSkillPack.active = false;
    gSkillPack.pos = makeVec(0.0f, 0.0f);
    gSkillPack.radius = 17.0f;
    gNextSkillPackFrames = randInt(336, 504);
    gLeftHasSkillShot = false;
    gRightHasSkillShot = false;
    gLeftSkillKeyHeld = false;
    gRightSkillKeyHeld = false;
    for (int i = 0; i < 2; ++i) {
        gYolkShots[i].active = false;
        gYolkShots[i].owner = i;
        gYolkShots[i].pos = makeVec(0.0f, 0.0f);
        gYolkShots[i].vel = makeVec(0.0f, 0.0f);
        gYolkShots[i].radiusX = 18.0f;
        gYolkShots[i].radiusY = 14.0f;
        gYolkShots[i].damage = 2;
        gYolkShots[i].lifeFrames = 0;
    }

    int targetObstacles = getTargetObstacleCount();
    int tries = 0;
    while (gObstacleCount < targetObstacles && tries < 1200) {
        tries++;
        Vec2 p = makeVec((float)randInt(ARENA_L + 130, ARENA_R - 130), (float)randInt(ARENA_T + 100, ARENA_B - 100));
        float r = (float)randInt(24, 34);
        if (!isObstaclePosValid(p, r, leftEgg.pos, rightEgg.pos, gObstacleCount)) continue;
        gObstacles[gObstacleCount].active = true;
        gObstacles[gObstacleCount].pos = p;
        gObstacles[gObstacleCount].radius = r;
        gObstacleCount++;
    }
}

bool isHealthPackPosValid(Vec2 p, float r, const EggEntity& leftEgg, const EggEntity& rightEgg) {
    if (p.x < ARENA_L + 80 || p.x > ARENA_R - 80) return false;
    if (p.y < ARENA_T + 70 || p.y > ARENA_B - 70) return false;
    if (distanceVec(p, leftEgg.pos) < 120.0f) return false;
    if (distanceVec(p, rightEgg.pos) < 120.0f) return false;
    for (int i = 0; i < gObstacleCount; ++i) {
        if (!gObstacles[i].active) continue;
        if (distanceVec(p, gObstacles[i].pos) < gObstacles[i].radius + r + 22.0f) return false;
    }
    return true;
}


int findFreeHealthPackSlot() {
    for (int i = 0; i < MAX_HEALTH_PACKS; ++i) {
        if (!gHealthPacks[i].active) return i;
    }
    return -1;
}

bool hasPersistentHealthPack() {
    for (int i = 0; i < MAX_HEALTH_PACKS; ++i) {
        if (gHealthPacks[i].active && !gHealthPacks[i].tiedToSpike) return true;
    }
    return false;
}

bool hasAnyHealthPack() {
    for (int i = 0; i < MAX_HEALTH_PACKS; ++i) {
        if (gHealthPacks[i].active) return true;
    }
    return false;
}

bool hasTiedHealthPackForSpike(int spikeIndex) {
    for (int i = 0; i < MAX_HEALTH_PACKS; ++i) {
        if (gHealthPacks[i].active && gHealthPacks[i].tiedToSpike && gHealthPacks[i].tiedSpikeIndex == spikeIndex) return true;
    }
    return false;
}

void clearHealthPackForSpike(int spikeIndex) {
    for (int i = 0; i < MAX_HEALTH_PACKS; ++i) {
        if (gHealthPacks[i].active && gHealthPacks[i].tiedToSpike && gHealthPacks[i].tiedSpikeIndex == spikeIndex) {
            gHealthPacks[i].active = false;
        }
    }
}


void spawnRandomHealthPack(const EggEntity& leftEgg, const EggEntity& rightEgg) {
    if (hasPersistentHealthPack()) return;
    int slot = findFreeHealthPackSlot();
    if (slot < 0) return;

    int tries = 0;
    while (tries < 400) {
        tries++;
        Vec2 p = makeVec((float)randInt(ARENA_L + 80, ARENA_R - 80), (float)randInt(ARENA_T + 70, ARENA_B - 70));
        if (!isHealthPackPosValid(p, 16.0f, leftEgg, rightEgg)) continue;
        gHealthPacks[slot].active = true;
        gHealthPacks[slot].tiedToSpike = false;
        gHealthPacks[slot].tiedSpikeIndex = -1;
        gHealthPacks[slot].pos = p;
        gHealthPacks[slot].radius = 16.0f;
        gHealthPacks[slot].healPoints = 5;
        return;
    }
}

void spawnSpikeHealthPack(int spikeIndex, const EggEntity& leftEgg, const EggEntity& rightEgg) {
    if (spikeIndex < 0 || spikeIndex >= MAX_SPIKES) return;
    if (hasTiedHealthPackForSpike(spikeIndex)) return;

    int slot = findFreeHealthPackSlot();
    if (slot < 0) return;

    float x = gSpikes[spikeIndex].x;
    for (int tries = 0; tries < 300; ++tries) {
        float y = (float)randInt(ARENA_T + 90, ARENA_B - 90);
        Vec2 p = makeVec(x, y);
        if (!isHealthPackPosValid(p, 16.0f, leftEgg, rightEgg)) continue;
        gHealthPacks[slot].active = true;
        gHealthPacks[slot].tiedToSpike = true;
        gHealthPacks[slot].tiedSpikeIndex = spikeIndex;
        gHealthPacks[slot].pos = p;
        gHealthPacks[slot].radius = 16.0f;
        gHealthPacks[slot].healPoints = 5;
        return;
    }
}

void drawHealthPack() {
    for (int i = 0; i < MAX_HEALTH_PACKS; ++i) {
        if (!gHealthPacks[i].active) continue;

        int cx = (int)gHealthPacks[i].pos.x;
        int cy = (int)gHealthPacks[i].pos.y;
        int r = (int)gHealthPacks[i].radius;

        setlinecolor(RGB(42, 120, 56));
        setfillcolor(RGB(204, 245, 212));
        solidcircle(cx, cy, r);
        circle(cx, cy, r);

        setfillcolor(RGB(240, 255, 244));
        solidcircle(cx, cy, r - 4);

        setlinecolor(RGB(46, 150, 70));
        setfillcolor(RGB(62, 178, 89));
        solidrectangle(cx - 4, cy - 10, cx + 4, cy + 10);
        solidrectangle(cx - 10, cy - 4, cx + 10, cy + 4);

        if (gHealthPacks[i].tiedToSpike) {
            setlinecolor(RGB(180, 40, 40));
            circle(cx, cy, r + 5);
        }
    }
}

int restoreEggHealth(EggEntity& egg, int healPoints) {
    int restored = 0;
    while (healPoints > 0) {
        int sharpDeficit = egg.sharpMaxHP - egg.sharpHP;
        int bluntDeficit = egg.bluntMaxHP - egg.bluntHP;
        int sideDeficit = egg.sideMaxHP - egg.sideHP;
        if (sharpDeficit <= 0 && bluntDeficit <= 0 && sideDeficit <= 0) break;

        if (sideDeficit >= sharpDeficit && sideDeficit >= bluntDeficit && sideDeficit > 0) {
            egg.sideHP++;
        } else if (sharpDeficit >= bluntDeficit && sharpDeficit > 0) {
            egg.sharpHP++;
        } else if (bluntDeficit > 0) {
            egg.bluntHP++;
        }

        healPoints--;
        restored++;
    }

    egg.totalIntegrity += restored;
    if (egg.totalIntegrity > egg.totalMaxIntegrity) egg.totalIntegrity = egg.totalMaxIntegrity;
    return restored;
}

bool tryPickupHealthPack(EggEntity& egg, const wchar_t* pickerName, wstring& log1, wstring& log2, wstring& log3) {
    for (int i = 0; i < MAX_HEALTH_PACKS; ++i) {
        if (!gHealthPacks[i].active) continue;
        if (distanceVec(egg.pos, gHealthPacks[i].pos) > egg.radius + gHealthPacks[i].radius + 4.0f) continue;

        int restored = restoreEggHealth(egg, gHealthPacks[i].healPoints);
        bool tied = gHealthPacks[i].tiedToSpike;
        gHealthPacks[i].active = false;
        if (!isSpikeMode()) gNextHealthPackFrames = randInt(480, 720);

        log1 = wstring(pickerName) + L"吃到了血包";
        log2 = restored > 0 ? L"蛋壳裂损得到修复" : L"这次补给没能继续恢复耐久";
        log3 = tied ? L"冒着尖刺风险抢到了回复机会" : L"场上的恢复点已经被拿走";
        return true;
    }
    return false;
}

void updateHealthPackSystem(EggEntity& leftEgg, EggEntity& rightEgg, wstring& log1, wstring& log2, wstring& log3) {
    if (!isSpikeMode()) {
        if (!hasPersistentHealthPack()) {
            gNextHealthPackFrames--;
            if (gNextHealthPackFrames <= 0) {
                spawnRandomHealthPack(leftEgg, rightEgg);
                if (!hasPersistentHealthPack()) gNextHealthPackFrames = randInt(180, 260);
            }
        }
    } else {
        for (int i = 0; i < gSpikeCount; ++i) {
            if (gSpikes[i].state == SPIKE_LINGER || gSpikes[i].state == SPIKE_IDLE) {
                clearHealthPackForSpike(i);
            }
        }
    }

    if (!hasAnyHealthPack()) return;
    if (tryPickupHealthPack(leftEgg, L"左侧鸡蛋", log1, log2, log3)) return;
    tryPickupHealthPack(rightEgg, L"右侧鸡蛋", log1, log2, log3);
}


bool hasAnyHeldSkill() {
    return gLeftHasSkillShot || gRightHasSkillShot;
}

bool isSkillPackPosValid(Vec2 p, float r, const EggEntity& leftEgg, const EggEntity& rightEgg) {
    if (!isHealthPackPosValid(p, r, leftEgg, rightEgg)) return false;
    for (int i = 0; i < MAX_HEALTH_PACKS; ++i) {
        if (gHealthPacks[i].active && distanceVec(p, gHealthPacks[i].pos) < r + gHealthPacks[i].radius + 26.0f) return false;
    }
    return true;
}

void spawnSkillPack(const EggEntity& leftEgg, const EggEntity& rightEgg) {
    if (gSkillPack.active || hasAnyHeldSkill()) return;

    int tries = 0;
    while (tries < 400) {
        tries++;
        Vec2 p = makeVec((float)randInt(ARENA_L + 90, ARENA_R - 90), (float)randInt(ARENA_T + 80, ARENA_B - 80));
        if (!isSkillPackPosValid(p, 18.0f, leftEgg, rightEgg)) continue;
        gSkillPack.active = true;
        gSkillPack.pos = p;
        gSkillPack.radius = 17.0f;
        return;
    }
}

void drawSkillPack() {
    if (!gSkillPack.active) return;

    int cx = (int)gSkillPack.pos.x;
    int cy = (int)gSkillPack.pos.y;
    int r = (int)gSkillPack.radius;

    setlinecolor(RGB(168, 110, 18));
    setfillcolor(RGB(255, 226, 142));
    solidcircle(cx, cy, r);
    circle(cx, cy, r);
    setfillcolor(RGB(255, 242, 190));
    solidcircle(cx, cy, r - 4);

    setlinecolor(RGB(196, 136, 24));
    circle(cx, cy, r + 5);
    line(cx - 8, cy, cx + 8, cy);
    line(cx, cy - 8, cx, cy + 8);
}

bool tryPickupSkillPack(EggEntity& egg, int sideOwner, wstring& log1, wstring& log2, wstring& log3) {
    if (!gSkillPack.active) return false;
    if (distanceVec(egg.pos, gSkillPack.pos) > egg.radius + gSkillPack.radius + 5.0f) return false;

    if (sideOwner == 0) gLeftHasSkillShot = true;
    else gRightHasSkillShot = true;

    gSkillPack.active = false;
    gNextSkillPackFrames = randInt(336, 504);

    log1 = sideOwner == 0 ? L"左侧鸡蛋拾取了技能包" : L"右侧鸡蛋拾取了技能包";
    log2 = L"已获得一次蛋黄发射机会";
    log3 = L"抓住时机发射，能打出远程压制和明显击退";
    return true;
}

void updateSkillPackSystem(EggEntity& leftEgg, EggEntity& rightEgg, wstring& log1, wstring& log2, wstring& log3) {
    if (!gSkillPack.active && !hasAnyHeldSkill()) {
        gNextSkillPackFrames--;
        if (gNextSkillPackFrames <= 0) {
            spawnSkillPack(leftEgg, rightEgg);
            if (!gSkillPack.active) gNextSkillPackFrames = randInt(336, 504);
        }
    }

    if (!gSkillPack.active) return;
    if (tryPickupSkillPack(leftEgg, 0, log1, log2, log3)) return;
    tryPickupSkillPack(rightEgg, 1, log1, log2, log3);
}

void drawYolkProjectile(const YolkProjectile& p) {
    if (!p.active) return;

    POINT pts[64];
    int count = 0;
    for (int i = 0; i < 32; ++i) {
        float t = 2.0f * PI_F * i / 32.0f;
        float x = cosf(t) * p.radiusX;
        float y = sinf(t) * p.radiusY;
        float ang = atan2f(p.vel.y, p.vel.x);
        Vec2 w = addVec(p.pos, rotateLocal(makeVec(x, y), ang));
        pts[count].x = (LONG)w.x;
        pts[count].y = (LONG)w.y;
        count++;
    }

    setlinecolor(RGB(185, 132, 18));
    setfillcolor(RGB(246, 194, 44));
    solidpolygon(pts, count);
    polygon(pts, count);

    setlinecolor(RGB(255, 234, 160));
    line((int)(p.pos.x - p.vel.x * 0.25f), (int)(p.pos.y - p.vel.y * 0.25f), (int)(p.pos.x - p.vel.x * 0.75f), (int)(p.pos.y - p.vel.y * 0.75f));
}

void fireYolkShot(int owner, const EggEntity& shooter, wstring& log1, wstring& log2, wstring& log3) {
    if (owner < 0 || owner > 1) return;
    if (gYolkShots[owner].active) return;

    Vec2 f = eggForward(shooter);
    gYolkShots[owner].active = true;
    gYolkShots[owner].owner = owner;
    gYolkShots[owner].pos = addVec(shooter.pos, mulVec(f, shooter.radius + 18.0f));
    float speed = ((gArenaMode == ARENA_NIGHTMARE) ? 21.0f : 18.0f) * 1.3f;
    gYolkShots[owner].vel = mulVec(f, speed);
    gYolkShots[owner].radiusX = 18.0f;
    gYolkShots[owner].radiusY = 13.0f;
    gYolkShots[owner].damage = gSpikeDamage + 1;
    if (gYolkShots[owner].damage < 2) gYolkShots[owner].damage = 2;
    gYolkShots[owner].lifeFrames = 85;

    if (owner == 0) gLeftHasSkillShot = false;
    else gRightHasSkillShot = false;

    log1 = owner == 0 ? L"左侧鸡蛋发射了蛋黄" : L"右侧鸡蛋发射了蛋黄";
    log2 = L"高速蛋黄直线飞出";
    log3 = L"命中后会造成强力冲击";
}

bool yolkHitsObstacle(const YolkProjectile& p, const ArenaObstacle& ob) {
    if (!ob.active) return false;
    return distanceVec(p.pos, ob.pos) < ob.radius + p.radiusX * 0.7f;
}

void applyYolkHitToEgg(EggEntity& egg, const YolkProjectile& p, wstring& log1, wstring& log2, wstring& log3) {
    Vec2 d = subVec(p.pos, egg.pos);
    ContactRegion r = getContactRegion(egg, d);
    applyDamage(egg, r, p.damage);
    Vec2 push = normVec(p.vel);
    egg.vel = addVec(egg.vel, mulVec(push, 7.4f));
    egg.pos = addVec(egg.pos, mulVec(push, 12.0f));
    egg.hitFlashFrames = 10;

    log1 = p.owner == 0 ? L"左侧鸡蛋的蛋黄命中" : L"右侧鸡蛋的蛋黄命中";
    log2 = wstring(getRegionName(r)) + L"被高速蛋黄砸中";
    log3 = L"这一下带来了明显的远程击退";
}

void updateYolkProjectiles(EggEntity& leftEgg, EggEntity& rightEgg, wstring& log1, wstring& log2, wstring& log3) {
    for (int i = 0; i < 2; ++i) {
        YolkProjectile& p = gYolkShots[i];
        if (!p.active) continue;

        p.pos = addVec(p.pos, p.vel);
        p.lifeFrames--;
        if (p.lifeFrames <= 0) {
            p.active = false;
            continue;
        }

        if (p.pos.x < ARENA_L || p.pos.x > ARENA_R || p.pos.y < ARENA_T || p.pos.y > ARENA_B) {
            p.active = false;
            continue;
        }

        bool hitObstacle = false;
        for (int k = 0; k < gObstacleCount; ++k) {
            if (yolkHitsObstacle(p, gObstacles[k])) {
                hitObstacle = true;
                break;
            }
        }
        if (hitObstacle) {
            p.active = false;
            continue;
        }

        EggEntity& target = p.owner == 0 ? rightEgg : leftEgg;
        if (distanceVec(p.pos, target.pos) < target.radius + p.radiusX * 0.70f) {
            applyYolkHitToEgg(target, p, log1, log2, log3);
            p.active = false;
        }
    }
}


void drawArenaObstacles() {
    for (int i = 0; i < gObstacleCount; ++i) {
        if (!gObstacles[i].active) continue;
        int cx = (int)gObstacles[i].pos.x;
        int cy = (int)gObstacles[i].pos.y;
        int r = (int)gObstacles[i].radius;

        setlinecolor(RGB(88, 78, 64));
        setfillcolor(RGB(124, 118, 110));
        solidcircle(cx, cy, r);
        circle(cx, cy, r);

        setlinecolor(RGB(170, 164, 150));
        circle(cx, cy, r - 5);
        line(cx - r / 3, cy - r / 3, cx + r / 4, cy + r / 5);
    }
}

void drawSpikeHazard() {
    if (!isSpikeMode()) return;

    for (int i = 0; i < gSpikeCount; ++i) {
        FallingSpike& sp = gSpikes[i];
        if (sp.state == SPIKE_IDLE) continue;

        if (sp.state == SPIKE_WARNING) {
            setlinecolor(RGB(180, 40, 40));
            int left = (int)(sp.x - sp.halfWidth * 0.70f);
            int right = (int)(sp.x + sp.halfWidth * 0.70f);
            line(left, ARENA_T + 10, left, ARENA_B - 10);
            line(right, ARENA_T + 10, right, ARENA_B - 10);
            setfillcolor(RGB(220, 64, 64));
            solidrectangle(left, ARENA_T + 18, right, ARENA_T + 34);
            continue;
        }

        int tipX = (int)sp.x;
        int tipY = (int)sp.y;
        POINT tri[3];
        tri[0] = { tipX, tipY };
        tri[1] = { (int)(tipX - sp.halfWidth), (int)(tipY - sp.height) };
        tri[2] = { (int)(tipX + sp.halfWidth), (int)(tipY - sp.height) };
        setlinecolor(RGB(95, 95, 100));
        setfillcolor(RGB(184, 184, 190));
        solidpolygon(tri, 3);
        polygon(tri, 3);
        line(tipX, ARENA_T + 2, tipX, (int)(tipY - sp.height));
    }
}

void startSpikeWarning(const EggEntity& leftEgg, const EggEntity& rightEgg) {
    int burstCount = gSpikeCount;
    float scale = getSpikeScale();

    for (int i = 0; i < burstCount; ++i) {
        gSpikes[i].state = SPIKE_WARNING;
        gSpikes[i].framesLeft = 46;
        gSpikes[i].y = (float)(ARENA_T - 60);
        gSpikes[i].speed = (gArenaMode == ARENA_NIGHTMARE) ? 21.0f : 18.0f;
        gSpikes[i].hitLeft = false;
        gSpikes[i].hitRight = false;
        gSpikes[i].damage = gSpikeDamage;
        gSpikes[i].halfWidth = 12.0f * scale;
        gSpikes[i].height = 42.0f * scale;
    }

    for (int i = 0; i < burstCount; ++i) {
        bool ok = false;
        for (int tries = 0; tries < 400 && !ok; ++tries) {
            float x = (float)randInt(ARENA_L + 90, ARENA_R - 90);
            ok = true;
            for (int k = 0; k < i; ++k) {
                if (fabsf(x - gSpikes[k].x) < gSpikes[k].halfWidth + gSpikes[i].halfWidth + 130.0f) {
                    ok = false;
                    break;
                }
            }
            if (ok) gSpikes[i].x = x;
        }
        if (!ok) gSpikes[i].x = (float)(ARENA_L + 140 + i * ((ARENA_R - ARENA_L - 280) / (burstCount > 1 ? burstCount - 1 : 1)));
        spawnSpikeHealthPack(i, leftEgg, rightEgg);
    }
}

void applySpikeHit(EggEntity& egg, FallingSpike& sp, bool& hitFlag, wstring& log1, wstring& log2, wstring& log3) {
    if (hitFlag) return;
    Vec2 tip = makeVec(sp.x, sp.y - 8.0f);
    Vec2 d = subVec(tip, egg.pos);
    if (fabsf(egg.pos.x - sp.x) > sp.halfWidth + egg.radius * 0.55f) return;
    if (lenVec(d) > egg.radius + sp.halfWidth * 0.35f + 10.0f) return;

    ContactRegion r = getContactRegion(egg, d);
    applyDamage(egg, r, sp.damage);
    egg.vel.y += 4.4f;
    if (egg.vel.y > 8.0f) egg.vel.y = 8.0f;
    egg.vel.x += (egg.pos.x >= sp.x ? 1.0f : -1.0f) * 2.2f;
    hitFlag = true;

    log1 = L"尖刺自上方坠落";
    log2 = wstring(getRegionName(r)) + L"被尖刺命中";
    log3 = L"这一击带着很强的穿透力";
}

void updateSpikeSystem(EggEntity& leftEgg, EggEntity& rightEgg, wstring& log1, wstring& log2, wstring& log3) {
    if (!isSpikeMode()) return;

    bool anyActive = false;
    for (int i = 0; i < gSpikeCount; ++i) {
        if (gSpikes[i].state != SPIKE_IDLE) {
            anyActive = true;
            break;
        }
    }

    if (!anyActive) {
        gNextSpikeFrames--;
        if (gNextSpikeFrames <= 0) {
            startSpikeWarning(leftEgg, rightEgg);
        }
        return;
    }

    bool allIdle = true;
    for (int i = 0; i < gSpikeCount; ++i) {
        FallingSpike& sp = gSpikes[i];
        if (sp.state == SPIKE_IDLE) continue;
        allIdle = false;

        if (sp.state == SPIKE_WARNING) {
            sp.framesLeft--;
            if (sp.framesLeft <= 0) {
                sp.state = SPIKE_FALLING;
                sp.y = (float)(ARENA_T - 40);
                playEffect(L"hit_light.wav");
            }
            continue;
        }

        if (sp.state == SPIKE_FALLING) {
            sp.y += sp.speed;
            applySpikeHit(leftEgg, sp, sp.hitLeft, log1, log2, log3);
            applySpikeHit(rightEgg, sp, sp.hitRight, log1, log2, log3);

            if (sp.y >= ARENA_B + 20) {
                sp.state = SPIKE_LINGER;
                sp.framesLeft = 40;
                clearHealthPackForSpike(i);
            }
            continue;
        }

        if (sp.state == SPIKE_LINGER) {
            sp.framesLeft--;
            if (sp.framesLeft <= 0) {
                sp.state = SPIKE_IDLE;
            }
        }
    }

    bool afterAllIdle = true;
    for (int i = 0; i < gSpikeCount; ++i) {
        if (gSpikes[i].state != SPIKE_IDLE) {
            afterAllIdle = false;
            break;
        }
    }
    if (afterAllIdle) {
        gNextSpikeFrames = randInt(getSpikeIntervalMinFrames(), getSpikeIntervalMaxFrames());
    }
}

void handleObstacleCollision(EggEntity& e, const ArenaObstacle& ob, wstring& log1, wstring& log2, wstring& log3) {
    if (!ob.active) return;

    Vec2 diff = subVec(e.pos, ob.pos);
    float dist = lenVec(diff);
    float minDist = e.radius + ob.radius;
    if (dist < 0.001f) {
        diff = makeVec(1.0f, 0.0f);
        dist = 1.0f;
    }
    if (dist >= minDist) return;

    Vec2 n = normVec(diff);
    float overlap = minDist - dist;
    e.pos = addVec(e.pos, mulVec(n, overlap));

    float toward = dotVec(e.vel, mulVec(n, -1.0f));
    if (toward > 0.0f) {
        e.vel = addVec(e.vel, mulVec(n, toward * 1.25f));
    }

    float impactSpeed = lenVec(e.vel) + toward;
    if (impactSpeed > 4.0f && e.wallCooldown <= 0) {
        ContactRegion r = getContactRegion(e, mulVec(n, -1.0f));
        int dmg = (impactSpeed > 6.5f) ? 2 : 1;
        applyDamage(e, r, dmg);
        e.wallCooldown = 12;
        log1 = L"撞上了场地刚体";
        log2 = wstring(getRegionName(r)) + L"承受了硬碰硬冲击";
        log3 = (dmg >= 2) ? L"这一下很疼" : L"所幸只是轻度擦撞";
    }
}

void updateArenaObstacles(EggEntity& leftEgg, EggEntity& rightEgg, wstring& log1, wstring& log2, wstring& log3) {
    for (int i = 0; i < gObstacleCount; ++i) {
        handleObstacleCollision(leftEgg, gObstacles[i], log1, log2, log3);
        handleObstacleCollision(rightEgg, gObstacles[i], log1, log2, log3);
    }
}

void drawArenaBackground() {
    drawSceneBackground(RGB(188, 198, 212), RGB(110, 132, 158), RGB(158, 138, 110), RGB(96, 74, 46));

    setfillcolor(RGB(161, 142, 108));
    solidrectangle(ARENA_L, ARENA_T, ARENA_R, ARENA_B);

    setfillcolor(RGB(188, 169, 132));
    solidrectangle(ARENA_L + 12, ARENA_T + 12, ARENA_R - 12, ARENA_B - 12);

    setlinecolor(RGB(106, 84, 54));
    rectangle(ARENA_L, ARENA_T, ARENA_R, ARENA_B);
    rectangle(ARENA_L + 12, ARENA_T + 12, ARENA_R - 12, ARENA_B - 12);
    drawCornerOrnaments(ARENA_L + 4, ARENA_T + 4, ARENA_R - 4, ARENA_B - 4, RGB(128, 100, 60));

    setlinecolor(RGB(152, 136, 108));
    for (int y = ARENA_T + 30; y < ARENA_B - 10; y += 24) {
        line(ARENA_L + 20, y, ARENA_R - 20, y);
    }
    for (int x = ARENA_L + 28; x < ARENA_R - 20; x += 82) {
        line(x, ARENA_T + 18, x + 24, ARENA_T + 18);
    }

    drawArenaObstacles();
    drawHealthPack();
    drawSkillPack();
    drawSpikeHazard();
    for (int i = 0; i < 2; ++i) drawYolkProjectile(gYolkShots[i]);
}

void drawMenuScene(const Button& btnPVP, const Button& btnPVE, const Button& btnExit) {
    drawSceneBackground(RGB(196, 208, 222), RGB(124, 146, 170), RGB(168, 150, 122), RGB(62, 79, 99));

    int panelL = SCREEN_W / 2 - 300;
    int panelR = SCREEN_W / 2 + 300;
    int panelT = SCREEN_H / 2 - 210;
    int panelB = SCREEN_H / 2 + 200;

    drawCenteredTextAtY(SCREEN_W / 2, 50, L"蛋尖对决", 62, BLACK);
    drawSectionCaption(SCREEN_W / 2, 120, 190, L"实时竞技模式", 24, RGB(70, 76, 86), RGB(122, 106, 82));

    drawPanel(panelL, panelT, panelR, panelB, RGB(246, 244, 239), RGB(118, 128, 140), RGB(165, 150, 120));
    drawCenteredTextAtY(SCREEN_W / 2, panelT + 26, L"开始游戏", 34, RGB(58, 62, 66));
    drawCenteredTextAtY(SCREEN_W / 2, panelT + 72, L"选择模式后进入地图与选蛋阶段", 22, RGB(88, 88, 88));

    drawButton(btnPVP, true);
    drawButton(btnPVE, true);
    drawButton(btnExit, true);
}

void drawMapSelectScene(const Button& btnBasic, const Button& btnAdvanced, const Button& btnNightmare, const Button& btnBack, int currentMode) {
    drawSceneBackground(RGB(198, 210, 223), RGB(126, 148, 172), RGB(171, 152, 124), RGB(64, 80, 100));

    int panelL = SCREEN_W / 2 - 360;
    int panelR = SCREEN_W / 2 + 360;
    int panelT = SCREEN_H / 2 - 245;
    int panelB = SCREEN_H / 2 + 250;

    drawPanel(panelL, panelT, panelR, panelB, RGB(246, 244, 239), RGB(118, 128, 140), RGB(165, 150, 120));

    drawCenteredTextAtY(SCREEN_W / 2, panelT + 24, L"选择地图", 48, BLACK);
    drawCenteredTextAtY(SCREEN_W / 2, panelT + 86, currentMode == 1 ? L"双人对战" : L"人机对战", 26, RGB(58, 58, 58));
    drawCenteredTextAtY(SCREEN_W / 2, panelT + 124, L"不同地图将决定障碍与环境风险", 22, RGB(88, 88, 88));

    drawButton(btnBasic, true);
    drawButton(btnAdvanced, true);
    drawButton(btnNightmare, true);
    drawButton(btnBack, true);
}

void drawSelectScene(
    const Button& btnEgg1,
    const Button& btnEgg2,
    const Button& btnEgg3,
    const Button& btnBack,
    int currentMode,
    int selectStep,
    const EggRuntimeState candidates[3]
) {
    drawSceneBackground(RGB(200, 212, 224), RGB(130, 151, 174), RGB(173, 155, 128), RGB(76, 68, 52));

    if (currentMode == 1 && selectStep == 2) {
        drawCenteredTextAtY(SCREEN_W / 2, 22, L"玩家2 选择鸡蛋", 42, BLACK);
    }
    else {
        drawCenteredTextAtY(SCREEN_W / 2, 22, L"玩家1 选择鸡蛋", 42, BLACK);
    }

    drawCenteredTextAtY(SCREEN_W / 2, 78, L"观察线索、判断手感，再决定本局要上场的那一枚蛋", 24, RGB(58, 58, 58));
    drawCenteredTextAtY(SCREEN_W / 2, 112, getArenaModeText(gArenaMode), 22, RGB(84, 72, 54));
    drawCenteredTextAtY(SCREEN_W / 2, 142, L"点击蛋卡本体可进入放大检视", 22, RGB(84, 84, 84));

    drawEggPreviewCard(CARD_X[0], CARD_Y, candidates[0], L"1号蛋", 0.0f);
    drawEggPreviewCard(CARD_X[1], CARD_Y, candidates[1], L"2号蛋", 1.3f);
    drawEggPreviewCard(CARD_X[2], CARD_Y, candidates[2], L"3号蛋", 2.2f);

    drawButton(btnEgg1, true);
    drawButton(btnEgg2, true);
    drawButton(btnEgg3, true);
    drawButton(btnBack, true);
}

void drawInspectScene(
    const Button& btnRotL,
    const Button& btnRotR,
    const Button& btnPick,
    const Button& btnBack,
    const EggRuntimeState& egg,
    int inspectIndex,
    float inspectAngle
) {
    drawSceneBackground(RGB(198, 210, 224), RGB(129, 150, 173), RGB(173, 154, 126), RGB(82, 72, 54));

    drawCenteredTextAtY(SCREEN_W / 2, 18, L"放大检视", 46, BLACK);

    wstring title = L"正在检视：" + toW(inspectIndex + 1) + L"号蛋";
    drawCenteredTextAtY(SCREEN_W / 2, 72, title.c_str(), 28, RGB(55, 55, 55));

    int leftL = SCREEN_W / 2 - 455;
    int leftR = SCREEN_W / 2 - 15;
    int rightL = SCREEN_W / 2 + 20;
    int rightR = SCREEN_W / 2 + 425;

    drawPanel(leftL, 118, leftR, SCREEN_H - 126, RGB(247, 245, 240), RGB(132, 122, 104), RGB(170, 152, 120));

    EggEntity preview = makeBattleEgg(egg, (float)((leftL + leftR) / 2), (float)(SCREEN_H / 2 - 18 + idleBob(0.8f, 4.0f)), inspectAngle);
    drawEggShadow(preview, 1.98f);
    drawEggShape(preview, 1.98f);

    drawCenteredTextAtY((leftL + leftR) / 2, SCREEN_H - 232, L"这里更强调蛋形、角度和尖钝端差异", 22, RGB(76, 76, 76));

    drawPanel(rightL, 118, rightR, SCREEN_H - 126, RGB(247, 245, 240), RGB(132, 122, 104), RGB(170, 152, 120));

    drawCenteredTextAtY((rightL + rightR) / 2, 140, L"检视线索", 30, BLACK);

    setUIFontAndColor(22, BLACK);
    for (int i = 0; i < egg.clueCount; ++i) {
        outtextxy(rightL + 28, 194 + i * 38, egg.clues[i].c_str());
    }

    setUIFontAndColor(24, BLACK);
    outtextxy(rightL + 28, 456, L"轻敲反馈：");
    outtextxy(rightL + 28, 494, fitTextToWidth(getTapFeedback(egg), rightR - rightL - 56).c_str());

    drawButton(btnRotL, true);
    drawButton(btnRotR, true);
    drawButton(btnPick, true);
    drawButton(btnBack, true);
}

void drawBattleScene(
    const Button& btnBack,
    int currentMode,
    int openingFrames,
    const EggEntity& leftEgg,
    const EggEntity& rightEgg,
    const wstring& log1,
    const wstring& log2,
    const wstring& log3,
    int shakeFrames,
    int shakeStrength
) {
    drawArenaBackground();

    int ox = 0;
    int oy = 0;
    if (shakeFrames > 0 && shakeStrength > 0) {
        ox = randInt(-shakeStrength, shakeStrength);
        oy = randInt(-shakeStrength, shakeStrength);
    }

    EggEntity l = leftEgg;
    EggEntity r = rightEgg;
    l.pos.x += (float)ox;
    l.pos.y += (float)oy;
    r.pos.x += (float)ox;
    r.pos.y += (float)oy;

    drawCenteredTextAtY(SCREEN_W / 2, 12, L"对战中", 36, BLACK);
    drawCenteredTextAtY(SCREEN_W / 2, 52, getArenaModeText(gArenaMode), 22, RGB(70, 70, 70));
    if (currentMode == 1) {
        setUIFontAndColor(20, BLACK);
        outtextxy(96, 24, L"P1: WASD 移动，Q/E 微调，G 加速，F 发射蛋黄");
        outtextxy(SCREEN_W - 520, 24, L"P2: 方向键移动，K/L 微调，RShift 加速，右Ctrl 发射蛋黄");
    }
    else {
        setUIFontAndColor(20, BLACK);
        outtextxy(82, 24, L"你: WASD 移动，Q/E 微调，G 加速，F 发射蛋黄，按住鼠标左键可辅助转向");
        outtextxy(SCREEN_W - 120, 24, L"AI");
    }

    if (openingFrames > 0) {
        setUIFontAndColor(24, RGB(80, 40, 20));
        outtextxy(SCREEN_W / 2 - 154, 84, L"开局试探期：暂时不能加速");
    }

    drawStaminaBar(110, 60, 260, 24, leftEgg.stamina / leftEgg.maxStamina);
    drawIntegrityBar(110, 92, 260, 18, (float)leftEgg.totalIntegrity / (float)leftEgg.totalMaxIntegrity);
    drawStaminaBar(SCREEN_W - 370, 60, 260, 24, rightEgg.stamina / rightEgg.maxStamina);
    drawIntegrityBar(SCREEN_W - 370, 92, 260, 18, (float)rightEgg.totalIntegrity / (float)rightEgg.totalMaxIntegrity);

    setUIFontAndColor(20, RGB(70, 56, 18));
    outtextxy(110, 116, gLeftHasSkillShot ? L"左侧技能：蛋黄已就绪" : L"左侧技能：未持有");
    outtextxy(SCREEN_W - 370, 116, gRightHasSkillShot ? L"右侧技能：蛋黄已就绪" : L"右侧技能：未持有");

    float ls = 1.0f + leftEgg.hitFlashFrames * 0.006f;
    float rs = 1.0f + rightEgg.hitFlashFrames * 0.006f;

    drawEggShadow(l, ls);
    drawEggShape(l, ls);
    drawEggCracks(l, ls);

    drawEggShadow(r, rs);
    drawEggShape(r, rs);
    drawEggCracks(r, rs);

    drawBattleLogBox(SCREEN_W / 2 - 390, SCREEN_H - 182, 780, 140, log1, log2, log3);
    drawButton(btnBack, true);
}

void drawResultScene(
    const Button& btnReplay,
    const Button& btnMenu,
    int currentMode,
    int winnerState,
    const EggRuntimeState& leftDraft,
    const EggRuntimeState& rightDraft,
    const EggEntity& leftEgg,
    const EggEntity& rightEgg,
    const wstring& log1,
    const wstring& log2,
    const wstring& log3
) {
    drawSceneBackground(RGB(198, 210, 224), RGB(129, 150, 173), RGB(173, 154, 126), RGB(82, 72, 54));

    wstring winnerText = L"本局平局";
    if (winnerState == 1) winnerText = (currentMode == 1) ? L"玩家1 获胜" : L"你获胜了";
    if (winnerState == 2) winnerText = (currentMode == 1) ? L"玩家2 获胜" : L"电脑获胜";

    drawCenteredTextAtY(SCREEN_W / 2, 18, L"对战结果", 44, BLACK);
    drawCenteredTextAtY(SCREEN_W / 2, 76, winnerText.c_str(), 34, RGB(62, 56, 48));

    float pulse = 1.0f + 0.03f * sinf(nowTimeSec() * 5.0f);
    float leftMul = 1.0f;
    float rightMul = 1.0f;
    if (winnerState == 1) leftMul = pulse;
    if (winnerState == 2) rightMul = pulse;

    int leftPanelX = SCREEN_W / 2 - 310 - 270;
    int rightPanelX = SCREEN_W / 2 + 40;
    drawBattleEggPanel(leftPanelX, 135, L"左侧最终状态", leftEgg, leftMul);
    drawBattleEggPanel(rightPanelX, 135, L"右侧最终状态", rightEgg, rightMul);

    setUIFontAndColor(20, BLACK);
    outtextxy(leftPanelX + 20, 472, getEggSummary(leftDraft).c_str());
    outtextxy(rightPanelX + 20, 472, getEggSummary(rightDraft).c_str());
    outtextxy(leftPanelX + 20, 502, fitTextToWidth(getEggRevealLine2(leftDraft), 236).c_str());
    outtextxy(rightPanelX + 20, 502, fitTextToWidth(getEggRevealLine2(rightDraft), 236).c_str());

    drawBattleLogBox(SCREEN_W / 2 - 390, 530, 780, 132, log1, log2, log3);

    drawButton(btnReplay, true);
    drawButton(btnMenu, true);
}

void updateHumanControl(
    EggEntity& e,
    int upKey,
    int downKey,
    int leftKey,
    int rightKey,
    int turnLKey,
    int turnRKey,
    int boostKey,
    bool allowMouseAim,
    bool allowBoost,
    int mouseX,
    int mouseY
) {
    int mx = 0;
    int my = 0;

    if (GetAsyncKeyState(leftKey) & 0x8000) mx -= 1;
    if (GetAsyncKeyState(rightKey) & 0x8000) mx += 1;
    if (GetAsyncKeyState(upKey) & 0x8000) my -= 1;
    if (GetAsyncKeyState(downKey) & 0x8000) my += 1;

    Vec2 moveDir = makeVec((float)mx, (float)my);
    float moveLen = lenVec(moveDir);
    if (moveLen > 0.01f) moveDir = normVec(moveDir);

    bool wantsBoost = allowBoost &&
        ((GetAsyncKeyState(boostKey) & 0x8000) != 0) &&
        (moveLen > 0.01f) &&
        (e.stamina > 1.5f);

    e.boosting = wantsBoost;

    if (moveLen > 0.01f) {
        float desiredAngle = (float)atan2((double)moveDir.y, (double)moveDir.x);
        float diff = angleDiff(desiredAngle, e.angle);

        float autoTurnSpeed = 0.16f * 0.6f;
        if (mx != 0 && my != 0) autoTurnSpeed *= 0.92f;
        if (e.boosting) autoTurnSpeed *= 0.84f;

        if (diff > autoTurnSpeed) diff = autoTurnSpeed;
        if (diff < -autoTurnSpeed) diff = -autoTurnSpeed;

        e.angle = normalizeAngle(e.angle + diff);
    }

    float microTurn = 0.042f * 0.6f;
    if (GetAsyncKeyState(turnLKey) & 0x8000) e.angle -= microTurn;
    if (GetAsyncKeyState(turnRKey) & 0x8000) e.angle += microTurn;

    if (allowMouseAim && (GetAsyncKeyState(VK_LBUTTON) & 0x8000)) {
        float mouseAngle = (float)atan2((double)(mouseY - e.pos.y), (double)(mouseX - e.pos.x));
        float diff = angleDiff(mouseAngle, e.angle);
        float mouseTurnSpeed = 0.19f * 0.65f;
        if (diff > mouseTurnSpeed) diff = mouseTurnSpeed;
        if (diff < -mouseTurnSpeed) diff = -mouseTurnSpeed;
        e.angle = normalizeAngle(e.angle + diff);
    }

    e.angle = normalizeAngle(e.angle);

    float baseSpeed = 2.85f;
    float boostMul = 2.22f;
    float finalSpeed = wantsBoost ? baseSpeed * boostMul : baseSpeed;

    Vec2 targetVel = mulVec(moveDir, finalSpeed);

    if (moveLen > 0.01f) {
        e.vel = addVec(mulVec(e.vel, 0.73f), mulVec(targetVel, 0.27f));
    }
    else {
        e.vel = mulVec(e.vel, 0.84f);
    }

    if (wantsBoost) {
        e.stamina -= 0.735f;
    }
    else {
        if (moveLen > 0.01f) e.stamina += 0.35f;
        else e.stamina += 0.62f;
    }

    e.stamina = clampFloat(e.stamina, 0.0f, e.maxStamina);
    e.pos = addVec(e.pos, e.vel);
}


Vec2 computeAIDangerVector(const EggEntity& ai) {
    Vec2 avoid = makeVec(0.0f, 0.0f);

    float wallMargin = 95.0f;
    if (ai.pos.x < ARENA_L + wallMargin) avoid.x += (ARENA_L + wallMargin - ai.pos.x) / wallMargin;
    if (ai.pos.x > ARENA_R - wallMargin) avoid.x -= (ai.pos.x - (ARENA_R - wallMargin)) / wallMargin;
    if (ai.pos.y < ARENA_T + wallMargin) avoid.y += (ARENA_T + wallMargin - ai.pos.y) / wallMargin;
    if (ai.pos.y > ARENA_B - wallMargin) avoid.y -= (ai.pos.y - (ARENA_B - wallMargin)) / wallMargin;

    for (int i = 0; i < gObstacleCount; ++i) {
        if (!gObstacles[i].active) continue;
        Vec2 d = subVec(ai.pos, gObstacles[i].pos);
        float dist = lenVec(d);
        float safe = ai.radius + gObstacles[i].radius + 78.0f;
        if (dist < safe && dist > 0.01f) {
            float k = (safe - dist) / safe;
            avoid = addVec(avoid, mulVec(normVec(d), 1.35f * k));
        }
    }

    if (isSpikeMode()) {
        for (int i = 0; i < gSpikeCount; ++i) {
            FallingSpike& sp = gSpikes[i];
            if (sp.state == SPIKE_WARNING) {
                float dx = ai.pos.x - sp.x;
                if (fabsf(dx) < sp.halfWidth + 90.0f) {
                    avoid.x += dx >= 0.0f ? 1.35f : -1.35f;
                }
            } else if (sp.state == SPIKE_FALLING || sp.state == SPIKE_LINGER) {
                float dx = ai.pos.x - sp.x;
                float dy = ai.pos.y - sp.y;
                if (fabsf(dx) < sp.halfWidth + 60.0f && fabsf(dy) < sp.height + 110.0f) {
                    avoid.x += dx >= 0.0f ? 1.45f : -1.45f;
                }
            }
        }
    }

    return avoid;
}

void startAIRecover() {
    gAIState = AI_RECOVER;
    gAIRecoverFrames = randInt(38, 68);
    gAIAttackFrames = 0;
}

void updateAIOrbitRhythm() {
    gAIOrbitFrames--;
    if (gAIOrbitFrames <= 0) {
        gAIOrbitFrames = randInt(130, 230);
        gAIOrbitBias = randInt(0, 1) == 0 ? -1.0f : 1.0f;
    }
}

void updateAIControl(EggEntity& ai, const EggEntity& target, bool allowBoost) {
    updateAIOrbitRhythm();

    float level = getAIStrengthFactor();
    Vec2 toTarget = subVec(target.pos, ai.pos);
    float dist = lenVec(toTarget);
    Vec2 dir = normVec(toTarget);
    Vec2 side = makeVec(-dir.y, dir.x);

    if (ai.hitFlashFrames >= (level >= 2.0f ? 5 : 6) && gAIState != AI_RECOVER) {
        startAIRecover();
    }

    Vec2 dangerVec = computeAIDangerVector(ai);
    bool hasDanger = lenVec(dangerVec) > 0.12f;

    if (hasDanger) {
        gAIState = AI_AVOID;
        gAIAvoidFrames = level >= 2.0f ? 22 : 16;
    } else if (gAIState == AI_AVOID) {
        gAIAvoidFrames--;
        if (gAIAvoidFrames <= 0) gAIState = AI_PATROL;
    }

    if (gAINextAttackFrames > 0) gAINextAttackFrames--;
    int nextMin = 360, nextMax = 540;
    if (gArenaMode == ARENA_ADVANCED) { nextMin = 240; nextMax = 360; }
    if (gArenaMode == ARENA_NIGHTMARE) { nextMin = 150; nextMax = 240; }

    float attackStaminaNeed = 26.0f;
    if (gArenaMode == ARENA_ADVANCED) attackStaminaNeed = 20.0f;
    if (gArenaMode == ARENA_NIGHTMARE) attackStaminaNeed = 16.0f;

    if (gAIState == AI_PATROL &&
        gAINextAttackFrames <= 0 &&
        allowBoost &&
        ai.stamina > attackStaminaNeed &&
        dist > 155.0f &&
        dist < (gArenaMode == ARENA_NIGHTMARE ? 620.0f : 540.0f) &&
        !hasDanger) {
        gAIState = AI_ATTACK;
        gAIAttackFrames = (gArenaMode == ARENA_NIGHTMARE) ? randInt(82, 126) : (gArenaMode == ARENA_ADVANCED ? randInt(68, 104) : randInt(54, 84));
        gAINextAttackFrames = randInt(nextMin, nextMax);
    }

    if (gAIState == AI_ATTACK) {
        gAIAttackFrames--;
        if (gAIAttackFrames <= 0 || dist < 90.0f || hasDanger) {
            gAIState = AI_PATROL;
        }
    }

    if (gAIState == AI_RECOVER) {
        gAIRecoverFrames--;
        if (gAIRecoverFrames <= 0) gAIState = AI_PATROL;
    }

    Vec2 moveDir = makeVec(0.0f, 0.0f);
    float desiredAngle = ai.angle;
    float turnSpeed = 0.043f;
    float baseSpeed = 2.55f;
    bool wantsBoost = false;

    Vec2 targetForward = eggForward(target);
    Vec2 weakAim = mulVec(targetForward, -1.0f);
    float weakBias = dotVec(side, weakAim);

    if (gAIState == AI_AVOID) {
        Vec2 extra = mulVec(side, 0.35f * gAIOrbitBias);
        moveDir = normVec(addVec(dangerVec, extra));
        desiredAngle = (float)atan2((double)moveDir.y, (double)moveDir.x);
        turnSpeed = 0.060f + 0.010f * level;
        baseSpeed = 3.05f + 0.20f * level;
    } else if (gAIState == AI_RECOVER) {
        moveDir = normVec(addVec(mulVec(dir, -0.90f), mulVec(side, (0.82f + 0.08f * level) * gAIOrbitBias)));
        desiredAngle = (float)atan2((double)moveDir.y, (double)moveDir.x) + (0.10f - 0.02f * level) * gAIOrbitBias;
        turnSpeed = 0.052f + 0.008f * level;
        baseSpeed = 2.90f + 0.10f * level;
    } else if (gAIState == AI_ATTACK) {
        float attackBias = 0.16f * gAIOrbitBias + 0.18f * weakBias + 0.03f * sinf(nowTimeSec() * (2.4f + 0.2f * level));
        moveDir = normVec(addVec(dir, mulVec(side, attackBias)));
        desiredAngle = (float)atan2((double)moveDir.y, (double)moveDir.x);
        turnSpeed = 0.056f + 0.010f * level;
        baseSpeed = 3.15f + 0.18f * level;
        bool aligned = fabsf(angleDiff(desiredAngle, ai.angle)) < (0.30f - 0.04f * level);
        wantsBoost = allowBoost && aligned && ai.stamina > (22.0f - 3.0f * level) && dist > 135.0f && dist < 360.0f + 40.0f * level;
    } else {
        if (dist > 380.0f - 20.0f * level) {
            moveDir = normVec(addVec(mulVec(dir, 0.92f), mulVec(side, (0.22f + 0.05f * level) * gAIOrbitBias)));
        } else if (dist < 170.0f + 10.0f * level) {
            moveDir = normVec(addVec(mulVec(dir, -(0.72f + 0.06f * level)), mulVec(side, (0.82f + 0.08f * level) * gAIOrbitBias)));
        } else {
            moveDir = normVec(addVec(mulVec(dir, 0.20f + 0.05f * level), mulVec(side, (1.02f - 0.08f * level) * gAIOrbitBias)));
        }
        desiredAngle = (float)atan2((double)moveDir.y, (double)moveDir.x);
        turnSpeed = 0.046f + 0.008f * level;
        baseSpeed = 2.60f + 0.12f * level;
    }

    float d = angleDiff(desiredAngle, ai.angle);
    if (d > turnSpeed) d = turnSpeed;
    if (d < -turnSpeed) d = -turnSpeed;
    ai.angle = normalizeAngle(ai.angle + d);

    ai.boosting = wantsBoost;
    float finalSpeed = wantsBoost ? baseSpeed * (1.98f + 0.05f * level) : baseSpeed;
    Vec2 targetVel = mulVec(moveDir, finalSpeed);
    ai.vel = addVec(mulVec(ai.vel, 0.73f), mulVec(targetVel, 0.27f));

    if (wantsBoost) ai.stamina -= (0.66f + 0.04f * level);
    else ai.stamina += (dist < 150.0f ? 0.52f : 0.31f);
    ai.stamina = clampFloat(ai.stamina, 0.0f, ai.maxStamina);

    ai.pos = addVec(ai.pos, ai.vel);
}

ContactRegion wallContactRegion(const EggEntity& e, Vec2 wallDir) {
    return getContactRegion(e, wallDir);
}

void handleWallCollision(EggEntity& e, wstring& log1, wstring& log2, wstring& log3) {
    bool hit = false;
    Vec2 wallDir = makeVec(0.0f, 0.0f);
    float impactSpeed = lenVec(e.vel);

    if (e.pos.x < ARENA_L + e.radius) {
        e.pos.x = ARENA_L + e.radius;
        e.vel.x *= -0.35f;
        hit = true;
        wallDir = makeVec(-1.0f, 0.0f);
    }
    else if (e.pos.x > ARENA_R - e.radius) {
        e.pos.x = ARENA_R - e.radius;
        e.vel.x *= -0.35f;
        hit = true;
        wallDir = makeVec(1.0f, 0.0f);
    }

    if (e.pos.y < ARENA_T + e.radius) {
        e.pos.y = ARENA_T + e.radius;
        e.vel.y *= -0.35f;
        hit = true;
        wallDir = makeVec(0.0f, -1.0f);
    }
    else if (e.pos.y > ARENA_B - e.radius) {
        e.pos.y = ARENA_B - e.radius;
        e.vel.y *= -0.35f;
        hit = true;
        wallDir = makeVec(0.0f, 1.0f);
    }

    if (hit && e.wallCooldown <= 0 && impactSpeed > 4.0f) {
        ContactRegion r = wallContactRegion(e, wallDir);
        int dmg = (impactSpeed > 6.0f) ? 2 : 1;
        applyDamage(e, r, dmg);
        e.wallCooldown = 12;

        log1 = L"\u9ad8\u901f\u78d5\u5230\u8fb9\u754c";
        log2 = wstring(getRegionName(r)) + L"\u627f\u53d7\u4e86\u51b2\u51fb";
        log3 = (dmg >= 2) ? L"\u8fd9\u4e00\u649e\u975e\u5e38\u91cd" : L"\u8fd8\u597d\u53ea\u662f\u8f7b\u5ea6\u78b0\u58c1";
    }
}

void resolveEggCollision(
    EggEntity& leftEgg,
    EggEntity& rightEgg,
    int& hitPauseFrames,
    int& slowMotionFrames,
    int& shakeFrames,
    int& shakeStrength,
    wstring& log1,
    wstring& log2,
    wstring& log3
) {
    Vec2 diff = subVec(rightEgg.pos, leftEgg.pos);
    float dist = lenVec(diff);
    float minDist = leftEgg.radius + rightEgg.radius;

    if (dist < 0.001f) {
        diff = makeVec(1.0f, 0.0f);
        dist = 1.0f;
    }

    if (dist < minDist) {
        Vec2 n = normVec(diff);
        float overlap = minDist - dist;

        leftEgg.pos = addVec(leftEgg.pos, mulVec(n, -overlap * 0.5f));
        rightEgg.pos = addVec(rightEgg.pos, mulVec(n, overlap * 0.5f));

        if (leftEgg.collisionCooldown <= 0 && rightEgg.collisionCooldown <= 0) {
            float closing = fabsf(dotVec(subVec(rightEgg.vel, leftEgg.vel), n));

            ContactRegion leftRegion = getContactRegion(leftEgg, n);
            ContactRegion rightRegion = getContactRegion(rightEgg, mulVec(n, -1.0f));

            float leftOffense = (float)getAttackByRegion(leftEgg, leftRegion) + closing * 0.88f + (leftEgg.boosting ? 1.35f : 0.0f);
            float rightOffense = (float)getAttackByRegion(rightEgg, rightRegion) + closing * 0.88f + (rightEgg.boosting ? 1.35f : 0.0f);

            float leftDefense = (float)getDefenseByRegion(leftEgg, leftRegion);
            float rightDefense = (float)getDefenseByRegion(rightEgg, rightRegion);

            if (leftRegion == REGION_SHARP && rightRegion == REGION_BLUNT) leftOffense += 1.10f;
            if (rightRegion == REGION_SHARP && leftRegion == REGION_BLUNT) rightOffense += 1.10f;
            if (leftRegion == REGION_BLUNT && rightRegion == REGION_SHARP) leftDefense += 0.50f;
            if (rightRegion == REGION_BLUNT && leftRegion == REGION_SHARP) rightDefense += 0.50f;
            if (leftRegion == REGION_SHARP && rightRegion == REGION_SHARP) {
                leftOffense += 0.50f;
                rightOffense += 0.50f;
            }
            if (leftRegion == REGION_BLUNT && rightRegion == REGION_BLUNT) {
                leftOffense -= 0.70f;
                rightOffense -= 0.70f;
            }
            if (leftRegion == REGION_SIDE) leftDefense -= 0.20f;
            if (rightRegion == REGION_SIDE) rightDefense -= 0.20f;

            leftDefense = clampFloat(leftDefense, 0.5f, 99.0f);
            rightDefense = clampFloat(rightDefense, 0.5f, 99.0f);

            int dmgToRight = calcDamageValue(leftOffense, rightDefense);
            int dmgToLeft = calcDamageValue(rightOffense, leftDefense);

            applyDamage(rightEgg, rightRegion, dmgToRight);
            applyDamage(leftEgg, leftRegion, dmgToLeft);

            float knockBase = 7.0f + closing * 1.08f;
            if (leftEgg.boosting || rightEgg.boosting) knockBase += 1.3f;
            if (dmgToLeft >= 2 || dmgToRight >= 2) knockBase += 1.8f;
            if (knockBase > 20.0f) knockBase = 20.0f;

            if (dmgToRight > dmgToLeft) {
                leftEgg.vel = addVec(leftEgg.vel, mulVec(n, -knockBase * 0.48f));
                rightEgg.vel = addVec(rightEgg.vel, mulVec(n, knockBase * 1.28f));
                leftEgg.pos = addVec(leftEgg.pos, mulVec(n, -knockBase * 0.66f));
                rightEgg.pos = addVec(rightEgg.pos, mulVec(n, knockBase * 1.42f));
            }
            else if (dmgToLeft > dmgToRight) {
                leftEgg.vel = addVec(leftEgg.vel, mulVec(n, -knockBase * 1.28f));
                rightEgg.vel = addVec(rightEgg.vel, mulVec(n, knockBase * 0.48f));
                leftEgg.pos = addVec(leftEgg.pos, mulVec(n, -knockBase * 1.42f));
                rightEgg.pos = addVec(rightEgg.pos, mulVec(n, knockBase * 0.66f));
            }
            else {
                leftEgg.vel = addVec(leftEgg.vel, mulVec(n, -knockBase * 0.92f));
                rightEgg.vel = addVec(rightEgg.vel, mulVec(n, knockBase * 0.92f));
                leftEgg.pos = addVec(leftEgg.pos, mulVec(n, -knockBase * 1.05f));
                rightEgg.pos = addVec(rightEgg.pos, mulVec(n, knockBase * 1.05f));
            }

            leftEgg.collisionCooldown = 12;
            rightEgg.collisionCooldown = 12;

            log1 = L"发生正面碰撞";
            log2 = wstring(L"左蛋") + getRegionName(leftRegion) + L" 对 右蛋" + getRegionName(rightRegion);

            bool heavy = false;
            int totalDmg = dmgToLeft + dmgToRight;

            if (dmgToLeft == 0 && dmgToRight == 0) {
                log3 = L"这一撞更像试探，没有明显破损";
                shakeFrames = 3;
                shakeStrength = 1;
                playEffect(L"hit_light.wav");
            }
            else if (leftRegion == REGION_SHARP && rightRegion == REGION_SHARP && (dmgToLeft >= 2 || dmgToRight >= 2)) {
                log3 = L"尖对尖重撞！双方都非常危险";
                hitPauseFrames = 5;
                slowMotionFrames = 16;
                shakeFrames = 10;
                shakeStrength = 6;
                heavy = true;
            }
            else if ((leftRegion == REGION_SHARP && rightRegion == REGION_BLUNT && dmgToRight >= 2) ||
                     (rightRegion == REGION_SHARP && leftRegion == REGION_BLUNT && dmgToLeft >= 2)) {
                log3 = L"找准了角度，这次击中很实";
                hitPauseFrames = 4;
                slowMotionFrames = 14;
                shakeFrames = 8;
                shakeStrength = 5;
                heavy = true;
            }
            else if (dmgToLeft >= 2 || dmgToRight >= 2 || closing > 5.2f) {
                log3 = L"重击！裂纹明显扩散";
                hitPauseFrames = 5;
                slowMotionFrames = 16;
                shakeFrames = 8;
                shakeStrength = 5;
                heavy = true;
            }
            else {
                log3 = L"双方都被撞得向后弹开";
                shakeFrames = 5;
                shakeStrength = 3;
            }

            if (heavy || totalDmg >= 4) playEffect(L"hit_heavy.wav");
            else playEffect(L"hit_light.wav");
        }
    }
}

void updateBattle(
    int currentMode,
    int& openingFrames,
    EggEntity& leftEgg,
    EggEntity& rightEgg,
    int& winnerState,
    int& hitPauseFrames,
    int& slowMotionFrames,
    int& shakeFrames,
    int& shakeStrength,
    int mouseX,
    int mouseY,
    wstring& log1,
    wstring& log2,
    wstring& log3
) {
    if (leftEgg.hitFlashFrames > 0) leftEgg.hitFlashFrames--;
    if (rightEgg.hitFlashFrames > 0) rightEgg.hitFlashFrames--;

    if (leftEgg.collisionCooldown > 0) leftEgg.collisionCooldown--;
    if (rightEgg.collisionCooldown > 0) rightEgg.collisionCooldown--;

    if (leftEgg.wallCooldown > 0) leftEgg.wallCooldown--;
    if (rightEgg.wallCooldown > 0) rightEgg.wallCooldown--;

    if (shakeFrames > 0) shakeFrames--;

    if (hitPauseFrames > 0) {
        hitPauseFrames--;
        if (isEggDead(leftEgg) && isEggDead(rightEgg)) winnerState = 3;
        else if (isEggDead(rightEgg)) winnerState = 1;
        else if (isEggDead(leftEgg)) winnerState = 2;
        return;
    }

    bool doPhysics = true;
    if (slowMotionFrames > 0) {
        if ((slowMotionFrames % 2) == 0) doPhysics = false;
        slowMotionFrames--;
    }

    if (!doPhysics) {
        if (isEggDead(leftEgg) && isEggDead(rightEgg)) winnerState = 3;
        else if (isEggDead(rightEgg)) winnerState = 1;
        else if (isEggDead(leftEgg)) winnerState = 2;
        return;
    }

    bool allowBoost = (openingFrames <= 0);

    updateHumanControl(leftEgg, 'W', 'S', 'A', 'D', 'Q', 'E', 'G', currentMode == 2, allowBoost, mouseX, mouseY);
    if (currentMode == 1) {
        updateHumanControl(rightEgg, VK_UP, VK_DOWN, VK_LEFT, VK_RIGHT, 'K', 'L', VK_RSHIFT, false, allowBoost, mouseX, mouseY);
    } else {
        updateAIControl(rightEgg, leftEgg, allowBoost);
    }

    if (openingFrames > 0) openingFrames--;

    bool leftFireDown = (GetAsyncKeyState('F') & 0x8000) != 0;
    if (gLeftHasSkillShot && leftFireDown && !gLeftSkillKeyHeld) {
        fireYolkShot(0, leftEgg, log1, log2, log3);
        playEffect(L"hit_light.wav");
    }
    gLeftSkillKeyHeld = leftFireDown;

    bool rightFireDown = (GetAsyncKeyState(VK_RCONTROL) & 0x8000) != 0;
    if (currentMode == 1) {
        if (gRightHasSkillShot && rightFireDown && !gRightSkillKeyHeld) {
            fireYolkShot(1, rightEgg, log1, log2, log3);
            playEffect(L"hit_light.wav");
        }
        gRightSkillKeyHeld = rightFireDown;
    } else {
        gRightSkillKeyHeld = false;
        if (gRightHasSkillShot && gArenaMode != ARENA_BASIC) {
            Vec2 toLeft = subVec(leftEgg.pos, rightEgg.pos);
            float dist = lenVec(toLeft);
            float ang = (float)atan2((double)toLeft.y, (double)toLeft.x);
            float align = fabsf(angleDiff(ang, rightEgg.angle));
            if (align < (gArenaMode == ARENA_NIGHTMARE ? 0.16f : 0.24f) &&
                dist > 120.0f && dist < (gArenaMode == ARENA_NIGHTMARE ? 520.0f : 420.0f) &&
                gAIState == AI_ATTACK) {
                fireYolkShot(1, rightEgg, log1, log2, log3);
                playEffect(L"hit_light.wav");
            }
        }
    }

    handleWallCollision(leftEgg, log1, log2, log3);
    handleWallCollision(rightEgg, log1, log2, log3);
    updateArenaObstacles(leftEgg, rightEgg, log1, log2, log3);
    updateHealthPackSystem(leftEgg, rightEgg, log1, log2, log3);
    updateSkillPackSystem(leftEgg, rightEgg, log1, log2, log3);

    resolveEggCollision(leftEgg, rightEgg, hitPauseFrames, slowMotionFrames, shakeFrames, shakeStrength, log1, log2, log3);
    updateYolkProjectiles(leftEgg, rightEgg, log1, log2, log3);
    updateSpikeSystem(leftEgg, rightEgg, log1, log2, log3);

    if (isEggDead(leftEgg) && isEggDead(rightEgg)) winnerState = 3;
    else if (isEggDead(rightEgg)) winnerState = 1;
    else if (isEggDead(leftEgg)) winnerState = 2;
}

bool pointInCard(int x, int y, int cardX, int cardY) {
    return x >= cardX && x <= cardX + 230 && y >= cardY && y <= cardY + 360;
}

void chooseEggAndAdvance(
    int currentMode,
    int& selectStep,
    int chosen,
    EggRuntimeState candidates[3],
    EggRuntimeState& leftDraft,
    EggRuntimeState& rightDraft,
    EggEntity& leftEgg,
    EggEntity& rightEgg,
    int& winnerState,
    int& hitPauseFrames,
    int& slowMotionFrames,
    int& openingFrames,
    int& shakeFrames,
    int& shakeStrength,
    wstring& log1,
    wstring& log2,
    wstring& log3,
    SceneType& currentScene
) {
    if (currentMode == 1) {
        if (selectStep == 1) {
            leftDraft = candidates[chosen];
            selectStep = 2;
            generateCandidateBatch(candidates);

            log1 = L"玩家1 已完成选蛋。";
            log2 = L"现在轮到玩家2。";
            log3 = L"请继续挑选。";
            currentScene = SCENE_SELECT_EGG;
        } else {
            rightDraft = candidates[chosen];
            leftEgg = makeBattleEgg(leftDraft, (float)(ARENA_L + 170), (float)((ARENA_T + ARENA_B) / 2), 0.0f);
            rightEgg = makeBattleEgg(rightDraft, (float)(ARENA_R - 170), (float)((ARENA_T + ARENA_B) / 2), PI_F);

            winnerState = 0;
            hitPauseFrames = 0;
            slowMotionFrames = 0;
            openingFrames = 110;
            shakeFrames = 0;
            shakeStrength = 0;
            gSpikeDamage = computeSpikeDamageFromDrafts(leftDraft, rightDraft);
            initArenaHazards(leftEgg, rightEgg);
            resetAIState();

            log1 = L"对战开始";
            log2 = L"地图已经扩大，耐久也显著提高了";
            log3 = L"找准角度，利用空间和场地机制去撞击";
            currentScene = SCENE_BATTLE;
        }
    } else {
        leftDraft = candidates[chosen];
        EggRuntimeState aiCandidates[3];
        generateCandidateBatch(aiCandidates);
        rightDraft = aiCandidates[randInt(0, 2)];

        leftEgg = makeBattleEgg(leftDraft, (float)(ARENA_L + 170), (float)((ARENA_T + ARENA_B) / 2), 0.0f);
        rightEgg = makeBattleEgg(rightDraft, (float)(ARENA_R - 170), (float)((ARENA_T + ARENA_B) / 2), PI_F);

        winnerState = 0;
        hitPauseFrames = 0;
        slowMotionFrames = 0;
        openingFrames = 110;
        shakeFrames = 0;
        shakeStrength = 0;
        gSpikeDamage = computeSpikeDamageFromDrafts(leftDraft, rightDraft);
        initArenaHazards(leftEgg, rightEgg);
        resetAIState();

        log1 = L"你已选定鸡蛋";
        log2 = L"电脑也已挑选完成";
        log3 = L"现在开始对撞";
        currentScene = SCENE_BATTLE;
    }
}

int main() {
    srand((unsigned int)time(NULL));

    updateLayoutMetrics();
    initgraph(SCREEN_W, SCREEN_H);
    makeFullscreenWindow();
    BeginBatchDraw();
    startBGM();

    SceneType currentScene = SCENE_MENU;
    int currentMode = 0;
    int selectStep = 1;
    int winnerState = 0;

    EggRuntimeState selectCandidates[3];
    EggRuntimeState leftDraft{};
    EggRuntimeState rightDraft{};
    EggRuntimeState inspectEgg{};

    EggEntity leftEgg{};
    EggEntity rightEgg{};

    int hitPauseFrames = 0;
    int slowMotionFrames = 0;
    int openingFrames = 0;
    int shakeFrames = 0;
    int shakeStrength = 0;

    int lastMouseX = SCREEN_W / 2;
    int lastMouseY = SCREEN_H / 2;

    int inspectIndex = 0;
    float inspectAngle = -PI_F / 2.0f;

    bool resultSoundPlayed = false;

    wstring log1 = L"准备开始";
    wstring log2 = L"请先选择鸡蛋";
    wstring log3 = L"选定后即可开战";

    int menuW = 320;
    int menuH = 60;
    int centerX = SCREEN_W / 2;
    int menuBaseY = SCREEN_H / 2 - 90;
    Button btnPVP = { centerX - menuW / 2, menuBaseY, centerX + menuW / 2, menuBaseY + menuH, L"双人对战" };
    Button btnPVE = { centerX - menuW / 2, menuBaseY + 95, centerX + menuW / 2, menuBaseY + 95 + menuH, L"人机对战" };
    Button btnExit = { centerX - menuW / 2, menuBaseY + 190, centerX + menuW / 2, menuBaseY + 190 + menuH, L"退出游戏" };

    int mapPanelT = SCREEN_H / 2 - 245;
    Button btnBasic = { centerX - 230, mapPanelT + 150, centerX + 230, mapPanelT + 218, L"基础模式" };
    Button btnAdvanced = { centerX - 230, mapPanelT + 250, centerX + 230, mapPanelT + 318, L"进阶模式" };
    Button btnNightmare = { centerX - 230, mapPanelT + 350, centerX + 230, mapPanelT + 418, L"噩梦模式" };
    Button btnModeBack = { centerX - 150, mapPanelT + 438, centerX + 150, mapPanelT + 500, L"返回" };

    int selectBtnY = CARD_Y + CARD_H + 20;
    Button btnEgg1 = { CARD_X[0] + 18, selectBtnY, CARD_X[0] + 212, selectBtnY + 60, L"选择 1 号蛋" };
    Button btnEgg2 = { CARD_X[1] + 18, selectBtnY, CARD_X[1] + 212, selectBtnY + 60, L"选择 2 号蛋" };
    Button btnEgg3 = { CARD_X[2] + 18, selectBtnY, CARD_X[2] + 212, selectBtnY + 60, L"选择 3 号蛋" };
    Button btnBackSelect = { SCREEN_W / 2 - 120, SCREEN_H - 96, SCREEN_W / 2 + 120, SCREEN_H - 30, L"返回主菜单" };

    Button btnInspectRotL = { SCREEN_W / 2 - 390, SCREEN_H - 96, SCREEN_W / 2 - 230, SCREEN_H - 30, L"左转" };
    Button btnInspectRotR = { SCREEN_W / 2 - 205, SCREEN_H - 96, SCREEN_W / 2 - 45, SCREEN_H - 30, L"右转" };
    Button btnInspectPick = { SCREEN_W / 2 + 70, SCREEN_H - 96, SCREEN_W / 2 + 290, SCREEN_H - 30, L"选择这颗蛋" };
    Button btnInspectBack = { SCREEN_W / 2 + 315, SCREEN_H - 96, SCREEN_W / 2 + 475, SCREEN_H - 30, L"返回选蛋" };

    Button btnBackBattle = { SCREEN_W - 180, SCREEN_H - 78, SCREEN_W - 35, SCREEN_H - 23, L"返回菜单" };

    Button btnReplay = { SCREEN_W / 2 - 200, SCREEN_H - 74, SCREEN_W / 2 - 30, SCREEN_H - 14, L"再来一局" };
    Button btnResultMenu = { SCREEN_W / 2 + 30, SCREEN_H - 74, SCREEN_W / 2 + 200, SCREEN_H - 14, L"返回主菜单" };

    bool running = true;
    DWORD lastTick = GetTickCount();

    while (running) {
        DWORD now = GetTickCount();
        if (now - lastTick < 16) {
            Sleep(1);
            continue;
        }
        lastTick = now;

        ExMessage msg;
        while (peekmessage(&msg, EX_MOUSE)) {
            lastMouseX = msg.x;
            lastMouseY = msg.y;

            if (msg.message == WM_LBUTTONDOWN) {
                int x = msg.x;
                int y = msg.y;

                if (currentScene == SCENE_MENU) {
                    if (isInside(btnPVP, x, y)) {
                        playEffect(L"ui_click.wav");
                        currentMode = 1;
                        currentScene = SCENE_MAP_SELECT;
                    } else if (isInside(btnPVE, x, y)) {
                        playEffect(L"ui_click.wav");
                        currentMode = 2;
                        currentScene = SCENE_MAP_SELECT;
                    } else if (isInside(btnExit, x, y)) {
                        playEffect(L"ui_click.wav");
                        running = false;
                    }
                } else if (currentScene == SCENE_MAP_SELECT) {
                    if (isInside(btnBasic, x, y)) {
                        playEffect(L"ui_click.wav");
                        gArenaMode = ARENA_BASIC;
                        selectStep = 1;
                        winnerState = 0;
                        resultSoundPlayed = false;
                        generateCandidateBatch(selectCandidates);
                        currentScene = SCENE_SELECT_EGG;
                    } else if (isInside(btnAdvanced, x, y)) {
                        playEffect(L"ui_click.wav");
                        gArenaMode = ARENA_ADVANCED;
                        selectStep = 1;
                        winnerState = 0;
                        resultSoundPlayed = false;
                        generateCandidateBatch(selectCandidates);
                        currentScene = SCENE_SELECT_EGG;
                    } else if (isInside(btnNightmare, x, y)) {
                        playEffect(L"ui_click.wav");
                        gArenaMode = ARENA_NIGHTMARE;
                        selectStep = 1;
                        winnerState = 0;
                        resultSoundPlayed = false;
                        generateCandidateBatch(selectCandidates);
                        currentScene = SCENE_SELECT_EGG;
                    } else if (isInside(btnModeBack, x, y)) {
                        playEffect(L"ui_click.wav");
                        currentScene = SCENE_MENU;
                        currentMode = 0;
                    }
                } else if (currentScene == SCENE_SELECT_EGG) {
                    if (isInside(btnBackSelect, x, y)) {
                        playEffect(L"ui_click.wav");
                        currentScene = SCENE_MENU;
                        currentMode = 0;
                        selectStep = 1;
                    } else {
                        int chosen = -1;
                        if (isInside(btnEgg1, x, y)) chosen = 0;
                        else if (isInside(btnEgg2, x, y)) chosen = 1;
                        else if (isInside(btnEgg3, x, y)) chosen = 2;

                        if (chosen != -1) {
                            playEffect(L"ui_click.wav");
                            chooseEggAndAdvance(currentMode, selectStep, chosen, selectCandidates, leftDraft, rightDraft, leftEgg, rightEgg,
                                                winnerState, hitPauseFrames, slowMotionFrames, openingFrames, shakeFrames, shakeStrength,
                                                log1, log2, log3, currentScene);
                        } else {
                            if (pointInCard(x, y, CARD_X[0], CARD_Y)) chosen = 0;
                            else if (pointInCard(x, y, CARD_X[1], CARD_Y)) chosen = 1;
                            else if (pointInCard(x, y, CARD_X[2], CARD_Y)) chosen = 2;
                            if (chosen != -1) {
                                playEffect(L"inspect.wav");
                                inspectEgg = selectCandidates[chosen];
                                inspectIndex = chosen;
                                inspectAngle = -PI_F / 2.0f;
                                currentScene = SCENE_INSPECT;
                            }
                        }
                    }
                } else if (currentScene == SCENE_INSPECT) {
                    if (isInside(btnInspectRotL, x, y)) {
                        playEffect(L"ui_click.wav");
                        inspectAngle -= 0.18f;
                    } else if (isInside(btnInspectRotR, x, y)) {
                        playEffect(L"ui_click.wav");
                        inspectAngle += 0.18f;
                    } else if (isInside(btnInspectBack, x, y)) {
                        playEffect(L"ui_click.wav");
                        currentScene = SCENE_SELECT_EGG;
                    } else if (isInside(btnInspectPick, x, y)) {
                        playEffect(L"ui_click.wav");
                        chooseEggAndAdvance(currentMode, selectStep, inspectIndex, selectCandidates, leftDraft, rightDraft, leftEgg, rightEgg,
                                            winnerState, hitPauseFrames, slowMotionFrames, openingFrames, shakeFrames, shakeStrength,
                                            log1, log2, log3, currentScene);
                    }
                } else if (currentScene == SCENE_BATTLE) {
                    if (isInside(btnBackBattle, x, y)) {
                        playEffect(L"ui_click.wav");
                        currentScene = SCENE_MENU;
                        currentMode = 0;
                        selectStep = 1;
                    }
                } else if (currentScene == SCENE_RESULT) {
                    if (isInside(btnReplay, x, y)) {
                        playEffect(L"ui_click.wav");
                        selectStep = 1;
                        winnerState = 0;
                        resultSoundPlayed = false;
                        generateCandidateBatch(selectCandidates);
                        currentScene = SCENE_SELECT_EGG;
                    } else if (isInside(btnResultMenu, x, y)) {
                        playEffect(L"ui_click.wav");
                        currentScene = SCENE_MENU;
                        currentMode = 0;
                        selectStep = 1;
                    }
                }
            }
        }

        if (currentScene == SCENE_BATTLE && winnerState == 0) {
            updateBattle(currentMode, openingFrames, leftEgg, rightEgg, winnerState, hitPauseFrames, slowMotionFrames,
                         shakeFrames, shakeStrength, lastMouseX, lastMouseY, log1, log2, log3);

            if (winnerState != 0) {
                if (winnerState == 1) {
                    log1 = (currentMode == 1) ? L"玩家1 获胜" : L"你获胜了";
                    log2 = L"关键一撞已经决定胜负";
                    log3 = L"地图和环境都被你利用得更好";
                } else if (winnerState == 2) {
                    log1 = (currentMode == 1) ? L"玩家2 获胜" : L"电脑获胜";
                    log2 = L"关键一撞已经决定胜负";
                    log3 = L"对手在空间和节奏上更占优势";
                } else {
                    log1 = L"双方同时报废";
                    log2 = L"这是一局平局";
                    log3 = L"尖对尖的高风险就是这样";
                }
                currentScene = SCENE_RESULT;
                resultSoundPlayed = false;
            }
        }

        if (currentScene == SCENE_RESULT && !resultSoundPlayed) {
            if (winnerState == 1) playEffect(L"win.wav");
            else if (winnerState == 2) playEffect(L"lose.wav");
            else playEffect(L"draw.wav");
            resultSoundPlayed = true;
        }

        if (currentScene == SCENE_MENU) {
            drawMenuScene(btnPVP, btnPVE, btnExit);
        } else if (currentScene == SCENE_MAP_SELECT) {
            drawMapSelectScene(btnBasic, btnAdvanced, btnNightmare, btnModeBack, currentMode);
        } else if (currentScene == SCENE_SELECT_EGG) {
            drawSelectScene(btnEgg1, btnEgg2, btnEgg3, btnBackSelect, currentMode, selectStep, selectCandidates);
        } else if (currentScene == SCENE_INSPECT) {
            drawInspectScene(btnInspectRotL, btnInspectRotR, btnInspectPick, btnInspectBack, inspectEgg, inspectIndex, inspectAngle);
        } else if (currentScene == SCENE_BATTLE) {
            drawBattleScene(btnBackBattle, currentMode, openingFrames, leftEgg, rightEgg, log1, log2, log3, shakeFrames, shakeStrength);
        } else if (currentScene == SCENE_RESULT) {
            drawResultScene(btnReplay, btnResultMenu, currentMode, winnerState, leftDraft, rightDraft, leftEgg, rightEgg, log1, log2, log3);
        }

        drawMouseFocus(lastMouseX, lastMouseY);
        FlushBatchDraw();
    }

    stopBGM();
    EndBatchDraw();
    closegraph();
    return 0;
}
