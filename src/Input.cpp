#include "Input.h"

bool Input::IsKeyDown(int key) {
    return ::IsKeyDown(key);
}

bool Input::IsKeyPressed(int key) {
    return ::IsKeyPressed(key);
}

bool Input::IsKeyReleased(int key) {
    return ::IsKeyReleased(key);
}

bool Input::IsMouseButtonDown(int button) {
    return ::IsMouseButtonDown(button);
}

bool Input::IsMouseButtonPressed(int button) {
    return ::IsMouseButtonPressed(button);
}

raylib::Vector2 Input::GetMousePosition() {
    return ::GetMousePosition();
}

float Input::GetMouseWheelMove() {
    return ::GetMouseWheelMove();
}
