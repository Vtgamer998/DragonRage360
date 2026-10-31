#define DR_XBOX360 1
#define DR_BACKEND_TEST_MAIN previous_backend_tests
#include "test_gs_backend.c"
#undef main
static GsBackend *sBackend;
int gsPendingScale4,gsGlowPercent,gsFilter,gGsAstcOn,gGsFragDepth;
unsigned gsFxOff;
GsTex gsWhite;
static int sTexPackOn,sSmooth2d,sHudLayout;
int gPortMusicPercent,gPortSePercent,gPortVoicePercent,gPortStagePercent;
static int Port_Setting(const char *name,int fallback) {(void)name;return fallback;}
int GsGpu_Init(void) {
    const char *api = getenv("BT3_GPU_API");
    GsBackend *sb;
#ifdef DR_XBOX360
    if (api != NULL && strcmp(api, "auto") != 0 && strcmp(api, "xenos") != 0) {
        fprintf(stderr, "bt3: Xbox 360 renderer must be xenos\n");
        return 0;
    }
#endif

    /* BT3_SCALE=1.5 and the like; the setting "scale4" in quarters, else the older whole "scale" */
    if (getenv("BT3_SCALE") != NULL) {
        gsScale4 = (int)(atof(getenv("BT3_SCALE")) * 4.0 + 0.5);
    } else {
        gsScale4 = Port_Setting("scale4", 0);
        if (gsScale4 <= 0) {
            gsScale4 = Port_Setting("scale", 2) * 4;
        }
    }
    gsScale4 = gsScale4 < 4 ? 4 : gsScale4 > 32 ? 32 : gsScale4;
    gsPendingScale4 = 0;
    gsVerts = malloc(MAX_VERTS * sizeof(Vtx));
    gsVuVerts = malloc(MAX_VU_VERTS * 48);
    gsVuIdx = malloc(MAX_VU_IDX * sizeof(uint32_t));
    gsVuUni = malloc(MAX_VU_UNIFORMS * sizeof(Vu0Uniform));
    gsDraws = malloc(MAX_DRAWS * sizeof(GsDraw));
    if (gsVerts == NULL || gsVuVerts == NULL || gsVuIdx == NULL || gsVuUni == NULL || gsDraws == NULL) {
        free(gsVerts); free(gsVuVerts); free(gsVuIdx); free(gsVuUni); free(gsDraws);
        gsVerts = NULL; gsVuVerts = NULL; gsVuIdx = NULL; gsVuUni = NULL; gsDraws = NULL;
        return 0;
    }
    /* BT3_GPU_API=vulkan|gl|auto picks the back end. Both are complete renderers of the same list, so a
       picture from either should be the same (see the frame-by-frame compare in the port's docs). */
#ifdef DR_XBOX360
    sb = &sXenosBackend;
#else
    if (api == NULL || strcmp(api, "auto") == 0) {
        /* the settings window's choice (Renderer; Vulkan unless OpenGL was picked there) */
        sb = Port_Setting("gpu_api", 0) == 1 ? &sGlBackend : &sVulkanBackend;
        api = NULL;
    } else if (strcmp(api, "vk") == 0 || strcmp(api, "vulkan") == 0) {
        sb = &sVulkanBackend;
    } else if (strcmp(api, "gl") == 0 || strcmp(api, "opengl") == 0) {
        sb = &sGlBackend;
    } else {
        fprintf(stderr, "bt3: unknown BT3_GPU_API=%s (try vulkan, gl or auto)\n", api);
        return 0;
    }
#endif
#ifdef __ANDROID__
    /* Android: the setting "gpu_api" (1 = OpenGL ES 3.2, the OpenGL back end built for ES; else Vulkan) is written by the
       launcher, which picks OpenGL ES for Mali GPUs by itself unless the player chose; if the chosen one cannot start,
       the other is tried (api NULL) */
    fprintf(stderr, "bt3: renderer chosen: %s\n", sb == &sGlBackend ? "OpenGL ES" : "Vulkan");
#endif
    sBackend = sb;
    if (!sb->init()) {
#ifdef DR_XBOX360
        sBackend = NULL;
        free(gsVerts); free(gsVuVerts); free(gsVuIdx); free(gsVuUni); free(gsDraws);
        gsVerts = NULL; gsVuVerts = NULL; gsVuIdx = NULL; gsVuUni = NULL; gsDraws = NULL;
        return 0;
#else
        /* The chosen renderer cannot start (no Vulkan driver, an old graphics card): the other one is tried,
           unless one was asked for by name. */
        GsBackend *other = sb == &sVulkanBackend ? &sGlBackend : &sVulkanBackend;
        sBackend = NULL;
        if (api != NULL) {
            return 0;
        }
        fprintf(stderr, "bt3: the %s renderer could not start; trying %s\n", sb == &sVulkanBackend ? "Vulkan" : "OpenGL",
                other == &sVulkanBackend ? "Vulkan" : "OpenGL");
        sb = other;
        sBackend = sb;
        if (!sb->init()) {
            sBackend = NULL;
            return 0;
        }
#endif
    }
    gsWhite = sb->whiteTex();
    gsFxOff = (unsigned)(getenv("BT3_FX_OFF") != NULL ? atoi(getenv("BT3_FX_OFF")) : Port_Setting("fx_off", 0)) & 63;
    gsGlowPercent = getenv("BT3_GLOW") != NULL ? atoi(getenv("BT3_GLOW")) : Port_Setting("glow", GLOW_DEFAULT);
    gsFilter = getenv("BT3_FILTER") != NULL ? atoi(getenv("BT3_FILTER")) : Port_Setting("filter", 0);
    gsFilter = gsFilter < 0 || gsFilter > 4 ? 0 : gsFilter;
#ifndef DR_XBOX360
    MenuText_SetFontId(Port_Setting("menu_font_id", 0));
    MenuText_SetBattleFontId(Port_Setting("battle_font_id", 0));
    MenuText_SetSharedFontId(Port_Setting("text_font_id", 0));
#endif
    gGsAstcOn = getenv("BT3_ASTC") != NULL ? atoi(getenv("BT3_ASTC")) != 0 : Port_Setting("astc", 0) != 0;
    gGsFragDepth = getenv("BT3_FRAG_DEPTH") != NULL ? atoi(getenv("BT3_FRAG_DEPTH")) != 0 : Port_Setting("frag_depth", 1) != 0;
    sTexPackOn = Port_Setting("texture_pack", 1) != 0;
    sSmooth2d = getenv("BT3_2D_SMOOTH") != NULL ? atoi(getenv("BT3_2D_SMOOTH")) != 0 : Port_Setting("smooth_2d", 1) != 0;
    gPortMusicPercent = Port_Setting("music", 100);
    gPortSePercent = Port_Setting("effects", 100);

    sHudLayout = getenv("BT3_HUD") != NULL ? atoi(getenv("BT3_HUD")) : Port_Setting("hud_layout", 0);
    sHudLayout = sHudLayout < 0 || sHudLayout > 2 ? 0 : sHudLayout;
    gPortVoicePercent = Port_Setting("voices", gPortSePercent); /* (one slider before 0.1.30: what it was set to) */
    gPortStagePercent = Port_Setting("stage_sounds", gPortSePercent);
#ifndef DR_XBOX360
    TexPack_Init();
#endif
    return 1;
}

int main(void) {
    previous_backend_tests();
    assert(setenv("BT3_GPU_API","xenos",1)==0);
    assert(GsGpu_Init()==1 && sBackend==&sXenosBackend && gsWhite==sXenosBackend.whiteTex());
    assert(gsVerts && gsVuVerts && gsVuIdx && gsVuUni && gsDraws && gsScale4==4);
    assert(dr_gs_backend_close()==0);
    free(gsVerts);free(gsVuVerts);free(gsVuIdx);free(gsVuUni);free(gsDraws);
    puts("PASS: patched GsGpu_Init body selects sXenosBackend under DR_XBOX360, no GL/Vulkan linked; remaining capture source not compiled");
}
