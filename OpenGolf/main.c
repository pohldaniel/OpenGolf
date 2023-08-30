#include <stdbool.h>
#include <stdio.h>

#include <sokol_app.h>
#include <sokol_gfx.h>
#include <sokol_glue.h>
#include <sokol_imgui.h>
#include <sokol_time.h>
#include <audio.h>
#include <common.h>
#include <data.h>
#include <debug_console.h>
#include <graphics.h>
#include <inputs.h>
#include <log.h>
#include <storage.h>
#include "draw.h"
#include "game.h"
#include "golf.h"
#include "ui.h"

static void init(void) {
    stm_setup();
    sg_setup(&(sg_desc){ 
            .buffer_pool_size = 2048, 
            .image_pool_size = 2048,
            .context = sapp_sgcontext(),
            });
    simgui_setup(&(simgui_desc_t) {
            .dpi_scale = sapp_dpi_scale() 
            });
}

static void cleanup(void) {
    sg_shutdown();
}

static void frame(void) {
    static bool storage_inited = false;
    static bool inited = false;
    static uint64_t last_time = 0;

    float dt = (float) stm_sec(stm_laptime(&last_time));
    if (!inited) {
        golf_data_init();
        golf_data_load("data/static_data.static_data", false);

        golf_storage_init();
        golf_audio_init();
        golf_debug_console_init();
        golf_inputs_init();
        golf_graphics_init();
        golf_draw_init();
        golf_init();
        inited = true;
    }

    if (!storage_inited && !golf_storage_finish_init()) {
        return;
    }
    storage_inited = true;

    golf_data_update(dt);

    golf_graphics_begin_frame(dt);
    golf_inputs_begin_frame();

    golf_update(dt);

    golf_graphics_set_viewport(V2(0, 0), V2((float)sapp_width(), (float)sapp_height()));
    golf_graphics_update_proj_view_mat();
    golf_draw();

    golf_inputs_end_frame();
    golf_graphics_end_frame();

    fflush(stdout);
}

static void event(const sapp_event *event) {
    simgui_handle_event(event);
    golf_inputs_handle_event(event);
}

sapp_desc sokol_main(int argc, char *argv[]) {
    GOLF_UNUSED(argc);
    GOLF_UNUSED(argv);

    golf_alloc_init();
    golf_log_init();
    return (sapp_desc){
        .init_cb = init,
            .frame_cb = frame,
            .cleanup_cb = cleanup,
            .event_cb = event,
            .width = 375,
            .height = 667,
            .window_title = "Minigolf",
            .enable_clipboard = true,
            .clipboard_size = 1024,
            .fullscreen = false,
            .high_dpi = false,
            .html5_canvas_resize = false,
            .win32_console_utf8 = true,
            .win32_console_create = true,
            .swap_interval = 1,
    };
}
