#include "UpgradeSystem.h"

UpgradeSystem::UpgradeSystem() = default;

int UpgradeSystem::GetLevel(UpgradeType type) const {
    switch (type) {
        case UpgradeType::GRILL_SPEED: return grillLevel;
        case UpgradeType::OVEN_SPEED:  return ovenLevel;
        case UpgradeType::CHEF_SPEED:  return chefSpeedLevel;
    }
    return 0;
}

int UpgradeSystem::GetCost(UpgradeType type) const {
    switch (type) {
        case UpgradeType::GRILL_SPEED: return CostForLevel(grillLevel + 1);
        case UpgradeType::OVEN_SPEED:  return CostForLevel(ovenLevel + 1);
        case UpgradeType::CHEF_SPEED:  return CostForLevel(chefSpeedLevel + 1);
    }
    return 0;
}

bool UpgradeSystem::CanPurchase(UpgradeType type, int coins) const {
    if (IsMaxed(type)) return false;
    return coins >= GetCost(type);
}

bool UpgradeSystem::IsMaxed(UpgradeType type) const {
    return GetLevel(type) >= MAX_LEVEL;
}

bool UpgradeSystem::Purchase(UpgradeType type, int& coins) {
    if (!CanPurchase(type, coins)) return false;

    int cost = GetCost(type);
    coins -= cost;

    switch (type) {
        case UpgradeType::GRILL_SPEED: grillLevel++;     break;
        case UpgradeType::OVEN_SPEED:  ovenLevel++;      break;
        case UpgradeType::CHEF_SPEED:  chefSpeedLevel++; break;
    }
    return true;
}

float UpgradeSystem::GetSpeedMultiplier(UpgradeType type) const {
    int level = GetLevel(type);
    if (type == UpgradeType::CHEF_SPEED) {
        // +20% speed per level: 1.0 → 1.8
        return 1.0f + 0.2f * level;
    } else {
        // -20% time per level: 1.0 → 0.2 (level 4)
        return 1.0f - 0.2f * level;
    }
}

void UpgradeSystem::Reset() {
    grillLevel = 0;
    ovenLevel = 0;
    chefSpeedLevel = 0;
}

int UpgradeSystem::CostForLevel(int level) {
    return 30 * (1 << (level - 1));
}
