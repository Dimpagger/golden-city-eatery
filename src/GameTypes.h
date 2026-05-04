#pragma once

// --- Station ---
enum class StationType { GRILL, CUTTING, ASSEMBLY, SERVING };
enum class StationState { IDLE, WORKING, DONE };

// Station class → see Station.h
// Food class → see Food.h
enum class FoodState { RAW, COOKED, SLICED, ASSEMBLED };
enum class RecipeType { NONE, ROUJIAMO, LIANGPI, KEBAB };

// Customer class → see Customer.h
enum class CustomerType { NORMAL, IMPATIENT, VIP };

// --- Upgrade ---
enum class UpgradeType { GRILL_SPEED, OVEN_SPEED, CHEF_SPEED };
