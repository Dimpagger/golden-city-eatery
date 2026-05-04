#pragma once

#include <vector>

enum class Language { CHINESE, JAPANESE, ENGLISH };

namespace Loc {

void SetLanguage(Language lang);
Language GetLanguage();
void ToggleLanguage();

// Returns localized string for the given key
const char* T(const char* key);

// Collects all unique codepoints needed to render all translations
std::vector<int> GetRequiredCodepoints();

} // namespace Loc
