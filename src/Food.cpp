#include "Food.h"

Food::Food(FoodState initialState) : state(initialState) {}

FoodState Food::GetState() const { return state; }

void Food::Advance() {
    switch (state) {
        case FoodState::RAW:       state = FoodState::COOKED;   break;
        case FoodState::COOKED:    state = FoodState::SLICED;   break;
        case FoodState::SLICED:    state = FoodState::ASSEMBLED; break;
        case FoodState::ASSEMBLED: break; // already final
    }
}

bool Food::IsAssembled() const { return state == FoodState::ASSEMBLED; }
bool Food::IsHeld() const { return held; }
void Food::PickUp() { held = true; }
void Food::PutDown() { held = false; }
