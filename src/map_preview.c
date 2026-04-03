#include "global.h"
#include "region_map.h"
#include "main.h"
#include "menu.h"
#include "field_weather.h"
#include "field_screen_effect.h"
#include "overworld.h"
#include "event_data.h"
#include "map_preview.h"
#include "bg.h"
#include "fldeff.h"
#include "window.h"
#include "malloc.h"
#include "palette.h"
#include "gpu_regs.h"
#include "string_util.h"
#include "constants/region_map_sections.h"
#include "constants/rgb.h"

// Set to TRUE to display previews longer on the first visit, shorter on repeat visits.
// Requires adding a FLAG_VISITED_* flag to each entry in sMapPreviewScreenData.
#define FLAG_BASED_MAP_PREVIEW_TIME FALSE

// Function Declarations
static void Task_RunMapPreviewScreenForest(u8 taskId);
static u16 MapPreview_GetDuration(u16 mapsec);
static bool32 MapPreview_IsGfxLoadFinished(void);
static void MapPreview_Unload(s32 windowId);
static u16 MapPreview_CreateMapNameWindow(u16 mapsec);

// EWRAM
static EWRAM_DATA bool8 sHasVisitedMapBefore = FALSE;
static EWRAM_DATA bool8 sAllocedBg0TilemapBuffer = FALSE;

// Graphics data — Hoenn dungeons use repurposed FRLG assets as placeholders.
// Replace with custom Hoenn artwork as desired.
static const u8 sViridianForestMapPreviewPalette[]    = INCBIN_U8("graphics/map_preview/viridian_forest_pal.gbapal");
static const u8 sViridianForestMapPreviewTiles[]      = INCBIN_U8("graphics/map_preview/viridian_forest_tiles.4bpp.lz");
static const u8 sViridianForestMapPreviewTilemap[]    = INCBIN_U8("graphics/map_preview/viridian_forest_tilemap.bin");

static const u8 sRockTunnelMapPreviewPalette[]        = INCBIN_U8("graphics/map_preview/rock_tunnel_pal.gbapal");
static const u8 sRockTunnelMapPreviewTiles[]          = INCBIN_U8("graphics/map_preview/rock_tunnel_tiles.4bpp.lz");
static const u8 sRockTunnelMapPreviewTilemap[]        = INCBIN_U8("graphics/map_preview/rock_tunnel_tilemap.bin");

static const u8 sMtMoonMapPreviewPalette[]            = INCBIN_U8("graphics/map_preview/mt_moon_pal.gbapal");
static const u8 sMtMoonMapPreviewTiles[]              = INCBIN_U8("graphics/map_preview/mt_moon_tiles.4bpp.lz");
static const u8 sMtMoonMapPreviewTilemap[]            = INCBIN_U8("graphics/map_preview/mt_moon_tilemap.bin");

static const u8 sSeafoamIslandsMapPreviewPalette[]    = INCBIN_U8("graphics/map_preview/seafoam_islands_pal.gbapal");
static const u8 sSeafoamIslandsMapPreviewTiles[]      = INCBIN_U8("graphics/map_preview/seafoam_islands_tiles.4bpp.lz");
static const u8 sSeafoamIslandsMapPreviewTilemap[]    = INCBIN_U8("graphics/map_preview/seafoam_islands_tilemap.bin");

static const u8 sCeruleanCaveMapPreviewPalette[]      = INCBIN_U8("graphics/map_preview/cerulean_cave_pal.gbapal");
static const u8 sCeruleanCaveMapPreviewTiles[]        = INCBIN_U8("graphics/map_preview/cerulean_cave_tiles.4bpp.lz");
static const u8 sCeruleanCaveMapPreviewTilemap[]      = INCBIN_U8("graphics/map_preview/cerulean_cave_tilemap.bin");

static const u8 sKantoVictoryRoadMapPreviewPalette[]  = INCBIN_U8("graphics/map_preview/kanto_victory_road_pal.gbapal");
static const u8 sKantoVictoryRoadMapPreviewTiles[]    = INCBIN_U8("graphics/map_preview/kanto_victory_road_tiles.4bpp.lz");
static const u8 sKantoVictoryRoadMapPreviewTilemap[]  = INCBIN_U8("graphics/map_preview/kanto_victory_road_tilemap.bin");

static const u8 sPokemonTowerMapPreviewPalette[]      = INCBIN_U8("graphics/map_preview/pokemon_tower_pal.gbapal");
static const u8 sPokemonTowerMapPreviewTiles[]        = INCBIN_U8("graphics/map_preview/pokemon_tower_tiles.4bpp.lz");
static const u8 sPokemonTowerMapPreviewTilemap[]      = INCBIN_U8("graphics/map_preview/pokemon_tower_tilemap.bin");

static const struct MapPreviewScreen sMapPreviewScreenData[MPS_COUNT] =
{
    [MPS_PETALBURG_WOODS] =
    {
        .mapsec   = MAPSEC_PETALBURG_WOODS,
        .type     = MPS_TYPE_FOREST,
        .tilesptr = sViridianForestMapPreviewTiles,
        .tilemapptr = sViridianForestMapPreviewTilemap,
        .palptr   = sViridianForestMapPreviewPalette,
    },
    [MPS_GRANITE_CAVE] =
    {
        .mapsec   = MAPSEC_GRANITE_CAVE,
        .type     = MPS_TYPE_CAVE,
        .tilesptr = sRockTunnelMapPreviewTiles,
        .tilemapptr = sRockTunnelMapPreviewTilemap,
        .palptr   = sRockTunnelMapPreviewPalette,
    },
    [MPS_FIERY_PATH] =
    {
        .mapsec   = MAPSEC_FIERY_PATH,
        .type     = MPS_TYPE_CAVE,
        .tilesptr = sRockTunnelMapPreviewTiles,
        .tilemapptr = sRockTunnelMapPreviewTilemap,
        .palptr   = sRockTunnelMapPreviewPalette,
    },
    [MPS_METEOR_FALLS] =
    {
        .mapsec   = MAPSEC_METEOR_FALLS,
        .type     = MPS_TYPE_CAVE,
        .tilesptr = sMtMoonMapPreviewTiles,
        .tilemapptr = sMtMoonMapPreviewTilemap,
        .palptr   = sMtMoonMapPreviewPalette,
    },
    [MPS_SEAFLOOR_CAVERN] =
    {
        .mapsec   = MAPSEC_SEAFLOOR_CAVERN,
        .type     = MPS_TYPE_CAVE,
        .tilesptr = sSeafoamIslandsMapPreviewTiles,
        .tilemapptr = sSeafoamIslandsMapPreviewTilemap,
        .palptr   = sSeafoamIslandsMapPreviewPalette,
    },
    [MPS_CAVE_OF_ORIGIN] =
    {
        .mapsec   = MAPSEC_CAVE_OF_ORIGIN,
        .type     = MPS_TYPE_CAVE,
        .tilesptr = sCeruleanCaveMapPreviewTiles,
        .tilemapptr = sCeruleanCaveMapPreviewTilemap,
        .palptr   = sCeruleanCaveMapPreviewPalette,
    },
    [MPS_VICTORY_ROAD] =
    {
        .mapsec   = MAPSEC_VICTORY_ROAD,
        .type     = MPS_TYPE_CAVE,
        .tilesptr = sKantoVictoryRoadMapPreviewTiles,
        .tilemapptr = sKantoVictoryRoadMapPreviewTilemap,
        .palptr   = sKantoVictoryRoadMapPreviewPalette,
    },
    [MPS_SKY_PILLAR] =
    {
        .mapsec   = MAPSEC_SKY_PILLAR,
        .type     = MPS_TYPE_CAVE,
        .tilesptr = sPokemonTowerMapPreviewTiles,
        .tilemapptr = sPokemonTowerMapPreviewTilemap,
        .palptr   = sPokemonTowerMapPreviewPalette,
    },
};

static const struct WindowTemplate sMapNameWindow =
{
    .bg          = 0,
    .tilemapLeft = 0,
    .tilemapTop  = 0,
    .width       = 13,
    .height      = 2,
    .paletteNum  = 14,
    .baseBlock   = 0x1C2
};

static const struct BgTemplate sMapPreviewBgTemplate[1] =
{
    {.mapBaseIndex = 31}
};

// Functions

static u8 GetMapPreviewScreenIdx(u16 mapsec)
{
    s32 i;

    for (i = 0; i < MPS_COUNT; i++)
    {
        if (sMapPreviewScreenData[i].mapsec == mapsec)
            return i;
    }
    return MPS_COUNT;
}

bool8 MapHasPreviewScreen(u16 mapsec, u8 type)
{
    u8 idx = GetMapPreviewScreenIdx(mapsec);

    if (idx == MPS_COUNT)
        return FALSE;

    if (type == MPS_TYPE_ANY)
        return TRUE;

    return sMapPreviewScreenData[idx].type == type;
}

static void MapPreview_InitBgs(void)
{
    InitBgsFromTemplates(0, sMapPreviewBgTemplate, NELEMS(sMapPreviewBgTemplate));
    ShowBg(0);
}

void MapPreview_LoadGfx(u16 mapsec)
{
    u8 idx = GetMapPreviewScreenIdx(mapsec);

    if (idx == MPS_COUNT)
        return;

    ResetTempTileDataBuffers();
    LoadPalette(sMapPreviewScreenData[idx].palptr, 0xD0, 0x60);
    DecompressAndCopyTileDataToVram(0, sMapPreviewScreenData[idx].tilesptr, 0, 0, 0);
    if (GetBgTilemapBuffer(0) == NULL)
    {
        SetBgTilemapBuffer(0, Alloc(BG_SCREEN_SIZE));
        sAllocedBg0TilemapBuffer = TRUE;
    }
    else
    {
        sAllocedBg0TilemapBuffer = FALSE;
    }
    CopyToBgTilemapBuffer(0, sMapPreviewScreenData[idx].tilemapptr, 0, 0x000);
    CopyBgTilemapBufferToVram(0);
}

static void MapPreview_Unload(s32 windowId)
{
    RemoveWindow(windowId);
    if (sAllocedBg0TilemapBuffer)
        Free(GetBgTilemapBuffer(0));
}

static bool32 MapPreview_IsGfxLoadFinished(void)
{
    return FreeTempTileDataBuffersIfPossible();
}

static u16 MapPreview_CreateMapNameWindow(u16 mapsec)
{
    u16 windowId;
    u32 xctr;
    u8 color[3];

    windowId = AddWindow(&sMapNameWindow);
    FillWindowPixelBuffer(windowId, PIXEL_FILL(1));
    PutWindowTilemap(windowId);
    color[0] = TEXT_COLOR_WHITE;
    color[1] = TEXT_COLOR_RED;
    color[2] = TEXT_COLOR_LIGHT_GRAY;
    GetMapName(gStringVar4, mapsec, 0);
    xctr = 104 - GetStringWidth(2, gStringVar4, 0);
    AddTextPrinterParameterized4(windowId, 2, xctr / 2, 2, 0, 0, color, -1, gStringVar4);
    return windowId;
}

bool32 ForestMapPreviewScreenIsRunning(void)
{
    // Returns TRUE when the task is NOT running (screen done), FALSE while running.
    // Used by WaitForWeatherFadeIn to block weather until the preview finishes.
    return !FuncIsActiveTask(Task_RunMapPreviewScreenForest);
}

void MapPreview_StartForestTransition(u16 mapsec)
{
    u8 taskId = CreateTask(Task_RunMapPreviewScreenForest, 0);
    gTasks[taskId].data[2]  = GetBgAttribute(0, BG_ATTR_PRIORITY);
    gTasks[taskId].data[4]  = GetGpuReg(REG_OFFSET_BLDCNT);
    gTasks[taskId].data[5]  = GetGpuReg(REG_OFFSET_BLDALPHA);
    gTasks[taskId].data[3]  = GetGpuReg(REG_OFFSET_DISPCNT);
    gTasks[taskId].data[6]  = GetGpuReg(REG_OFFSET_WININ);
    gTasks[taskId].data[7]  = GetGpuReg(REG_OFFSET_WINOUT);
    gTasks[taskId].data[10] = MapPreview_GetDuration(mapsec);
    gTasks[taskId].data[8]  = 16;
    gTasks[taskId].data[9]  = 0;
    SetBgAttribute(0, BG_ATTR_PRIORITY, 0);
    SetGpuReg(REG_OFFSET_BLDCNT, BLDCNT_TGT1_BG0 | BLDCNT_EFFECT_BLEND | BLDCNT_TGT2_BG1 | BLDCNT_TGT2_BG2 | BLDCNT_TGT2_BG3 | BLDCNT_TGT2_OBJ | BLDCNT_TGT2_BD);
    SetGpuReg(REG_OFFSET_BLDALPHA, BLDALPHA_BLEND(16, 0));
    SetGpuRegBits(REG_OFFSET_WININ, WININ_WIN0_CLR | WININ_WIN1_CLR);
    SetGpuRegBits(REG_OFFSET_WINOUT, WINOUT_WIN01_CLR);
    gTasks[taskId].data[11] = MapPreview_CreateMapNameWindow(mapsec);
}

static void Task_RunMapPreviewScreenForest(u8 taskId)
{
    s16 *data = gTasks[taskId].data;

    switch (data[0])
    {
    case 0:
        if (!MapPreview_IsGfxLoadFinished() && !IsDma3ManagerBusyWithBgCopy())
        {
            CopyWindowToVram(data[11], 3);
            data[0]++;
        }
        break;
    case 1:
        // Re-assert blend registers every frame: weather system may override them
        SetGpuReg(REG_OFFSET_BLDCNT, BLDCNT_TGT1_BG0 | BLDCNT_EFFECT_BLEND | BLDCNT_TGT2_BG1 | BLDCNT_TGT2_BG2 | BLDCNT_TGT2_BG3 | BLDCNT_TGT2_OBJ | BLDCNT_TGT2_BD);
        SetGpuReg(REG_OFFSET_BLDALPHA, BLDALPHA_BLEND(16, 0));
        if (!IsDma3ManagerBusyWithBgCopy())
        {
            FadeInFromBlack();
            FillPalBufferBlack();
            data[0]++;
        }
        break;
    case 2:
        // Re-assert blend registers and protect preview palette from weather shade effect
        SetGpuReg(REG_OFFSET_BLDCNT, BLDCNT_TGT1_BG0 | BLDCNT_EFFECT_BLEND | BLDCNT_TGT2_BG1 | BLDCNT_TGT2_BG2 | BLDCNT_TGT2_BG3 | BLDCNT_TGT2_OBJ | BLDCNT_TGT2_BD);
        SetGpuReg(REG_OFFSET_BLDALPHA, BLDALPHA_BLEND(16, 0));
        CpuCopy16(gPlttBufferUnfaded + 0xD0, gPlttBufferFaded + 0xD0, 0x60);
        if (IsWeatherNotFadingIn())
        {
            Overworld_PlaySpecialMapMusic();
            data[0]++;
        }
        break;
    case 3:
        // Re-assert blend registers and protect preview palette from weather shade effect
        SetGpuReg(REG_OFFSET_BLDCNT, BLDCNT_TGT1_BG0 | BLDCNT_EFFECT_BLEND | BLDCNT_TGT2_BG1 | BLDCNT_TGT2_BG2 | BLDCNT_TGT2_BG3 | BLDCNT_TGT2_OBJ | BLDCNT_TGT2_BD);
        SetGpuReg(REG_OFFSET_BLDALPHA, BLDALPHA_BLEND(16, 0));
        CpuCopy16(gPlttBufferUnfaded + 0xD0, gPlttBufferFaded + 0xD0, 0x60);
        data[1]++;
        if (data[1] > data[10])
        {
            data[1] = 0;
            data[0]++;
        }
        break;
    case 4:
        switch (data[1])
        {
        case 0:
            data[9]++;
            if (data[9] > 16)
                data[9] = 16;
            break;
        case 1:
            data[8]--;
            if (data[8] < 0)
                data[8] = 0;
            break;
        }
        data[1] = (data[1] + 1) % 3;
        SetGpuReg(REG_OFFSET_BLDALPHA, BLDALPHA_BLEND(data[8], data[9]));
        if (data[8] == 0 && data[9] == 16)
        {
            FillBgTilemapBufferRect_Palette0(0, 0, 0, 0, 32, 32);
            CopyBgTilemapBufferToVram(0);
            data[0]++;
        }
        break;
    case 5:
        if (!IsDma3ManagerBusyWithBgCopy())
        {
            MapPreview_Unload(data[11]);
            SetBgAttribute(0, BG_ATTR_PRIORITY, data[2]);
            SetGpuReg(REG_OFFSET_DISPCNT, data[3]);
            SetGpuReg(REG_OFFSET_BLDCNT, data[4]);
            SetGpuReg(REG_OFFSET_BLDALPHA, data[5]);
            SetGpuReg(REG_OFFSET_WININ, data[6]);
            SetGpuReg(REG_OFFSET_WINOUT, data[7]);
            DestroyTask(taskId);
        }
        break;
    }
}

const struct MapPreviewScreen *GetDungeonMapPreviewScreenInfo(u16 mapsec)
{
    u8 idx = GetMapPreviewScreenIdx(mapsec);

    if (idx == MPS_COUNT)
        return NULL;

    return &sMapPreviewScreenData[idx];
}

static u16 MapPreview_GetDuration(u16 mapsec)
{
    u8 idx;
    u16 flagId;

    idx = GetMapPreviewScreenIdx(mapsec);
    if (idx == MPS_COUNT)
        return 0;

#if FLAG_BASED_MAP_PREVIEW_TIME
    flagId = sMapPreviewScreenData[idx].flagId;
    if (sMapPreviewScreenData[idx].type == MPS_TYPE_CAVE)
    {
        return FlagGet(flagId) ? 40 : 120;
    }
    else
    {
        return sHasVisitedMapBefore ? 40 : 120;
    }
#else
    (void)flagId;
    return 120;
#endif
}

void MapPreview_SetFlag(u16 flagId)
{
#if FLAG_BASED_MAP_PREVIEW_TIME
    sHasVisitedMapBefore = !FlagGet(flagId);
    FlagSet(flagId);
#else
    (void)flagId;
    sHasVisitedMapBefore = FALSE;
#endif
}

// Cave preview screen — standalone (used when entering caves via normal warp).
// After the preview, hands off to Task_EnterCaveTransition2 for the flash-in effect.
static void Task_MapPreviewScreen_0(u8 taskId)
{
    s16 *data = gTasks[taskId].data;

    switch (data[0])
    {
    case 0:
        SetWordTaskArg(taskId, 5, (uintptr_t)gMain.vblankCallback);
        SetVBlankCallback(NULL);
        MapPreview_InitBgs();
        MapPreview_LoadGfx(data[3]);
        BlendPalettes(0xFFFFFFFF, 0x10, RGB_BLACK);
        data[0]++;
        break;
    case 1:
        if (!MapPreview_IsGfxLoadFinished())
        {
            data[4] = MapPreview_CreateMapNameWindow(data[3]);
            CopyWindowToVram(data[4], 3);
            data[0]++;
        }
        break;
    case 2:
        if (!IsDma3ManagerBusyWithBgCopy())
        {
            BeginNormalPaletteFade(0xFFFFFFFF, -1, 16, 0, RGB_BLACK);
            SetVBlankCallback((IntrCallback)GetWordTaskArg(taskId, 5));
            data[0]++;
        }
        break;
    case 3:
        if (!UpdatePaletteFade())
        {
            data[2] = MapPreview_GetDuration(data[3]);
            data[0]++;
        }
        break;
    case 4:
        data[1]++;
        if (data[1] > data[2] || JOY_HELD(B_BUTTON))
        {
            BeginNormalPaletteFade(0xFFFFFFFF, -2, 0, 16, RGB_WHITE);
            data[0]++;
        }
        break;
    case 5:
        if (!UpdatePaletteFade())
        {
            int i;
            for (i = 0; i < 16; i++)
                data[i] = 0;
            MapPreview_Unload(data[4]);
            gTasks[taskId].func = Task_EnterCaveTransition2;
        }
        break;
    }
}

void RunMapPreviewScreen(u16 mapSecId)
{
    u8 taskId = CreateTask(Task_MapPreviewScreen_0, 0);
    gTasks[taskId].data[3] = mapSecId;
}
