#ifndef CURSOR_HPP
#define CURSOR_HPP

#include "imgui/imgui.h"

/**
 * Cursor click position:
 *   - needed to save previous click position to draw line
 *   - unset (set to (-1, -1)) when mouse released in paint line mode
 * Accessible from `Toolbar`/`Menu` and `Canvas`
 */

// extern means it's only a declaration (avoids multiple definitions error if included in many files)
extern const ImVec2 VECTOR_UNSET;
extern ImVec2 cursor;

#endif // CURSOR_HPP
