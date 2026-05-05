#pragma once

#include <memory>
#include <raylib-cpp.hpp>

class Food;

class Chef {
public:
    Chef(float x, float y);

    void Update(float dt);

    float GetX() const { return x; }
    float GetY() const { return y; }
    float GetWidth() const { return w; }
    float GetHeight() const { return h; }
    raylib::Rectangle GetRect() const;

    bool IsHoldingFood() const;
    Food* GetHeldFood() const;
    void PickUpFood(std::unique_ptr<Food> food);
    std::unique_ptr<Food> DropFood();

    void SetSpeed(float s);
    float GetSpeed() const;
    void ResetPosition(float newX, float newY);
    bool IsFacingRight() const { return facingRight; }

private:
    float x, y;
    float w = 32.0f;
    float h = 32.0f;
    float speed = 300.0f;
    bool facingRight = true;
    std::unique_ptr<Food> heldFood;
};
