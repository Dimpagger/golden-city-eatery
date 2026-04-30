#include "raylib.h"
#include <stdio.h>
#include <unistd.h>

int main() {
    const int screenWidth = 1280;
    const int screenHeight = 720;

    printf("[INFO] Starting Golden City Eatery...\n");

    // 初始化窗口
    InitWindow(screenWidth, screenHeight, "Golden City Eatery");

    if (!IsWindowReady()) {
        printf("[ERROR] Window failed to initialize!\n");
        return 1;
    }

    printf("[INFO] Window initialized successfully\n");

    SetTargetFPS(60);

    // 尝试让窗口获得焦点
    SetWindowFocused();

    // 主循环
    while (!WindowShouldClose()) {
        // 手动轮询输入事件
        PollInputEvents();

        // 按 ESC 或 Q 退出
        if (IsKeyDown(KEY_ESCAPE) || IsKeyDown(KEY_Q)) {
            printf("[INFO] Exit key pressed\n");
            break;
        }

        // 如果窗口最小化，等待恢复
        if (IsWindowMinimized()) {
            printf("[INFO] Window minimized, waiting...\n");
            while (IsWindowMinimized()) {
                PollInputEvents();
                if (IsKeyDown(KEY_ESCAPE) || IsKeyDown(KEY_Q)) {
                    printf("[INFO] Exit while minimized\n");
                    goto exit_loop;
                }
                usleep(10000); // 10ms
            }
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawText("Golden City Eatery", 400, 300, 40, DARKGRAY);
        DrawText("Press ESC or Q to exit", 450, 360, 20, GRAY);

        // 显示窗口状态
        if (IsWindowFocused()) {
            DrawText("Window Focused: YES", 10, 50, 20, GREEN);
        } else {
            DrawText("Window Focused: NO", 10, 50, 20, RED);
        }

        EndDrawing();
    }

exit_loop:
    printf("[INFO] Cleaning up...\n");
    CloseWindow();
    printf("[INFO] Done!\n");

    return 0;
}
