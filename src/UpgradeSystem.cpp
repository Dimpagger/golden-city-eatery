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
        // +20% speed per level: 1.0 → 2.0
        return 1.0f + 0.2f * level;
    } else {
        // -20% time per level: 1.0 → 0.2
        float reduction = 0.2f * level;
        if (reduction > 0.8f) reduction = 0.8f;
        return 1.0f - reduction;
    }
}

void UpgradeSystem::Reset() {
    grillLevel = 0;
    ovenLevel = 0;
    chefSpeedLevel = 0;
}

int UpgradeSystem::CostForLevel(int level) {
    switch (level) {
        case 1:  return 30;
        case 2:  return 60;
        case 3:  return 120;
        case 4:  return 240;
        case 5:  return 480;
        default: return 999;
    }
}
