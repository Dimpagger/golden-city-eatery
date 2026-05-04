#include <raylib-cpp.hpp>
#include "Customer.h"
#include "Constants.h"
#include <cmath>

Customer::Customer(float tx, float ty, CustomerType t, float p)
    : x(SCREEN_WIDTH + 20.0f)
    , y(ty)
    , targetX(tx)
    , targetY(ty)
    , type(t)
    , patience(p) {}

void Customer::Update(float dt) {
    switch (state) {
        case State::ENTERING: {
            float dx = targetX - x;
            float dy = targetY - y;
            float dist = std::sqrt(dx * dx + dy * dy);
            if (dist < 2.0f) {
                x = targetX;
                y = targetY;
                state = State::WAITING;
            } else {
                x += (dx / dist) * moveSpeed * dt;
                y += (dy / dist) * moveSpeed * dt;
            }
            break;
        }
        case State::WAITING: {
            patience -= dt;
            if (patience <= 0.0f) {
                patience = 0.0f;
                state = State::LEAVING;
            }
            break;
        }
        case State::LEAVING: {
            x += moveSpeed * dt;
            break;
        }
    }
}

void Customer::Serve() {
    if (state == State::WAITING) {
        served = true;
        state = State::LEAVING;
    }
}

bool Customer::HasLeft() const {
    return state == State::LEAVING && x > SCREEN_WIDTH + 40.0f;
}

bool Customer::IsWaiting() const { return state == State::WAITING; }
bool Customer::WasServed() const { return served; }

Customer::State Customer::GetState() const { return state; }
CustomerType Customer::GetType() const { return type; }

float Customer::GetRewardMultiplier() const {
    switch (type) {
        case CustomerType::NORMAL:   return REWARD_MULT_NORMAL;
        case CustomerType::IMPATIENT: return REWARD_MULT_IMPATIENT;
        case CustomerType::VIP:      return REWARD_MULT_VIP;
    }
    return REWARD_MULT_NORMAL;
}

float Customer::GetX() const { return x; }
float Customer::GetY() const { return y; }
float Customer::GetPatience() const { return patience; }

void Customer::MoveTo(float newX, float newY) {
    targetX = newX;
    targetY = newY;
    if (state == State::WAITING) {
        state = State::ENTERING;
    }
}
