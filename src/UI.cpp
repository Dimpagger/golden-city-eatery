#include "UI.h"
#include "Constants.h"
#include "Food.h"

// ── Visual constants ──────────────────────────────────
static constexpr Color COLOR_PANEL_BG  = {240, 240, 240, 220};
static constexpr Color COLOR_PANEL_BD  = {180, 180, 180, 255};
static constexpr Color COLOR_BAR_BG    = {200, 200, 200, 255};
static constexpr Color COLOR_BAR_FG    = ORANGE;
static constexpr Color COLOR_GOLD      = {218, 165, 32, 255};
static constexpr int   PANEL_PAD       = 6;

static constexpr int FONT_TITLE = 36;
static constexpr int FONT_HEAD  = 24;
static constexpr int FONT_BODY  = 20;
static constexpr int FONT_SMALL = 14;
static constexpr int FONT_TINY  = 12;

// ── Helpers ───────────────────────────────────────────

// cx,cy,cw,ch = content area; panel extends PANEL_PAD outward on all sides
void UI::DrawPanel(float cx, float cy, float cw, float ch, Color bg) {
    float x = cx - PANEL_PAD;
    float y = cy - PANEL_PAD;
    float w = cw + 2 * PANEL_PAD;
    float h = ch + 2 * PANEL_PAD;
    DrawRectangle((int)x, (int)y, (int)w, (int)h, bg);
    DrawRectangleLines((int)x, (int)y, (int)w, (int)h, COLOR_PANEL_BD);
}

const char* UI::FoodStateLabel(FoodState state) {
    switch (state) {
        case FoodState::RAW:       return "Raw";
        case FoodState::COOKED:    return "Cooked";
        case FoodState::SLICED:    return "Sliced";
        case FoodState::ASSEMBLED: return "Assembled";
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
    DrawPanel(206, 136, 388, 348, COLOR_PANEL_BG);

    DrawText("Roujiamo Legend", 235, 170, FONT_TITLE, DARKGRAY);
    DrawText("A 2D cooking sim", 300, 220, FONT_SMALL, GRAY);

    DrawText("How to Play:", 250, 270, FONT_BODY, DARKGRAY);
    DrawText("A/D    Move left / right", 260, 300, FONT_SMALL, DARKGRAY);
    DrawText("Space  Interact with station", 260, 322, FONT_SMALL, DARKGRAY);
    DrawText("1/2/3  Buy upgrades", 260, 344, FONT_SMALL, DARKGRAY);
    DrawText("P      Pause", 260, 366, FONT_SMALL, DARKGRAY);

    DrawText("Press SPACE to Start", 270, 415, FONT_BODY, DARKGREEN);
}

void UI::DrawGameOverScreen(int score, int coins, int lost) {
    DrawPanel(206, 126, 388, 368, COLOR_PANEL_BG);

    DrawText("GAME OVER", 280, 160, FONT_TITLE, RED);

    DrawText(TextFormat("Customers Served: %d", score), 250, 230, FONT_BODY, DARKGREEN);
    DrawText(TextFormat("Customers Lost:   %d", lost), 250, 260, FONT_BODY, RED);
    DrawText(TextFormat("Total Coins:      %d", coins), 250, 300, FONT_BODY, COLOR_GOLD);

    DrawText("Press SPACE to Play Again", 240, 380, FONT_BODY, DARKGREEN);
}

void UI::DrawPauseOverlay() {
    // Semi-transparent background
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, {0, 0, 0, 128});

    DrawPanel(276, 236, 248, 108, COLOR_PANEL_BG);
    DrawText("PAUSED", 320, 260, FONT_TITLE, DARKGRAY);
    DrawText("Press P to Resume", 295, 310, FONT_SMALL, DARKGRAY);
}

// ── HUD ───────────────────────────────────────────────

void UI::DrawTopBar(int coins, int score, int queueCount,
                    int lostCount, int maxLost) {
    // Background panel
    DrawPanel(10, 10, SCREEN_WIDTH - 20, 24, COLOR_PANEL_BG);

    int x = 14;
    DrawText(TextFormat("Coins: %d", coins), x, 10, FONT_BODY, COLOR_GOLD);
    x += 130;
    DrawText(TextFormat("Score: %d", score), x, 10, FONT_BODY, DARKBLUE);
    x += 130;
    DrawText(TextFormat("Queue: %d/3", queueCount), x, 10, FONT_SMALL, DARKGRAY);
    x += 100;
    Color lostColor = (lostCount >= maxLost - 1) ? RED : DARKGRAY;
    DrawText(TextFormat("Lost: %d/%d", lostCount, maxLost), x, 10, FONT_SMALL, lostColor);
}

// ── Game Entities ─────────────────────────────────────

void UI::DrawStation(const Station& s, const char* label,
                     Color color, float workTime) {
    int sx = (int)s.GetX();
    int sy = (int)s.GetY();
    int sw = (int)s.GetWidth();
    int sh = (int)s.GetHeight();

    // Body
    DrawRectangle(sx, sy, sw, sh, color);
    DrawRectangleLines(sx, sy, sw, sh, DARKGRAY);

    // Label
    int labelW = MeasureText(label, FONT_SMALL);
    DrawText(label, sx + (sw - labelW) / 2, sy + 6, FONT_SMALL, WHITE);

    // Work time
    DrawText(TextFormat("%.1fs", workTime), sx + 6, sy + sh - 18, FONT_TINY, BLACK);

    // State text
    if (s.IsDone() && s.HasFood()) {
        DrawText("DONE", sx + sw - 40, sy + sh - 18, FONT_TINY, DARKGREEN);
    } else if (s.HasFood() && s.GetState() == StationState::IDLE) {
        DrawText("READY", sx + sw - 45, sy + sh - 18, FONT_TINY, DARKGRAY);
    }

    // Progress bar
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

    // Body
    Color bodyColor;
    if (c.GetState() == Customer::State::LEAVING) {
        bodyColor = c.WasServed() ? DARKGREEN : RED;
    } else {
        bodyColor = c.GetPatience() > 10.0f ? GREEN : YELLOW;
    }
    DrawRectangle(cx, cy, 28, 40, bodyColor);
    DrawRectangleLines(cx, cy, 28, 40, DARKGRAY);

    // Face
    const char* face = CustomerFace(c);
    if (face[0]) {
        DrawText(face, cx + 2, cy - 18, FONT_SMALL, BLACK);
    }

    // Patience bar
    if (c.IsWaiting()) {
        float pct = c.GetPatience() / CUSTOMER_PATIENCE;
        DrawRectangle(cx - 2, cy + 44, 32, 4, COLOR_BAR_BG);
        DrawRectangle(cx - 2, cy + 44, (int)(32 * pct), 4,
                      pct > 0.3f ? GREEN : RED);
    }
}

void UI::DrawChef(const Chef& chef) {
    chef.Draw();  // uses Chef's own Draw
    if (chef.IsHoldingFood()) {
        DrawText(FoodStateLabel(chef.GetHeldFood()->GetState()),
                 (int)chef.GetX(), (int)chef.GetY() - 18, FONT_SMALL, DARKGRAY);
    }
}

void UI::DrawInteractionHint(const Station& s) {
    DrawText("[Space]",
             (int)s.GetX() + (int)s.GetWidth() / 2 - 20,
             (int)s.GetY() - 20, FONT_SMALL, BLACK);
}

void UI::DrawCountdown(const Station& s) {
    if (s.GetState() != StationState::WORKING) return;
    float remaining = s.GetWorkTime() * (1.0f - s.GetProgress() / 100.0f);
    const char* text = TextFormat("%.1fs", remaining);
    int barY = (int)s.GetY() + (int)s.GetHeight() + 4;
    int textW = MeasureText(text, FONT_SMALL);
    // Centered on the progress bar
    DrawText(text, (int)s.GetX() + ((int)s.GetWidth() - textW) / 2, barY - 3,
             FONT_SMALL, DARKGRAY);
}

// ── Panels ────────────────────────────────────────────

void UI::DrawUpgradePanel(const UpgradeSystem& upg, int coins) {
    int ux = 602;
    int uy = 48;

    DrawPanel(ux, uy, 188, 66, COLOR_PANEL_BG);

    DrawText("Upgrades (1/2/3)", ux, uy, FONT_TINY, DARKGRAY);
    uy += 16;

    // Grill
    float gt = BASE_GRILL_TIME * upg.GetSpeedMultiplier(UpgradeType::GRILL_SPEED);
    DrawText(TextFormat("[1] Grill  Lv.%d  %.1fs",
             upg.GetLevel(UpgradeType::GRILL_SPEED), gt),
             ux, uy, FONT_SMALL, DARKGRAY);
    if (!upg.IsMaxed(UpgradeType::GRILL_SPEED)) {
        Color costColor = coins >= upg.GetCost(UpgradeType::GRILL_SPEED) ? DARKGREEN : RED;
        DrawText(TextFormat("%dg", upg.GetCost(UpgradeType::GRILL_SPEED)),
                 ux + 145, uy, FONT_SMALL, costColor);
    } else {
        DrawText("MAX", ux + 145, uy, FONT_SMALL, GRAY);
    }
    uy += 18;

    // Assembly
    float at = BASE_ASSEMBLY_TIME * upg.GetSpeedMultiplier(UpgradeType::OVEN_SPEED);
    DrawText(TextFormat("[2] Asm   Lv.%d  %.1fs",
             upg.GetLevel(UpgradeType::OVEN_SPEED), at),
             ux, uy, FONT_SMALL, DARKGRAY);
    if (!upg.IsMaxed(UpgradeType::OVEN_SPEED)) {
        Color costColor = coins >= upg.GetCost(UpgradeType::OVEN_SPEED) ? DARKGREEN : RED;
        DrawText(TextFormat("%dg", upg.GetCost(UpgradeType::OVEN_SPEED)),
                 ux + 145, uy, FONT_SMALL, costColor);
    } else {
        DrawText("MAX", ux + 145, uy, FONT_SMALL, GRAY);
    }
    uy += 18;

    // Chef Speed
    float cs = BASE_CHEF_SPEED * upg.GetSpeedMultiplier(UpgradeType::CHEF_SPEED);
    DrawText(TextFormat("[3] Chef  Lv.%d  %.0fpx/s",
             upg.GetLevel(UpgradeType::CHEF_SPEED), cs),
             ux, uy, FONT_SMALL, DARKGRAY);
    if (!upg.IsMaxed(UpgradeType::CHEF_SPEED)) {
        Color costColor = coins >= upg.GetCost(UpgradeType::CHEF_SPEED) ? DARKGREEN : RED;
        DrawText(TextFormat("%dg", upg.GetCost(UpgradeType::CHEF_SPEED)),
                 ux + 145, uy, FONT_SMALL, costColor);
    } else {
        DrawText("MAX", ux + 145, uy, FONT_SMALL, GRAY);
    }
}

void UI::DrawFeedbackMessage(const char* text) {
    int w = MeasureText(text, FONT_HEAD);
    DrawText(text, (SCREEN_WIDTH - w) / 2, 90, FONT_HEAD, ORANGE);
}

void UI::DrawControlsHint() {
    DrawText("A/D: Move  |  Space: Interact  |  1/2/3: Upgrade  |  P: Pause",
             10, SCREEN_HEIGHT - 24, FONT_SMALL, GRAY);
}

// ── Effects ────────────────────────────────────────────

void UI::DrawFloatText(float x, float y, float alpha) {
    Color c = COLOR_GOLD;
    c.a = (unsigned char)(255 * alpha);
    DrawText("+10", (int)x - 15, (int)y, FONT_BODY, c);
}

void UI::DrawGameOverFlash(float alpha) {
    Color c = {255, 0, 0, (unsigned char)(180 * alpha)};
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, c);
}
