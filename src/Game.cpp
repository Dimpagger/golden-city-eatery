#include <cstdlib>
#include <cmath>
#include <fstream>
#include "Game.h"
#include "Input.h"
#include "Food.h"
#include "Constants.h"
#include "Localization.h"
#include "UI.h"

const std::string Game::HIGH_SCORE_FILE = "highscore.dat";

Game::Game(): chef(CHEF_X_INIT, CHEF_Y) {
    LoadHighScore();
    Reset();
}

Game::~Game() = default;

void Game::LoadHighScore() {
    std::ifstream in(HIGH_SCORE_FILE);
    if (in) {
        in >> bestScore;
        if (in.fail()) bestScore = 0;
    }
}

void Game::SaveHighScore() {
    if (score > bestScore) {
        bestScore = score;
        std::ofstream out(HIGH_SCORE_FILE);
        if (out) out << bestScore;
    }
}

float Game::GetDayMultiplier(int day) {
    return 1.0f - (day - 1) * DAY_DIFFICULTY_STEP;
}

void Game::Reset() {
    coins = 0;
    score = 0;
    streak = 0;
    paused = false;

    feedbackGrill = false;
    feedbackAssembly = false;
    feedbackChef = false;
    feedbackTimer = 0.0f;
    gameOverTimer = 0.0f;
    milestoneTimer = 0.0f;
    milestoneStreak = 0;
    lastLostCount = 0;
    floatTexts.clear();
    upgradeSys.Reset();

    StartDay(1);
}

void Game::StartDay(int day) {
    currentDay = day;
    servedToday = 0;

    chef = Chef(CHEF_X_INIT, CHEF_Y);
    chef.PickUpFood(std::make_unique<Food>(FoodState::RAW));

    for (int i = 0; i < STATION_COUNT; i++) {
        stations[i] = Station(static_cast<StationType>(i), STATION_X[i], STATION_Y);
    }

    customerMgr.ResetForDay(GetDayMultiplier(day));
    ApplyUpgrades();

    lastLostCount = 0;
    gameOverTimer = 0.0f;
    milestoneTimer = 0.0f;
    milestoneStreak = 0;
    feedbackTimer = 0.0f;
    feedbackGrill = false;
    feedbackAssembly = false;
    feedbackChef = false;
    floatTexts.clear();
}

void Game::Update(float dt) {
    switch (state) {
        case GameState::MENU:
            if (Input::IsKeyPressed(KEY_INTERACT)) {
                Reset();
                state = GameState::PLAYING;
            }
            if (Input::IsKeyPressed(KEY_L)) {
                Loc::ToggleLanguage();
            }
            break;

        case GameState::PLAYING:
            if (Input::IsKeyPressed(KEY_P)) {
                paused = !paused;
            }
            if (Input::IsKeyPressed(KEY_L)) {
                Loc::ToggleLanguage();
            }
            if (gameOverTimer > 0.0f) {
                gameOverTimer -= dt;
                if (gameOverTimer <= 0.0f) {
                    state = GameState::GAME_OVER;
                    paused = false;
                }
            } else if (!paused) {
                UpdatePlaying(dt);
                if (customerMgr.GetLostCount() >= MAX_LOST) {
                    SaveHighScore();
                    gameOverTimer = 1.0f;
                } else if (servedToday >= DAY_TARGETS[currentDay - 1]) {
                    SaveHighScore();
                    state = (currentDay >= TOTAL_DAYS) ? GameState::VICTORY : GameState::DAY_COMPLETE;
                }
            }
            break;

        case GameState::DAY_COMPLETE:
            if (Input::IsKeyPressed(KEY_INTERACT)) {
                StartDay(currentDay + 1);
                state = GameState::PLAYING;
            }
            break;

        case GameState::GAME_OVER:
            if (Input::IsKeyPressed(KEY_INTERACT)) {
                Reset();
                state = GameState::PLAYING;
            }
            if (Input::IsKeyPressed(KEY_L)) {
                Loc::ToggleLanguage();
            }
            break;

        case GameState::VICTORY:
            if (Input::IsKeyPressed(KEY_INTERACT)) {
                Reset();
                state = GameState::MENU;
            }
            break;
    }
}

void Game::UpdatePlaying(float dt) {
    // --- Entities ---
    chef.Update(dt);
    for (int i = 0; i < STATION_COUNT; i++) {
        stations[i].Update(dt);
    }
    customerMgr.Update(dt);

    // --- Milestone timer ---
    if (milestoneTimer > 0.0f) {
        milestoneTimer -= dt;
    }

    // --- Float texts ---
    for (auto it = floatTexts.begin(); it != floatTexts.end(); ) {
        it->y -= 40.0f * dt;
        it->timer -= dt;
        if (it->timer <= 0.0f)
            it = floatTexts.erase(it);
        else
            ++it;
    }

    // --- Feedback timer ---
    if (feedbackTimer > 0.0f) {
        feedbackTimer -= dt;
        if (feedbackTimer <= 0.0f) {
            feedbackGrill = false;
            feedbackAssembly = false;
            feedbackChef = false;
        }
    }

    // --- Upgrades ---
    HandleUpgradeInput();

    // --- Interaction ---
    if (Input::IsKeyPressed(KEY_INTERACT)) {
        Station* target = GetNearbyStation();

        if (target) {
            if (chef.IsHoldingFood()) {
                if (target->CanAccept(*chef.GetHeldFood())) {
                    target->PlaceFood(chef.DropFood());
                    target->StartWork();
                }
            } else {
                if (target->IsDone()) {
                    chef.PickUpFood(target->TakeFood());
                } else if (target->HasFood() && target->GetState() == StationState::IDLE) {
                    target->StartWork();
                }
            }
        }
    }

    // --- Serving delivery ---
    if (stations[3].IsDone() && stations[3].HasFood()) {
        int baseReward = stations[3].GetFood()->GetReward();
        stations[3].TakeFood();
        float mult = customerMgr.ServeFirstWaiting();
        int reward = (int)(baseReward * mult);
        coins += reward;
        score++;
        streak++;
        servedToday++;

        floatTexts.push_back({stations[3].GetX() + stations[3].GetWidth() / 2.0f,
                              stations[3].GetY(), 1.0f, reward});

        if (streak > 0 && streak % MILESTONE_INTERVAL == 0) {
            milestoneTimer = 2.0f;
            milestoneStreak = streak;
        }

        if (!chef.IsHoldingFood()) {
            chef.PickUpFood(std::make_unique<Food>(FoodState::RAW));
        }
    }

    // --- Streak reset on lost customer ---
    int currentLost = customerMgr.GetLostCount();
    if (currentLost > lastLostCount) {
        streak = 0;
    }
    lastLostCount = currentLost;
}

Station* Game::GetNearbyStation() {
    raylib::Rectangle chefRect = chef.GetRect();
    for (int i = 0; i < STATION_COUNT; i++) {
        raylib::Rectangle sRect(stations[i].GetX(), stations[i].GetY(),
                                stations[i].GetWidth(), stations[i].GetHeight());
        if (CheckCollisionRecs(chefRect, sRect)) {
            return &stations[i];
        }
    }
    return nullptr;
}

GameState Game::GetState() const { return state; }

// ─── Draw ───────────────────────────────────────────

void Game::Draw() {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    switch (state) {
        case GameState::MENU:
            UI::DrawMenuScreen();
            break;
        case GameState::PLAYING:
            DrawPlaying();
            if (paused) UI::DrawPauseOverlay();
            break;
        case GameState::DAY_COMPLETE:
            UI::DrawDayCompleteScreen(currentDay, score, coins);
            break;
        case GameState::GAME_OVER:
            UI::DrawGameOverScreen(score, coins, customerMgr.GetLostCount(), bestScore, currentDay);
            break;
        case GameState::VICTORY:
            UI::DrawVictoryScreen(score, coins, bestScore);
            break;
    }

    EndDrawing();
}

void Game::DrawPlaying() {
    const char* label[] = {Loc::T("grill"), Loc::T("cutting"), Loc::T("assembly"), Loc::T("serving")};
    const Color  color[] = {RED, GRAY, {210, 160, 0, 255}, GREEN};

    UI::DrawTopBar(coins, score, customerMgr.GetQueueCount(),
                   customerMgr.GetLostCount(), MAX_LOST, bestScore, streak,
                   currentDay, servedToday);

    for (int i = 0; i < STATION_COUNT; i++) {
        UI::DrawStation(stations[i], label[i], color[i], stations[i].GetWorkTime());
        UI::DrawCountdown(stations[i]);
    }

    for (auto& c : customerMgr.GetCustomers()) {
        UI::DrawCustomer(*c);
    }

    UI::DrawChef(chef);

    Station* near = GetNearbyStation();
    if (near) {
        UI::DrawInteractionHint(*near);
    }

    UI::DrawUpgradePanel(upgradeSys, coins);

    for (auto& ft : floatTexts) {
        UI::DrawFloatText(ft.x, ft.y, ft.timer, ft.value);
    }

    if (milestoneTimer > 0.0f) {
        UI::DrawMilestone(milestoneStreak, milestoneTimer);
    }

    if (feedbackTimer > 0.0f) {
        if (feedbackGrill)       UI::DrawFeedbackMessage(Loc::T("grill_upgraded"));
        if (feedbackAssembly)    UI::DrawFeedbackMessage(Loc::T("asm_upgraded"));
        if (feedbackChef)        UI::DrawFeedbackMessage(Loc::T("chef_upgraded"));
    }

    if (gameOverTimer > 0.0f) {
        UI::DrawGameOverFlash(gameOverTimer);
    }

    UI::DrawControlsHint();
}

// ─── Upgrades ────────────────────────────────────────

void Game::ApplyUpgrades() {
    stations[0].SetWorkTime(BASE_GRILL_TIME * upgradeSys.GetSpeedMultiplier(UpgradeType::GRILL_SPEED));
    stations[1].SetWorkTime(BASE_CUTTING_TIME);
    stations[2].SetWorkTime(BASE_ASSEMBLY_TIME * upgradeSys.GetSpeedMultiplier(UpgradeType::OVEN_SPEED));
    stations[3].SetWorkTime(BASE_SERVING_TIME);
    chef.SetSpeed(BASE_CHEF_SPEED * upgradeSys.GetSpeedMultiplier(UpgradeType::CHEF_SPEED));
}

void Game::HandleUpgradeInput() {
    if (Input::IsKeyPressed(KEY_ONE)) {
        if (upgradeSys.Purchase(UpgradeType::GRILL_SPEED, coins)) {
            ApplyUpgrades();
            feedbackGrill = true;
            feedbackTimer = 1.5f;
        }
    }
    if (Input::IsKeyPressed(KEY_TWO)) {
        if (upgradeSys.Purchase(UpgradeType::OVEN_SPEED, coins)) {
            ApplyUpgrades();
            feedbackAssembly = true;
            feedbackTimer = 1.5f;
        }
    }
    if (Input::IsKeyPressed(KEY_THREE)) {
        if (upgradeSys.Purchase(UpgradeType::CHEF_SPEED, coins)) {
            ApplyUpgrades();
            feedbackChef = true;
            feedbackTimer = 1.5f;
        }
    }
}
