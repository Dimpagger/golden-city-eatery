#include "Chef.h"
#include "Food.h"
#include "Input.h"
#include "Constants.h"

Chef::Chef(float startX, float startY) : x(startX), y(startY) {}

void Chef::Update(float dt) {
    if (Input::IsKeyDown(KEY_MOVE_LEFT)) {
        x -= speed * dt;
        facingRight = false;
    }
    if (Input::IsKeyDown(KEY_MOVE_RIGHT)) {
        x += speed * dt;
        facingRight = true;
    }

    if (x < 0) x = 0;
    if (x > SCREEN_WIDTH - w) x = SCREEN_WIDTH - w;
}

raylib::Rectangle Chef::GetRect() const {
    return {x, y, w, h};
}

bool Chef::IsHoldingFood() const { return heldFood != nullptr; }

Food* Chef::GetHeldFood() const { return heldFood.get(); }

void Chef::PickUpFood(std::unique_ptr<Food> food) {
    heldFood = std::move(food);
    if (heldFood) heldFood->PickUp();
}

std::unique_ptr<Food> Chef::DropFood() {
    if (heldFood) heldFood->PutDown();
    return std::move(heldFood);
}

void Chef::SetSpeed(float s) { speed = s; }
float Chef::GetSpeed() const { return speed; }

void Chef::ResetPosition(float newX, float newY) {
    x = newX;
    y = newY;
    heldFood.reset();
    speed = BASE_CHEF_SPEED;
}
