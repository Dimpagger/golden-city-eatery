#pragma once

class Input {
public:
    static bool IsKeyDown(int key);
    static bool IsKeyPressed(int key);
    static bool IsMousePressed(int button = 0);
    static int GetMouseX();
    static int GetMouseY();
    static bool IsMouseInRect(float x, float y, float w, float h);
};
