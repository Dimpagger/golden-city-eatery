#pragma once

#include "GameTypes.h"

class Food {
public:
    Food(FoodState initialState = FoodState::RAW);

    FoodState GetState() const;
    RecipeType GetRecipe() const;
    int GetReward() const;
    const char* GetRecipeName() const;
    void ProcessedAt(StationType station);

    bool IsHeld() const;
    void PickUp();
    void PutDown();

private:
    FoodState state;
    RecipeType recipe = RecipeType::NONE;
    bool held = false;
};
