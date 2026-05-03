#include <cstdlib>
#include <cmath>
#include "Game.h"
#include "Input.h"
#include "Food.h"
#include "Constants.h"
#include "UI.h"

Game::Game(): chef(CHEF_X_INIT, CHEF_Y) {
    Reset();
}

Game::~Game() {
    for (auto* c : customers) delete c;
    customers.clear();  // clear pointers that are now dangling
}

void Game::Reset() {
    // Clean up old food from previous game
    for (int i = 0; i < STATION_COUNT; i++) {
        if (stations[i].HasFood()) delete stations[i].GetFood();
    }
    if (chef.IsHoldingFood()) delete chef.GetHeldFood();

    // Chef
    chef = Chef(CHEF_X_INIT, CHEF_Y);

    // Stations
    for (int i = 0; i < STATION_COUNT; i++) {
        stations[i] = Station(static_cast<StationType>(i), STATION_X[i], STATION_Y);
    }

    stations[0].PlaceFood(new Food(FoodState::RAW));

    // Customers
    for (auto* c : customers) delete c;
    customers.clear();

    // Stats
    coins = 0;
    score = 0;
    lostCustomers = 0;
    paused = false;

    feedbackGrill = false;
    feedbackAssembly = false;
    feedbackChef = false;
    feedbackTimer = 0.0f;
    gameOverTimer = 0.0f;
    floatTexts.clear();
    upgradeSys.Reset();
    ApplyUpgrades();

    spawnTimer = 0.0f;
    spawnInterval = SPAWN_INTERVAL_MIN + (rand() % (int)(SPAWN_INTERVAL_MAX - SPAWN_INTERVAL_MIN + 1));
}

void Game::Update(float dt) {
    switch (state) {
        case GameState::MENU:
            if (Input::IsKeyPressed(KEY_INTERACT)) {
                Reset();
                state = GameState::PLAYING;
            }
            break;

        case GameState::PLAYING:
            if (Input::IsKeyPressed(KEY_P)) {
                paused = !paused;
            }
            if (gameOverTimer > 0.0f) {
                // Freeze game during flash, only count down
                gameOverTimer -= dt;
                if (gameOverTimer <= 0.0f) {
                    state = GameState::GAME_OVER;
                    paused = false;
                }
            } else if (!paused) {
                UpdatePlaying(dt);
                if (lostCustomers >= MAX_LOST) {
                    gameOverTimer = 1.0f;   // start red flash transition
                }
            }
            break;

        case GameState::GAME_OVER:
            if (Input::IsKeyPressed(KEY_INTERACT)) {
                Reset();
                state = GameState::PLAYING;
            }
            break;
    }
}

void Game::UpdatePlaying(float dt) {
    // --- Chef ---
    chef.Update(dt);

    // --- Stations ---
    for (int i = 0; i < STATION_COUNT; i++) {
        stations[i].Update(dt);
    }

    // --- Float texts ---
    for (auto it = floatTexts.begin(); it != floatTexts.end(); ) {
        it->y -= 40.0f * dt;   // float upward
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
        delete stations[3].TakeFood();
        coins += BASE_REWARD;
        score++;

        // Floating "+10" text
        floatTexts.push_back({stations[3].GetX() + stations[3].GetWidth() / 2.0f,
                              stations[3].GetY(), 1.0f});

        for (auto* c : customers) {
            if (c->IsWaiting()) {
                c->Serve();
                break;
            }
        }

        if (!stations[0].HasFood()) {
            stations[0].PlaceFood(new Food(FoodState::RAW));
        }
    }

    // --- Customer spawn ---
    spawnTimer += dt;
    if (spawnTimer >= spawnInterval && (int)customers.size() < MAX_QUEUE) {
        int slot = (int)customers.size();
        customers.push_back(new Customer(QUEUE_X[slot], QUEUE_Y));
        spawnTimer = 0.0f;
        spawnInterval = SPAWN_INTERVAL_MIN + (rand() % (int)(SPAWN_INTERVAL_MAX - SPAWN_INTERVAL_MIN + 1));
    }

    // --- Update customers ---
    for (auto* c : customers) {
        c->Update(dt);
    }

    // --- Remove left customers ---
    for (auto it = customers.begin(); it != customers.end(); ) {
        if ((*it)->HasLeft()) {
            if (!(*it)->WasServed()) {
                lostCustomers++;
            }
            delete *it;
            it = customers.erase(it);
            for (int i = 0; i < (int)customers.size(); i++) {
                customers[i]->MoveTo(QUEUE_X[i], QUEUE_Y);
            }
        } else {
            ++it;
        }
    }
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
        case GameState::GAME_OVER:
            UI::DrawGameOverScreen(score, coins, lostCustomers);
            break;
    }

    EndDrawing();
}

void Game::DrawPlaying() {
    const char* label[] = {"Grill", "Cutting", "Assembly", "Serving"};
    const Color  color[] = {RED, GRAY, {210, 160, 0, 255}, GREEN};  // dark gold for Assembly

    UI::DrawTopBar(coins, score, (int)customers.size(), lostCustomers, MAX_LOST);

    for (int i = 0; i < STATION_COUNT; i++) {
        UI::DrawStation(stations[i], label[i], color[i], stations[i].GetWorkTime());
        UI::DrawCountdown(stations[i]);
    }

    for (auto* c : customers) {
        UI::DrawCustomer(*c);
    }

    UI::DrawChef(chef);

    Station* near = GetNearbyStation();
    if (near) {
        UI::DrawInteractionHint(*near);
    }

    UI::DrawUpgradePanel(upgradeSys, coins);

    for (auto& ft : floatTexts) {
        UI::DrawFloatText(ft.x, ft.y, ft.timer);
    }

    if (feedbackTimer > 0.0f) {
        if (feedbackGrill)       UI::DrawFeedbackMessage("Grill Upgraded!");
        if (feedbackAssembly)    UI::DrawFeedbackMessage("Assembly Upgraded!");
        if (feedbackChef)        UI::DrawFeedbackMessage("Chef Speed Upgraded!");
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

