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
    feedbackCutting = false;
    feedbackAssembly = false;
    feedbackChef = false;
    feedbackTimer = 0.0f;
    gameOverTimer = 0.0f;
    milestoneTimer = 0.0f;
    milestoneStreak = 0;
    lastLostCount = 0;
    showRecipePanel = false;
    showHelpPanel = false;
    shakeTimer = 0.0f;
    shakeIntensity = 0.0f;
    endlessMode = false;

    discoveredLiangpi = false;
    discoveredKebab = false;
    totalServed = 0;
    totalCoins = 0;
    totalRoujiamo = 0;
    totalLiangpi = 0;
    totalKebab = 0;
    particles.clear();

    floatTexts.clear();
    upgradeSys.Reset();

    StartDay(1);
}

void Game::StartDay(int day) {
    currentDay = day;
    servedToday = 0;

    chef.ResetPosition(CHEF_X_INIT, CHEF_Y);
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
    feedbackCutting = false;
    feedbackAssembly = false;
    feedbackChef = false;
    floatTexts.clear();
}

void Game::Update(float dt) {
    // Global: language toggle works in all states
    if (Input::IsKeyPressed(KEY_L)) {
        Loc::ToggleLanguage();
    }

    // --- Screen shake ---
    if (shakeTimer > 0.0f) {
        shakeTimer -= dt;
    }

    switch (state) {
        case GameState::MENU:
            if (Input::IsKeyPressed(KEY_INTERACT)) {
                Reset();
                state = GameState::PLAYING;
            }
            if (Input::IsKeyPressed(KEY_HELP)) {
                showHelpPanel = !showHelpPanel;
            }
            break;

        case GameState::PLAYING:
            if (Input::IsKeyPressed(KEY_P) && gameOverTimer <= 0.0f) {
                paused = !paused;
            }
            if (Input::IsKeyPressed(KEY_RECIPE_GUIDE)) {
                showRecipePanel = !showRecipePanel;
            }
            if (Input::IsKeyPressed(KEY_HELP)) {
                showHelpPanel = !showHelpPanel;
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
                    shakeTimer = 0.5f;
                    shakeIntensity = 10.0f;
                } else if (servedToday >= DAY_TARGETS[currentDay - 1]) {
                    SaveHighScore();
                    if (currentDay >= TOTAL_DAYS && !endlessMode) {
                        state = GameState::VICTORY;
                    } else {
                        state = GameState::DAY_COMPLETE;
                    }
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
            break;

        case GameState::VICTORY:
            if (Input::IsKeyPressed(KEY_INTERACT)) {
                endlessMode = true;
                StartDay(currentDay + 1);
                state = GameState::PLAYING;
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

    // --- Particles ---
    for (auto it = particles.begin(); it != particles.end(); ) {
        it->x += it->vx * dt;
        it->y += it->vy * dt;
        it->life -= dt;
        if (it->life <= 0.0f) it = particles.erase(it);
        else ++it;
    }

    // --- Feedback timer ---
    if (feedbackTimer > 0.0f) {
        feedbackTimer -= dt;
        if (feedbackTimer <= 0.0f) {
            feedbackGrill = false;
            feedbackCutting = false;
            feedbackAssembly = false;
            feedbackChef = false;
        }
    }

    // --- Upgrades ---
    HandleUpgradeInput();

    // --- Mouse station click ---
    if (Input::IsMousePressed()) {
        for (int i = 0; i < STATION_COUNT; i++) {
            if (Input::IsMouseInRect(stations[i].GetX(), stations[i].GetY(),
                                      stations[i].GetWidth(), stations[i].GetHeight())) {
                nearStation = &stations[i];
                if (chef.IsHoldingFood()) {
                    if (nearStation->CanAccept(*chef.GetHeldFood())) {
                        nearStation->PlaceFood(chef.DropFood());
                        nearStation->StartWork();
                    }
                } else {
                    if (nearStation->IsDone()) {
                        chef.PickUpFood(nearStation->TakeFood());
                    }
                }
                break;
            }
        }
    }

    // --- Keyboard interaction ---
    nearStation = GetNearbyStation();
    if (Input::IsKeyPressed(KEY_INTERACT) && nearStation) {
        if (chef.IsHoldingFood()) {
            if (nearStation->CanAccept(*chef.GetHeldFood())) {
                nearStation->PlaceFood(chef.DropFood());
                nearStation->StartWork();
                // SFX: play cooking start sound
            }
        } else {
            if (nearStation->IsDone()) {
                chef.PickUpFood(nearStation->TakeFood());
            }
        }
    }

    // --- Serving delivery ---
    auto& serving = stations[static_cast<int>(StationType::SERVING)];
    if (serving.IsDone() && serving.HasFood()) {
        int baseReward = serving.GetFood()->GetReward();
        RecipeType foodRecipe = serving.GetFood()->GetRecipe();
        serving.TakeFood();

        // Recipe matching: full reward for correct recipe, half for wrong
        const Customer* customer = customerMgr.GetFirstWaiting();
        bool recipeMatch = customer && customer->GetDesiredRecipe() == foodRecipe;
        float mult = customerMgr.ServeFirstWaiting();
        int reward = recipeMatch ? (int)(baseReward * mult) : baseReward / 2;

        coins += reward;
        score++;
        streak++;
        servedToday++;

        // Stats
        totalServed++;
        totalCoins += reward;
        switch (foodRecipe) {
            case RecipeType::ROUJIAMO: totalRoujiamo++; break;
            case RecipeType::LIANGPI:  totalLiangpi++;  break;
            case RecipeType::KEBAB:    totalKebab++;    break;
            default: break;
        }

        // Recipe discovery
        if (foodRecipe == RecipeType::LIANGPI && !discoveredLiangpi) {
            discoveredLiangpi = true;
            feedbackTimer = 2.0f;
            milestoneTimer = 0.0f; // reuse milestone display for discovery
            milestoneStreak = -1;   // signal "discovery" to UI
        }
        if (foodRecipe == RecipeType::KEBAB && !discoveredKebab) {
            discoveredKebab = true;
            feedbackTimer = 2.0f;
            milestoneTimer = 0.0f;
            milestoneStreak = -2;
        }

        floatTexts.push_back({serving.GetX() + serving.GetWidth() / 2.0f,
                              serving.GetY(), 1.0f, reward});

        // Particles on serve
        SpawnParticles(serving.GetX() + serving.GetWidth() / 2.0f,
                       serving.GetY(), GOLD, 6);

        // SFX: play serve sound (different tone for match vs mismatch)
        if (streak > 0 && streak % MILESTONE_INTERVAL == 0) {
            milestoneTimer = 2.0f;
            milestoneStreak = streak;
            shakeTimer = 0.3f;
            shakeIntensity = 5.0f;
            SpawnParticles(SCREEN_WIDTH / 2.0f, 150, ORANGE, 15);
            // SFX: play milestone fanfare
        }

        if (!chef.IsHoldingFood()) {
            chef.PickUpFood(std::make_unique<Food>(FoodState::RAW));
            floatTexts.push_back({chef.GetX() + 16, chef.GetY() - 10, 0.8f, 0});
        }
    }

    // --- Streak reset on lost customer ---
    int currentLost = customerMgr.GetLostCount();
    if (currentLost > lastLostCount) {
        streak = 0;
        floatTexts.push_back({customerMgr.GetLastLostX(),
                              customerMgr.GetLastLostY() - 10, 1.5f, -1});
        shakeTimer = 0.2f;
        shakeIntensity = 3.0f;
    }
    lastLostCount = currentLost;
}

Station* Game::GetNearbyStation() {
    raylib::Rectangle chefRect = chef.GetRect();
    for (int i = 0; i < STATION_COUNT; i++) {
        // Use a tighter interaction zone (60% of station size)
        const float marginX = stations[i].GetWidth() * 0.2f;
        const float marginY = stations[i].GetHeight() * 0.2f;
        raylib::Rectangle sRect(stations[i].GetX() + marginX, stations[i].GetY() + marginY,
                                stations[i].GetWidth() - 2 * marginX,
                                stations[i].GetHeight() - 2 * marginY);
        if (CheckCollisionRecs(chefRect, sRect)) {
            return &stations[i];
        }
    }
    return nullptr;
}

// ─── Draw ───────────────────────────────────────────

void Game::Draw() {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    // Screen shake via 2D camera offset
    Camera2D shakeCam = {};
    shakeCam.zoom = 1.0f;
    if (shakeTimer > 0.0f) {
        float i = shakeIntensity * shakeTimer;
        shakeCam.offset.x = ((rand() % 100) / 50.0f - 1.0f) * i;
        shakeCam.offset.y = ((rand() % 100) / 50.0f - 1.0f) * i;
    }
    BeginMode2D(shakeCam);

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
            UI::DrawGameOverScreen(score, coins, customerMgr.GetLostCount(), bestScore, currentDay,
                                   totalRoujiamo, totalLiangpi, totalKebab);
            break;
        case GameState::VICTORY:
            UI::DrawVictoryScreen(score, coins, bestScore,
                                  totalRoujiamo, totalLiangpi, totalKebab);
            break;
    }

    if (showHelpPanel) {
        UI::DrawHelpPanel();
    }

    EndMode2D();
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

    if (nearStation) {
        UI::DrawInteractionHint(*nearStation);
    }

    UI::DrawUpgradePanel(upgradeSys, coins);

    for (auto& ft : floatTexts) {
        UI::DrawFloatText(ft.x, ft.y, ft.timer, ft.value);
    }

    if (milestoneTimer > 0.0f) {
        if (milestoneStreak < 0) {
            // Recipe discovery
            UI::DrawDiscoveryMessage(milestoneStreak == -1 ? RecipeType::LIANGPI : RecipeType::KEBAB,
                                     milestoneTimer);
        } else {
            UI::DrawMilestone(milestoneStreak, milestoneTimer);
        }
    }

    // --- Particles ---
    for (auto& p : particles) {
        Color c = p.color;
        c.a = (unsigned char)(255.0f * p.life / 0.6f);
        DrawCircle((int)p.x, (int)p.y, 2.5f, c);
    }

    if (feedbackTimer > 0.0f) {
        if (feedbackGrill)       UI::DrawFeedbackMessage(Loc::T("grill_upgraded"));
        if (feedbackCutting)     UI::DrawFeedbackMessage(Loc::T("cutting_upgraded"));
        if (feedbackAssembly)    UI::DrawFeedbackMessage(Loc::T("asm_upgraded"));
        if (feedbackChef)        UI::DrawFeedbackMessage(Loc::T("chef_upgraded"));
    }

    if (gameOverTimer > 0.0f) {
        UI::DrawGameOverFlash(gameOverTimer);
    }

    if (showRecipePanel) {
        UI::DrawRecipePanel();
    }

    UI::DrawControlsHint();
}

// ─── Particles ──────────────────────────────────────

void Game::SpawnParticles(float x, float y, Color color, int count) {
    for (int i = 0; i < count; i++) {
        float angle = (float)(rand() % 360) * 3.14159f / 180.0f;
        float speed = 50.0f + (rand() % 100);
        particles.push_back({x, y,
                             cosf(angle) * speed,
                             sinf(angle) * speed - 30.0f,
                             0.3f + (rand() % 30) / 100.0f, color});
    }
}

// ─── Upgrades ────────────────────────────────────────

void Game::ApplyUpgrades() {
    using S = StationType;
    stations[static_cast<int>(S::GRILL)].SetWorkTime(BASE_GRILL_TIME * upgradeSys.GetSpeedMultiplier(UpgradeType::GRILL_SPEED));
    stations[static_cast<int>(S::CUTTING)].SetWorkTime(BASE_CUTTING_TIME * upgradeSys.GetSpeedMultiplier(UpgradeType::CUTTING_SPEED));
    stations[static_cast<int>(S::ASSEMBLY)].SetWorkTime(BASE_ASSEMBLY_TIME * upgradeSys.GetSpeedMultiplier(UpgradeType::OVEN_SPEED));
    stations[static_cast<int>(S::SERVING)].SetWorkTime(BASE_SERVING_TIME);
    chef.SetSpeed(BASE_CHEF_SPEED * upgradeSys.GetSpeedMultiplier(UpgradeType::CHEF_SPEED));
}

void Game::HandleUpgradeInput() {
    struct Binding { int key; UpgradeType type; bool& flag; };
    Binding bindings[] = {
        {KEY_ONE,        UpgradeType::GRILL_SPEED,   feedbackGrill},
        {KEY_TWO,        UpgradeType::CUTTING_SPEED, feedbackCutting},
        {KEY_THREE,      UpgradeType::OVEN_SPEED,    feedbackAssembly},
        {KEY_UPGRADE_FOUR, UpgradeType::CHEF_SPEED,  feedbackChef},
    };
    for (auto& b : bindings) {
        if (Input::IsKeyPressed(b.key) && upgradeSys.Purchase(b.type, coins)) {
            ApplyUpgrades();
            b.flag = true;
            feedbackTimer = 1.5f;
            // SFX: play upgrade purchase sound
        }
    }
}
