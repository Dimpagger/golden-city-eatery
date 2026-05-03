#pragma once

#include <raylib-cpp.hpp>

class Input {
public:
    static bool IsKeyDown(int key);
    static bool IsKeyPressed(int key);
    static bool IsKeyReleased(int key);
    static bool IsMouseButtonDown(int button);
    static bool IsMouseButtonPressed(int button);
    static raylib::Vector2 GetMousePosition();
    static float GetMouseWheelMove();
};
