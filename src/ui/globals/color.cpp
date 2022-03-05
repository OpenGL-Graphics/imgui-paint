#include "ui/globals/color.hpp"

/* Static member requires a separate definition (avoids linking errors) */
ImVec4 Color::stroke = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
ImVec4 Color::fill = ImVec4(1.0f, 1.0f, 1.0f, 0.0f);
