#include <raylib-cpp.hpp>
#include "Input.h"

bool Input::IsKeyDown(int key) {
    return ::IsKeyDown(key);
}

bool Input::IsKeyPressed(int key) {
    return ::IsKeyPressed(key);
}

bool Input::IsMousePressed(int button) {
    return ::IsMouseButtonPressed(button);
}

int Input::GetMouseX() {
    return ::GetMouseX();
}

int Input::GetMouseY() {
    return ::GetMouseY();
}

bool Input::IsMouseInRect(float x, float y, float w, float h) {
    int mx = GetMouseX(), my = GetMouseY();
    return mx >= (int)x && mx <= (int)(x + w) && my >= (int)y && my <= (int)(y + h);
}
