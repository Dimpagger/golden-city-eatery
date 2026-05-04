#include <raylib-cpp.hpp>
#include "Input.h"

bool Input::IsKeyDown(int key) {
    return ::IsKeyDown(key);
}

bool Input::IsKeyPressed(int key) {
    return ::IsKeyPressed(key);
}
