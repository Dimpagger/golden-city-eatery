#include "Localization.h"
#include <set>
#include <cstring>

static Language currentLang = Language::CHINESE;

struct TranslationEntry {
    const char* key;
    const char* en;
    const char* zh;
    const char* ja;
};

// Sorted by key for binary search
static const TranslationEntry kTranslations[] = {
    // ── Menu ──────────────────────────────────────
    {"asm_upgraded",    "Assembly Upgraded!",      "组装台已升级!",       "組立台 強化!"},
    {"assembled",       "Assembled",               "已组装",             "組立済"},
    {"assembly",        "Assembly",                "组装台",             "組立台"},
    {"best",            "Best",                    "最高",               "ベスト"},
    {"best_score",      "Best Score:",             "最高分:",             "ベスト:"},
    {"chef_speed",      "Chef Speed",              "厨师速度",           "移動速度"},
    {"chef_upgraded",   "Chef Speed Upgraded!",    "厨师速度已升级!",     "移動速度 強化!"},
    {"coin_fmt",        "%dg",                     "%d 金币",            "%d コイン"},
    {"coins",           "Coins",                   "金币",               "コイン"},
    {"controls_hint",   "A/D: Move  |  Space: Interact  |  1-4: Upgrade  |  Tab: Recipes  |  P: Pause  |  L: Lang",
                        "A/D: 移动  |  空格: 交互  |  1-4: 升级  |  Tab: 食谱  |  P: 暂停  |  L: 语言",
                        "A/D: 移動  |  SPACE: 作業  |  1-4: 強化  |  Tab: レシピ  |  P: ポーズ  |  L: 言語"},
    {"cooked",          "Cooked",                  "已烤",               "焼上済"},
    {"ctrl_interact",   "Space  Interact with station","空格  与工位交互", "スペース  作業台を使う"},
    {"ctrl_move",       "A/D    Move left / right","A/D    左右移动",     "A/D    左右に移動"},
    {"ctrl_pause",      "P      Pause",            "P      暂停",         "P      ポーズ"},
    {"ctrl_upgrade",    "1/2/3  Buy upgrades",     "1/2/3  购买升级",     "1/2/3  強化を購入"},
    {"cust_impatient",  "! Impatient  1.5x",       "! 急躁  1.5x",       "! せっかち  1.5x"},
    {"cust_normal",     ":) Normal  1.0x",         ":) 普通  1.0x",      ":) 普通  1.0x"},
    {"cust_vip",        "$ VIP  2.5x",             "$ VIP  2.5x",        "$ VIP  2.5x"},
    {"customers_lost",  "Customers Lost:",         "流失顾客:",           "ロスト数:"},
    {"customers_title", "Customer Types:",         "顾客类型:",           "お客様タイプ:"},
    {"cutting",         "Cutting",                 "砧板",               "まな板"},
    {"cutting_upgraded","Cutting Upgraded!",       "砧板已升级!",         "まな板 強化!"},
    {"day_complete",    "Complete!",               "完成!",              "クリア!"},
    {"day_info",        "D%d %d/%d",               "第%d天  %d/%d",      "%d日目  %d/%d"},
    {"day_label",       "Day %d",                  "第%d天",             "%d日目"},
    {"day_target",      "Target:",                 "目标:",              "目標:"},
    {"done",            "DONE",                    "完成",               "完了"},
    {"game_over",       "GAME OVER",               "游戏结束",           "ゲームオーバー"},
    {"grill",           "Grill",                   "烤肉架",             "焼き台"},
    {"grill_upgraded",  "Grill Upgraded!",         "烤肉架已升级!",       "焼き台 強化!"},
    {"help_close",      "Press H to close",         "按 H 关闭",          "Hで閉じる"},
    {"help_customers",  "Customer Types",           "顾客类型",           "お客様タイプ"},
    {"help_flow",       "Basic Flow",               "基本流程",           "基本の流れ"},
    {"help_flow1",      "1. Take Raw meat to the right station", "1. 把生肉送到正确的工位", "1. 生肉を正しい作業台へ"},
    {"help_flow2",      "2. Match the recipe each customer wants (RM/LP/KB)", "2. 匹配顾客想要的食谱 (RM/LP/KB)", "2. お客様の注文に合わせる (RM/LP/KB)"},
    {"help_flow3",      "3. Serve at the rightmost counter", "3. 在最右侧出餐口完成服务", "3. 右端の提供口で提供"},
    {"help_recipes",    "Recipes",                  "食谱",               "レシピ"},
    {"help_tip1",       "5-in-a-row streak = bonus coins & milestone!", "连续5次服务 = 额外金币 + 里程碑!", "5連続提供 = ボーナスコイン!"},
    {"help_tip2",       "Lose 3 customers = Game Over", "流失3个顾客 = 游戏结束", "3人ロスト = ゲームオーバー"},
    {"help_tip3",       "Match recipe = full reward  |  Wrong = half coins", "食谱匹配 = 全额奖励  |  错误 = 半额", "注文一致 = 全额  |  不一致 = 半额"},
    {"help_tips",       "Tips",                     "提示",               "ヒント"},
    {"help_title",      "How to Play",              "游戏指南",           "遊び方"},
    {"how_to_play",     "How to Play:",            "操作说明:",           "遊び方:"},
    {"interact_hint",   "[Space]",                 "[空格]",             "[スペース]"},
    {"kebab",           "Kebab",                   "烤串",               "串焼き"},
    {"liangpi",         "Liangpi",                 "凉皮",               "涼皮"},
    {"lost",            "Lost",                    "流失",               "ロスト"},
    {"max_lvl",         "MAX",                     "满级",               "最大"},
    {"n_in_a_row",      "%d IN A ROW!",            "连续服务 %d 个!",     "%d連続!"},
    {"new_recipe",      "New Recipe",              "新食谱解锁",         "新レシピ"},
    {"new_record",      "NEW RECORD!",             "新纪录!",             "新記録!"},
    {"paused",          "PAUSED",                  "暂停",               "ポーズ中"},
    {"press_again",     "Press SPACE to Play Again","按空格键重新开始",   "スペースでリトライ"},
    {"press_continue",  "Press SPACE to Continue", "按空格键继续",       "スペースで次へ"},
    {"press_menu",      "Press SPACE for Menu",    "按空格键返回菜单",    "スペースでメニュー"},
    {"press_resume",    "Press P to Resume",       "按 P 键继续",        "Pで再開"},
    {"press_start",     "Press SPACE to Start",    "按空格键开始",        "スペースでスタート"},
    {"queue",           "Q",                       "队列",               "待ち"},
    {"raw",             "Raw",                     "生肉",               "生肉"},
    {"ready",           "READY",                   "就绪",               "準備可"},
    {"recipes_title",   "Recipes",                 "食谱",               "レシピ"},
    {"roujiamo",        "Roujiamo",                "肉夹馍",             "肉夾馍"},
    {"score",           "Score",                   "分数",               "スコア"},
    {"served",          "Customers Served:",       "已服务顾客:",         "提供数:"},
    {"serving",         "Serving",                 "出餐口",             "提供口"},
    {"sliced",          "Sliced",                  "已切",               "切済"},
    {"streak_hud",      "Streak",                  "连击",               "連続"},
    {"subtitle",        "A 2D cooking sim",        "一款 2D 模拟经营游戏", "2Dクッキングゲーム"},
    {"title",           "Roujiamo Legend",         "肉夹馍传奇",         "肉夾馍伝説"},
    {"total_coins",     "Total Coins:",            "总金币:",             "総コイン:"},
    {"upgrades_title",  "Upgrades",                "升级",               "強化"},
    {"victory",         "CONGRATULATIONS!",        "恭喜通关!",          "おめでとう!"},
};

void Loc::SetLanguage(Language lang) {
    currentLang = lang;
}

Language Loc::GetLanguage() {
    return currentLang;
}

void Loc::ToggleLanguage() {
    switch (currentLang) {
        case Language::CHINESE:  currentLang = Language::JAPANESE; break;
        case Language::JAPANESE: currentLang = Language::ENGLISH;  break;
        case Language::ENGLISH:  currentLang = Language::CHINESE;  break;
    }
}

const char* Loc::T(const char* key) {
    // Direct binary search on the sorted kTranslations array — no heap allocations
    const int n = sizeof(kTranslations) / sizeof(kTranslations[0]);
    int lo = 0, hi = n - 1;
    while (lo <= hi) {
        int mid = (lo + hi) / 2;
        int cmp = std::strcmp(kTranslations[mid].key, key);
        if (cmp == 0) {
            switch (currentLang) {
                case Language::CHINESE:  return kTranslations[mid].zh;
                case Language::JAPANESE: return kTranslations[mid].ja;
                default:                 return kTranslations[mid].en;
            }
        }
        if (cmp < 0) lo = mid + 1;
        else hi = mid - 1;
    }
    return key;
}

// Simple UTF-8 decoder, avoids raylib dependency
static int DecodeUtf8(const char* s, int* cp) {
    unsigned char c = (unsigned char)s[0];
    if (c < 0x80) { *cp = c; return 1; }
    if (c < 0xE0) { *cp = ((c & 0x1F) << 6) | ((unsigned char)s[1] & 0x3F); return 2; }
    if (c < 0xF0) {
        *cp = ((c & 0x0F) << 12) | (((unsigned char)s[1] & 0x3F) << 6) | ((unsigned char)s[2] & 0x3F);
        return 3;
    }
    *cp = ((c & 0x07) << 18) | (((unsigned char)s[1] & 0x3F) << 12)
        | (((unsigned char)s[2] & 0x3F) << 6) | ((unsigned char)s[3] & 0x3F);
    return 4;
}

std::vector<int> Loc::GetRequiredCodepoints() {
    std::set<int> unique;

    // Always include ASCII
    for (int i = 32; i <= 126; i++) unique.insert(i);

    // Scan all translation strings for codepoints
    for (auto& e : kTranslations) {
        for (const char* s : {e.en, e.zh, e.ja}) {
            for (int i = 0; s[i]; ) {
                int cp;
                int len = DecodeUtf8(s + i, &cp);
                unique.insert(cp);
                i += len;
            }
        }
    }

    return std::vector<int>(unique.begin(), unique.end());
}
