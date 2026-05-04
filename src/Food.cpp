#include "Food.h"
#include "Constants.h"
#include "Localization.h"

Food::Food(FoodState initialState) : state(initialState) {}

FoodState Food::GetState() const { return state; }
RecipeType Food::GetRecipe() const { return recipe; }

int Food::GetReward() const {
    switch (recipe) {
        case RecipeType::ROUJIAMO: return REWARD_ROUJIAMO;
        case RecipeType::LIANGPI:  return REWARD_LIANGPI;
        case RecipeType::KEBAB:    return REWARD_KEBAB;
        default:                   return 0;
    }
}

const char* Food::GetRecipeName() const {
    switch (recipe) {
        case RecipeType::ROUJIAMO: return Loc::T("roujiamo");
        case RecipeType::LIANGPI:  return Loc::T("liangpi");
        case RecipeType::KEBAB:    return Loc::T("kebab");
        default:                   return "";
    }
}

void Food::ProcessedAt(StationType station) {
    FoodState prev = state;
    switch (station) {
        case StationType::GRILL:
            state = FoodState::COOKED;
            break;
        case StationType::CUTTING:
            state = FoodState::SLICED;
            if (recipe == RecipeType::NONE) {
                recipe = (prev == FoodState::RAW) ? RecipeType::LIANGPI
                                                  : RecipeType::ROUJIAMO;
            }
            break;
        case StationType::ASSEMBLY:
            state = FoodState::ASSEMBLED;
            if (recipe == RecipeType::NONE && prev == FoodState::COOKED) {
                recipe = RecipeType::KEBAB;
            }
            break;
        case StationType::SERVING:
            break;
    }
}

bool Food::IsHeld() const { return held; }
void Food::PickUp() { held = true; }
void Food::PutDown() { held = false; }
