#include "ui/birthday_ui.hpp"

#include "liblvgl/lvgl.h"
#include "pros/rtos.h"
#include <cstdio>

static_assert(LVGL_VERSION_MAJOR == 9 && LVGL_VERSION_MINOR == 2 &&
              LVGL_VERSION_PATCH == 0, "Recheck birthday UI boot scheduling after an LVGL upgrade.");
static_assert(LV_USE_OS == LV_OS_NONE, "Recheck birthday UI boot scheduling after enabling LVGL OS support.");

namespace {
constexpr const char* kImagePath = "S:birthday/top_cw.png";
constexpr unsigned kDisplayPriority = TASK_PRIORITY_MIN + 2;

// Static storage is essential: LVGL keeps pointers to both the descriptor and
// its decoded pixels. Keep one PNG decoding session alive for the program's
// lifetime instead of re-reading or decoding the SD file on future redraws.
lv_image_decoder_dsc_t decoder{};
lv_image_dsc_t resident_image{};
bool requested = false;

void show_image(void*) {
  // This callback runs inside PROS's existing LVGL display task. All image
  // decoding and widget changes happen here; no second display task is created.
  lv_display_t* display = lv_display_get_default();
  if (display == nullptr ||
      lv_display_get_horizontal_resolution(display) != 480 ||
      lv_display_get_vertical_resolution(display) != 240 ||
      lv_display_get_rotation(display) != LV_DISPLAY_ROTATION_0) {
    std::puts("Birthday UI: expected the native 480x240 display, rotation 0.");
    return;
  }

  lv_image_header_t header{};
  if (lv_image_decoder_get_info(kImagePath, &header) != LV_RESULT_OK ||
      header.w != 480 || header.h != 240) {
    std::puts("Birthday UI: missing/invalid birthday/top_cw.png (need 480x240).");
    return;
  }

  lv_image_decoder_args_t args{};
  args.no_cache = true;  // This session owns the pixels, independently of cache eviction.
  if (lv_image_decoder_open(&decoder, kImagePath, &args) != LV_RESULT_OK) {
    std::puts("Birthday UI: PNG decoding failed.");
    return;
  }
  if (decoder.decoded == nullptr || decoder.decoded->header.w != 480 ||
      decoder.decoded->header.h != 240) {
    lv_image_decoder_close(&decoder);
    std::puts("Birthday UI: PNG did not produce a complete 480x240 image.");
    return;
  }

  lv_draw_buf_to_image(decoder.decoded, &resident_image);
  // The PNG session owns this allocation. The image widget receives a read-only view.
  resident_image.header.flags &= ~(LV_IMAGE_FLAGS_ALLOCATED | LV_IMAGE_FLAGS_MODIFIABLE);

  lv_obj_t* image = lv_image_create(lv_layer_top());
  if (image == nullptr) {
    lv_image_decoder_close(&decoder);
    std::puts("Birthday UI: insufficient memory for image widget.");
    return;
  }
  lv_obj_remove_style_all(image);
  lv_image_set_src(image, &resident_image);
  lv_obj_set_pos(image, 0, 0);
  lv_obj_remove_flag(image, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_remove_flag(image, LV_OBJ_FLAG_SCROLLABLE);
  // The asset is already rotated clockwise for a Brain with its power button
  // at the TOP. Do not add software rotation, scaling, labels, or touch actions.
}
}  // namespace

namespace birthday_ui {
void show_on_boot() {
  if (requested) return;
  requested = true;

  // This exact bundled library has LV_USE_OS=NONE: lv_lock() is a no-op.
  // Bootstrap only from initialize() (priority 8), above its display task
  // (priority 3). Wait until a previously preempted LVGL handler has finished.
  // Once idle, the lower-priority display task cannot preempt us while
  // lv_async_call allocates its timer from LVGL's non-blocking internal heap.
  // The private timer-state field is pinned to the bundled LVGL 9.2.0 headers.
  if (pros::c::task_get_priority(nullptr) <= kDisplayPriority) {
    std::puts("Birthday UI: call show_on_boot first in initialize().");
    return;
  }
  const auto started = pros::c::millis();
  while (LV_GLOBAL_DEFAULT()->timer_state.already_running) {
    if (pros::c::millis() - started >= 1000) {
      std::puts("Birthday UI: display busy; skipping artwork.");
      return;
    }
    pros::c::task_delay(5);
  }
  if (lv_async_call(show_image, nullptr) != LV_RESULT_OK) {
    std::puts("Birthday UI: could not queue artwork.");
  }
  // No fallback UI, polling task, control-loop hook, or permanent retry loop.
  // Errors go to the PROS terminal once; robot initialization continues.
}
}  // namespace birthday_ui
