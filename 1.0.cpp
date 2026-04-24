#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <limits>
#include <vector>
#include <sstream>

#ifdef _WIN32
#include <windows.h>
#endif

using namespace std;

const string ZH_SHARP = "\xE5\xB0\x96\xE7\xAB\xAF";
const string ZH_BLUNT = "\xE9\x92\x9D\xE7\xAB\xAF";
const string ZH_INTACT = "\xE5\xAE\x8C\xE6\x95\xB4";
const string ZH_SLIGHT = "\xE8\xBD\xBB\xE5\xBE\xAE\xE8\xA3\x82\xE7\xBA\xB9";
const string ZH_VISIBLE = "\xE6\x98\x8E\xE6\x98\xBE\xE8\xA3\x82\xE7\xBA\xB9";
const string ZH_BROKEN = "\xE7\xA0\xB4\xE8\xA3\x82";
const string ZH_UNKNOWN = "\xE6\x9C\xAA\xE7\x9F\xA5";
const string ZH_PLAYER = "\xE7\x8E\xA9\xE5\xAE\xB6";
const string ZH_PLAYER1 = "\xE7\x8E\xA9\xE5\xAE\xB6\x31";
const string ZH_PLAYER2 = "\xE7\x8E\xA9\xE5\xAE\xB6\x32";
const string ZH_COMPUTER = "\xE7\x94\xB5\xE8\x84\x91";

const string ZH_INVALID_01 = "\xE8\xBE\x93\xE5\x85\xA5\xE6\x97\xA0\xE6\x95\x88\xEF\xBC\x8C\xE8\xAF\xB7\xE8\xBE\x93\xE5\x85\xA5\x20\x30\x20\xE6\x88\x96\x20\x31\xEF\xBC\x9A";
const string ZH_INVALID_129 = "\xE8\xBE\x93\xE5\x85\xA5\xE6\x97\xA0\xE6\x95\x88\xEF\xBC\x8C\xE8\xAF\xB7\xE8\xBE\x93\xE5\x85\xA5\x20\x31\xE3\x80\x81\x32\x20\xE6\x88\x96\x20\x39\xEF\xBC\x9A";
const string ZH_INVALID_123 = "\xE8\xBE\x93\xE5\x85\xA5\xE6\x97\xA0\xE6\x95\x88\xEF\xBC\x8C\xE8\xAF\xB7\xE8\xBE\x93\xE5\x85\xA5\x20\x31\xE3\x80\x81\x32\x20\xE6\x88\x96\x20\x33\xEF\xBC\x9A";

const string ZH_ASK_REPLAY = "\xE6\x98\xAF\xE5\x90\xA6\xE7\x94\xA8\xE7\x9B\xB8\xE5\x90\x8C\xE6\xA8\xA1\xE5\xBC\x8F\xE5\x92\x8C\xE5\x90\x8D\xE5\xAD\x97\xE5\x86\x8D\xE6\x9D\xA5\xE4\xB8\x80\xE5\xB1\x80\xEF\xBC\x9F";
const string ZH_BACK_MENU = "\x30\x20\x3D\x20\xE8\xBF\x94\xE5\x9B\x9E\xE4\xB8\xBB\xE8\x8F\x9C\xE5\x8D\x95";
const string ZH_PLAY_AGAIN = "\x31\x20\x3D\x20\xE5\x86\x8D\xE6\x9D\xA5\xE4\xB8\x80\xE5\xB1\x80";
const string ZH_ENTER_01 = "\xE8\xAF\xB7\xE8\xBE\x93\xE5\x85\xA5\x20\x30\x20\xE6\x88\x96\x20\x31\xEF\xBC\x9A";

const string ZH_TITLE = "\xE8\x9B\x8B\xE5\xB0\x96\xE5\xAF\xB9\xE5\x86\xB3\x20\xE5\xA4\xA7\xE4\xBD\x9C\xE4\xB8\x9A\x30\x2E\x37\xE7\x89\x88";
const string ZH_PVP = "\x31\x20\x3D\x20\xE5\x8F\x8C\xE4\xBA\xBA\xE5\xAF\xB9\xE6\x88\x98";
const string ZH_PVE = "\x32\x20\x3D\x20\xE4\xBA\xBA\xE6\x9C\xBA\xE5\xAF\xB9\xE6\x88\x98";
const string ZH_EXIT_GAME = "\x39\x20\x3D\x20\xE9\x80\x80\xE5\x87\xBA\xE6\xB8\xB8\xE6\x88\x8F";
const string ZH_ENTER_129 = "\xE8\xAF\xB7\xE8\xBE\x93\xE5\x85\xA5\x20\x31\xE3\x80\x81\x32\x20\xE6\x88\x96\x20\x39\xEF\xBC\x9A";

const string ZH_STATUS = "\xE5\xBD\x93\xE5\x89\x8D\xE7\x8A\xB6\xE6\x80\x81";
const string ZH_EGG_OF = "\x20\xE7\x9A\x84\xE9\xB8\xA1\xE8\x9B\x8B\xEF\xBC\x9A";
const string ZH_SHARP_STATE = "\x20\x20\xE5\xB0\x96\xE7\xAB\xAF\x20\x2D\x20\xE7\x8A\xB6\xE6\x80\x81\xEF\xBC\x9A";
const string ZH_BLUNT_STATE = "\x20\x20\xE9\x92\x9D\xE7\xAB\xAF\x20\x2D\x20\xE7\x8A\xB6\xE6\x80\x81\xEF\xBC\x9A";
const string ZH_CHOOSE_FOR = "\xEF\xBC\x8C\xE8\xAF\xB7\xE9\x80\x89\xE6\x8B\xA9\xE7\x94\xA8\xE4\xBA\x8E";
const string ZH_END_POINT = "\xE7\x9A\x84\xE7\xAB\xAF\xE7\x82\xB9\xEF\xBC\x9A";
const string ZH_OPTION_SHARP = "\x20\x20\x30\x20\x3D\x20\xE5\xB0\x96\xE7\xAB\xAF\x20\x5B\xE7\x8A\xB6\xE6\x80\x81\xEF\xBC\x9A";
const string ZH_OPTION_BLUNT = "\x20\x20\x31\x20\x3D\x20\xE9\x92\x9D\xE7\xAB\xAF\x20\x5B\xE7\x8A\xB6\xE6\x80\x81\xEF\xBC\x9A";

const string ZH_AI_CHOOSE_LEFT = "\x20\xE9\x80\x89\xE6\x8B\xA9\xE4\xBA\x86\x20\x5B";
const string ZH_AI_CHOOSE_RIGHT = "\x5D\x20\xE7\x94\xA8\xE4\xBA\x8E";
const string ZH_FULL_STOP = "\xE3\x80\x82";

const string ZH_BATTLE_RESULT = "\xE7\xA2\xB0\xE6\x92\x9E\xE7\xBB\x93\xE6\x9E\x9C";
const string ZH_USE_LEFT = "\x20\xE4\xBD\xBF\xE7\x94\xA8\x20\x5B";
const string ZH_ATTACK_RIGHT = "\x5D\x20\xE8\xBF\x9B\xE6\x94\xBB\xEF\xBC\x8C";
const string ZH_DEFEND_RIGHT = "\x5D\x20\xE9\x98\xB2\xE5\xAE\x88";
const string ZH_ATTACK_POWER = "\xE6\x94\xBB\xE5\x87\xBB\xE5\x8A\x9B\x20";
const string ZH_DEFENSE_POWER = "\x20\x2D\x20\xE9\x98\xB2\xE5\xBE\xA1\xE5\x8A\x9B\x20";
const string ZH_RANDOM_VALUE = "\x20\x2B\x20\xE9\x9A\x8F\xE6\x9C\xBA\xE5\x80\xBC\x28";
const string ZH_EQUAL = "\x20\x3D\x20";
const string ZH_DEFENSE_SUCCESS = "\xE9\x98\xB2\xE5\xAE\x88\xE6\x88\x90\xE5\x8A\x9F\xEF\xBC\x81";
const string ZH_OF_LEFT = "\x20\xE7\x9A\x84\x20\x5B";
const string ZH_TAKES = "\x5D\x20\xE5\x8F\x97\xE5\x88\xB0\x20";
const string ZH_RECOIL_DAMAGE = "\x20\xE7\x82\xB9\xE5\x8F\x8D\xE9\x9C\x87\xE4\xBC\xA4\xE5\xAE\xB3";
const string ZH_DAMAGE = "\x20\xE7\x82\xB9\xE4\xBC\xA4\xE5\xAE\xB3";
const string ZH_HIT = "\xE5\x91\xBD\xE4\xB8\xAD\xEF\xBC\x81";
const string ZH_HEAVY_HIT = "\xE9\x87\x8D\xE5\x87\xBB\xEF\xBC\x81";
const string ZH_ALREADY_BROKEN = "\x5D\x20\xE5\xB7\xB2\xE7\xBB\x8F\xE7\xA0\xB4\xE8\xA3\x82\xEF\xBC\x81";
const string ZH_BROKE_FROM_RECOIL = "\x5D\x20\xE5\x9B\xA0\xE5\x8F\x8D\xE9\x9C\x87\xE7\xA0\xB4\xE8\xA3\x82\xEF\xBC\x81";

const string ZH_LOG = "\xE5\xAF\xB9\xE6\x88\x98\xE6\x97\xA5\xE5\xBF\x97";
const string ZH_NO_LOG = "\xE6\x9A\x82\xE6\x97\xA0\xE5\xAF\xB9\xE6\x88\x98\xE8\xAE\xB0\xE5\xBD\x95\xE3\x80\x82";
const string ZH_WIN = "\x20\xE8\x8E\xB7\xE8\x83\x9C\xEF\xBC\x81";
const string ZH_DRAW = "\xE5\x8F\x8C\xE6\x96\xB9\xE9\xB8\xA1\xE8\x9B\x8B\xE9\x83\xBD\xE7\xA0\xB4\xE8\xA3\x82\xE4\xBA\x86\xEF\xBC\x8C\xE6\x9C\xAC\xE5\xB1\x80\xE5\xB9\xB3\xE5\xB1\x80\xEF\xBC\x81";
const string ZH_PHASE1 = "\xE9\x98\xB6\xE6\xAE\xB5\x31";
const string ZH_PHASE2 = "\xE9\x98\xB6\xE6\xAE\xB5\x32";
const string ZH_ATTACK_COMMA = "\x20\xE8\xBF\x9B\xE6\x94\xBB\xEF\xBC\x8C";
const string ZH_DEFEND = "\x20\xE9\x98\xB2\xE5\xAE\x88";

const string ZH_GOODBYE = "\xE6\xB8\xB8\xE6\x88\x8F\xE7\xBB\x93\xE6\x9D\x9F\xEF\xBC\x8C\xE5\x86\x8D\xE8\xA7\x81\xEF\xBC\x81";
const string ZH_INPUT_NAME1 = "\xE7\x8E\xA9\xE5\xAE\xB6\x31\xEF\xBC\x8C\xE8\xAF\xB7\xE8\xBE\x93\xE5\x85\xA5\xE4\xBD\xA0\xE7\x9A\x84\xE5\x90\x8D\xE5\xAD\x97\xEF\xBC\x9A";
const string ZH_INPUT_NAME2 = "\xE7\x8E\xA9\xE5\xAE\xB6\x32\xEF\xBC\x8C\xE8\xAF\xB7\xE8\xBE\x93\xE5\x85\xA5\xE4\xBD\xA0\xE7\x9A\x84\xE5\x90\x8D\xE5\xAD\x97\xEF\xBC\x9A";
const string ZH_PRESS_ENTER = "\xE6\x8C\x89\xE5\x9B\x9E\xE8\xBD\xA6\xE9\x94\xAE\xE9\x80\x80\xE5\x87\xBA\x2E\x2E\x2E";

const string ZH_DI = "\xE7\xAC\xAC";
const string ZH_ROUND_ONLY = "\xE5\x9B\x9E\xE5\x90\x88";
const string ZH_ROUND_COLON = "\xE5\x9B\x9E\xE5\x90\x88\xEF\xBC\x9A";
const string ZH_USE_BRACKET = "\xE4\xBD\xBF\xE7\x94\xA8\x5B";
const string ZH_ATTACK_BRACKET_COMMA = "\x5D\xE8\xBF\x9B\xE6\x94\xBB\xEF\xBC\x8C";
const string ZH_DEFEND_BRACKET_COMMA = "\x5D\xE9\x98\xB2\xE5\xAE\x88\xEF\xBC\x8C";
const string ZH_ATTACKER_RECOIL = "\xE6\x94\xBB\xE5\x87\xBB\xE6\x96\xB9\xE5\x8F\x8D\xE9\x9C\x87\xE5\x8F\x97\xE4\xBC\xA4";
const string ZH_POINT = "\xE7\x82\xB9";
const string ZH_DEFENDER_DAMAGE = "\xE9\x98\xB2\xE5\xAE\x88\xE6\x96\xB9\xE5\x8F\x97\xE4\xBC\xA4";
const string ZH_ATTACK_END_BROKEN = "\xEF\xBC\x8C\xE6\x94\xBB\xE5\x87\xBB\xE7\xAB\xAF\xE7\xA0\xB4\xE8\xA3\x82";
const string ZH_DEFEND_END_BROKEN = "\xEF\xBC\x8C\xE9\x98\xB2\xE5\xAE\x88\xE7\xAB\xAF\xE7\xA0\xB4\xE8\xA3\x82";

const string ZH_SUMMARY = "\xE6\x9C\xAC\xE5\xB1\x80\xE7\xBB\x9F\xE8\xAE\xA1";
const string ZH_TOTAL_ROUNDS = "\xE6\x80\xBB\xE5\x9B\x9E\xE5\x90\x88\xE6\x95\xB0\xEF\xBC\x9A";
const string ZH_TOTAL_COLLISIONS = "\xE6\x80\xBB\xE7\xA2\xB0\xE6\x92\x9E\xE6\xAC\xA1\xE6\x95\xB0\xEF\xBC\x9A";
const string ZH_DAMAGE_DEALT = "\xE9\x80\xA0\xE6\x88\x90\xE4\xBC\xA4\xE5\xAE\xB3\xEF\xBC\x9A";
const string ZH_RECOIL_TAKEN = "\xE6\x89\xBF\xE5\x8F\x97\xE5\x8F\x8D\xE9\x9C\x87\xEF\xBC\x9A";

const string ZH_SELECTION_STAGE = "\xE9\x80\x89\xE8\x9B\x8B\xE9\x98\xB6\xE6\xAE\xB5";
const string ZH_CANDIDATE_EGGS = "\xE5\x80\x99\xE9\x80\x89\xE9\xB8\xA1\xE8\x9B\x8B";
const string ZH_OBSERVE_3 = "\xE8\xAF\xB7\xE5\x85\x88\xE8\xA7\x82\xE5\xAF\x9F\xE4\xBB\xA5\xE4\xB8\x8B\x20\x33\x20\xE9\xA2\x97\xE9\xB8\xA1\xE8\x9B\x8B\xEF\xBC\x9A";
const string ZH_ENTER_CHOOSE_123 = "\xE8\xAF\xB7\xE8\xBE\x93\xE5\x85\xA5\xE4\xBD\xA0\xE8\xA6\x81\xE9\x80\x89\xE6\x8B\xA9\xE7\x9A\x84\xE9\xB8\xA1\xE8\x9B\x8B\xE7\xBC\x96\xE5\x8F\xB7\xEF\xBC\x88\x31\x2D\x33\xEF\xBC\x89\xEF\xBC\x9A";
const string ZH_OBSERVE_DESC = "\xE8\xA7\x82\xE5\xAF\x9F\xE6\x8F\x8F\xE8\xBF\xB0\xEF\xBC\x9A";
const string ZH_BODY_SLENDER = "\xE6\x95\xB4\xE4\xBD\x93\xE5\x81\x8F\xE4\xBF\xAE\xE9\x95\xBF";
const string ZH_BODY_ROUND = "\xE6\x95\xB4\xE4\xBD\x93\xE5\x81\x8F\xE5\x9C\x86\xE6\xB6\xA6";
const string ZH_BODY_BALANCED = "\xE6\x95\xB4\xE4\xBD\x93\xE8\xBE\x83\xE5\x8C\x80\xE7\xA7\xB0";
const string ZH_SHARP_POINTY = "\xE5\xB0\x96\xE7\xAB\xAF\xE7\x9C\x8B\xE8\xB5\xB7\xE6\x9D\xA5\xE6\x9B\xB4\xE5\xB0\x96\xE4\xB8\x80\xE4\xBA\x9B";
const string ZH_SHARP_NORMAL = "\xE5\xB0\x96\xE7\xAB\xAF\xE5\xBD\xA2\xE7\x8A\xB6\xE6\x99\xAE\xE9\x80\x9A";
const string ZH_BLUNT_FULL = "\xE9\x92\x9D\xE7\xAB\xAF\xE6\x9B\xB4\xE9\xA5\xB1\xE6\xBB\xA1\xE5\x9C\x86\xE6\xB6\xA6";
const string ZH_BLUNT_NORMAL = "\xE9\x92\x9D\xE7\xAB\xAF\xE7\x9C\x8B\xE8\xB5\xB7\xE6\x9D\xA5\xE6\x99\xAE\xE9\x80\x9A";
const string ZH_SURFACE_SMOOTH = "\xE8\xA1\xA8\xE9\x9D\xA2\xE6\xAF\x94\xE8\xBE\x83\xE5\x85\x89\xE6\xBB\x91";
const string ZH_SURFACE_ROUGH = "\xE8\xA1\xA8\xE9\x9D\xA2\xE7\x95\xA5\xE6\x98\xBE\xE7\xB2\x97\xE7\xB3\x99";
const string ZH_SURFACE_LINE = "\xE4\xB8\x80\xE4\xBE\xA7\xE4\xBC\xBC\xE4\xB9\x8E\xE6\x9C\x89\xE7\xBB\x86\xE5\xB0\x8F\xE7\xBA\xB9\xE8\xB7\xAF";
const string ZH_SOUND_CRISP = "\xE8\xBD\xBB\xE6\x95\xB2\xE5\xA3\xB0\xE9\x9F\xB3\xE8\xBE\x83\xE6\xB8\x85\xE8\x84\x86";
const string ZH_SOUND_DULL = "\xE8\xBD\xBB\xE6\x95\xB2\xE5\xA3\xB0\xE9\x9F\xB3\xE6\x9C\x89\xE4\xBA\x9B\xE5\x8F\x91\xE9\x97\xB7";
const string ZH_YOU_CHOSE_LEFT = "\xE4\xBD\xA0\xE9\x80\x89\xE6\x8B\xA9\xE4\xBA\x86\xE7\xAC\xAC\x20";
const string ZH_NO_DOT_EGG = "\x20\xE5\x8F\xB7\xE9\xB8\xA1\xE8\x9B\x8B\xE3\x80\x82";
const string ZH_COMPUTER_DONE_CHOOSE = "\xE7\x94\xB5\xE8\x84\x91\xE5\xB7\xB2\xE5\xAE\x8C\xE6\x88\x90\xE9\x80\x89\xE8\x9B\x8B\xE3\x80\x82";
const string ZH_COMPUTER_CHOSE_LEFT = "\xE7\x94\xB5\xE8\x84\x91\xE9\x80\x89\xE6\x8B\xA9\xE4\xBA\x86\xE7\xAC\xAC\x20";
const string ZH_REVEAL = "\xE9\x80\x89\xE8\x9B\x8B\xE7\xBB\x93\xE6\x9E\x9C\xE6\x8F\xAD\xE7\xA4\xBA";
const string ZH_HIDDEN_ATTR = "\xE9\x9A\x90\xE8\x97\x8F\xE5\xB1\x9E\xE6\x80\xA7";
const string ZH_SA = "\xE5\xB0\x96\xE7\xAB\xAF\xE6\x94\xBB\xE5\x87\xBB";
const string ZH_SD = "\xE5\xB0\x96\xE7\xAB\xAF\xE9\x98\xB2\xE5\xBE\xA1";
const string ZH_SH = "\xE5\xB0\x96\xE7\xAB\xAF\xE8\x80\x90\xE4\xB9\x85";
const string ZH_BA = "\xE9\x92\x9D\xE7\xAB\xAF\xE6\x94\xBB\xE5\x87\xBB";
const string ZH_BD = "\xE9\x92\x9D\xE7\xAB\xAF\xE9\x98\xB2\xE5\xBE\xA1";
const string ZH_BH = "\xE9\x92\x9D\xE7\xAB\xAF\xE8\x80\x90\xE4\xB9\x85";
const string ZH_HIDDEN_NONE = "\xE6\x9A\x97\xE8\xA3\x82\xE4\xBD\x8D\xE7\xBD\xAE\xEF\xBC\x9A\xE6\x97\xA0";
const string ZH_HIDDEN_SHARP = "\xE6\x9A\x97\xE8\xA3\x82\xE4\xBD\x8D\xE7\xBD\xAE\xEF\xBC\x9A\xE5\xB0\x96\xE7\xAB\xAF";
const string ZH_HIDDEN_BLUNT = "\xE6\x9A\x97\xE8\xA3\x82\xE4\xBD\x8D\xE7\xBD\xAE\xEF\xBC\x9A\xE9\x92\x9D\xE7\xAB\xAF";

const string ZH_ATTACK_ROLE = "\xE8\xBF\x9B\xE6\x94\xBB";
const string ZH_DEFENSE_ROLE = "\xE9\x98\xB2\xE5\xAE\x88";

enum CrackState {
    INTACT,
    SLIGHT,
    VISIBLE,
    BROKEN
};

enum EndType {
    SHARP = 0,
    BLUNT = 1
};

class EggEnd {
public:
    string name;
    int attack;
    int defense;
    int hp;
    int maxHp;

    EggEnd() : name(""), attack(0), defense(0), hp(0), maxHp(0) {}

    EggEnd(const string& n, int atk, int def, int h)
        : name(n), attack(atk), defense(def), hp(h), maxHp(h) {}

    bool isBroken() const {
        return hp <= 0;
    }

    void takeDamage(int dmg) {
        if (dmg < 0) {
            return;
        }

        hp -= dmg;
        if (hp < 0) {
            hp = 0;
        }
    }

    CrackState getState() const {
        if (hp <= 0) {
            return BROKEN;
        }

        int percent = hp * 100 / maxHp;
        if (percent <= 30) {
            return VISIBLE;
        }
        if (percent <= 70) {
            return SLIGHT;
        }
        return INTACT;
    }

    string getStateStr() const {
        switch (getState()) {
        case INTACT:
            return ZH_INTACT;
        case SLIGHT:
            return ZH_SLIGHT;
        case VISIBLE:
            return ZH_VISIBLE;
        case BROKEN:
            return ZH_BROKEN;
        default:
            return ZH_UNKNOWN;
        }
    }
};

class Egg {
public:
    EggEnd sharp;
    EggEnd blunt;
    int hiddenCrackPos;
    string obs1;
    string obs2;
    string obs3;
    string obs4;
    string obs5;

    Egg()
        : sharp(ZH_SHARP, 3, 1, 4),
        blunt(ZH_BLUNT, 1, 3, 6),
        hiddenCrackPos(0),
        obs1(""),
        obs2(""),
        obs3(""),
        obs4(""),
        obs5("") {}

    EggEnd& getEnd(EndType type) {
        return (type == SHARP) ? sharp : blunt;
    }

    const EggEnd& getEnd(EndType type) const {
        return (type == SHARP) ? sharp : blunt;
    }

    bool isBroken() const {
        return sharp.isBroken() || blunt.isBroken();
    }

    int getScore() const {
        return sharp.attack + sharp.defense + sharp.hp +
            blunt.attack + blunt.defense + blunt.hp;
    }
};

class Player {
public:
    string name;
    Egg egg;

    Player() : name(ZH_PLAYER), egg() {}
    Player(const string& n) : name(n), egg() {}
};

struct BattleResult {
    string attackerName;
    string defenderName;
    string attackEndName;
    string defendEndName;
    int attackPower;
    int defensePower;
    int randomDelta;
    int finalValue;
    int attackerDamage;
    int defenderDamage;
    bool attackEndBroken;
    bool defendEndBroken;
};

class Game {
private:
    Player p1;
    Player p2;
    int round;
    vector<string> battleLog;
    int totalCollisions;
    int p1DamageDealt;
    int p2DamageDealt;
    int p1RecoilTaken;
    int p2RecoilTaken;

public:
    Game(const string& name1, const string& name2, const Egg& egg1, const Egg& egg2)
        : p1(name1.empty() ? ZH_PLAYER1 : name1),
        p2(name2.empty() ? ZH_PLAYER2 : name2),
        round(1),
        totalCollisions(0),
        p1DamageDealt(0),
        p2DamageDealt(0),
        p1RecoilTaken(0),
        p2RecoilTaken(0) {
        p1.egg = egg1;
        p2.egg = egg2;
    }

    const Player& getPlayer1() const { return p1; }
    const Player& getPlayer2() const { return p2; }
    int getRound() const { return round; }
    int getTotalCollisions() const { return totalCollisions; }
    int getP1DamageDealt() const { return p1DamageDealt; }
    int getP2DamageDealt() const { return p2DamageDealt; }
    int getP1RecoilTaken() const { return p1RecoilTaken; }
    int getP2RecoilTaken() const { return p2RecoilTaken; }

    void nextRound() {
        round++;
    }

    const vector<string>& getBattleLog() const {
        return battleLog;
    }

    BattleResult doBattle(int attackerIndex, EndType attackType, EndType defendType) {
        Player* attacker;
        Player* defender;

        if (attackerIndex == 1) {
            attacker = &p1;
            defender = &p2;
        }
        else {
            attacker = &p2;
            defender = &p1;
        }

        EggEnd& attackEnd = attacker->egg.getEnd(attackType);
        EggEnd& defendEnd = defender->egg.getEnd(defendType);

        BattleResult result;
        result.attackerName = attacker->name;
        result.defenderName = defender->name;
        result.attackEndName = attackEnd.name;
        result.defendEndName = defendEnd.name;
        result.attackPower = attackEnd.attack;
        result.defensePower = defendEnd.defense;
        result.randomDelta = (rand() % 3) - 1;
        result.finalValue = result.attackPower - result.defensePower + result.randomDelta;
        result.attackerDamage = 0;
        result.defenderDamage = 0;
        result.attackEndBroken = false;
        result.defendEndBroken = false;

        if (result.finalValue <= 0) {
            result.attackerDamage = 1;
            attackEnd.takeDamage(result.attackerDamage);

            if (attackerIndex == 1) {
                p1RecoilTaken += result.attackerDamage;
            }
            else {
                p2RecoilTaken += result.attackerDamage;
            }
        }
        else if (result.finalValue == 1) {
            result.defenderDamage = 1;
            defendEnd.takeDamage(result.defenderDamage);

            if (attackerIndex == 1) {
                p1DamageDealt += result.defenderDamage;
            }
            else {
                p2DamageDealt += result.defenderDamage;
            }
        }
        else {
            result.defenderDamage = 2;
            defendEnd.takeDamage(result.defenderDamage);

            if (attackerIndex == 1) {
                p1DamageDealt += result.defenderDamage;
            }
            else {
                p2DamageDealt += result.defenderDamage;
            }
        }

        totalCollisions++;

        result.attackEndBroken = attackEnd.isBroken();
        result.defendEndBroken = defendEnd.isBroken();

        ostringstream logLine;
        logLine << ZH_DI << round << ZH_ROUND_COLON
            << result.attackerName << ZH_USE_BRACKET << result.attackEndName
            << ZH_ATTACK_BRACKET_COMMA
            << result.defenderName << ZH_USE_BRACKET << result.defendEndName
            << ZH_DEFEND_BRACKET_COMMA;

        if (result.attackerDamage > 0) {
            logLine << ZH_ATTACKER_RECOIL << result.attackerDamage << ZH_POINT;
        }
        else {
            logLine << ZH_DEFENDER_DAMAGE << result.defenderDamage << ZH_POINT;
        }

        if (result.attackEndBroken) {
            logLine << ZH_ATTACK_END_BROKEN;
        }
        if (result.defendEndBroken) {
            logLine << ZH_DEFEND_END_BROKEN;
        }

        battleLog.push_back(logLine.str());
        return result;
    }

    int judgeWinner() const {
        bool p1Broken = p1.egg.isBroken();
        bool p2Broken = p2.egg.isBroken();

        if (p1Broken && p2Broken) {
            return 3;
        }
        if (p2Broken) {
            return 1;
        }
        if (p1Broken) {
            return 2;
        }
        return 0;
    }
};

int readZeroOrOne() {
    int choice;

    while (true) {
        cin >> choice;

        if (cin.fail()) {
            cin.clear();
            cin.ignore((numeric_limits<streamsize>::max)(), '\n');
            cout << ZH_INVALID_01;
            continue;
        }

        if (choice == 0 || choice == 1) {
            cin.ignore((numeric_limits<streamsize>::max)(), '\n');
            return choice;
        }

        cout << ZH_INVALID_01;
        cin.ignore((numeric_limits<streamsize>::max)(), '\n');
    }
}

int readMenuChoice() {
    int choice;

    while (true) {
        cin >> choice;

        if (cin.fail()) {
            cin.clear();
            cin.ignore((numeric_limits<streamsize>::max)(), '\n');
            cout << ZH_INVALID_129;
            continue;
        }

        if (choice == 1 || choice == 2 || choice == 9) {
            cin.ignore((numeric_limits<streamsize>::max)(), '\n');
            return choice;
        }

        cout << ZH_INVALID_129;
        cin.ignore((numeric_limits<streamsize>::max)(), '\n');
    }
}

int readOneToThree() {
    int choice;

    while (true) {
        cin >> choice;

        if (cin.fail()) {
            cin.clear();
            cin.ignore((numeric_limits<streamsize>::max)(), '\n');
            cout << ZH_INVALID_123;
            continue;
        }

        if (choice >= 1 && choice <= 3) {
            cin.ignore((numeric_limits<streamsize>::max)(), '\n');
            return choice;
        }

        cout << ZH_INVALID_123;
        cin.ignore((numeric_limits<streamsize>::max)(), '\n');
    }
}

string endTypeToString(EndType type) {
    if (type == SHARP) {
        return ZH_SHARP;
    }
    return ZH_BLUNT;
}

string hiddenCrackToString(int pos) {
    if (pos == 1) {
        return ZH_HIDDEN_SHARP;
    }
    if (pos == 2) {
        return ZH_HIDDEN_BLUNT;
    }
    return ZH_HIDDEN_NONE;
}

Egg createRandomEgg() {
    Egg egg;

    int bodyType = rand() % 3;
    int surfaceType = rand() % 3;
    int soundType = rand() % 2;
    int hiddenCrack = rand() % 5;

    egg.sharp.attack = 2 + (rand() % 3);
    egg.sharp.defense = 1 + (rand() % 2);
    egg.sharp.hp = 4 + (rand() % 2);

    egg.blunt.attack = 1 + (rand() % 2);
    egg.blunt.defense = 3 + (rand() % 2);
    egg.blunt.hp = 5 + (rand() % 2);

    if (bodyType == 0) {
        egg.sharp.attack += 1;
        egg.obs1 = ZH_BODY_SLENDER;
        egg.obs2 = ZH_SHARP_POINTY;
        egg.obs3 = ZH_BLUNT_NORMAL;
    }
    else if (bodyType == 1) {
        egg.blunt.defense += 1;
        egg.obs1 = ZH_BODY_ROUND;
        egg.obs2 = ZH_SHARP_NORMAL;
        egg.obs3 = ZH_BLUNT_FULL;
    }
    else {
        egg.obs1 = ZH_BODY_BALANCED;
        egg.obs2 = ZH_SHARP_NORMAL;
        egg.obs3 = ZH_BLUNT_NORMAL;
    }

    if (surfaceType == 0) {
        egg.sharp.hp += 1;
        egg.blunt.hp += 1;
        egg.obs4 = ZH_SURFACE_SMOOTH;
    }
    else if (surfaceType == 1) {
        egg.obs4 = ZH_SURFACE_ROUGH;
    }
    else {
        egg.obs4 = ZH_SURFACE_LINE;
    }

    if (soundType == 0) {
        egg.sharp.defense += 1;
        egg.blunt.hp += 1;
        egg.obs5 = ZH_SOUND_CRISP;
    }
    else {
        egg.obs5 = ZH_SOUND_DULL;
    }

    if (hiddenCrack == 0) {
        egg.hiddenCrackPos = 1;
        if (egg.sharp.hp > 1) {
            egg.sharp.hp -= 1;
        }
    }
    else if (hiddenCrack == 1) {
        egg.hiddenCrackPos = 2;
        if (egg.blunt.hp > 1) {
            egg.blunt.hp -= 1;
        }
    }
    else {
        egg.hiddenCrackPos = 0;
    }

    egg.sharp.maxHp = egg.sharp.hp;
    egg.blunt.maxHp = egg.blunt.hp;

    return egg;
}

vector<Egg> createCandidateEggs() {
    vector<Egg> eggs;
    eggs.push_back(createRandomEgg());
    eggs.push_back(createRandomEgg());
    eggs.push_back(createRandomEgg());
    return eggs;
}

int chooseEggAI(const vector<Egg>& eggs) {
    int bestIndex = 0;
    int bestScore = eggs[0].getScore();
    int i;

    for (i = 1; i < 3; ++i) {
        if (eggs[i].getScore() > bestScore) {
            bestScore = eggs[i].getScore();
            bestIndex = i;
        }
    }

    return bestIndex;
}

EndType chooseAttackAI(const Egg& egg) {
    bool sharpBroken = egg.sharp.isBroken();
    bool bluntBroken = egg.blunt.isBroken();

    if (sharpBroken) {
        return BLUNT;
    }
    if (bluntBroken) {
        return SHARP;
    }

    if (egg.sharp.getState() == VISIBLE && egg.blunt.getState() == INTACT) {
        if ((rand() % 100) < 70) {
            return BLUNT;
        }
    }

    if ((rand() % 100) < 75) {
        return SHARP;
    }

    return BLUNT;
}

EndType chooseDefenseAI(const Egg& egg) {
    bool sharpBroken = egg.sharp.isBroken();
    bool bluntBroken = egg.blunt.isBroken();

    if (sharpBroken) {
        return BLUNT;
    }
    if (bluntBroken) {
        return SHARP;
    }

    if (egg.blunt.getState() == INTACT) {
        return BLUNT;
    }

    if ((rand() % 100) < 30) {
        return SHARP;
    }

    return BLUNT;
}

void showMainMenu() {
    cout << "===============================\n";
    cout << "     " << ZH_TITLE << "\n";
    cout << "===============================\n";
    cout << ZH_PVP << "\n";
    cout << ZH_PVE << "\n";
    cout << ZH_EXIT_GAME << "\n";
    cout << ZH_ENTER_129;
}

bool askPlayAgain() {
    cout << ZH_ASK_REPLAY << "\n";
    cout << ZH_BACK_MENU << "\n";
    cout << ZH_PLAY_AGAIN << "\n";
    cout << ZH_ENTER_01;

    return readZeroOrOne() == 1;
}

void showStatus(const Game& game) {
    const Player& p1 = game.getPlayer1();
    const Player& p2 = game.getPlayer2();

    cout << "\n========== " << ZH_STATUS << " ==========\n";

    cout << p1.name << ZH_EGG_OF << "\n";
    cout << ZH_SHARP_STATE << p1.egg.sharp.getStateStr() << "\n";
    cout << ZH_BLUNT_STATE << p1.egg.blunt.getStateStr() << "\n";

    cout << p2.name << ZH_EGG_OF << "\n";
    cout << ZH_SHARP_STATE << p2.egg.sharp.getStateStr() << "\n";
    cout << ZH_BLUNT_STATE << p2.egg.blunt.getStateStr() << "\n";

    cout << "==============================\n";
}

void showCandidateEggs(const string& ownerName, const vector<Egg>& eggs) {
    int i;

    cout << "\n========== " << ZH_SELECTION_STAGE << " ==========\n";
    cout << ownerName << " - " << ZH_CANDIDATE_EGGS << "\n";
    cout << ZH_OBSERVE_3 << "\n";

    for (i = 0; i < 3; ++i) {
        cout << "\n" << ZH_CANDIDATE_EGGS << " " << (i + 1) << "\n";
        cout << ZH_OBSERVE_DESC << "\n";
        cout << "  1. " << eggs[i].obs1 << "\n";
        cout << "  2. " << eggs[i].obs2 << "\n";
        cout << "  3. " << eggs[i].obs3 << "\n";
        cout << "  4. " << eggs[i].obs4 << "\n";
        cout << "  5. " << eggs[i].obs5 << "\n";
    }

    cout << "\n==============================\n";
}

int chooseEggHuman(const string& playerName, const vector<Egg>& eggs) {
    showCandidateEggs(playerName, eggs);
    cout << ZH_ENTER_CHOOSE_123;
    return readOneToThree() - 1;
}

void showAIChoice(const string& aiName, const string& role, EndType type) {
    cout << aiName << ZH_AI_CHOOSE_LEFT << endTypeToString(type)
        << ZH_AI_CHOOSE_RIGHT << role << ZH_FULL_STOP << "\n";
}

void showBattleResult(const BattleResult& result) {
    cout << "\n--- " << ZH_BATTLE_RESULT << " ---\n";
    cout << result.attackerName << ZH_USE_LEFT << result.attackEndName
        << ZH_ATTACK_RIGHT
        << result.defenderName << ZH_USE_LEFT << result.defendEndName
        << ZH_DEFEND_RIGHT << "\n";

    cout << ZH_ATTACK_POWER << result.attackPower
        << ZH_DEFENSE_POWER << result.defensePower
        << ZH_RANDOM_VALUE << (result.randomDelta >= 0 ? "+" : "") << result.randomDelta << ")"
        << ZH_EQUAL << result.finalValue << "\n";

    if (result.attackerDamage > 0) {
        cout << ZH_DEFENSE_SUCCESS << result.attackerName
            << ZH_OF_LEFT << result.attackEndName << ZH_TAKES
            << result.attackerDamage << ZH_RECOIL_DAMAGE << "\n";
    }

    if (result.defenderDamage == 1) {
        cout << ZH_HIT << result.defenderName
            << ZH_OF_LEFT << result.defendEndName << ZH_TAKES
            << "1" << ZH_DAMAGE << "\n";
    }
    else if (result.defenderDamage == 2) {
        cout << ZH_HEAVY_HIT << result.defenderName
            << ZH_OF_LEFT << result.defendEndName << ZH_TAKES
            << "2" << ZH_DAMAGE << "\n";
    }

    if (result.defendEndBroken) {
        cout << result.defenderName << ZH_OF_LEFT << result.defendEndName
            << ZH_ALREADY_BROKEN << "\n";
    }

    if (result.attackEndBroken) {
        cout << result.attackerName << ZH_OF_LEFT << result.attackEndName
            << ZH_BROKE_FROM_RECOIL << "\n";
    }
}

void showBattleLog(const Game& game) {
    const vector<string>& logs = game.getBattleLog();
    size_t i;

    cout << "\n========== " << ZH_LOG << " ==========\n";

    if (logs.empty()) {
        cout << ZH_NO_LOG << "\n";
    }
    else {
        for (i = 0; i < logs.size(); ++i) {
            cout << i + 1 << ". " << logs[i] << "\n";
        }
    }

    cout << "==============================\n";
}

void showMatchSummary(const Game& game) {
    cout << "\n========== " << ZH_SUMMARY << " ==========\n";
    cout << ZH_TOTAL_ROUNDS << game.getRound() << "\n";
    cout << ZH_TOTAL_COLLISIONS << game.getTotalCollisions() << "\n";
    cout << game.getPlayer1().name << " - " << ZH_DAMAGE_DEALT << game.getP1DamageDealt() << "\n";
    cout << game.getPlayer1().name << " - " << ZH_RECOIL_TAKEN << game.getP1RecoilTaken() << "\n";
    cout << game.getPlayer2().name << " - " << ZH_DAMAGE_DEALT << game.getP2DamageDealt() << "\n";
    cout << game.getPlayer2().name << " - " << ZH_RECOIL_TAKEN << game.getP2RecoilTaken() << "\n";
    cout << "==============================\n";
}

void showEggReveal(const string& ownerName, const Egg& egg) {
    cout << "\n" << ownerName << ZH_EGG_OF << "\n";
    cout << ZH_HIDDEN_ATTR << ":\n";
    cout << "  " << ZH_SA << " = " << egg.sharp.attack << "\n";
    cout << "  " << ZH_SD << " = " << egg.sharp.defense << "\n";
    cout << "  " << ZH_SH << " = " << egg.sharp.maxHp << "\n";
    cout << "  " << ZH_BA << " = " << egg.blunt.attack << "\n";
    cout << "  " << ZH_BD << " = " << egg.blunt.defense << "\n";
    cout << "  " << ZH_BH << " = " << egg.blunt.maxHp << "\n";
    cout << "  " << hiddenCrackToString(egg.hiddenCrackPos) << "\n";
}

void showSelectionReveal(const string& name1, const Egg& egg1, const string& name2, const Egg& egg2) {
    cout << "\n========== " << ZH_REVEAL << " ==========\n";
    showEggReveal(name1, egg1);
    showEggReveal(name2, egg2);
    cout << "==============================\n";
}

bool printResultAndCheckEnd(const Game& game) {
    int result = game.judgeWinner();

    if (result == 0) {
        return false;
    }

    showStatus(game);

    if (result == 1) {
        cout << "\n" << game.getPlayer1().name << ZH_WIN << "\n";
    }
    else if (result == 2) {
        cout << "\n" << game.getPlayer2().name << ZH_WIN << "\n";
    }
    else {
        cout << "\n" << ZH_DRAW << "\n";
    }

    return true;
}

void playSingleMatch(int mode, const string& name1, const string& name2) {
    vector<Egg> eggs1 = createCandidateEggs();
    vector<Egg> eggs2 = createCandidateEggs();

    int selected1 = chooseEggHuman(name1, eggs1);
    cout << ZH_YOU_CHOSE_LEFT << (selected1 + 1) << ZH_NO_DOT_EGG << "\n";

    int selected2;

    if (mode == 2) {
        selected2 = chooseEggAI(eggs2);
        cout << ZH_COMPUTER_DONE_CHOOSE << "\n";
        cout << ZH_COMPUTER_CHOSE_LEFT << (selected2 + 1) << ZH_NO_DOT_EGG << "\n";
    }
    else {
        selected2 = chooseEggHuman(name2, eggs2);
        cout << name2 << ZH_YOU_CHOSE_LEFT << (selected2 + 1) << ZH_NO_DOT_EGG << "\n";
    }

    Game game(name1, name2, eggs1[selected1], eggs2[selected2]);
    bool vsAI = (mode == 2);

    while (true) {
        cout << "\n========== " << ZH_DI << " " << game.getRound() << " "
            << ZH_ROUND_ONLY << " ==========\n";

        showStatus(game);

        cout << "\n[" << ZH_PHASE1 << "] " << game.getPlayer1().name
            << ZH_ATTACK_COMMA << game.getPlayer2().name << ZH_DEFEND << "\n";

        cout << game.getPlayer1().name << ZH_CHOOSE_FOR << ZH_ATTACK_ROLE << ZH_END_POINT << "\n";
        cout << ZH_OPTION_SHARP << game.getPlayer1().egg.sharp.getStateStr() << "]\n";
        cout << ZH_OPTION_BLUNT << game.getPlayer1().egg.blunt.getStateStr() << "]\n";
        cout << ZH_ENTER_01;
        EndType atkChoice1 = (readZeroOrOne() == 0) ? SHARP : BLUNT;

        EndType defChoice1;
        if (vsAI) {
            defChoice1 = chooseDefenseAI(game.getPlayer2().egg);
            showAIChoice(game.getPlayer2().name, ZH_DEFENSE_ROLE, defChoice1);
        }
        else {
            cout << game.getPlayer2().name << ZH_CHOOSE_FOR << ZH_DEFENSE_ROLE << ZH_END_POINT << "\n";
            cout << ZH_OPTION_SHARP << game.getPlayer2().egg.sharp.getStateStr() << "]\n";
            cout << ZH_OPTION_BLUNT << game.getPlayer2().egg.blunt.getStateStr() << "]\n";
            cout << ZH_ENTER_01;
            defChoice1 = (readZeroOrOne() == 0) ? SHARP : BLUNT;
        }

        BattleResult result1 = game.doBattle(1, atkChoice1, defChoice1);
        showBattleResult(result1);

        if (printResultAndCheckEnd(game)) {
            showBattleLog(game);
            showMatchSummary(game);
            showSelectionReveal(name1, eggs1[selected1], name2, eggs2[selected2]);
            break;
        }

        cout << "\n[" << ZH_PHASE2 << "] " << game.getPlayer2().name
            << ZH_ATTACK_COMMA << game.getPlayer1().name << ZH_DEFEND << "\n";

        EndType atkChoice2;
        if (vsAI) {
            atkChoice2 = chooseAttackAI(game.getPlayer2().egg);
            showAIChoice(game.getPlayer2().name, ZH_ATTACK_ROLE, atkChoice2);
        }
        else {
            cout << game.getPlayer2().name << ZH_CHOOSE_FOR << ZH_ATTACK_ROLE << ZH_END_POINT << "\n";
            cout << ZH_OPTION_SHARP << game.getPlayer2().egg.sharp.getStateStr() << "]\n";
            cout << ZH_OPTION_BLUNT << game.getPlayer2().egg.blunt.getStateStr() << "]\n";
            cout << ZH_ENTER_01;
            atkChoice2 = (readZeroOrOne() == 0) ? SHARP : BLUNT;
        }

        cout << game.getPlayer1().name << ZH_CHOOSE_FOR << ZH_DEFENSE_ROLE << ZH_END_POINT << "\n";
        cout << ZH_OPTION_SHARP << game.getPlayer1().egg.sharp.getStateStr() << "]\n";
        cout << ZH_OPTION_BLUNT << game.getPlayer1().egg.blunt.getStateStr() << "]\n";
        cout << ZH_ENTER_01;
        EndType defChoice2 = (readZeroOrOne() == 0) ? SHARP : BLUNT;

        BattleResult result2 = game.doBattle(2, atkChoice2, defChoice2);
        showBattleResult(result2);

        if (printResultAndCheckEnd(game)) {
            showBattleLog(game);
            showMatchSummary(game);
            showSelectionReveal(name1, eggs1[selected1], name2, eggs2[selected2]);
            break;
        }

        game.nextRound();
    }
}

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
#endif

    srand(static_cast<unsigned int>(time(NULL)));

    while (true) {
        showMainMenu();
        int mode = readMenuChoice();

        if (mode == 9) {
            cout << ZH_GOODBYE << "\n";
            break;
        }

        string name1;
        string name2;

        cout << ZH_INPUT_NAME1;
        getline(cin, name1);

        if (mode == 1) {
            cout << ZH_INPUT_NAME2;
            getline(cin, name2);
        }
        else {
            name2 = ZH_COMPUTER;
        }

        bool playAgain = true;
        while (playAgain) {
            playSingleMatch(mode, name1, name2);
            playAgain = askPlayAgain();
        }
    }

    cout << ZH_PRESS_ENTER;
    cin.get();
    return 0;
}