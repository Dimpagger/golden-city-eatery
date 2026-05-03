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
    switch (type) {
        case StationType::GRILL:    return f.GetState() == FoodState::RAW;
        case StationType::CUTTING:  return f.GetState() == FoodState::COOKED;
        case StationType::ASSEMBLY: return f.GetState() == FoodState::SLICED;
        case StationType::SERVING:  return f.GetState() == FoodState::ASSEMBLED;
    }
    return false;
}

void Station::PlaceFood(Food* f) {
    food = f;
    food->PutDown();
    state = StationState::IDLE;
    progress = 0.0f;
}

Food* Station::TakeFood() {
    Food* f = food;
    food = nullptr;
    state = StationState::IDLE;
    progress = 0.0f;
    if (f) f->PickUp();
    return f;
}

Food* Station::GetFood() const { return food; }

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
            food->Advance();
        }
    }
}

bool Station::IsDone() const { return state == StationState::DONE; }
