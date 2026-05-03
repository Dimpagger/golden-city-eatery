#pragma once

// --- Station ---
enum class StationType { GRILL, CUTTING, ASSEMBLY, SERVING };
enum class StationState { IDLE, WORKING, DONE };

// Station class → see Station.h
// Food class → see Food.h
enum class FoodState { RAW, COOKED, SLICED, ASSEMBLED };

// Customer class → see Customer.h

// --- Upgrade ---
enum class UpgradeType { GRILL_SPEED, OVEN_SPEED, CHEF_SPEED };

struct Upgrade {
    UpgradeType type;
    int level = 0;
    int cost = 0;
};
