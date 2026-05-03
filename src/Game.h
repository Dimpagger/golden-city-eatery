#pragma once

#include <vector>
#include "Constants.h"
#include "GameTypes.h"
#include "Station.h"
#include "Chef.h"
#include "Customer.h"
#include "UpgradeSystem.h"

enum class GameState { MENU, PLAYING, GAME_OVER };

class Game {
public:
    Game();
    ~Game();

    void Update(float dt);
    void Draw();

    GameState GetState() const;

private:
    void Reset();
    void ApplyUpgrades();
    void HandleUpgradeInput();
    void UpdatePlaying(float dt);
    void DrawPlaying();

    Station* GetNearbyStation();

    GameState state = GameState::MENU;
    Chef chef;

    Station stations[STATION_COUNT];

    std::vector<Customer*> customers;
    float spawnTimer = 0.0f;
    float spawnInterval = 0.0f;

    int coins = 0;
    int score = 0;
    int lostCustomers = 0;

    bool paused = false;

    UpgradeSystem upgradeSys;

    float feedbackTimer = 0.0f;
    bool feedbackGrill = false;
    bool feedbackAssembly = false;
    bool feedbackChef = false;

    float gameOverTimer = 0.0f;

    struct FloatText {
        float x, y;
        float timer = 1.0f;
    };
    std::vector<FloatText> floatTexts;
};
