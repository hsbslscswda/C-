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

const int SCREEN_W = 1000;
const int SCREEN_H = 720;

const int ARENA_L = 90;
const int ARENA_T = 130;
const int ARENA_R = 910;
const int ARENA_B = 620;

const float PI_F = 3.1415926f;
const wchar_t* FONT_NAME = L"Microsoft YaHei";

enum SceneType {
    SCENE_MENU,
    SCENE_SELECT_EGG,
    SCENE_INSPECT,
    SCENE_BATTLE,
    SCENE_RESULT
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
    solidrectangle(0, 0, SCREEN_W, 96);

    setfillcolor(band2);
    solidrectangle(0, SCREEN_H - 76, SCREEN_W, SCREEN_H);

    setlinecolor(accent);
    line(0, 96, SCREEN_W, 96);
    line(0, SCREEN_H - 76, SCREEN_W, SCREEN_H - 76);

    drawCornerOrnaments(18, 18, SCREEN_W - 18, SCREEN_H - 18, accent);

    for (int x = 70; x < SCREEN_W - 40; x += 110) {
        drawDiamond(x, 48, 8, accent, RGB(230, 224, 210));
        drawDiamond(x + 40, SCREEN_H - 38, 6, accent, RGB(230, 224, 210));
    }
}

void drawPanel(int l, int t, int r, int b, COLORREF fill, COLORREF border, COLORREF accent) {
    setlinecolor(border);
    setfillcolor(fill);
    solidrectangle(l, t, r, b);
    rectangle(l, t, r, b);

    setlinecolor(accent);
    rectangle(l + 5, t + 5, r - 5, b - 5);
    drawCornerOrnaments(l + 4, t + 4, r - 4, b - 4, accent);
}

void drawTextCenteredInButton(const Button& btn, const wchar_t* text, int fontSize, COLORREF color) {
    setUIFontAndColor(fontSize, color);

    int tw = textwidth(text);
    int th = textheight(text);
    int tx = (btn.left + btn.right - tw) / 2;
    int ty = (btn.top + btn.bottom - th) / 2;
    outtextxy(tx, ty, text);
}

void drawButton(const Button& btn, bool enabled) {
    if (enabled) {
        setlinecolor(RGB(82, 109, 130));
        setfillcolor(RGB(222, 233, 242));
    }
    else {
        setlinecolor(RGB(150, 150, 150));
        setfillcolor(RGB(232, 232, 232));
    }

    solidrectangle(btn.left + 3, btn.top + 3, btn.right + 3, btn.bottom + 3);

    if (enabled) {
        setfillcolor(RGB(236, 243, 249));
        setlinecolor(RGB(74, 101, 123));
    }
    else {
        setfillcolor(RGB(240, 240, 240));
        setlinecolor(RGB(165, 165, 165));
    }

    solidrectangle(btn.left, btn.top, btn.right, btn.bottom);
    rectangle(btn.left, btn.top, btn.right, btn.bottom);

    setlinecolor(enabled ? RGB(180, 198, 214) : RGB(210, 210, 210));
    rectangle(btn.left + 4, btn.top + 4, btn.right - 4, btn.bottom - 4);

    drawCornerOrnaments(btn.left + 2, btn.top + 2, btn.right - 2, btn.bottom - 2,
        enabled ? RGB(125, 150, 170) : RGB(185, 185, 185));
    drawTextCenteredInButton(btn, btn.text, 22, enabled ? BLACK : RGB(130, 130, 130));
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

    egg.sharpMaxHP = clampMin(sharpHP, 2);
    egg.bluntMaxHP = clampMin(bluntHP, 2);
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

    e.sideMaxHP = clampMin(4 + draft.thicknessType + (draft.symmetryType == 0 ? 1 : 0), 3);
    e.sideHP = e.sideMaxHP;

    e.totalMaxIntegrity = e.sharpMaxHP + e.bluntMaxHP + e.sideMaxHP + 3;
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

    setUIFontAndColor(24, BLACK);
    outtextxy(x + 82, y + 18, title);

    EggEntity preview = makeBattleEgg(egg, (float)(x + 115), (float)(y + 128 + idleBob(phase, 3.0f)), -PI_F / 2.0f);
    drawEggShadow(preview, 0.92f);
    drawEggShape(preview, 0.92f);

    setUIFontAndColor(17, BLACK);
    for (int i = 0; i < egg.clueCount; ++i) {
        outtextxy(x + 16, y + 215 + i * 22, egg.clues[i].c_str());
    }
}

void drawBattleEggPanel(int x, int y, const wchar_t* title, const EggEntity& egg, float scaleMul) {
    drawPanel(x, y, x + 270, y + 320, RGB(247, 245, 240), RGB(132, 122, 104), RGB(170, 152, 120));

    setUIFontAndColor(24, BLACK);
    outtextxy(x + 70, y + 12, title);

    EggEntity preview = egg;
    preview.pos = makeVec((float)(x + 135), (float)(y + 125 + idleBob((float)x * 0.01f, 2.0f)));
    drawEggShadow(preview, 0.96f * scaleMul);
    drawEggShape(preview, 0.96f * scaleMul);
    drawEggCracks(preview, 0.96f * scaleMul);

    wstring s1 = wstring(L"\u5c16\u7aef\uff1a") + getRegionStateText(egg.sharpHP, egg.sharpMaxHP);
    wstring s2 = wstring(L"\u949d\u7aef\uff1a") + getRegionStateText(egg.bluntHP, egg.bluntMaxHP);
    wstring s3 = wstring(L"\u603b\u5b8c\u6574\u5ea6\uff1a") + toW(egg.totalIntegrity) + L"/" + toW(egg.totalMaxIntegrity);

    setUIFontAndColor(18, BLACK);
    outtextxy(x + 18, y + 215, s1.c_str());
    outtextxy(x + 18, y + 240, s2.c_str());
    outtextxy(x + 18, y + 265, s3.c_str());
    outtextxy(x + 18, y + 290, getEggRevealLine1(egg.draft).c_str());
}

void drawBattleLogBox(int x, int y, int w, int h, const wstring& line1, const wstring& line2, const wstring& line3) {
    drawPanel(x, y, x + w, y + h, RGB(247, 245, 240), RGB(132, 122, 104), RGB(170, 152, 120));

    setUIFontAndColor(22, BLACK);
    outtextxy(x + 18, y + 10, L"\u5bf9\u6218\u65e5\u5fd7");

    setUIFontAndColor(18, BLACK);
    outtextxy(x + 18, y + 44, fitTextToWidth(line1, w - 36).c_str());
    outtextxy(x + 18, y + 70, fitTextToWidth(line2, w - 36).c_str());
    outtextxy(x + 18, y + 96, fitTextToWidth(line3, w - 36).c_str());
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

void drawArenaBackground() {
    drawSceneBackground(RGB(214, 220, 228), RGB(189, 198, 209), RGB(196, 184, 160), RGB(106, 84, 54));

    setfillcolor(RGB(183, 163, 126));
    solidrectangle(ARENA_L, ARENA_T, ARENA_R, ARENA_B);

    setfillcolor(RGB(206, 187, 151));
    solidrectangle(ARENA_L + 12, ARENA_T + 12, ARENA_R - 12, ARENA_B - 12);

    setlinecolor(RGB(106, 84, 54));
    rectangle(ARENA_L, ARENA_T, ARENA_R, ARENA_B);
    rectangle(ARENA_L + 12, ARENA_T + 12, ARENA_R - 12, ARENA_B - 12);
    drawCornerOrnaments(ARENA_L + 4, ARENA_T + 4, ARENA_R - 4, ARENA_B - 4, RGB(128, 100, 60));

    setlinecolor(RGB(174, 156, 122));
    for (int y = ARENA_T + 30; y < ARENA_B - 10; y += 24) {
        line(ARENA_L + 20, y, ARENA_R - 20, y);
    }
    for (int x = ARENA_L + 28; x < ARENA_R - 20; x += 82) {
        line(x, ARENA_T + 18, x + 24, ARENA_T + 18);
    }
}

void drawMenuScene(const Button& btnPVP, const Button& btnPVE, const Button& btnExit) {
    drawSceneBackground(RGB(222, 227, 234), RGB(189, 198, 209), RGB(198, 188, 166), RGB(96, 112, 128));

    setUIFontAndColor(48, BLACK);
    outtextxy(340, 80, L"\u86cb\u5c16\u5bf9\u51b3");

    drawPanel(230, 180, 770, 540, RGB(245, 244, 240), RGB(118, 128, 140), RGB(165, 150, 120));
    drawButton(btnPVP, true);
    drawButton(btnPVE, true);
    drawButton(btnExit, true);
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
    drawSceneBackground(RGB(223, 228, 234), RGB(190, 198, 208), RGB(200, 190, 168), RGB(118, 106, 84));

    setUIFontAndColor(34, BLACK);
    if (currentMode == 1 && selectStep == 2) {
        outtextxy(320, 28, L"\u73a9\u5bb62 \u9009\u62e9\u9e21\u86cb");
    }
    else {
        outtextxy(320, 28, L"\u73a9\u5bb61 \u9009\u62e9\u9e21\u86cb");
    }

    setUIFontAndColor(20, RGB(55, 55, 55));
    outtextxy(96, 74, L"\u968f\u673a\u5019\u9009\u86cb\uff0c\u6839\u636e\u7ebf\u7d22\u3001\u76f4\u89c9\u548c\u8fd0\u6c14\u6765\u6311\u9009\uff1a");

    setUIFontAndColor(18, RGB(70, 70, 70));
    outtextxy(300, 100, L"\u70b9\u51fb\u86cb\u5361\u672c\u4f53\u53ef\u8fdb\u5165\u653e\u5927\u68c0\u89c6");

    drawEggPreviewCard(35, 120, candidates[0], L"1\u53f7\u86cb", 0.0f);
    drawEggPreviewCard(385, 120, candidates[1], L"2\u53f7\u86cb", 1.3f);
    drawEggPreviewCard(735, 120, candidates[2], L"3\u53f7\u86cb", 2.2f);

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
    drawSceneBackground(RGB(222, 227, 234), RGB(188, 197, 208), RGB(198, 188, 166), RGB(118, 106, 84));

    setUIFontAndColor(34, BLACK);
    outtextxy(360, 24, L"\u653e\u5927\u68c0\u89c6");

    wstring title = L"\u6b63\u5728\u68c0\u89c6\uff1a" + toW(inspectIndex + 1) + L"\u53f7\u86cb";
    setUIFontAndColor(22, BLACK);
    outtextxy(390, 72, title.c_str());

    drawPanel(85, 120, 500, 590, RGB(247, 245, 240), RGB(132, 122, 104), RGB(170, 152, 120));

    EggEntity preview = makeBattleEgg(egg, 292.0f, 320.0f + idleBob(0.8f, 4.0f), inspectAngle);
    drawEggShadow(preview, 1.75f);
    drawEggShape(preview, 1.75f);

    setUIFontAndColor(18, BLACK);
    outtextxy(126, 505, L"\u8fd9\u91cc\u66f4\u5f3a\u8c03\u86cb\u5f62\u3001\u89d2\u5ea6\u548c\u5c16\u949d\u7aef\u5dee\u5f02");

    drawPanel(545, 120, 915, 590, RGB(247, 245, 240), RGB(132, 122, 104), RGB(170, 152, 120));

    setUIFontAndColor(22, BLACK);
    outtextxy(680, 140, L"\u68c0\u89c6\u7ebf\u7d22");

    setUIFontAndColor(18, BLACK);
    for (int i = 0; i < egg.clueCount; ++i) {
        outtextxy(575, 185 + i * 34, egg.clues[i].c_str());
    }

    setUIFontAndColor(20, BLACK);
    outtextxy(575, 415, L"\u8f7b\u6572\u53cd\u9988\uff1a");
    outtextxy(575, 448, getTapFeedback(egg).c_str());

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

    setUIFontAndColor(28, BLACK);
    outtextxy(398, 18, L"\u5bf9\u6218\u4e2d");

    setUIFontAndColor(18, BLACK);
    if (currentMode == 1) {
        outtextxy(105, 24, L"P1: WASD \u79fb\u52a8\uff0cQ/E \u5fae\u8c03\uff0cG \u52a0\u901f");
        outtextxy(610, 24, L"P2: \u65b9\u5411\u952e\u79fb\u52a8\uff0cK/L \u5fae\u8c03\uff0cRShift \u52a0\u901f");
    }
    else {
        outtextxy(92, 24, L"\u4f60: WASD \u79fb\u52a8\uff0cQ/E \u5fae\u8c03\uff0cG \u52a0\u901f\uff0c\u6309\u4f4f\u9f20\u6807\u5de6\u952e\u53ef\u8f85\u52a9\u8f6c\u5411");
        outtextxy(790, 24, L"AI");
    }

    if (openingFrames > 0) {
        setUIFontAndColor(20, RGB(80, 40, 20));
        outtextxy(380, 54, L"\u5f00\u5c40\u8bd5\u63a2\u671f\uff1a\u6682\u65f6\u4e0d\u80fd\u52a0\u901f");
    }

    drawStaminaBar(110, 60, 220, 24, leftEgg.stamina / leftEgg.maxStamina);
    drawIntegrityBar(110, 92, 220, 18, (float)leftEgg.totalIntegrity / (float)leftEgg.totalMaxIntegrity);

    drawStaminaBar(670, 60, 220, 24, rightEgg.stamina / rightEgg.maxStamina);
    drawIntegrityBar(670, 92, 220, 18, (float)rightEgg.totalIntegrity / (float)rightEgg.totalMaxIntegrity);

    float ls = 1.0f + leftEgg.hitFlashFrames * 0.006f;
    float rs = 1.0f + rightEgg.hitFlashFrames * 0.006f;

    drawEggShadow(l, ls);
    drawEggShape(l, ls);
    drawEggCracks(l, ls);

    drawEggShadow(r, rs);
    drawEggShape(r, rs);
    drawEggCracks(r, rs);

    drawBattleLogBox(190, 540, 620, 122, log1, log2, log3);
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
    drawSceneBackground(RGB(222, 227, 234), RGB(188, 197, 208), RGB(198, 188, 166), RGB(118, 106, 84));

    wstring winnerText = L"\u672c\u5c40\u5e73\u5c40";
    if (winnerState == 1) winnerText = (currentMode == 1) ? L"\u73a9\u5bb61 \u83b7\u80dc" : L"\u4f60\u83b7\u80dc\u4e86";
    if (winnerState == 2) winnerText = (currentMode == 1) ? L"\u73a9\u5bb62 \u83b7\u80dc" : L"\u7535\u8111\u83b7\u80dc";

    setUIFontAndColor(36, BLACK);
    outtextxy(392, 26, L"\u5bf9\u6218\u7ed3\u679c");

    setUIFontAndColor(28, BLACK);
    outtextxy(390, 78, winnerText.c_str());

    float pulse = 1.0f + 0.03f * sinf(nowTimeSec() * 5.0f);
    float leftMul = 1.0f;
    float rightMul = 1.0f;
    if (winnerState == 1) leftMul = pulse;
    if (winnerState == 2) rightMul = pulse;

    drawBattleEggPanel(80, 135, L"\u5de6\u4fa7\u6700\u7ec8\u72b6\u6001", leftEgg, leftMul);
    drawBattleEggPanel(650, 135, L"\u53f3\u4fa7\u6700\u7ec8\u72b6\u6001", rightEgg, rightMul);

    setUIFontAndColor(18, BLACK);
    outtextxy(100, 470, getEggSummary(leftDraft).c_str());
    outtextxy(670, 470, getEggSummary(rightDraft).c_str());
    outtextxy(100, 495, getEggRevealLine2(leftDraft).c_str());
    outtextxy(670, 495, getEggRevealLine2(rightDraft).c_str());

    drawBattleLogBox(180, 520, 640, 118, log1, log2, log3);

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

void updateAIControl(EggEntity& ai, const EggEntity& target, bool allowBoost) {
    Vec2 toTarget = subVec(target.pos, ai.pos);
    float dist = lenVec(toTarget);
    Vec2 dir = normVec(toTarget);

    float desiredAngle = (float)atan2((double)dir.y, (double)dir.x);
    float d = angleDiff(desiredAngle, ai.angle);

    float turnSpeed = 0.045f;
    if (d > turnSpeed) d = turnSpeed;
    if (d < -turnSpeed) d = -turnSpeed;
    ai.angle = normalizeAngle(ai.angle + d);

    Vec2 moveDir = makeVec(0.0f, 0.0f);

    if (dist > 205.0f) {
        moveDir = dir;
    }
    else if (dist < 110.0f) {
        moveDir = mulVec(dir, -1.0f);
    }
    else {
        Vec2 side = makeVec(-dir.y, dir.x);
        float bias = ((GetTickCount() / 300) % 2 == 0) ? 1.0f : -1.0f;
        moveDir = addVec(mulVec(dir, 0.48f), mulVec(side, 0.52f * bias));
        moveDir = normVec(moveDir);
    }

    bool aligned = fabsf(angleDiff(desiredAngle, ai.angle)) < 0.35f;
    bool wantsBoost = allowBoost && (dist > 150.0f && dist < 320.0f && aligned && ai.stamina > 35.0f);

    ai.boosting = wantsBoost;

    float baseSpeed = 2.68f;
    float finalSpeed = wantsBoost ? baseSpeed * 2.05f : baseSpeed;

    Vec2 targetVel = mulVec(moveDir, finalSpeed);
    ai.vel = addVec(mulVec(ai.vel, 0.76f), mulVec(targetVel, 0.24f));

    if (wantsBoost) ai.stamina -= 0.665f;
    else ai.stamina += (dist < 140.0f ? 0.50f : 0.28f);

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

        float lv = dotVec(leftEgg.vel, n);
        float rv = dotVec(rightEgg.vel, n);
        leftEgg.vel = addVec(leftEgg.vel, mulVec(n, -(lv + 0.8f)));
        rightEgg.vel = addVec(rightEgg.vel, mulVec(n, -(rv - 0.8f)));

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

            leftEgg.collisionCooldown = 12;
            rightEgg.collisionCooldown = 12;

            log1 = L"\u53d1\u751f\u6b63\u9762\u78b0\u649e";
            log2 = wstring(L"\u5de6\u86cb") + getRegionName(leftRegion) + L" \u5bf9 \u53f3\u86cb" + getRegionName(rightRegion);

            bool heavy = false;
            int totalDmg = dmgToLeft + dmgToRight;

            if (dmgToLeft == 0 && dmgToRight == 0) {
                log3 = L"\u8fd9\u4e00\u649e\u66f4\u50cf\u8bd5\u63a2\uff0c\u6ca1\u6709\u660e\u663e\u7834\u635f";
                shakeFrames = 3;
                shakeStrength = 1;
                playEffect(L"hit_light.wav");
            }
            else if (leftRegion == REGION_SHARP && rightRegion == REGION_SHARP && (dmgToLeft >= 2 || dmgToRight >= 2)) {
                log3 = L"\u5c16\u5bf9\u5c16\u91cd\u649e\uff01\u53cc\u65b9\u90fd\u975e\u5e38\u5371\u9669";
                hitPauseFrames = 5;
                slowMotionFrames = 16;
                shakeFrames = 10;
                shakeStrength = 6;
                heavy = true;
            }
            else if ((leftRegion == REGION_SHARP && rightRegion == REGION_BLUNT && dmgToRight >= 2) ||
                (rightRegion == REGION_SHARP && leftRegion == REGION_BLUNT && dmgToLeft >= 2)) {
                log3 = L"\u627e\u51c6\u4e86\u89d2\u5ea6\uff0c\u8fd9\u6b21\u51fb\u4e2d\u5f88\u5b9e";
                hitPauseFrames = 4;
                slowMotionFrames = 14;
                shakeFrames = 8;
                shakeStrength = 5;
                heavy = true;
            }
            else if (dmgToLeft >= 2 || dmgToRight >= 2 || closing > 5.2f) {
                log3 = L"\u91cd\u51fb\uff01\u88c2\u7eb9\u660e\u663e\u6269\u6563";
                hitPauseFrames = 5;
                slowMotionFrames = 16;
                shakeFrames = 8;
                shakeStrength = 5;
                heavy = true;
            }
            else {
                log3 = L"\u53cc\u65b9\u90fd\u53d7\u5230\u4e86\u5b9e\u6253\u5b9e\u7684\u78b0\u649e";
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
    }
    else {
        updateAIControl(rightEgg, leftEgg, allowBoost);
    }

    if (openingFrames > 0) openingFrames--;

    handleWallCollision(leftEgg, log1, log2, log3);
    handleWallCollision(rightEgg, log1, log2, log3);

    resolveEggCollision(leftEgg, rightEgg, hitPauseFrames, slowMotionFrames, shakeFrames, shakeStrength, log1, log2, log3);

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

            log1 = L"\u73a9\u5bb61 \u5df2\u5b8c\u6210\u9009\u86cb\u3002";
            log2 = L"\u73b0\u5728\u8f6e\u5230\u73a9\u5bb62\u3002";
            log3 = L"\u8bf7\u7ee7\u7eed\u6311\u9009\u3002";

            currentScene = SCENE_SELECT_EGG;
        }
        else {
            rightDraft = candidates[chosen];

            leftEgg = makeBattleEgg(leftDraft, 260.0f, 360.0f, 0.0f);
            rightEgg = makeBattleEgg(rightDraft, 740.0f, 360.0f, PI_F);

            winnerState = 0;
            hitPauseFrames = 0;
            slowMotionFrames = 0;
            openingFrames = 110;
            shakeFrames = 0;
            shakeStrength = 0;

            log1 = L"\u5bf9\u6218\u5f00\u59cb";
            log2 = L"\u4f9d\u9760\u89d2\u5ea6\u3001\u8fd0\u52a8\u548c\u8282\u594f\u53bb\u649e\u51fb";
            log3 = L"\u627e\u51c6\u5c16\u7aef\uff0c\u4f46\u4e5f\u8981\u5f53\u5fc3\u53cd\u9707";

            currentScene = SCENE_BATTLE;
        }
    }
    else {
        leftDraft = candidates[chosen];

        EggRuntimeState aiCandidates[3];
        generateCandidateBatch(aiCandidates);
        rightDraft = aiCandidates[randInt(0, 2)];

        leftEgg = makeBattleEgg(leftDraft, 260.0f, 360.0f, 0.0f);
        rightEgg = makeBattleEgg(rightDraft, 740.0f, 360.0f, PI_F);

        winnerState = 0;
        hitPauseFrames = 0;
        slowMotionFrames = 0;
        openingFrames = 110;
        shakeFrames = 0;
        shakeStrength = 0;

        log1 = L"\u4f60\u5df2\u9009\u5b9a\u9e21\u86cb";
        log2 = L"\u7535\u8111\u4e5f\u5df2\u6311\u9009\u5b8c\u6210";
        log3 = L"\u73b0\u5728\u5f00\u59cb\u5bf9\u649e";

        currentScene = SCENE_BATTLE;
    }
}

int main() {
    srand((unsigned int)time(NULL));

    initgraph(SCREEN_W, SCREEN_H);
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

    wstring log1 = L"\u51c6\u5907\u5f00\u59cb";
    wstring log2 = L"\u8bf7\u5148\u9009\u62e9\u9e21\u86cb";
    wstring log3 = L"\u9009\u5b9a\u540e\u5373\u53ef\u5f00\u6218";

    Button btnPVP = { 350, 220, 650, 290, L"\u53cc\u4eba\u5bf9\u6218" };
    Button btnPVE = { 350, 330, 650, 400, L"\u4eba\u673a\u5bf9\u6218" };
    Button btnExit = { 350, 440, 650, 510, L"\u9000\u51fa\u6e38\u620f" };

    Button btnEgg1 = { 70, 510, 265, 570, L"\u9009\u62e9 1 \u53f7\u86cb" };
    Button btnEgg2 = { 405, 510, 600, 570, L"\u9009\u62e9 2 \u53f7\u86cb" };
    Button btnEgg3 = { 740, 510, 935, 570, L"\u9009\u62e9 3 \u53f7\u86cb" };
    Button btnBackSelect = { 390, 620, 610, 680, L"\u8fd4\u56de\u4e3b\u83dc\u5355" };

    Button btnInspectRotL = { 130, 620, 270, 680, L"\u5de6\u8f6c" };
    Button btnInspectRotR = { 310, 620, 450, 680, L"\u53f3\u8f6c" };
    Button btnInspectPick = { 610, 620, 790, 680, L"\u9009\u62e9\u8fd9\u9897\u86cb" };
    Button btnInspectBack = { 810, 620, 950, 680, L"\u8fd4\u56de\u9009\u86cb" };

    Button btnBackBattle = { 820, 645, 965, 700, L"\u8fd4\u56de\u83dc\u5355" };

    Button btnReplay = { 300, 646, 470, 706, L"\u518d\u6765\u4e00\u5c40" };
    Button btnResultMenu = { 530, 646, 700, 706, L"\u8fd4\u56de\u4e3b\u83dc\u5355" };

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
                        selectStep = 1;
                        winnerState = 0;
                        resultSoundPlayed = false;
                        generateCandidateBatch(selectCandidates);
                        currentScene = SCENE_SELECT_EGG;
                    }
                    else if (isInside(btnPVE, x, y)) {
                        playEffect(L"ui_click.wav");
                        currentMode = 2;
                        selectStep = 1;
                        winnerState = 0;
                        resultSoundPlayed = false;
                        generateCandidateBatch(selectCandidates);
                        currentScene = SCENE_SELECT_EGG;
                    }
                    else if (isInside(btnExit, x, y)) {
                        playEffect(L"ui_click.wav");
                        running = false;
                    }
                }
                else if (currentScene == SCENE_SELECT_EGG) {
                    if (isInside(btnBackSelect, x, y)) {
                        playEffect(L"ui_click.wav");
                        currentScene = SCENE_MENU;
                        currentMode = 0;
                        selectStep = 1;
                    }
                    else {
                        int chosen = -1;

                        if (isInside(btnEgg1, x, y)) chosen = 0;
                        else if (isInside(btnEgg2, x, y)) chosen = 1;
                        else if (isInside(btnEgg3, x, y)) chosen = 2;

                        if (chosen != -1) {
                            playEffect(L"ui_click.wav");
                            chooseEggAndAdvance(
                                currentMode,
                                selectStep,
                                chosen,
                                selectCandidates,
                                leftDraft,
                                rightDraft,
                                leftEgg,
                                rightEgg,
                                winnerState,
                                hitPauseFrames,
                                slowMotionFrames,
                                openingFrames,
                                shakeFrames,
                                shakeStrength,
                                log1,
                                log2,
                                log3,
                                currentScene
                            );
                        }
                        else {
                            if (pointInCard(x, y, 35, 120)) chosen = 0;
                            else if (pointInCard(x, y, 385, 120)) chosen = 1;
                            else if (pointInCard(x, y, 735, 120)) chosen = 2;

                            if (chosen != -1) {
                                playEffect(L"inspect.wav");
                                inspectEgg = selectCandidates[chosen];
                                inspectIndex = chosen;
                                inspectAngle = -PI_F / 2.0f;
                                currentScene = SCENE_INSPECT;
                            }
                        }
                    }
                }
                else if (currentScene == SCENE_INSPECT) {
                    if (isInside(btnInspectRotL, x, y)) {
                        playEffect(L"ui_click.wav");
                        inspectAngle -= 0.18f;
                    }
                    else if (isInside(btnInspectRotR, x, y)) {
                        playEffect(L"ui_click.wav");
                        inspectAngle += 0.18f;
                    }
                    else if (isInside(btnInspectBack, x, y)) {
                        playEffect(L"ui_click.wav");
                        currentScene = SCENE_SELECT_EGG;
                    }
                    else if (isInside(btnInspectPick, x, y)) {
                        playEffect(L"ui_click.wav");
                        chooseEggAndAdvance(
                            currentMode,
                            selectStep,
                            inspectIndex,
                            selectCandidates,
                            leftDraft,
                            rightDraft,
                            leftEgg,
                            rightEgg,
                            winnerState,
                            hitPauseFrames,
                            slowMotionFrames,
                            openingFrames,
                            shakeFrames,
                            shakeStrength,
                            log1,
                            log2,
                            log3,
                            currentScene
                        );
                    }
                }
                else if (currentScene == SCENE_BATTLE) {
                    if (isInside(btnBackBattle, x, y)) {
                        playEffect(L"ui_click.wav");
                        currentScene = SCENE_MENU;
                        currentMode = 0;
                        selectStep = 1;
                    }
                }
                else if (currentScene == SCENE_RESULT) {
                    if (isInside(btnReplay, x, y)) {
                        playEffect(L"ui_click.wav");
                        selectStep = 1;
                        winnerState = 0;
                        resultSoundPlayed = false;
                        generateCandidateBatch(selectCandidates);
                        currentScene = SCENE_SELECT_EGG;
                    }
                    else if (isInside(btnResultMenu, x, y)) {
                        playEffect(L"ui_click.wav");
                        currentScene = SCENE_MENU;
                        currentMode = 0;
                        selectStep = 1;
                    }
                }
            }
        }

        if (currentScene == SCENE_BATTLE && winnerState == 0) {
            updateBattle(
                currentMode,
                openingFrames,
                leftEgg,
                rightEgg,
                winnerState,
                hitPauseFrames,
                slowMotionFrames,
                shakeFrames,
                shakeStrength,
                lastMouseX,
                lastMouseY,
                log1,
                log2,
                log3
            );

            if (winnerState != 0) {
                if (winnerState == 1) {
                    log1 = (currentMode == 1) ? L"\u73a9\u5bb61 \u83b7\u80dc" : L"\u4f60\u83b7\u80dc\u4e86";
                    log2 = L"\u5173\u952e\u4e00\u649e\u5df2\u7ecf\u51b3\u5b9a\u80dc\u8d1f";
                    log3 = L"\u8fd9\u5c40\u7684\u8282\u594f\u548c\u89d2\u5ea6\u628a\u63e1\u5f97\u66f4\u597d";
                }
                else if (winnerState == 2) {
                    log1 = (currentMode == 1) ? L"\u73a9\u5bb62 \u83b7\u80dc" : L"\u7535\u8111\u83b7\u80dc";
                    log2 = L"\u5173\u952e\u4e00\u649e\u5df2\u7ecf\u51b3\u5b9a\u80dc\u8d1f";
                    log3 = L"\u8fd9\u5c40\u7684\u5173\u952e\u78b0\u649e\u88ab\u5bf9\u624b\u62ff\u4e0b\u4e86";
                }
                else {
                    log1 = L"\u53cc\u65b9\u540c\u65f6\u62a5\u5e9f";
                    log2 = L"\u8fd9\u662f\u4e00\u5c40\u5e73\u5c40";
                    log3 = L"\u5c16\u5bf9\u5c16\u7684\u9ad8\u98ce\u9669\u5c31\u662f\u8fd9\u6837";
                }
                currentScene = SCENE_RESULT;
                resultSoundPlayed = false;
            }
        }

        if (currentScene == SCENE_RESULT && !resultSoundPlayed) {
            if (winnerState == 1) {
                if (currentMode == 1) playEffect(L"win.wav");
                else playEffect(L"win.wav");
            }
            else if (winnerState == 2) {
                if (currentMode == 1) playEffect(L"lose.wav");
                else playEffect(L"lose.wav");
            }
            else {
                playEffect(L"draw.wav");
            }
            resultSoundPlayed = true;
        }

        if (currentScene == SCENE_MENU) {
            drawMenuScene(btnPVP, btnPVE, btnExit);
        }
        else if (currentScene == SCENE_SELECT_EGG) {
            drawSelectScene(btnEgg1, btnEgg2, btnEgg3, btnBackSelect, currentMode, selectStep, selectCandidates);
        }
        else if (currentScene == SCENE_INSPECT) {
            drawInspectScene(btnInspectRotL, btnInspectRotR, btnInspectPick, btnInspectBack, inspectEgg, inspectIndex, inspectAngle);
        }
        else if (currentScene == SCENE_BATTLE) {
            drawBattleScene(btnBackBattle, currentMode, openingFrames, leftEgg, rightEgg, log1, log2, log3, shakeFrames, shakeStrength);
        }
        else if (currentScene == SCENE_RESULT) {
            drawResultScene(btnReplay, btnResultMenu, currentMode, winnerState, leftDraft, rightDraft, leftEgg, rightEgg, log1, log2, log3);
        }

        FlushBatchDraw();
    }

    stopBGM();
    EndBatchDraw();
    closegraph();
    return 0;
}