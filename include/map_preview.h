#ifndef GUARD_MAP_PREVIEW_H
#define GUARD_MAP_PREVIEW_H

enum MapPreviewScreenId
{
    MPS_PETALBURG_WOODS = 0,
    MPS_GRANITE_CAVE,
    MPS_FIERY_PATH,
    MPS_METEOR_FALLS,
    MPS_SEAFLOOR_CAVERN,
    MPS_CAVE_OF_ORIGIN,
    MPS_VICTORY_ROAD,
    MPS_SKY_PILLAR,
    MPS_COUNT
};

#define MPS_TYPE_CAVE   0
#define MPS_TYPE_FOREST 1
#define MPS_TYPE_ANY    2

struct MapPreviewScreen
{
    u16 mapsec;
    u8 type;
    u16 flagId;
    const void *tilesptr;
    const void *tilemapptr;
    const void *palptr;
};

void MapPreview_SetFlag(u16 flagId);
bool8 MapHasPreviewScreen(u16 mapsec, u8 type);
bool32 ForestMapPreviewScreenIsRunning(void);
const struct MapPreviewScreen *GetDungeonMapPreviewScreenInfo(u16 mapsec);
void MapPreview_LoadGfx(u16 mapsec);
void MapPreview_StartForestTransition(u16 mapsec);
void RunMapPreviewScreen(u16 mapSecId);

#endif // GUARD_MAP_PREVIEW_H
