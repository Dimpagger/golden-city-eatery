#pragma once

#include "GameTypes.h"

class UpgradeSystem {
public:
    UpgradeSystem();

    int GetLevel(UpgradeType type) const;
    int GetCost(UpgradeType type) const;       // cost to buy NEXT level
    bool IsMaxed(UpgradeType type) const;
    bool Purchase(UpgradeType type, int& coins);   // deducts coins, returns success

    float GetSpeedMultiplier(UpgradeType type) const;  // 1.0 → faster

    void Reset();

private:
    bool CanPurchase(UpgradeType type, int coins) const;

    int grillLevel = 0;
    int cuttingLevel = 0;
    int ovenLevel = 0;
    int chefSpeedLevel = 0;
    static constexpr int MAX_LEVEL = 4;

    static int CostForLevel(int level);
};
