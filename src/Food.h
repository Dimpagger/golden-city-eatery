#pragma once

#include "GameTypes.h"

class Food {
public:
    Food(FoodState initialState = FoodState::RAW);

    FoodState GetState() const;
    void Advance();            // RAW → COOKED → SLICED → ASSEMBLED
    bool IsAssembled() const;

    bool IsHeld() const;
    void PickUp();
    void PutDown();

private:
    FoodState state;
    bool held = false;
};
