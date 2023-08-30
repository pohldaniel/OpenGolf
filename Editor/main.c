#include <float.h>
#include <stdbool.h>
#include <stdio.h>

#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <cimgui.h>
#include <glad.h>
#include <IconsFontAwesome5.h>
#include <sokol_app.h>
#include <sokol_audio.h>
#include <sokol_gfx.h>
#include <sokol_glue.h>
#include <sokol_imgui.h>
#include <sokol_time.h>
#include <alloc.h>
#include <base64.h>
#include <common.h>
#include <data.h>
#include <debug_console.h>
#include <graphics.h>
#include <inputs.h>
#include <log.h>
#include <script.h>
#include "draw.h"
#include "editor.h"

static void init(void) {
    int load_gl = gladLoadGL();
    if (!load_gl) {
        golf_log_error("Unable to load GL");
    }

    stm_setup();
    sg_setup(&(sg_desc){ 
            .buffer_pool_size = 2048, 
            .image_pool_size = 2048,
            .context = sapp_sgcontext(),
            });
    simgui_setup(&(simgui_desc_t) {
            .dpi_scale = sapp_dpi_scale(),
            .no_default_font = true,
            });
    saudio_setup(&(saudio_desc){
            .sample_rate = 44100,
            .buffer_frames = 1024,
            .packet_frames = 64,
            .num_packets = 32, 
            });

    {
        // setup ImGui font with custom icons
        ImGuiIO *io = igGetIO();
        ImFontAtlas_AddFontDefault(io->Fonts, NULL);

        static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };

        ImFontConfig icons_config;
        memset(&icons_config, 0, sizeof(icons_config));
        icons_config.SizePixels = 16;
        icons_config.OversampleH = 1;
        icons_config.OversampleV = 1;
        icons_config.RasterizerMultiply = 1.0f;
        icons_config.EllipsisChar = -1;
        icons_config.GlyphMaxAdvanceX = FLT_MAX;
        icons_config.GlyphMinAdvanceX = 16;
        icons_config.MergeMode = true;
        icons_config.PixelSnapH = true;
        icons_config.FontDataOwnedByAtlas = false;
        icons_config.GlyphOffset.x -= 2.0f;
        icons_config.GlyphOffset.y += 3.0f;
        ImFontAtlas_AddFontFromFileTTF(io->Fonts, "data/font/fa-solid-900.ttf", 16, &icons_config, icons_ranges);

        unsigned char* font_pixels;
        int font_width, font_height, bytes_per_pixel;
        ImFontAtlas_GetTexDataAsRGBA32(io->Fonts, &font_pixels, &font_width, &font_height, &bytes_per_pixel);
        {
            sg_image_desc desc;
            memset(&desc, 0, sizeof(desc));
            desc.width = font_width;
            desc.height = font_height;
            desc.pixel_format = SG_PIXELFORMAT_RGBA8;
            desc.wrap_u = SG_WRAP_CLAMP_TO_EDGE;
            desc.wrap_v = SG_WRAP_CLAMP_TO_EDGE;
            desc.min_filter = SG_FILTER_LINEAR;
            desc.mag_filter = SG_FILTER_LINEAR;
            desc.data.subimage[0][0].ptr = font_pixels;
            desc.data.subimage[0][0].size = font_width * font_height * 4;
            io->Fonts->TexID = (ImTextureID)(uintptr_t) sg_make_image(&desc).id;
        }
    }
}

static void cleanup(void) {
    sg_shutdown();
}

static void frame(void) {
    static bool inited = false;
    static uint64_t last_time = 0;

    float dt = (float) stm_sec(stm_laptime(&last_time));
    if (!inited) {
        golf_data_turn_off_reload(".level");
        golf_data_init();
        golf_data_load("data/static_data.static_data", false);

        golf_inputs_init();
        golf_graphics_init();
        golf_draw_init();
        golf_editor_init();
        golf_debug_console_init();
        inited = true;
    }

    golf_editor_t *editor = golf_editor_get();

    golf_data_update(dt);

    golf_graphics_begin_frame(dt);
    golf_inputs_begin_frame();

    golf_editor_update(dt);
    golf_debug_console_update(dt);

    golf_graphics_set_viewport(editor->viewport_pos, editor->viewport_size);
    golf_graphics_update_proj_view_mat();
    golf_editor_draw();

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
    golf_script_store_init();
    return (sapp_desc){
        .init_cb = init,
            .frame_cb = frame,
            .cleanup_cb = cleanup,
            .event_cb = event,
            .width = 1280,
            .height = 720,
            .window_title = "Minigolf Editor",
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
