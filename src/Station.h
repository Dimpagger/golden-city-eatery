#pragma once

#include <memory>
#include "GameTypes.h"

class Food;

class Station {
public:
    Station();  // default, required for array
    Station(StationType type, float x, float y);

    StationType GetType() const;
    StationState GetState() const;
    float GetX() const;
    float GetY() const;
    float GetWidth() const;
    float GetHeight() const;
    float GetProgress() const;
    float GetWorkTime() const;
    void SetWorkTime(float time);

    bool HasFood() const;
    bool CanAccept(const Food& food) const;  // accepts the right state for this station

    void PlaceFood(std::unique_ptr<Food> food);
    std::unique_ptr<Food> TakeFood();
    Food* GetFood();
    const Food* GetFood() const;

    void StartWork();
    void Update(float dt);   // advance progress while WORKING
    bool IsDone() const;

private:
    StationType type;
    StationState state = StationState::IDLE;
    float x, y;
    float w = 100.0f, h = 80.0f;
    float progress = 0.0f;
    float workTime = 3.0f;   // base seconds to complete
    std::unique_ptr<Food> food;
};
