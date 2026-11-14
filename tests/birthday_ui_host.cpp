#include "liblvgl/lvgl.h"
#include "pros/rtos.h"
#include "ui/birthday_ui.hpp"
#include <cassert>
#include <cstring>
#include <string>

lv_global_t lv_global{};
namespace {
std::string scenario;
unsigned now_ms = 0, queued = 0, opened = 0, closed = 0, created = 0, sources = 0;
lv_async_cb_t callback = nullptr;
lv_display_t display{};
lv_obj_t object{};
lv_draw_buf_t pixels{};
const void* retained_source = nullptr;
}
namespace pros { namespace c {
uint32_t task_get_priority(task_t) { return scenario == "priority" ? 3 : 8; }
uint32_t millis(void) { return now_ms; }
void task_delay(uint32_t ms) { now_ms += ms; }
}}
lv_result_t lv_async_call(lv_async_cb_t fn, void*) {
  ++queued;
  if(scenario == "queue") return LV_RESULT_INVALID;
  callback = fn;
  return LV_RESULT_OK;
}
lv_display_t* lv_display_get_default(void) { return &display; }
int32_t lv_display_get_horizontal_resolution(const lv_display_t*) { return 480; }
int32_t lv_display_get_vertical_resolution(const lv_display_t*) { return 240; }
lv_display_rotation_t lv_display_get_rotation(lv_display_t*) {
  return scenario == "rotation" ? LV_DISPLAY_ROTATION_90 : LV_DISPLAY_ROTATION_0;
}
lv_result_t lv_image_decoder_get_info(const void* src, lv_image_header_t* out) {
  assert(std::strcmp(static_cast<const char*>(src), "S:birthday/top_cw.png") == 0);
  out->w = scenario == "size" ? 240 : 480;
  out->h = 240;
  return scenario == "missing" ? LV_RESULT_INVALID : LV_RESULT_OK;
}
lv_result_t lv_image_decoder_open(lv_image_decoder_dsc_t* d, const void*, const lv_image_decoder_args_t* args) {
  ++opened;
  assert(args->no_cache);
  if(scenario == "decode") return LV_RESULT_INVALID;
  pixels.header.w = 480;
  pixels.header.h = 240;
  pixels.header.flags = LV_IMAGE_FLAGS_ALLOCATED | LV_IMAGE_FLAGS_MODIFIABLE;
  d->decoded = &pixels;
  return LV_RESULT_OK;
}
void lv_image_decoder_close(lv_image_decoder_dsc_t*) { ++closed; }
void lv_draw_buf_to_image(const lv_draw_buf_t* src, lv_image_dsc_t* dst) {
  dst->header = src->header;
  dst->data = src->data;
}
lv_obj_t* lv_layer_top(void) { return &object; }
lv_obj_t* lv_image_create(lv_obj_t*) { ++created; return scenario == "allocation" ? nullptr : &object; }
void lv_obj_remove_style_all(lv_obj_t*) {}
void lv_image_set_src(lv_obj_t*, const void* src) { ++sources; retained_source=src; }
void lv_obj_set_pos(lv_obj_t*, int32_t x, int32_t y) { assert(x==0 && y==0); }
void lv_obj_remove_flag(lv_obj_t*, lv_obj_flag_t flag) { assert(flag==LV_OBJ_FLAG_CLICKABLE || flag==LV_OBJ_FLAG_SCROLLABLE); }
int main(int argc, char** argv) {
  assert(argc==2);
  scenario=argv[1];
  lv_global.timer_state.already_running = scenario == "busy";
  birthday_ui::show_on_boot();
  birthday_ui::show_on_boot();
  assert(opened == 0); // All decode work is deferred to the GUI callback.
  if(scenario=="priority" || scenario=="busy") {
    assert(queued==0 && callback==nullptr);
    if(scenario=="busy") assert(now_ms==1000);
  } else {
    assert(queued==1);
    if(callback) callback(nullptr);
  }
  if(scenario=="happy") {
    assert(opened==1 && closed==0 && created==1 && sources==1);
    const auto* src=static_cast<const lv_image_dsc_t*>(retained_source);
    assert(src->header.w==480 && src->header.h==240);
    assert((src->header.flags & (LV_IMAGE_FLAGS_ALLOCATED | LV_IMAGE_FLAGS_MODIFIABLE))==0);
  } else {
    assert(sources==0);
    if(scenario=="allocation") assert(opened==1 && closed==1);
    else assert(closed==0);
  }
  birthday_ui::show_on_boot();
  assert(queued<=1 && opened<=1);
}
