#include "Chef.h"
#include "Food.h"
#include "Input.h"
#include "Constants.h"

Chef::Chef(float startX, float startY) : x(startX), y(startY) {}

void Chef::Update(float dt) {
    if (Input::IsKeyDown(KEY_MOVE_LEFT)) {
        x -= speed * dt;
    }
    if (Input::IsKeyDown(KEY_MOVE_RIGHT)) {
        x += speed * dt;
    }

    if (x < 0) x = 0;
    if (x > SCREEN_WIDTH - w) x = SCREEN_WIDTH - w;
}

void Chef::Draw() const {
    DrawRectangle((int)x, (int)y, (int)w, (int)h, BLUE);
}

raylib::Rectangle Chef::GetRect() const {
    return {x, y, w, h};
}

bool Chef::IsHoldingFood() const { return heldFood != nullptr; }

Food* Chef::GetHeldFood() const { return heldFood; }

void Chef::PickUpFood(Food* food) {
    heldFood = food;
    if (heldFood) heldFood->PickUp();
}

Food* Chef::DropFood() {
    Food* f = heldFood;
    heldFood = nullptr;
    if (f) f->PutDown();
    return f;
}

void Chef::SetSpeed(float s) { speed = s; }
float Chef::GetSpeed() const { return speed; }
