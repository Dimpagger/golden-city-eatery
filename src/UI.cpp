#include "UI.h"
#include "Constants.h"
#include "Localization.h"
#include "Food.h"

// ── Visual constants ──────────────────────────────────
static constexpr Color COLOR_PANEL_BG  = {240, 240, 240, 220};
static constexpr Color COLOR_PANEL_BD  = {180, 180, 180, 255};
static constexpr Color COLOR_BAR_BG    = {200, 200, 200, 255};
static constexpr Color COLOR_BAR_FG    = ORANGE;
static constexpr Color COLOR_GOLD      = {218, 165, 32, 255};

static Color RecipeColor(RecipeType type) {
    switch (type) {
        case RecipeType::ROUJIAMO: return {180, 120, 60, 255};
        case RecipeType::LIANGPI:  return {220, 220, 220, 255};
        case RecipeType::KEBAB:    return {200, 60, 40, 255};
        default:                   return DARKGRAY;
    }
}

Font UI::s_font = {};

void UI::SetFont(Font font) {
    s_font = font;
}

// ── Helpers ───────────────────────────────────────────

int UI::TextWidth(const char* text, int size) {
    return (int)MeasureTextEx(s_font, text, (float)size, 1.0f).x;
}

void UI::DrawPanel(float cx, float cy, float cw, float ch, Color bg) {
    float x = cx - UI_PANEL_PAD;
    float y = cy - UI_PANEL_PAD;
    float w = cw + 2 * UI_PANEL_PAD;
    float h = ch + 2 * UI_PANEL_PAD;
    DrawRectangle((int)x, (int)y, (int)w, (int)h, bg);
    DrawRectangleLines((int)x, (int)y, (int)w, (int)h, COLOR_PANEL_BD);
}

const char* UI::FoodStateLabel(FoodState state) {
    switch (state) {
        case FoodState::RAW:       return Loc::T("raw");
        case FoodState::COOKED:    return Loc::T("cooked");
        case FoodState::SLICED:    return Loc::T("sliced");
        case FoodState::ASSEMBLED: return Loc::T("assembled");
    }
    return "?";
}

const char* UI::CustomerFace(const Customer& c) {
    if (c.WasServed()) return ":D";
    if (!c.IsWaiting()) return "";
    float p = c.GetPatience();
    if (p > 20.0f) return ":)";
    if (p > 10.0f) return ":|";
    return ":(";
}

// ── Screens ───────────────────────────────────────────

void UI::DrawMenuScreen() {
    DrawPanel(UI_MENU_PANEL_X, UI_MENU_PANEL_Y, UI_MENU_PANEL_W, UI_MENU_PANEL_H, COLOR_PANEL_BG);

    DrawTextEx(s_font, Loc::T("title"),    {235, 170}, UI_FONT_TITLE, 1.0f, DARKGRAY);
    DrawTextEx(s_font, Loc::T("subtitle"), {300, 220}, UI_FONT_SMALL, 1.0f, GRAY);

    DrawTextEx(s_font, Loc::T("how_to_play"),  {250, 270}, UI_FONT_BODY, 1.0f, DARKGRAY);
    DrawTextEx(s_font, Loc::T("ctrl_move"),    {260, 300}, UI_FONT_SMALL, 1.0f, DARKGRAY);
    DrawTextEx(s_font, Loc::T("ctrl_interact"),{260, 322}, UI_FONT_SMALL, 1.0f, DARKGRAY);
    DrawTextEx(s_font, Loc::T("ctrl_upgrade"), {260, 344}, UI_FONT_SMALL, 1.0f, DARKGRAY);
    DrawTextEx(s_font, Loc::T("ctrl_pause"),   {260, 366}, UI_FONT_SMALL, 1.0f, DARKGRAY);

    DrawTextEx(s_font, Loc::T("press_start"), {270, 415}, UI_FONT_BODY, 1.0f, DARKGREEN);
}

void UI::DrawGameOverScreen(int score, int coins, int lost, int bestScore, int day,
                              int rjm, int lp, int kb) {
    DrawPanel(UI_GAMEOVER_PANEL_X, UI_GAMEOVER_PANEL_Y,
              UI_GAMEOVER_PANEL_W, UI_GAMEOVER_PANEL_H, COLOR_PANEL_BG);

    DrawTextEx(s_font, Loc::T("game_over"), {280, 130}, UI_FONT_TITLE, 1.0f, RED);

    DrawTextEx(s_font, TextFormat(Loc::T("day_label"), day),
               {340, 175}, UI_FONT_BODY, 1.0f, DARKGRAY);

    DrawTextEx(s_font, TextFormat("%s %d", Loc::T("served"), score),
               {250, 210}, UI_FONT_BODY, 1.0f, DARKGREEN);
    DrawTextEx(s_font, TextFormat("%s %d", Loc::T("customers_lost"), lost),
               {250, 235}, UI_FONT_BODY, 1.0f, RED);
    DrawTextEx(s_font, TextFormat("%s %d", Loc::T("total_coins"), coins),
               {250, 260}, UI_FONT_BODY, 1.0f, COLOR_GOLD);

    Color bestColor = (score > 0 && score >= bestScore) ? COLOR_GOLD : DARKGRAY;
    DrawTextEx(s_font, TextFormat("%s %d", Loc::T("best_score"), bestScore),
               {250, 290}, UI_FONT_BODY, 1.0f, bestColor);

    if (score > 0 && score >= bestScore) {
        DrawTextEx(s_font, Loc::T("new_record"), {310, 315}, UI_FONT_BODY, 1.0f, ORANGE);
    }

    // Stats
    DrawTextEx(s_font, TextFormat("RM:%d  LP:%d  KB:%d", rjm, lp, kb),
               {250, 355}, UI_FONT_SMALL, 1.0f, DARKGRAY);

    DrawTextEx(s_font, Loc::T("press_again"), {240, 395}, UI_FONT_BODY, 1.0f, DARKGREEN);
}

void UI::DrawDayCompleteScreen(int day, int score, int coins) {
    DrawPanel(UI_DAY_PANEL_X, UI_DAY_PANEL_Y, UI_DAY_PANEL_W, UI_DAY_PANEL_H, COLOR_PANEL_BG);

    DrawTextEx(s_font, TextFormat("Day %d %s", day, Loc::T("day_complete")),
               {240, 190}, UI_FONT_TITLE, 1.0f, DARKGREEN);

    DrawTextEx(s_font, TextFormat("%s %d/%d", Loc::T("day_target"), score, DAY_TARGETS[day - 1]),
               {250, 250}, UI_FONT_BODY, 1.0f, DARKGRAY);
    DrawTextEx(s_font, TextFormat("%s %d", Loc::T("total_coins"), coins),
               {250, 280}, UI_FONT_BODY, 1.0f, COLOR_GOLD);

    DrawTextEx(s_font, Loc::T("press_continue"), {250, 350}, UI_FONT_BODY, 1.0f, DARKGREEN);
}

void UI::DrawVictoryScreen(int score, int coins, int bestScore,
                             int rjm, int lp, int kb) {
    DrawPanel(UI_VICTORY_PANEL_X, UI_VICTORY_PANEL_Y,
              UI_VICTORY_PANEL_W, UI_VICTORY_PANEL_H, COLOR_PANEL_BG);

    DrawTextEx(s_font, Loc::T("victory"), {260, 160}, UI_FONT_TITLE, 1.0f, ORANGE);

    DrawTextEx(s_font, TextFormat("%s %d", Loc::T("served"), score),
               {250, 215}, UI_FONT_BODY, 1.0f, DARKGREEN);
    DrawTextEx(s_font, TextFormat("%s %d", Loc::T("total_coins"), coins),
               {250, 240}, UI_FONT_BODY, 1.0f, COLOR_GOLD);

    Color bestColor = (score > 0 && score >= bestScore) ? COLOR_GOLD : DARKGRAY;
    DrawTextEx(s_font, TextFormat("%s %d", Loc::T("best_score"), bestScore),
               {250, 270}, UI_FONT_BODY, 1.0f, bestColor);
    if (score > 0 && score >= bestScore) {
        DrawTextEx(s_font, Loc::T("new_record"), {370, 270}, UI_FONT_BODY, 1.0f, ORANGE);
    }

    // Stats
    DrawTextEx(s_font, TextFormat("RM:%d  LP:%d  KB:%d", rjm, lp, kb),
               {250, 305}, UI_FONT_SMALL, 1.0f, DARKGRAY);

    DrawTextEx(s_font, Loc::T("press_continue"),
               {220, 390}, UI_FONT_BODY, 1.0f, DARKGREEN);
    DrawTextEx(s_font, Loc::T("press_menu"),
               {(float)(SCREEN_WIDTH/2 - TextWidth(Loc::T("press_menu"), UI_FONT_SMALL)/2),
                415}, UI_FONT_SMALL, 1.0f, GRAY);
}

void UI::DrawPauseOverlay() {
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, {0, 0, 0, 128});

    DrawPanel(276, 236, 248, 108, COLOR_PANEL_BG);
    DrawTextEx(s_font, Loc::T("paused"), {320, 260}, UI_FONT_TITLE, 1.0f, DARKGRAY);
    DrawTextEx(s_font, Loc::T("press_resume"), {295, 310}, UI_FONT_SMALL, 1.0f, DARKGRAY);
}

// ── HUD ───────────────────────────────────────────────

void UI::DrawTopBar(int coins, int score, int queueCount,
                    int lostCount, int maxLost, int bestScore, int streak,
                    int currentDay, int servedToday) {
    DrawPanel(10, 10, SCREEN_WIDTH - 20, 24, COLOR_PANEL_BG);

    int x = 14;
    DrawTextEx(s_font, TextFormat(Loc::T("day_info"), currentDay, servedToday, DAY_TARGETS[currentDay - 1]),
               {(float)x, 10}, UI_FONT_BODY, 1.0f, DARKGREEN);
    x += 130;
    DrawTextEx(s_font, TextFormat("%s %d", Loc::T("coins"), coins),
               {(float)x, 10}, UI_FONT_BODY, 1.0f, COLOR_GOLD);
    x += 90;
    DrawTextEx(s_font, TextFormat("%s %d", Loc::T("best"), bestScore),
               {(float)x, 10}, UI_FONT_SMALL, 1.0f, DARKGRAY);
    x += 80;
    Color lostColor = (lostCount >= maxLost - 1) ? RED : DARKGRAY;
    DrawTextEx(s_font, TextFormat("%s %d/%d", Loc::T("lost"), lostCount, maxLost),
               {(float)x, 10}, UI_FONT_SMALL, 1.0f, lostColor);

    if (streak >= MILESTONE_INTERVAL) {
        Color streakColor = (streak >= 15) ? ORANGE : DARKGREEN;
        DrawTextEx(s_font, TextFormat("%s x%d", Loc::T("streak_hud"), streak),
                   {(float)(SCREEN_WIDTH - 120), 10}, UI_FONT_SMALL, 1.0f, streakColor);
    }
}

// ── Game Entities ─────────────────────────────────────

void UI::DrawStation(const Station& s, const char* label,
                     Color color, float workTime) {
    int sx = (int)s.GetX();
    int sy = (int)s.GetY();
    int sw = (int)s.GetWidth();
    int sh = (int)s.GetHeight();

    DrawRectangle(sx, sy, sw, sh, color);
    DrawRectangleLines(sx, sy, sw, sh, DARKGRAY);

    int labelW = TextWidth(label, UI_FONT_SMALL);
    DrawTextEx(s_font, label, {(float)(sx + (sw - labelW) / 2), (float)(sy + 6)},
               UI_FONT_SMALL, 1.0f, WHITE);

    // Food indicator — colored dot when station has food
    if (s.HasFood()) {
        const Food* f = s.GetFood();
        Color foodCol = DARKGRAY;
        if (f) {
            switch (f->GetState()) {
                case FoodState::RAW:       foodCol = {220, 80, 80, 255}; break;
                case FoodState::COOKED:    foodCol = {160, 100, 40, 255}; break;
                case FoodState::SLICED:    foodCol = {180, 160, 100, 255}; break;
                case FoodState::ASSEMBLED: foodCol = {100, 180, 80, 255}; break;
            }
        }
        DrawCircle(sx + sw / 2, sy + sh / 2 + 4, 6, foodCol);
        DrawCircleLines(sx + sw / 2, sy + sh / 2 + 4, 6, DARKGRAY);
    }

    DrawTextEx(s_font, TextFormat("%.1fs", workTime),
               {(float)(sx + 6), (float)(sy + sh - 18)}, UI_FONT_TINY, 1.0f, BLACK);

    if (s.IsDone() && s.HasFood()) {
        DrawTextEx(s_font, Loc::T("done"),
                   {(float)(sx + sw - 40), (float)(sy + sh - 18)}, UI_FONT_TINY, 1.0f, DARKGREEN);
    }

    if (s.GetState() == StationState::WORKING) {
        int barY = sy + sh + 4;
        int barW = (int)(sw * s.GetProgress() / 100.0f);
        DrawRectangle(sx, barY, sw, 6, COLOR_BAR_BG);
        DrawRectangle(sx, barY, barW, 6, COLOR_BAR_FG);
    }
}

void UI::DrawCustomer(const Customer& c) {
    int cx = (int)c.GetX() - 14;
    int cy = (int)c.GetY() - 20;

    Color bodyColor;
    if (c.GetState() == Customer::State::LEAVING) {
        bodyColor = c.WasServed() ? DARKGREEN : RED;
    } else {
        switch (c.GetType()) {
            case CustomerType::NORMAL:   bodyColor = GREEN;  break;
            case CustomerType::IMPATIENT: bodyColor = YELLOW; break;
            case CustomerType::VIP:      bodyColor = {218, 165, 32, 255}; break;
        }
        if (c.GetPatience() < 10.0f && c.GetType() != CustomerType::VIP) {
            bodyColor = ORANGE;
        }
    }
    DrawRectangle(cx, cy, 28, 40, bodyColor);
    DrawRectangleLines(cx, cy, 28, 40, DARKGRAY);

    if (c.IsWaiting()) {
        // Type indicator
        const char* typeLabel = "";
        switch (c.GetType()) {
            case CustomerType::IMPATIENT: typeLabel = "!"; break;
            case CustomerType::VIP:      typeLabel = "$"; break;
            default: break;
        }
        if (typeLabel[0]) {
            DrawTextEx(s_font, typeLabel,
                       {(float)(cx + 10), (float)(cy - 18)}, UI_FONT_SMALL, 1.0f, BLACK);
        }
        // Desired recipe
        const char* recipeLabel = "";
        switch (c.GetDesiredRecipe()) {
            case RecipeType::ROUJIAMO: recipeLabel = "RM"; break;
            case RecipeType::LIANGPI:  recipeLabel = "LP"; break;
            case RecipeType::KEBAB:    recipeLabel = "KB"; break;
            default: break;
        }
        DrawTextEx(s_font, recipeLabel,
                   {(float)(cx + 2), (float)(cy - 16)}, UI_FONT_TINY, 1.0f, DARKGRAY);
    }

    const char* face = CustomerFace(c);
    if (face[0]) {
        DrawTextEx(s_font, face, {(float)(cx + 2), (float)(cy + 2)}, UI_FONT_TINY, 1.0f, BLACK);
    }

    if (c.IsWaiting()) {
        float pct = c.GetPatience() / CUSTOMER_PATIENCE;
        DrawRectangle(cx - 2, cy + 44, 32, 4, COLOR_BAR_BG);
        DrawRectangle(cx - 2, cy + 44, (int)(32 * pct), 4,
                      pct > 0.3f ? GREEN : RED);
    }
}

void UI::DrawChef(const Chef& chef) {
    int cx = (int)chef.GetX(), cy = (int)chef.GetY();
    int cw = (int)chef.GetWidth(), ch = (int)chef.GetHeight();
    DrawRectangle(cx, cy, cw, ch, BLUE);
    // Facing direction indicator
    int eyeX = chef.IsFacingRight() ? cx + cw - 8 : cx + 8;
    DrawRectangle(eyeX, cy + 6, 4, 4, WHITE);
    DrawRectangle(eyeX, cy + 16, 4, 4, WHITE);

    if (chef.IsHoldingFood()) {
        const char* stateStr = FoodStateLabel(chef.GetHeldFood()->GetState());
        const char* recipeStr = chef.GetHeldFood()->GetRecipeName();
        Color rc = RecipeColor(chef.GetHeldFood()->GetRecipe());
        if (recipeStr[0]) {
            DrawTextEx(s_font, TextFormat("%s | %s", stateStr, recipeStr),
                       {(float)(int)chef.GetX(), (float)(int)chef.GetY() - 18},
                       UI_FONT_SMALL, 1.0f, rc);
        } else {
            DrawTextEx(s_font, stateStr,
                       {(float)(int)chef.GetX(), (float)(int)chef.GetY() - 18},
                       UI_FONT_SMALL, 1.0f, DARKGRAY);
        }
    }
}

void UI::DrawInteractionHint(const Station& s) {
    DrawTextEx(s_font, Loc::T("interact_hint"),
               {(float)((int)s.GetX() + (int)s.GetWidth() / 2 - 20),
                (float)((int)s.GetY() - 20)}, UI_FONT_SMALL, 1.0f, BLACK);
}

void UI::DrawCountdown(const Station& s) {
    if (s.GetState() != StationState::WORKING) return;
    float remaining = s.GetWorkTime() * (1.0f - s.GetProgress() / 100.0f);
    const char* text = TextFormat("%.1fs", remaining);
    int barY = (int)s.GetY() + (int)s.GetHeight() + 4;
    int textW = TextWidth(text, UI_FONT_SMALL);
    DrawTextEx(s_font, text,
               {(float)((int)s.GetX() + ((int)s.GetWidth() - textW) / 2),
                (float)(barY - 3)}, UI_FONT_SMALL, 1.0f, DARKGRAY);
}

// ── Panels ────────────────────────────────────────────

void UI::DrawUpgradeRow(int ux, int uy, int uw, const char* detailText,
                        const UpgradeSystem& upg, UpgradeType type, int coins) {
    const int costPad = 4;
    DrawTextEx(s_font, detailText, {(float)ux, (float)uy}, UI_FONT_SMALL, 1.0f, DARKGRAY);
    if (!upg.IsMaxed(type)) {
        const char* costText = TextFormat(Loc::T("coin_fmt"), upg.GetCost(type));
        Color costColor = coins >= upg.GetCost(type) ? DARKGREEN : RED;
        DrawTextEx(s_font, costText,
                   {(float)(ux + uw - TextWidth(costText, UI_FONT_SMALL) - costPad - UI_PANEL_PAD),
                    (float)uy}, UI_FONT_SMALL, 1.0f, costColor);
    } else {
        int w = TextWidth(Loc::T("max_lvl"), UI_FONT_SMALL);
        DrawTextEx(s_font, Loc::T("max_lvl"),
                   {(float)(ux + uw - w - costPad - UI_PANEL_PAD), (float)uy},
                   UI_FONT_SMALL, 1.0f, GRAY);
    }
}

void UI::DrawUpgradePanel(const UpgradeSystem& upg, int coins) {
    DrawPanel(UI_UPGRADE_PANEL_X, UI_UPGRADE_PANEL_Y,
              UI_UPGRADE_PANEL_W, UI_UPGRADE_PANEL_H, COLOR_PANEL_BG);

    int ux = (int)UI_UPGRADE_PANEL_X;
    int uy = (int)UI_UPGRADE_PANEL_Y;
    int uw = (int)UI_UPGRADE_PANEL_W;

    DrawTextEx(s_font, TextFormat("%s (1-4)", Loc::T("upgrades_title")),
               {(float)ux, (float)uy}, UI_FONT_TINY, 1.0f, DARKGRAY);
    uy += 14;

    // [1] Grill speed
    {
        float gt = BASE_GRILL_TIME * upg.GetSpeedMultiplier(UpgradeType::GRILL_SPEED);
        DrawUpgradeRow(ux, uy, uw,
            TextFormat("[1] %s  Lv.%d  %.1fs", Loc::T("grill"), upg.GetLevel(UpgradeType::GRILL_SPEED), gt),
            upg, UpgradeType::GRILL_SPEED, coins);
    }
    uy += 16;

    // [2] Cutting speed
    {
        float ct = BASE_CUTTING_TIME * upg.GetSpeedMultiplier(UpgradeType::CUTTING_SPEED);
        DrawUpgradeRow(ux, uy, uw,
            TextFormat("[2] %s  Lv.%d  %.1fs", Loc::T("cutting"), upg.GetLevel(UpgradeType::CUTTING_SPEED), ct),
            upg, UpgradeType::CUTTING_SPEED, coins);
    }
    uy += 16;

    // [3] Assembly speed
    {
        float at = BASE_ASSEMBLY_TIME * upg.GetSpeedMultiplier(UpgradeType::OVEN_SPEED);
        DrawUpgradeRow(ux, uy, uw,
            TextFormat("[3] %s  Lv.%d  %.1fs", Loc::T("assembly"), upg.GetLevel(UpgradeType::OVEN_SPEED), at),
            upg, UpgradeType::OVEN_SPEED, coins);
    }
    uy += 16;

    // [4] Chef speed
    {
        float cs = BASE_CHEF_SPEED * upg.GetSpeedMultiplier(UpgradeType::CHEF_SPEED);
        DrawUpgradeRow(ux, uy, uw,
            TextFormat("[4] %s  Lv.%d  %.0fpx/s", Loc::T("chef_speed"), upg.GetLevel(UpgradeType::CHEF_SPEED), cs),
            upg, UpgradeType::CHEF_SPEED, coins);
    }
}

void UI::DrawDiscoveryMessage(RecipeType recipe, float timer) {
    const char* name = "";
    switch (recipe) {
        case RecipeType::LIANGPI: name = Loc::T("liangpi"); break;
        case RecipeType::KEBAB:   name = Loc::T("kebab");   break;
        default: break;
    }
    const char* text = TextFormat("%s: %s!", Loc::T("new_recipe"), name);
    int w = TextWidth(text, UI_FONT_HEAD);
    float alpha = timer / 2.0f;
    Color c = ORANGE;
    c.a = (unsigned char)(255 * alpha);
    DrawTextEx(s_font, text,
               {(float)((SCREEN_WIDTH - w) / 2), 130}, UI_FONT_HEAD, 1.0f, c);
}

void UI::DrawFeedbackMessage(const char* text) {
    int w = TextWidth(text, UI_FONT_HEAD);
    DrawTextEx(s_font, text,
               {(float)((SCREEN_WIDTH - w) / 2), 90}, UI_FONT_HEAD, 1.0f, ORANGE);
}

void UI::DrawRecipePanel() {
    DrawPanel(10, 380, 310, 210, COLOR_PANEL_BG);
    int rx = 16, ry = 386;
    DrawTextEx(s_font, Loc::T("recipes_title"), {(float)rx, (float)ry}, UI_FONT_BODY, 1.0f, DARKGRAY);
    ry += 24;

    // Recipe paths with coin values
    DrawTextEx(s_font, TextFormat("%s %dg: Raw -> Grill -> Cutting -> Assembly",
               Loc::T("roujiamo"), REWARD_ROUJIAMO),
               {(float)rx, (float)ry}, UI_FONT_SMALL, 1.0f, DARKGREEN);
    ry += 18;
    DrawTextEx(s_font, TextFormat("%s %dg: Raw -> Cutting -> Assembly",
               Loc::T("liangpi"), REWARD_LIANGPI),
               {(float)rx, (float)ry}, UI_FONT_SMALL, 1.0f, DARKGRAY);
    ry += 18;
    DrawTextEx(s_font, TextFormat("%s %dg: Raw -> Grill -> Assembly",
               Loc::T("kebab"), REWARD_KEBAB),
               {(float)rx, (float)ry}, UI_FONT_SMALL, 1.0f, DARKGRAY);
    ry += 26;
    DrawTextEx(s_font, Loc::T("customers_title"), {(float)rx, (float)ry}, UI_FONT_BODY, 1.0f, DARKGRAY);
    ry += 22;
    DrawTextEx(s_font, Loc::T("cust_normal"), {(float)rx, (float)ry}, UI_FONT_SMALL, 1.0f, GREEN);
    ry += 16;
    DrawTextEx(s_font, Loc::T("cust_impatient"), {(float)rx, (float)ry}, UI_FONT_SMALL, 1.0f, YELLOW);
    ry += 16;
    DrawTextEx(s_font, Loc::T("cust_vip"), {(float)rx, (float)ry}, UI_FONT_SMALL, 1.0f, COLOR_GOLD);
    ry += 18;
    DrawTextEx(s_font, TextFormat("%s=RM  %s=LP  %s=KB",
               Loc::T("roujiamo"), Loc::T("liangpi"), Loc::T("kebab")),
               {(float)rx, (float)ry}, UI_FONT_TINY, 1.0f, DARKGRAY);
}

void UI::DrawHelpPanel() {
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, {0, 0, 0, 180});
    DrawPanel(50, 30, 700, 540, COLOR_PANEL_BG);

    int hx = 64, hy = 44;
    DrawTextEx(s_font, Loc::T("help_title"), {(float)hx, (float)hy}, UI_FONT_TITLE, 1.0f, DARKGREEN);
    hy += 46;

    // --- Basic Flow ---
    DrawTextEx(s_font, Loc::T("help_flow"), {(float)hx, (float)hy}, UI_FONT_HEAD, 1.0f, DARKGRAY);
    hy += 28;
    DrawTextEx(s_font, Loc::T("help_flow1"), {(float)hx + 16, (float)hy}, UI_FONT_SMALL, 1.0f, DARKGRAY);
    hy += 20;
    DrawTextEx(s_font, Loc::T("help_flow2"), {(float)hx + 16, (float)hy}, UI_FONT_SMALL, 1.0f, DARKGRAY);
    hy += 20;
    DrawTextEx(s_font, Loc::T("help_flow3"), {(float)hx + 16, (float)hy}, UI_FONT_SMALL, 1.0f, DARKGRAY);
    hy += 30;

    // --- Recipes ---
    DrawTextEx(s_font, Loc::T("help_recipes"), {(float)hx, (float)hy}, UI_FONT_HEAD, 1.0f, DARKGRAY);
    hy += 28;
    DrawTextEx(s_font, TextFormat("RM %s  %dg: Raw -> Grill -> Cutting -> Assembly",
               Loc::T("roujiamo"), REWARD_ROUJIAMO),
               {(float)hx + 16, (float)hy}, UI_FONT_SMALL, 1.0f, DARKGREEN);
    hy += 20;
    DrawTextEx(s_font, TextFormat("LP %s  %dg: Raw -> Cutting -> Assembly",
               Loc::T("liangpi"), REWARD_LIANGPI),
               {(float)hx + 16, (float)hy}, UI_FONT_SMALL, 1.0f, DARKGRAY);
    hy += 20;
    DrawTextEx(s_font, TextFormat("KB %s  %dg: Raw -> Grill -> Assembly",
               Loc::T("kebab"), REWARD_KEBAB),
               {(float)hx + 16, (float)hy}, UI_FONT_SMALL, 1.0f, DARKGRAY);
    hy += 30;

    // --- Customers ---
    DrawTextEx(s_font, Loc::T("help_customers"), {(float)hx, (float)hy}, UI_FONT_HEAD, 1.0f, DARKGRAY);
    hy += 28;
    DrawTextEx(s_font, Loc::T("cust_normal"), {(float)hx + 16, (float)hy}, UI_FONT_SMALL, 1.0f, GREEN);
    hy += 20;
    DrawTextEx(s_font, Loc::T("cust_impatient"), {(float)hx + 16, (float)hy}, UI_FONT_SMALL, 1.0f, YELLOW);
    hy += 20;
    DrawTextEx(s_font, Loc::T("cust_vip"), {(float)hx + 16, (float)hy}, UI_FONT_SMALL, 1.0f, COLOR_GOLD);
    hy += 30;

    // --- Tips ---
    DrawTextEx(s_font, Loc::T("help_tips"), {(float)hx, (float)hy}, UI_FONT_HEAD, 1.0f, DARKGRAY);
    hy += 28;
    DrawTextEx(s_font, Loc::T("help_tip1"), {(float)hx + 16, (float)hy}, UI_FONT_SMALL, 1.0f, DARKGRAY);
    hy += 20;
    DrawTextEx(s_font, Loc::T("help_tip2"), {(float)hx + 16, (float)hy}, UI_FONT_SMALL, 1.0f, RED);
    hy += 20;
    DrawTextEx(s_font, Loc::T("help_tip3"), {(float)hx + 16, (float)hy}, UI_FONT_SMALL, 1.0f, DARKGRAY);

    // Close hint
    DrawTextEx(s_font, Loc::T("help_close"),
               {(float)(hx + 700 - TextWidth(Loc::T("help_close"), UI_FONT_BODY)), 540}, UI_FONT_BODY, 1.0f, DARKGREEN);
}

void UI::DrawControlsHint() {
    DrawTextEx(s_font, Loc::T("controls_hint"),
               {10, (float)(SCREEN_HEIGHT - 24)}, UI_FONT_SMALL, 1.0f, GRAY);
}

// ── Effects ────────────────────────────────────────────

void UI::DrawFloatText(float x, float y, float alpha, int value) {
    Color c = COLOR_GOLD;
    c.a = (unsigned char)(255 * alpha);
    const char* text;
    if (value == -1) {
        c = RED;
        c.a = (unsigned char)(255 * alpha);
        text = "!";
    } else if (value == 0) {
        c = DARKGRAY;
        c.a = (unsigned char)(255 * alpha);
        text = "+Raw";
    } else {
        text = TextFormat("+%d", value);
    }
    int w = TextWidth(text, UI_FONT_BODY);
    DrawTextEx(s_font, text, {(float)((int)x - w / 2), y}, UI_FONT_BODY, 1.0f, c);
}

void UI::DrawMilestone(int streak, float timer) {
    const char* text = TextFormat(Loc::T("n_in_a_row"), streak);
    int w = TextWidth(text, UI_FONT_HEAD);
    float alpha = timer / 2.0f;
    Color c = ORANGE;
    c.a = (unsigned char)(255 * alpha);
    DrawTextEx(s_font, text,
               {(float)((SCREEN_WIDTH - w) / 2), 120}, UI_FONT_HEAD, 1.0f, c);
}

void UI::DrawGameOverFlash(float alpha) {
    Color c = {255, 0, 0, (unsigned char)(180 * alpha)};
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, c);
}
