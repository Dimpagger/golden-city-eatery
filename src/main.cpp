#include <raylib-cpp.hpp>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include "Constants.h"
#include "Localization.h"
#include "UI.h"
#include "Game.h"

#if __has_include("FontData.h")
#include "FontData.h"
#define HAS_EMBEDDED_FONT 1
#else
#define HAS_EMBEDDED_FONT 0
#endif

int main() {
    srand((unsigned int)time(nullptr));

    raylib::Window w(SCREEN_WIDTH, SCREEN_HEIGHT, "Roujiamo Legend");

    if (!IsWindowReady()) {
        std::cerr << "Failed to initialize raylib window." << std::endl;
        return 1;
    }

    SetTargetFPS(60);

    Font font;
    auto codepoints = Loc::GetRequiredCodepoints();

#if HAS_EMBEDDED_FONT
    font = LoadFontFromMemory(".ttf", FontData_ttf, FontData_ttf_len,
                              40, codepoints.data(), (int)codepoints.size());
    if (font.glyphCount <= 95) {
        font = GetFontDefault();
    }
#else
    font = GetFontDefault();
    // Try common CJK font paths per platform
    const char* fontPaths[] = {
        "/Library/Fonts/Arial Unicode.ttf",         // macOS
        "/System/Library/Fonts/PingFang.ttc",       // macOS
        "/usr/share/fonts/truetype/noto/NotoSansCJK-Regular.ttc",  // Linux
        "/usr/share/fonts/opentype/noto/NotoSansCJK-Regular.ttc",  // Linux
        "C:\\Windows\\Fonts\\msyh.ttc",             // Windows
    };
    for (const char* fp : fontPaths) {
        if (FileExists(fp)) {
            Font loaded = LoadFontEx(fp, 40, codepoints.data(), (int)codepoints.size());
            if (loaded.glyphCount > 95) {
                font = loaded;
                break;
            }
        }
    }
#endif

    UI::SetFont(font);

    try {
        Game game;

        while (!w.ShouldClose()) {
            game.Update(GetFrameTime());
            game.Draw();
        }
    } catch (const std::exception& e) {
        std::cerr << "Unexpected error: " << e.what() << std::endl;
        return 1;
    }

    if (font.texture.id != GetFontDefault().texture.id) UnloadFont(font);
    return 0;
}
