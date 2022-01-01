#include "ui/constants/mouse.hpp"

/* Static member requires a separate definition (avoids linking errors) */
HoverMode Mouse::hover_mode = HoverMode::NONE;
ClickMode Mouse::click_mode = ClickMode::NONE;
