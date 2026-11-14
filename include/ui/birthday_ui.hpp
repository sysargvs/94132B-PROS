#pragma once

namespace birthday_ui {
// Boot-only: call once as the FIRST statement in initialize(), before any delay,
// calibration, LLEMU initialization, or user-created task. Never call in a loop.
// Matched to 94132B-PROS's bundled PROS 4.2.2 / LVGL 9.2.0 libraries.
void show_on_boot();
}
