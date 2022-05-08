#ifndef MODE_GLOBAL_HPP
#define MODE_GLOBAL_HPP

#include "ui/enumerations/mode.hpp"

/**
 * extern means it's only a declaration (avoids multiple definitions error if included in many files)
 * `ifndef` only relevant within the same translation unit (i.e. same cpp file): https://stackoverflow.com/a/61880706/2228912
 *
 * Rendering to different textures accord. to mode (normal or drawing with nanoVG)
 */
extern Mode mode;

#endif // MODE_GLOBAL_HPP
