#pragma once

#include "Constants.h"
#include "GameTypes.h"

class Customer {
public:
    enum class State { ENTERING, WAITING, LEAVING };

    Customer(float targetX, float targetY, CustomerType type = CustomerType::NORMAL,
             float patience = CUSTOMER_PATIENCE);

    void Update(float dt);

    void Serve();
    bool HasLeft() const;
    bool IsWaiting() const;
    bool WasServed() const;

    State GetState() const;
    CustomerType GetType() const;
    float GetRewardMultiplier() const;
    float GetX() const;
    float GetY() const;
    float GetPatience() const;

    void MoveTo(float newTargetX, float newTargetY);

private:
    float x, y;
    float targetX, targetY;
    CustomerType type = CustomerType::NORMAL;
    float patience = 30.0f;
    float moveSpeed = 150.0f;
    State state = State::ENTERING;
    bool served = false;
};
