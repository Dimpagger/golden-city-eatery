#include <raylib-cpp.hpp>
#include <cstdlib>
#include <ctime>
#include "Constants.h"
#include "Game.h"

int main() {
    srand((unsigned int)time(nullptr));

    raylib::Window w(SCREEN_WIDTH, SCREEN_HEIGHT, "Roujiamo Legend");
    SetTargetFPS(60);

    Game game;

    while (!w.ShouldClose()) {
        game.Update(GetFrameTime());
        game.Draw();
    }

    return 0;
}
