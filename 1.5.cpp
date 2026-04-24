#include <graphics.h>
#include <conio.h>
#include <ctime>
#include <cstdlib>
#include <string>

using std::wstring;

const wchar_t* FONT_NAME = L"Microsoft YaHei";

enum SceneType {
    SCENE_MENU,
    SCENE_SELECT_EGG,
    SCENE_BATTLE,
    SCENE_RESULT
};

struct Button {
    int left;
    int top;
    int right;
    int bottom;
    const wchar_t* text;
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

int randInt(int l, int r) {
    return l + rand() % (r - l + 1);
}

wstring toW(int x) {
    return std::to_wstring(x);
}

bool isInside(const Button& btn, int x, int y) {
    return x >= btn.left && x <= btn.right &&
        y >= btn.top && y <= btn.bottom;
}

void setUIFont(int size) {
    settextstyle(size, 0, FONT_NAME);
}

void drawTextCenteredInButton(const Button& btn, const wchar_t* text, int fontSize, COLORREF color) {
    setbkmode(TRANSPARENT);
    settextcolor(color);
    setUIFont(fontSize);

    int tw = textwidth(text);
    int th = textheight(text);
    int tx = (btn.left + btn.right - tw) / 2;
    int ty = (btn.top + btn.bottom - th) / 2;

    outtextxy(tx, ty, text);
}

void drawButton(const Button& btn, bool enabled) {
    if (enabled) {
        setlinecolor(RGB(70, 130, 180));
        setfillcolor(RGB(220, 235, 250));
    }
    else {
        setlinecolor(RGB(170, 170, 170));
        setfillcolor(RGB(235, 235, 235));
    }

    solidrectangle(btn.left, btn.top, btn.right, btn.bottom);
    rectangle(btn.left, btn.top, btn.right, btn.bottom);

    drawTextCenteredInButton(btn, btn.text, 22, enabled ? BLACK : RGB(140, 140, 140));
}

void shuffleClues(wstring arr[], int n) {
    for (int i = n - 1; i > 0; --i) {
        int j = randInt(0, i);
        wstring t = arr[i];
        arr[i] = arr[j];
        arr[j] = t;
    }
}

int clampMin(int x, int mn) {
    return x < mn ? mn : x;
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
    for (int i = 0; i < 6; ++i) {
        egg.clues[i] = temp[i];
    }

    return egg;
}

void generateCandidateBatch(EggRuntimeState eggs[3]) {
    for (int i = 0; i < 3; ++i) {
        eggs[i] = generateRandomEgg();
    }
}

int getAttackValue(const EggRuntimeState& egg, int endType) {
    return (endType == 1) ? egg.sharpAttack : egg.bluntAttack;
}

int getDefenseValue(const EggRuntimeState& egg, int endType) {
    return (endType == 1) ? egg.sharpDefense : egg.bluntDefense;
}

int getMaxHP(const EggRuntimeState& egg, int endType) {
    return (endType == 1) ? egg.sharpMaxHP : egg.bluntMaxHP;
}

int getEndStateLevel(const EggRuntimeState& egg, int endType) {
    int hp = (endType == 1) ? egg.sharpHP : egg.bluntHP;
    int maxHP = getMaxHP(egg, endType);

    if (hp <= 0) return 3;
    if (hp * 100 / maxHP <= 30) return 2;
    if (hp * 100 / maxHP <= 70) return 1;
    return 0;
}

const wchar_t* getStateText(const EggRuntimeState& egg, int endType) {
    int level = getEndStateLevel(egg, endType);
    if (level == 3) return L"\u7834\u88c2";
    if (level == 2) return L"\u660e\u663e\u88c2\u7eb9";
    if (level == 1) return L"\u8f7b\u5fae\u88c2\u7eb9";
    return L"\u5b8c\u6574";
}

bool isEggDefeated(const EggRuntimeState& egg) {
    return egg.sharpHP <= 0 || egg.bluntHP <= 0;
}

const wchar_t* getActionName(int action, bool attackMode) {
    if (action == 1) return attackMode ? L"\u5c16\u7aef\u8fdb\u653b" : L"\u5c16\u7aef\u9632\u5b88";
    if (action == 2) return attackMode ? L"\u949d\u7aef\u8fdb\u653b" : L"\u949d\u7aef\u9632\u5b88";
    return L"\u65e0";
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

void getEggBodyRect(const EggRuntimeState& egg, int x, int y, int& left, int& top, int& right, int& bottom) {
    if (egg.shapeType == 0) {
        left = x + 66;
        top = y + 56;
        right = x + 174;
        bottom = y + 184;
    }
    else if (egg.shapeType == 1) {
        left = x + 70;
        top = y + 52;
        right = x + 170;
        bottom = y + 183;
    }
    else {
        left = x + 74;
        top = y + 50;
        right = x + 166;
        bottom = y + 182;
    }
}

void drawEggBase(const EggRuntimeState& egg, int x, int y) {
    int left, top, right, bottom, cx;
    getEggBodyRect(egg, x, y, left, top, right, bottom);
    cx = (left + right) / 2;

    setlinecolor(RGB(90, 90, 90));

    if (egg.toneType == 0) {
        setfillcolor(RGB(250, 248, 240));
    }
    else if (egg.toneType == 1) {
        setfillcolor(RGB(242, 232, 206));
    }
    else {
        setfillcolor(RGB(228, 210, 174));
    }

    solidellipse(left, top, right, bottom);
    ellipse(left, top, right, bottom);

    if (egg.surfaceType == 1) {
        setlinecolor(RGB(185, 185, 185));
        line(cx - 6, top + 64, cx + 4, top + 82);
        line(cx + 4, top + 82, cx - 2, top + 98);
    }
    else if (egg.surfaceType == 2) {
        setlinecolor(RGB(190, 186, 170));
        line(cx - 18, bottom - 36, cx + 20, bottom - 32);
        line(cx - 8, top + 34, cx - 2, top + 62);
    }
    else {
        setlinecolor(RGB(215, 215, 215));
        line(cx - 5, top + 30, cx - 1, top + 58);
    }
}

void drawSharpCrack(int cx, int top, int level) {
    setlinecolor(RGB(105, 82, 82));

    line(cx, top + 18, cx - 4, top + 28);
    line(cx - 4, top + 28, cx + 2, top + 38);

    if (level >= 1) {
        line(cx - 4, top + 28, cx - 12, top + 34);
        line(cx + 2, top + 38, cx + 9, top + 42);
    }

    if (level >= 2) {
        line(cx + 2, top + 38, cx - 2, top + 50);
        line(cx - 2, top + 50, cx + 6, top + 58);
        line(cx - 2, top + 50, cx - 10, top + 56);
        line(cx + 1, top + 34, cx + 10, top + 30);
    }

    if (level >= 3) {
        line(cx + 6, top + 58, cx + 1, top + 70);
        line(cx + 1, top + 70, cx + 11, top + 76);
        line(cx - 10, top + 56, cx - 16, top + 66);
        line(cx - 3, top + 46, cx - 15, top + 48);
        line(cx + 4, top + 52, cx + 16, top + 56);
    }
}

void drawBluntCrack(int cx, int bottom, int level) {
    setlinecolor(RGB(105, 82, 82));

    line(cx + 1, bottom - 40, cx - 5, bottom - 28);
    line(cx - 5, bottom - 28, cx + 2, bottom - 16);

    if (level >= 1) {
        line(cx - 5, bottom - 28, cx - 14, bottom - 22);
        line(cx + 2, bottom - 16, cx + 10, bottom - 10);
    }

    if (level >= 2) {
        line(cx + 2, bottom - 16, cx - 2, bottom - 4);
        line(cx - 2, bottom - 4, cx + 8, bottom + 2);
        line(cx - 1, bottom - 22, cx + 10, bottom - 18);
        line(cx - 4, bottom - 20, cx - 14, bottom - 12);
    }

    if (level >= 3) {
        line(cx + 8, bottom + 2, cx + 2, bottom + 12);
        line(cx + 2, bottom + 12, cx + 14, bottom + 16);
        line(cx - 14, bottom - 12, cx - 20, bottom - 2);
        line(cx + 6, bottom - 24, cx + 18, bottom - 22);
        line(cx - 7, bottom - 10, cx - 18, bottom - 6);
    }
}

void drawEggDamageOverlay(const EggRuntimeState& egg, int x, int y) {
    int left, top, right, bottom, cx;
    getEggBodyRect(egg, x, y, left, top, right, bottom);
    cx = (left + right) / 2;

    if (getEndStateLevel(egg, 1) > 0) {
        drawSharpCrack(cx - 4, top, getEndStateLevel(egg, 1));
    }
    if (getEndStateLevel(egg, 2) > 0) {
        drawBluntCrack(cx + 4, bottom, getEndStateLevel(egg, 2));
    }
}

void drawEggCard(int x, int y, const EggRuntimeState& egg, const wchar_t* title) {
    setlinecolor(RGB(120, 120, 120));
    setfillcolor(RGB(250, 250, 250));
    solidrectangle(x, y, x + 230, y + 360);
    rectangle(x, y, x + 230, y + 360);

    setbkmode(TRANSPARENT);
    settextcolor(BLACK);

    setUIFont(24);
    outtextxy(x + 82, y + 18, title);

    drawEggBase(egg, x + 2, y + 8);

    setUIFont(17);
    for (int i = 0; i < egg.clueCount; ++i) {
        outtextxy(x + 16, y + 215 + i * 22, egg.clues[i].c_str());
    }
}

void drawBattleLogBox(int x, int y, int w, int h, const wstring& line1, const wstring& line2, const wstring& line3) {
    setlinecolor(RGB(100, 100, 100));
    setfillcolor(RGB(250, 250, 250));
    solidrectangle(x, y, x + w, y + h);
    rectangle(x, y, x + w, y + h);

    setbkmode(TRANSPARENT);
    settextcolor(BLACK);

    setUIFont(22);
    outtextxy(x + 20, y + 12, L"\u5bf9\u6218\u65e5\u5fd7");

    setUIFont(18);
    outtextxy(x + 20, y + 48, line1.c_str());
    outtextxy(x + 20, y + 76, line2.c_str());
    outtextxy(x + 20, y + 104, line3.c_str());
}

void drawBattleEgg(int x, int y, const wchar_t* title, const EggRuntimeState& egg) {
    wstring state1 = wstring(L"\u5c16\u7aef\uff1a") + getStateText(egg, 1);
    wstring state2 = wstring(L"\u949d\u7aef\uff1a") + getStateText(egg, 2);

    setlinecolor(RGB(120, 120, 120));
    setfillcolor(RGB(250, 250, 250));
    solidrectangle(x, y, x + 260, y + 260);
    rectangle(x, y, x + 260, y + 260);

    setbkmode(TRANSPARENT);
    settextcolor(BLACK);

    setUIFont(26);
    outtextxy(x + 70, y + 15, title);

    drawEggBase(egg, x + 20, y + 20);
    drawEggDamageOverlay(egg, x + 20, y + 20);

    setUIFont(18);
    outtextxy(x + 20, y + 210, state1.c_str());
    outtextxy(x + 20, y + 235, state2.c_str());
}

void drawMenuScene(const Button& btnPVP, const Button& btnPVE, const Button& btnExit) {
    cleardevice();

    setbkmode(TRANSPARENT);
    settextcolor(BLACK);

    setUIFont(42);
    outtextxy(340, 80, L"\u86cb\u5c16\u5bf9\u51b3");

    setUIFont(22);
    outtextxy(330, 140, L"EasyX \u56fe\u5f62\u754c\u9762\u7248 1.5");

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
    cleardevice();

    setbkmode(TRANSPARENT);
    settextcolor(BLACK);

    setUIFont(34);
    if (currentMode == 1 && selectStep == 2) {
        outtextxy(300, 25, L"\u73a9\u5bb62 \u9009\u62e9\u9e21\u86cb");
    }
    else {
        outtextxy(300, 25, L"\u73a9\u5bb61 \u9009\u62e9\u9e21\u86cb");
    }

    setUIFont(20);
    outtextxy(110, 72, L"\u672c\u5c40\u4f1a\u968f\u673a\u751f\u6210\u4e09\u9897\u5019\u9009\u86cb\u3002\u53ea\u80fd\u6839\u636e\u89c2\u5bdf\u7ebf\u7d22\u3001\u76f4\u89c9\u548c\u4e00\u70b9\u8fd0\u6c14\u6765\u6311\u9009\uff1a");

    drawEggCard(35, 120, candidates[0], L"1\u53f7\u86cb");
    drawEggCard(385, 120, candidates[1], L"2\u53f7\u86cb");
    drawEggCard(735, 120, candidates[2], L"3\u53f7\u86cb");

    drawButton(btnEgg1, true);
    drawButton(btnEgg2, true);
    drawButton(btnEgg3, true);
    drawButton(btnBack, true);
}

wstring makeBattlePrompt(int currentMode, int battleSubPhase, int battleChoiceStage) {
    if (currentMode == 1) {
        if (battleChoiceStage == 1) return L"\u5f53\u524d\u64cd\u4f5c\uff1a\u8bf7\u9009\u62e9\u8fdb\u653b\u7aef";
        return L"\u5f53\u524d\u64cd\u4f5c\uff1a\u8bf7\u9009\u62e9\u9632\u5b88\u7aef";
    }

    if (battleSubPhase == 1) return L"\u5f53\u524d\u64cd\u4f5c\uff1a\u8bf7\u9009\u62e9\u4f60\u7684\u8fdb\u653b\u7aef";
    return L"\u5f53\u524d\u64cd\u4f5c\uff1a\u8bf7\u9009\u62e9\u4f60\u7684\u9632\u5b88\u7aef";
}

void drawBattleScene(
    const Button& btnSharpAttack,
    const Button& btnBluntAttack,
    const Button& btnSharpDefend,
    const Button& btnBluntDefend,
    const Button& btnResolve,
    const Button& btnBackToMenu,
    int currentMode,
    int battleRound,
    int battleSubPhase,
    int battleChoiceStage,
    const EggRuntimeState& leftEgg,
    const EggRuntimeState& rightEgg,
    int selectedAttack,
    int selectedDefense,
    const wstring& log1,
    const wstring& log2,
    const wstring& log3
) {
    bool enableSharpAttack = false;
    bool enableBluntAttack = false;
    bool enableSharpDefend = false;
    bool enableBluntDefend = false;
    bool enableResolve = false;

    cleardevice();

    setbkmode(TRANSPARENT);
    settextcolor(BLACK);

    setUIFont(34);
    outtextxy(360, 18, L"\u5bf9\u6218\u754c\u9762");

    setUIFont(20);
    if (currentMode == 1) {
        outtextxy(390, 58, L"\u6a21\u5f0f\uff1a\u53cc\u4eba\u5bf9\u6218");
    }
    else {
        outtextxy(390, 58, L"\u6a21\u5f0f\uff1a\u4eba\u673a\u5bf9\u6218");
    }

    wstring roundText;
    if (currentMode == 1) {
        if (battleSubPhase == 1) {
            roundText = L"\u7b2c " + toW(battleRound) + L" \u56de\u5408\uff1a\u73a9\u5bb61\u8fdb\u653b / \u73a9\u5bb62\u9632\u5b88";
        }
        else {
            roundText = L"\u7b2c " + toW(battleRound) + L" \u56de\u5408\uff1a\u73a9\u5bb62\u8fdb\u653b / \u73a9\u5bb61\u9632\u5b88";
        }
    }
    else {
        if (battleSubPhase == 1) {
            roundText = L"\u7b2c " + toW(battleRound) + L" \u56de\u5408\uff1a\u4f60\u8fdb\u653b / \u7535\u8111\u9632\u5b88";
        }
        else {
            roundText = L"\u7b2c " + toW(battleRound) + L" \u56de\u5408\uff1a\u7535\u8111\u8fdb\u653b / \u4f60\u9632\u5b88";
        }
    }
    outtextxy(220, 88, roundText.c_str());

    setUIFont(18);
    outtextxy(320, 118, makeBattlePrompt(currentMode, battleSubPhase, battleChoiceStage).c_str());

    drawBattleEgg(60, 150, L"\u5de6\u4fa7\u9e21\u86cb", leftEgg);
    drawBattleEgg(680, 150, L"\u53f3\u4fa7\u9e21\u86cb", rightEgg);

    if (currentMode == 1) {
        if (battleChoiceStage == 1) {
            enableSharpAttack = true;
            enableBluntAttack = true;
        }
        else {
            enableSharpDefend = true;
            enableBluntDefend = true;
            enableResolve = (selectedDefense != 0);
        }
    }
    else {
        if (battleSubPhase == 1) {
            enableSharpAttack = true;
            enableBluntAttack = true;
            enableResolve = (selectedAttack != 0);
        }
        else {
            enableSharpDefend = true;
            enableBluntDefend = true;
            enableResolve = (selectedDefense != 0);
        }
    }

    drawButton(btnSharpAttack, enableSharpAttack);
    drawButton(btnBluntAttack, enableBluntAttack);
    drawButton(btnSharpDefend, enableSharpDefend);
    drawButton(btnBluntDefend, enableBluntDefend);
    drawButton(btnResolve, enableResolve);
    drawButton(btnBackToMenu, true);

    wstring selectedAttackText = wstring(L"\u5df2\u9009\u8fdb\u653b\u7aef\uff1a") + getActionName(selectedAttack, true);
    wstring selectedDefenseText = wstring(L"\u5df2\u9009\u9632\u5b88\u7aef\uff1a") + getActionName(selectedDefense, false);

    setUIFont(18);
    outtextxy(410, 505, selectedAttackText.c_str());
    outtextxy(410, 530, selectedDefenseText.c_str());

    drawBattleLogBox(140, 570, 700, 120, log1, log2, log3);
}

void drawResultScene(
    const Button& btnReplay,
    const Button& btnBackMenu,
    int currentMode,
    int winnerState,
    const EggRuntimeState& leftEgg,
    const EggRuntimeState& rightEgg,
    const wstring& log1,
    const wstring& log2,
    const wstring& log3
) {
    wstring winnerText = L"\u672c\u5c40\u5e73\u5c40";

    cleardevice();

    if (winnerState == 1) {
        winnerText = (currentMode == 1) ? L"\u73a9\u5bb61 \u83b7\u80dc" : L"\u4f60\u83b7\u80dc\u4e86";
    }
    else if (winnerState == 2) {
        winnerText = (currentMode == 1) ? L"\u73a9\u5bb62 \u83b7\u80dc" : L"\u7535\u8111\u83b7\u80dc";
    }

    setbkmode(TRANSPARENT);
    settextcolor(BLACK);

    setUIFont(36);
    outtextxy(390, 30, L"\u5bf9\u6218\u7ed3\u679c");

    setUIFont(28);
    outtextxy(390, 80, winnerText.c_str());

    drawBattleEgg(90, 150, L"\u5de6\u4fa7\u6700\u7ec8\u72b6\u6001", leftEgg);
    drawBattleEgg(650, 150, L"\u53f3\u4fa7\u6700\u7ec8\u72b6\u6001", rightEgg);

    setUIFont(18);
    outtextxy(120, 430, getEggSummary(leftEgg).c_str());
    outtextxy(680, 430, getEggSummary(rightEgg).c_str());

    drawBattleLogBox(180, 470, 640, 120, log1, log2, log3);

    drawButton(btnReplay, true);
    drawButton(btnBackMenu, true);
}

int aiChooseAttack(const EggRuntimeState& egg) {
    if (egg.sharpHP <= 0) return 2;
    if (egg.bluntHP <= 0) return 1;
    return (egg.sharpAttack >= egg.bluntAttack) ? 1 : 2;
}

int aiChooseDefense(const EggRuntimeState& egg) {
    if (egg.sharpHP <= 0) return 2;
    if (egg.bluntHP <= 0) return 1;
    return (egg.bluntDefense >= egg.sharpDefense) ? 2 : 1;
}

void resolveBattleAction(
    int currentMode,
    int& battleRound,
    int& battleSubPhase,
    int& battleChoiceStage,
    int& selectedAttack,
    int& selectedDefense,
    EggRuntimeState& leftEgg,
    EggRuntimeState& rightEgg,
    int& winnerState,
    wstring& log1,
    wstring& log2,
    wstring& log3
) {
    EggRuntimeState* attacker;
    EggRuntimeState* defender;
    wstring attackerName;
    wstring defenderName;
    int* attackerEndHP;
    int* defenderEndHP;

    if (battleSubPhase == 1) {
        attacker = &leftEgg;
        defender = &rightEgg;
        attackerName = (currentMode == 1) ? L"\u73a9\u5bb61" : L"\u4f60";
        defenderName = (currentMode == 1) ? L"\u73a9\u5bb62" : L"\u7535\u8111";
    }
    else {
        attacker = &rightEgg;
        defender = &leftEgg;
        attackerName = (currentMode == 1) ? L"\u73a9\u5bb62" : L"\u7535\u8111";
        defenderName = (currentMode == 1) ? L"\u73a9\u5bb61" : L"\u4f60";
    }

    attackerEndHP = (selectedAttack == 1) ? &attacker->sharpHP : &attacker->bluntHP;
    defenderEndHP = (selectedDefense == 1) ? &defender->sharpHP : &defender->bluntHP;

    if (*attackerEndHP <= 0) {
        log1 = L"\u6240\u9009\u8fdb\u653b\u7aef\u5df2\u7ecf\u7834\u88c2\u3002";
        log2 = L"\u8bf7\u6539\u9009\u53e6\u4e00\u7aef\u3002";
        log3 = L"\u672c\u6b21\u6ca1\u6709\u5b8c\u6210\u7ed3\u7b97\u3002";
        return;
    }

    if (*defenderEndHP <= 0) {
        log1 = L"\u6240\u9009\u9632\u5b88\u7aef\u5df2\u7ecf\u7834\u88c2\u3002";
        log2 = L"\u8bf7\u6539\u9009\u53e6\u4e00\u7aef\u3002";
        log3 = L"\u672c\u6b21\u6ca1\u6709\u5b8c\u6210\u7ed3\u7b97\u3002";
        return;
    }

    int attackPower = getAttackValue(*attacker, selectedAttack);
    int defensePower = getDefenseValue(*defender, selectedDefense);
    int delta = randInt(-1, 1);
    int finalValue = attackPower - defensePower + delta;

    log1 = L"\u7b2c " + toW(battleRound) + L" \u56de\u5408\uff0c\u7b2c " + toW(battleSubPhase) + L" \u9636\u6bb5";
    log2 = attackerName + L" \u4f7f\u7528 " + getActionName(selectedAttack, true) +
        L"\uff0c" + defenderName + L" \u4f7f\u7528 " + getActionName(selectedDefense, false);

    if (finalValue <= 0) {
        *attackerEndHP -= 1;
        if (*attackerEndHP < 0) *attackerEndHP = 0;
        log3 = L"\u9632\u5b88\u6210\u529f\uff0c\u8fdb\u653b\u65b9\u53d7\u5230 1 \u70b9\u53cd\u9707\u4f24\u5bb3\u3002";
    }
    else if (finalValue == 1) {
        *defenderEndHP -= 1;
        if (*defenderEndHP < 0) *defenderEndHP = 0;
        log3 = L"\u8f7b\u5ea6\u547d\u4e2d\uff0c\u9632\u5b88\u65b9\u53d7\u5230 1 \u70b9\u4f24\u5bb3\u3002";
    }
    else {
        *defenderEndHP -= 2;
        if (*defenderEndHP < 0) *defenderEndHP = 0;
        log3 = L"\u91cd\u51fb\u547d\u4e2d\uff0c\u9632\u5b88\u65b9\u53d7\u5230 2 \u70b9\u4f24\u5bb3\u3002";
    }

    if (isEggDefeated(leftEgg) && isEggDefeated(rightEgg)) {
        winnerState = 3;
    }
    else if (isEggDefeated(rightEgg)) {
        winnerState = 1;
    }
    else if (isEggDefeated(leftEgg)) {
        winnerState = 2;
    }

    selectedAttack = 0;
    selectedDefense = 0;

    if (winnerState == 0) {
        if (battleSubPhase == 1) {
            battleSubPhase = 2;
        }
        else {
            battleSubPhase = 1;
            battleRound++;
        }
        battleChoiceStage = 1;
    }
}

int main() {
    srand((unsigned int)time(NULL));

    initgraph(1000, 720);
    setbkcolor(RGB(245, 248, 252));
    cleardevice();

    SceneType currentScene = SCENE_MENU;
    bool needRedraw = true;

    int currentMode = 0;
    int selectStep = 1;

    EggRuntimeState selectCandidates[3];
    EggRuntimeState leftEgg{};
    EggRuntimeState rightEgg{};

    int battleRound = 1;
    int battleSubPhase = 1;
    int battleChoiceStage = 1;
    int selectedAttack = 0;
    int selectedDefense = 0;
    int winnerState = 0;

    wstring log1 = L"\u51c6\u5907\u5f00\u59cb\u5bf9\u6218\u3002";
    wstring log2 = L"\u8bf7\u5148\u5b8c\u6210\u9009\u86cb\u3002";
    wstring log3 = L"\u8fdb\u5165\u5bf9\u6218\u540e\u6309\u63d0\u793a\u64cd\u4f5c\u3002";

    Button btnPVP = { 350, 220, 650, 290, L"\u53cc\u4eba\u5bf9\u6218" };
    Button btnPVE = { 350, 330, 650, 400, L"\u4eba\u673a\u5bf9\u6218" };
    Button btnExit = { 350, 440, 650, 510, L"\u9000\u51fa\u6e38\u620f" };

    Button btnEgg1 = { 70, 510, 265, 570, L"\u9009\u62e9 1 \u53f7\u86cb" };
    Button btnEgg2 = { 405, 510, 600, 570, L"\u9009\u62e9 2 \u53f7\u86cb" };
    Button btnEgg3 = { 740, 510, 935, 570, L"\u9009\u62e9 3 \u53f7\u86cb" };
    Button btnBackSelect = { 390, 620, 610, 680, L"\u8fd4\u56de\u4e3b\u83dc\u5355" };

    Button btnSharpAttack = { 390, 160, 610, 215, L"\u5c16\u7aef\u8fdb\u653b" };
    Button btnBluntAttack = { 390, 230, 610, 285, L"\u949d\u7aef\u8fdb\u653b" };
    Button btnSharpDefend = { 390, 300, 610, 355, L"\u5c16\u7aef\u9632\u5b88" };
    Button btnBluntDefend = { 390, 370, 610, 425, L"\u949d\u7aef\u9632\u5b88" };
    Button btnResolve = { 390, 440, 610, 495, L"\u6267\u884c\u78b0\u649e" };
    Button btnBackBattle = { 860, 650, 980, 705, L"\u8fd4\u56de\u83dc\u5355" };

    Button btnReplay = { 300, 630, 470, 690, L"\u518d\u6765\u4e00\u5c40" };
    Button btnResultMenu = { 530, 630, 700, 690, L"\u8fd4\u56de\u4e3b\u83dc\u5355" };

    while (true) {
        if (needRedraw) {
            if (currentScene == SCENE_MENU) {
                drawMenuScene(btnPVP, btnPVE, btnExit);
            }
            else if (currentScene == SCENE_SELECT_EGG) {
                drawSelectScene(btnEgg1, btnEgg2, btnEgg3, btnBackSelect, currentMode, selectStep, selectCandidates);
            }
            else if (currentScene == SCENE_BATTLE) {
                drawBattleScene(
                    btnSharpAttack, btnBluntAttack, btnSharpDefend, btnBluntDefend,
                    btnResolve, btnBackBattle,
                    currentMode, battleRound, battleSubPhase, battleChoiceStage,
                    leftEgg, rightEgg,
                    selectedAttack, selectedDefense,
                    log1, log2, log3
                );
            }
            else if (currentScene == SCENE_RESULT) {
                drawResultScene(
                    btnReplay, btnResultMenu,
                    currentMode, winnerState,
                    leftEgg, rightEgg,
                    log1, log2, log3
                );
            }
            needRedraw = false;
        }

        ExMessage msg;
        if (peekmessage(&msg, EX_MOUSE) && msg.message == WM_LBUTTONDOWN) {
            int x = msg.x;
            int y = msg.y;

            if (currentScene == SCENE_MENU) {
                if (isInside(btnPVP, x, y)) {
                    currentMode = 1;
                    selectStep = 1;
                    generateCandidateBatch(selectCandidates);
                    currentScene = SCENE_SELECT_EGG;
                    needRedraw = true;
                }
                else if (isInside(btnPVE, x, y)) {
                    currentMode = 2;
                    selectStep = 1;
                    generateCandidateBatch(selectCandidates);
                    currentScene = SCENE_SELECT_EGG;
                    needRedraw = true;
                }
                else if (isInside(btnExit, x, y)) {
                    break;
                }
            }
            else if (currentScene == SCENE_SELECT_EGG) {
                if (isInside(btnBackSelect, x, y)) {
                    currentScene = SCENE_MENU;
                    currentMode = 0;
                    selectStep = 1;
                    needRedraw = true;
                }
                else {
                    int chosen = -1;
                    if (isInside(btnEgg1, x, y)) chosen = 0;
                    else if (isInside(btnEgg2, x, y)) chosen = 1;
                    else if (isInside(btnEgg3, x, y)) chosen = 2;

                    if (chosen != -1) {
                        if (currentMode == 1) {
                            if (selectStep == 1) {
                                leftEgg = selectCandidates[chosen];
                                selectStep = 2;
                                generateCandidateBatch(selectCandidates);

                                log1 = L"\u73a9\u5bb61 \u5df2\u5b8c\u6210\u9009\u86cb\u3002";
                                log2 = L"\u73b0\u5728\u8f6e\u5230\u73a9\u5bb62\uff0c\u4ece\u53e6\u4e00\u6279\u5019\u9009\u86cb\u91cc\u9009\u62e9\u3002";
                                log3 = L"\u6bcf\u5c40\u5019\u9009\u86cb\u90fd\u4f1a\u968f\u673a\u53d8\u5316\u3002";
                                needRedraw = true;
                            }
                            else {
                                rightEgg = selectCandidates[chosen];

                                battleRound = 1;
                                battleSubPhase = 1;
                                battleChoiceStage = 1;
                                selectedAttack = 0;
                                selectedDefense = 0;
                                winnerState = 0;

                                log1 = L"\u53cc\u65b9\u5df2\u5b8c\u6210\u9009\u86cb\u3002";
                                log2 = L"\u8bf7\u6309\u63d0\u793a\u4f9d\u6b21\u9009\u62e9\u8fdb\u653b\u7aef\u548c\u9632\u5b88\u7aef\u3002";
                                log3 = L"\u7136\u540e\u70b9\u51fb\u201c\u6267\u884c\u78b0\u649e\u201d\u3002";

                                currentScene = SCENE_BATTLE;
                                needRedraw = true;
                            }
                        }
                        else {
                            leftEgg = selectCandidates[chosen];

                            EggRuntimeState aiCandidates[3];
                            generateCandidateBatch(aiCandidates);
                            rightEgg = aiCandidates[randInt(0, 2)];

                            battleRound = 1;
                            battleSubPhase = 1;
                            battleChoiceStage = 1;
                            selectedAttack = 0;
                            selectedDefense = 0;
                            winnerState = 0;

                            log1 = L"\u4f60\u5df2\u5b8c\u6210\u9009\u86cb\u3002";
                            log2 = L"\u7535\u8111\u4e5f\u4ece\u968f\u673a\u5019\u9009\u86cb\u4e2d\u9009\u4e86\u4e00\u9897\u3002";
                            log3 = L"\u8bf7\u5f00\u59cb\u7b2c\u4e00\u56de\u5408\u3002";

                            currentScene = SCENE_BATTLE;
                            needRedraw = true;
                        }
                    }
                }
            }
            else if (currentScene == SCENE_BATTLE) {
                if (isInside(btnBackBattle, x, y)) {
                    currentScene = SCENE_MENU;
                    currentMode = 0;
                    selectStep = 1;
                    needRedraw = true;
                }
                else {
                    if (currentMode == 1) {
                        if (battleChoiceStage == 1) {
                            if (isInside(btnSharpAttack, x, y)) {
                                selectedAttack = 1;
                                battleChoiceStage = 2;
                                log1 = L"\u5df2\u5b8c\u6210\u8fdb\u653b\u7aef\u9009\u62e9\u3002";
                                log2 = L"\u73b0\u5728\u8bf7\u9632\u5b88\u65b9\u9009\u62e9\u9632\u5b88\u7aef\u3002";
                                log3 = L"\u9632\u5b88\u7aef\u9009\u5b8c\u540e\u518d\u6267\u884c\u78b0\u649e\u3002";
                                needRedraw = true;
                            }
                            else if (isInside(btnBluntAttack, x, y)) {
                                selectedAttack = 2;
                                battleChoiceStage = 2;
                                log1 = L"\u5df2\u5b8c\u6210\u8fdb\u653b\u7aef\u9009\u62e9\u3002";
                                log2 = L"\u73b0\u5728\u8bf7\u9632\u5b88\u65b9\u9009\u62e9\u9632\u5b88\u7aef\u3002";
                                log3 = L"\u9632\u5b88\u7aef\u9009\u5b8c\u540e\u518d\u6267\u884c\u78b0\u649e\u3002";
                                needRedraw = true;
                            }
                        }
                        else {
                            if (isInside(btnSharpDefend, x, y)) {
                                selectedDefense = 1;
                                log1 = L"\u5df2\u5b8c\u6210\u9632\u5b88\u7aef\u9009\u62e9\u3002";
                                log2 = L"\u73b0\u5728\u53ef\u4ee5\u6267\u884c\u672c\u6b21\u78b0\u649e\u3002";
                                log3 = L"\u70b9\u51fb\u201c\u6267\u884c\u78b0\u649e\u201d\u7ee7\u7eed\u3002";
                                needRedraw = true;
                            }
                            else if (isInside(btnBluntDefend, x, y)) {
                                selectedDefense = 2;
                                log1 = L"\u5df2\u5b8c\u6210\u9632\u5b88\u7aef\u9009\u62e9\u3002";
                                log2 = L"\u73b0\u5728\u53ef\u4ee5\u6267\u884c\u672c\u6b21\u78b0\u649e\u3002";
                                log3 = L"\u70b9\u51fb\u201c\u6267\u884c\u78b0\u649e\u201d\u7ee7\u7eed\u3002";
                                needRedraw = true;
                            }
                            else if (selectedDefense != 0 && isInside(btnResolve, x, y)) {
                                resolveBattleAction(
                                    currentMode, battleRound, battleSubPhase, battleChoiceStage,
                                    selectedAttack, selectedDefense,
                                    leftEgg, rightEgg, winnerState,
                                    log1, log2, log3
                                );
                                if (winnerState != 0) currentScene = SCENE_RESULT;
                                needRedraw = true;
                            }
                        }
                    }
                    else {
                        if (battleSubPhase == 1) {
                            if (isInside(btnSharpAttack, x, y)) {
                                selectedAttack = 1;
                                log1 = L"\u4f60\u5df2\u9009\u62e9\u5c16\u7aef\u8fdb\u653b\u3002";
                                log2 = L"\u7535\u8111\u4f1a\u81ea\u52a8\u9009\u62e9\u9632\u5b88\u7aef\u3002";
                                log3 = L"\u70b9\u51fb\u201c\u6267\u884c\u78b0\u649e\u201d\u7ee7\u7eed\u3002";
                                needRedraw = true;
                            }
                            else if (isInside(btnBluntAttack, x, y)) {
                                selectedAttack = 2;
                                log1 = L"\u4f60\u5df2\u9009\u62e9\u949d\u7aef\u8fdb\u653b\u3002";
                                log2 = L"\u7535\u8111\u4f1a\u81ea\u52a8\u9009\u62e9\u9632\u5b88\u7aef\u3002";
                                log3 = L"\u70b9\u51fb\u201c\u6267\u884c\u78b0\u649e\u201d\u7ee7\u7eed\u3002";
                                needRedraw = true;
                            }
                            else if (selectedAttack != 0 && isInside(btnResolve, x, y)) {
                                selectedDefense = aiChooseDefense(rightEgg);
                                resolveBattleAction(
                                    currentMode, battleRound, battleSubPhase, battleChoiceStage,
                                    selectedAttack, selectedDefense,
                                    leftEgg, rightEgg, winnerState,
                                    log1, log2, log3
                                );
                                if (winnerState != 0) currentScene = SCENE_RESULT;
                                needRedraw = true;
                            }
                        }
                        else {
                            if (isInside(btnSharpDefend, x, y)) {
                                selectedDefense = 1;
                                log1 = L"\u4f60\u5df2\u9009\u62e9\u5c16\u7aef\u9632\u5b88\u3002";
                                log2 = L"\u7535\u8111\u4f1a\u81ea\u52a8\u9009\u62e9\u8fdb\u653b\u7aef\u3002";
                                log3 = L"\u70b9\u51fb\u201c\u6267\u884c\u78b0\u649e\u201d\u7ee7\u7eed\u3002";
                                needRedraw = true;
                            }
                            else if (isInside(btnBluntDefend, x, y)) {
                                selectedDefense = 2;
                                log1 = L"\u4f60\u5df2\u9009\u62e9\u949d\u7aef\u9632\u5b88\u3002";
                                log2 = L"\u7535\u8111\u4f1a\u81ea\u52a8\u9009\u62e9\u8fdb\u653b\u7aef\u3002";
                                log3 = L"\u70b9\u51fb\u201c\u6267\u884c\u78b0\u649e\u201d\u7ee7\u7eed\u3002";
                                needRedraw = true;
                            }
                            else if (selectedDefense != 0 && isInside(btnResolve, x, y)) {
                                selectedAttack = aiChooseAttack(rightEgg);
                                resolveBattleAction(
                                    currentMode, battleRound, battleSubPhase, battleChoiceStage,
                                    selectedAttack, selectedDefense,
                                    leftEgg, rightEgg, winnerState,
                                    log1, log2, log3
                                );
                                if (winnerState != 0) currentScene = SCENE_RESULT;
                                needRedraw = true;
                            }
                        }
                    }
                }
            }
            else if (currentScene == SCENE_RESULT) {
                if (isInside(btnReplay, x, y)) {
                    selectStep = 1;
                    generateCandidateBatch(selectCandidates);
                    currentScene = SCENE_SELECT_EGG;
                    needRedraw = true;
                }
                else if (isInside(btnResultMenu, x, y)) {
                    currentScene = SCENE_MENU;
                    currentMode = 0;
                    selectStep = 1;
                    needRedraw = true;
                }
            }
        }
    }

    closegraph();
    return 0;
}