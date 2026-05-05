#pragma once

#include <string>
#include <vector>
#include "Constants.h"
#include "GameTypes.h"
#include "Station.h"
#include "Chef.h"
#include "CustomerManager.h"
#include "UpgradeSystem.h"

enum class GameState { MENU, PLAYING, DAY_COMPLETE, GAME_OVER, VICTORY };

class Game {
public:
    Game();
    ~Game();
    Game(Game&&) = default;
    Game& operator=(Game&&) = default;

    void Update(float dt);
    void Draw();

private:
    void Reset();
    void StartDay(int day);
    void ApplyUpgrades();
    void HandleUpgradeInput();
    void UpdatePlaying(float dt);
    void DrawPlaying();

    Station* GetNearbyStation();

    void LoadHighScore();
    void SaveHighScore();

    static float GetDayMultiplier(int day);

    GameState state = GameState::MENU;
    Chef chef;

    Station stations[STATION_COUNT];
    CustomerManager customerMgr;

    int coins = 0;
    int score = 0;
    int bestScore = 0;
    int streak = 0;
    int currentDay = 1;
    int servedToday = 0;

    bool paused = false;

    Station* nearStation = nullptr;   // cached per-frame for Update+Drawing

    UpgradeSystem upgradeSys;

    float feedbackTimer = 0.0f;
    bool feedbackGrill = false;
    bool feedbackCutting = false;
    bool feedbackAssembly = false;
    bool feedbackChef = false;
    bool showRecipePanel = false;
    bool showHelpPanel = false;

    float shakeTimer = 0.0f;
    float shakeIntensity = 0.0f;
    bool endlessMode = false;

    bool discoveredLiangpi = false;
    bool discoveredKebab = false;

    int totalServed = 0;
    int totalCoins = 0;
    int totalRoujiamo = 0;
    int totalLiangpi = 0;
    int totalKebab = 0;

    struct Particle {
        float x, y, vx, vy, life = 0.6f;
        Color color;
    };
    std::vector<Particle> particles;
    void SpawnParticles(float x, float y, Color color, int count);

    float gameOverTimer = 0.0f;

    float milestoneTimer = 0.0f;
    int milestoneStreak = 0;

    int lastLostCount = 0;

    struct FloatText {
        float x, y;
        float timer = 1.0f;
        int value = 10;
    };
    std::vector<FloatText> floatTexts;

    static const std::string HIGH_SCORE_FILE;
};
