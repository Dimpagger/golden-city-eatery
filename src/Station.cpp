#include "Station.h"
#include "Food.h"

Station::Station() : type(StationType::GRILL), x(0), y(0) {}

Station::Station(StationType t, float xPos, float yPos)
    : type(t), x(xPos), y(yPos) {}

StationType Station::GetType() const { return type; }
StationState Station::GetState() const { return state; }
float Station::GetX() const { return x; }
float Station::GetY() const { return y; }
float Station::GetWidth() const { return w; }
float Station::GetHeight() const { return h; }
float Station::GetProgress() const { return progress; }
float Station::GetWorkTime() const { return workTime; }

void Station::SetWorkTime(float time) { workTime = time; }

bool Station::HasFood() const { return food != nullptr; }

bool Station::CanAccept(const Food& f) const {
    if (HasFood()) return false;
    FoodState fs = f.GetState();
    switch (type) {
        case StationType::GRILL:    return fs == FoodState::RAW;
        case StationType::CUTTING:  return fs == FoodState::RAW || fs == FoodState::COOKED;
        case StationType::ASSEMBLY: return fs == FoodState::SLICED || fs == FoodState::COOKED;
        case StationType::SERVING:  return fs == FoodState::ASSEMBLED;
    }
    return false;
}

void Station::PlaceFood(std::unique_ptr<Food> f) {
    food = std::move(f);
    food->PutDown();
    state = StationState::IDLE;
    progress = 0.0f;
}

std::unique_ptr<Food> Station::TakeFood() {
    if (food) food->PickUp();
    state = StationState::IDLE;
    progress = 0.0f;
    return std::move(food);
}

Food* Station::GetFood() { return food.get(); }
const Food* Station::GetFood() const { return food.get(); }

void Station::StartWork() {
    if (HasFood() && state == StationState::IDLE) {
        state = StationState::WORKING;
        progress = 0.0f;
    }
}

void Station::Update(float dt) {
    if (state != StationState::WORKING) return;

    progress += (100.0f / workTime) * dt;
    if (progress >= 100.0f) {
        progress = 100.0f;
        state = StationState::DONE;
        if (food) {
            food->ProcessedAt(type);
        }
    }
}

bool Station::IsDone() const { return state == StationState::DONE; }
