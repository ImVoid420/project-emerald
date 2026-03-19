#include "global.h"
#include "main.h"
#include "menu.h"
#include "quest_menu.h"
#include "overworld.h"
#include "script.h"
#include "strings.h"
#include "constants/quests.h"
#include "bg.h"
#include "gpu_regs.h"
#include "palette.h"
#include "decompress.h"
#include "scanline_effect.h"
#include "malloc.h"
#include "window.h"
#include "text.h"
#include "quests.h"
#include "sound.h"
#include "constants/songs.h"
#include "constants/rgb.h"
#include "constants/characters.h"
#include "string_util.h"
#include "event_object_movement.h"
#include "constants/event_object_movement.h"
#include "sprite.h"

// Risorse grafiche
extern const u32 gQuestMenu_Gfx[];
extern const u32 gQuestMenu_Tilemap[];
extern const u32 gQuestMenu_Pal[];

// Palette standard: index 0=trasparente, 1=bianco (lo sovrascriviamo con nero), 3=grigio chiaro
// Usiamo {0, WHITE(=nero), LIGHT_GRAY} per avere testo nero + ombra identica allo zaino
static const u16 sBlackColor[] = {RGB_BLACK};
static const u8 sQuestTextColor[3]   = {TEXT_COLOR_TRANSPARENT, TEXT_COLOR_WHITE, TEXT_COLOR_LIGHT_GRAY};
// Detail: palette 14 = standard non modificata, indice 1 = bianco originale
// Detail: palette 14, index 2 sovrascritto con nero per ombra nera su sfondo blu
static const u8 sQuestDetailColor[3] = {TEXT_COLOR_TRANSPARENT, TEXT_COLOR_WHITE, TEXT_COLOR_DARK_GRAY};

static const u8 sLabel_Info[]   = _("Info: ");
static const u8 sLabel_Luogo[]  = _("Luogo: ");

#define WIN_QUESTLIST   0
#define WIN_QUESTDETAIL 1

// Posizione del cursore nella lista (persiste tra aperture del menu)
static u8 sQuestCursorPos;
// Lista degli ID delle quest sbloccate
static u8 sUnlockedQuestCount;
static u8 sUnlockedQuestIds[SIDE_QUEST_COUNT];
// ID dello sprite NPC attualmente visualizzato nel riquadro bianco
static u8 sNpcSpriteId;

static void QuestMenu_MainCB(void);
static void QuestMenu_VBlankCB(void);
static void PrintQuestDetails(void);
static void ShowNpcSprite(void);

// BG0 = testo, BG1 = grafica di sfondo
static const struct BgTemplate sQuestMenuBgTemplates[] =
{
    {
        .bg = 0,
        .charBaseIndex = 0,
        .mapBaseIndex = 31,
        .screenSize = 0,
        .paletteMode = 0,
        .priority = 0,
        .baseTile = 0
    },
    {
        .bg = 1,
        .charBaseIndex = 1,
        .mapBaseIndex = 30,
        .screenSize = 0,
        .paletteMode = 0,
        .priority = 1,
        .baseTile = 0
    }
};

// WIN_QUESTLIST:   area beige, tile x=3..22, y=3..10 (4 slot da 2 righe ciascuno)
// WIN_QUESTDETAIL: area blu,   tile x=6..28, y=13..18 (6 righe = 48px)
static const struct WindowTemplate sQuestMenuWindowTemplates[] =
{
    { // WIN_QUESTLIST
        // Inizia a col 1 (arrow fuori dalle linee che partono a col 3)
        // Inizia a row 1 (testo 16px occupa rows 1-2, linea a row 3 -> testo sopra)
        .bg = 0,
        .tilemapLeft = 1,
        .tilemapTop = 2,
        .width = 22,   // tile 1..22
        .height = 9,   // tile 2..10, copre 4 slot
        .paletteNum = 15,
        .baseBlock = 1,
    },
    { // WIN_QUESTDETAIL  (baseBlock = 22*9+1 = 199, palette 14 separata per testo bianco)
        .bg = 0,
        .tilemapLeft = 6,
        .tilemapTop = 13,
        .width = 24,   // tile 6..29 (bordo destro riquadro blu)
        .height = 6,   // tile 13..18, 48px
        .paletteNum = 14,
        .baseBlock = 199,
    },
    DUMMY_WIN_TEMPLATE
};

extern const u8 gText_SelectorArrow2[];

// Popola sUnlockedQuestIds con le quest effettivamente sbloccate
static void BuildUnlockedQuestList(void)
{
    u8 i;
    sUnlockedQuestCount = 0;
    for (i = 0; i < SIDE_QUEST_COUNT; i++)
    {
        if (CheckQuestUnlocked(i))
            sUnlockedQuestIds[sUnlockedQuestCount++] = i;
    }
    // Evita cursore fuori range se alcune quest non sono più sbloccate
    if (sUnlockedQuestCount > 0 && sQuestCursorPos >= sUnlockedQuestCount)
        sQuestCursorPos = sUnlockedQuestCount - 1;
}

// Stampa la lista dei nomi con il cursore sulla quest selezionata
void PrintQuestsList(void)
{
    u8 i;
    u32 y = 0;

    FillWindowPixelBuffer(WIN_QUESTLIST, PIXEL_FILL(0));
    for (i = 0; i < sUnlockedQuestCount; i++)
    {
        if (i == sQuestCursorPos)
            AddTextPrinterParameterized4(WIN_QUESTLIST, FONT_NARROW, 4, y, 0, 0, sQuestTextColor, -1, gText_SelectorArrow2);
        AddTextPrinterParameterized4(WIN_QUESTLIST, FONT_NARROW, 16, y, 0, 0, sQuestTextColor, -1, gSideQuests[sUnlockedQuestIds[i]].name);
        y += 16;
    }
    CopyWindowToVram(WIN_QUESTLIST, COPYWIN_FULL);
}

// Stampa i dettagli della quest attualmente selezionata
static void PrintQuestDetails(void)
{
    const struct SideQuest *quest;

    FillWindowPixelBuffer(WIN_QUESTDETAIL, PIXEL_FILL(0));
    if (sUnlockedQuestCount == 0)
    {
        CopyWindowToVram(WIN_QUESTDETAIL, COPYWIN_FULL);
        return;
    }
    quest = &gSideQuests[sUnlockedQuestIds[sQuestCursorPos]];
    // Luogo: [map]
    StringCopy(gStringVar4, sLabel_Luogo);
    StringAppend(gStringVar4, quest->map);
    AddTextPrinterParameterized4(WIN_QUESTDETAIL, FONT_SMALL, 0,  0, 0, 0, sQuestDetailColor, -1, gStringVar4);
    // Info: [desc]
    StringCopy(gStringVar4, sLabel_Info);
    StringAppend(gStringVar4, quest->desc);
    AddTextPrinterParameterized4(WIN_QUESTDETAIL, FONT_SMALL, 0, 14, 0, 0, sQuestDetailColor, -1, gStringVar4);
    CopyWindowToVram(WIN_QUESTDETAIL, COPYWIN_FULL);
}

// Mostra lo sprite NPC della quest selezionata nel riquadro bianco (tile x=1-3, y=15-18)
// Centro del riquadro: pixel x=20, y=136
static void ShowNpcSprite(void)
{
    if (sNpcSpriteId < MAX_SPRITES)
    {
        DestroySprite(&gSprites[sNpcSpriteId]);
        sNpcSpriteId = MAX_SPRITES;
    }
    if (sUnlockedQuestCount == 0)
        return;
    sNpcSpriteId = CreateObjectGraphicsSprite(
        gSideQuests[sUnlockedQuestIds[sQuestCursorPos]].npcGraphicsId,
        SpriteCallbackDummy,
        18, 132, 0);
    if (sNpcSpriteId < MAX_SPRITES)
    {
        gSprites[sNpcSpriteId].oam.priority = 0;
        StartSpriteAnim(&gSprites[sNpcSpriteId], ANIM_STD_FACE_SOUTH);
    }
}

void QuestMenu_Init(void (*callback)(void))
{
    SetVBlankCallback(NULL);
    SetMainCallback2(QuestMenu_MainCB);
}

static void QuestMenu_VBlankCB(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

static void QuestMenu_MainCB(void)
{
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();

    switch (gMain.state)
    {
        case 0:
            SetVBlankCallback(NULL);
            SetGpuReg(REG_OFFSET_DISPCNT, 0);

            SetGpuReg(REG_OFFSET_BG0HOFS, 0);
            SetGpuReg(REG_OFFSET_BG0VOFS, 0);
            SetGpuReg(REG_OFFSET_BG1HOFS, 0);
            SetGpuReg(REG_OFFSET_BG1VOFS, 0);

            ResetBgs();
            InitBgsFromTemplates(0, sQuestMenuBgTemplates, ARRAY_COUNT(sQuestMenuBgTemplates));
            SetBgTilemapBuffer(1, Alloc(0x800));
            InitWindows(sQuestMenuWindowTemplates);

            DmaClearLarge16(3, (void *)VRAM, VRAM_SIZE, 0x1000);
            DmaClear32(3, (void *)OAM, OAM_SIZE);
            DmaClear16(3, (void *)PLTT, PLTT_SIZE);
            ResetSpriteData();
            sNpcSpriteId = MAX_SPRITES;
            gMain.state++;
            break;

        case 1:
            LZ77UnCompVram(gQuestMenu_Gfx, (void *)(VRAM + 0x4000));
            LZ77UnCompWram(gQuestMenu_Tilemap, GetBgTilemapBuffer(1));

            LoadPalette(gQuestMenu_Pal, 0, 32);
            // Slot 15: lista quest — testo nero (indice 1 sovrascritto), ombra grigio chiaro
            Menu_LoadStdPalAt(BG_PLTT_ID(15));
            LoadPalette(sBlackColor, BG_PLTT_ID(15) + 1, sizeof(u16));
            // Slot 14: dettagli quest — testo bianco (1), ombra nera (2 sovrascritto)
            Menu_LoadStdPalAt(BG_PLTT_ID(14));
            LoadPalette(sBlackColor, BG_PLTT_ID(14) + 2, sizeof(u16));
            // Forza il buffer faded a nero prima che il VBlank lo trasferisca
            BlendPalettes(PALETTES_ALL, 16, RGB_BLACK);

            SetVBlankCallback(QuestMenu_VBlankCB);
            gMain.state++;
            break;

        case 2:
            CopyBgTilemapBufferToVram(1);

            SetGpuReg(REG_OFFSET_BG1VOFS, 0);
            SetGpuReg(REG_OFFSET_BG1HOFS, 0);

            // Costruisce la lista delle quest sbloccate, poi stampa
            BuildUnlockedQuestList();
            PutWindowTilemap(WIN_QUESTLIST);
            PutWindowTilemap(WIN_QUESTDETAIL);
            PrintQuestsList();
            PrintQuestDetails();
            ShowNpcSprite();

            BlendPalettes(PALETTES_ALL, 16, RGB_BLACK);

            ShowBg(0);
            ShowBg(1);
            SetGpuReg(REG_OFFSET_DISPCNT, DISPCNT_MODE_0 | DISPCNT_OBJ_1D_MAP | DISPCNT_BG0_ON | DISPCNT_BG1_ON | DISPCNT_OBJ_ON);

            BeginNormalPaletteFade(PALETTES_ALL, 0, 16, 0, RGB_BLACK);
            gMain.state++;
            break;

        case 3:
            if (!gPaletteFade.active)
                gMain.state++;
            break;

        case 4:
            // Navigazione lista
            if (JOY_NEW(DPAD_UP) && sQuestCursorPos > 0)
            {
                PlaySE(SE_SELECT);
                sQuestCursorPos--;
                PrintQuestsList();
                PrintQuestDetails();
                ShowNpcSprite();
            }
            else if (JOY_NEW(DPAD_DOWN) && sQuestCursorPos < sUnlockedQuestCount - 1)
            {
                PlaySE(SE_SELECT);
                sQuestCursorPos++;
                PrintQuestsList();
                PrintQuestDetails();
                ShowNpcSprite();
            }
            else if (JOY_NEW(B_BUTTON))
            {
                PlaySE(SE_SELECT);
                BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
                gMain.state++;
            }
            break;

        case 5:
            if (!gPaletteFade.active)
            {
                if (sNpcSpriteId < MAX_SPRITES)
                    DestroySprite(&gSprites[sNpcSpriteId]);
                Free(GetBgTilemapBuffer(1));
                FreeAllWindowBuffers();
                SetMainCallback2(CB2_ReturnToFieldWithOpenMenu);
            }
            break;
    }
}
