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
#include "pokemon_icon.h"

// Risorse grafiche
extern const u32 gQuestMenu_Gfx[];
extern const u32 gQuestMenu_Tilemap[];
extern const u32 gQuestMenu_Pal[];

// Palette 15: lista quest — idx1=nero, idx4=rosso, idx5=verde
static const u16 sBlackColor[]  = {RGB_BLACK};
static const u16 sRedColor[]    = {RGB(28, 4, 4)};
static const u16 sGreenColor[]  = {RGB(4, 22, 4)};

static const u8 sQuestTextColor[3]        = {TEXT_COLOR_TRANSPARENT, TEXT_COLOR_WHITE,       TEXT_COLOR_LIGHT_GRAY};
static const u8 sQuestStatusRedColor[3]   = {TEXT_COLOR_TRANSPARENT, 4,                      TEXT_COLOR_LIGHT_GRAY};
static const u8 sQuestStatusGreenColor[3] = {TEXT_COLOR_TRANSPARENT, 5,                      TEXT_COLOR_LIGHT_GRAY};
// Palette 13: header — icone chiare (idx1=bianco standard), testo nero a idx2
static const u8 sHeaderTextColor[3]       = {TEXT_COLOR_TRANSPARENT, 2,                      TEXT_COLOR_LIGHT_GRAY};
// Palette 14: dettagli quest — testo bianco su sfondo blu
static const u8 sQuestDetailColor[3]      = {TEXT_COLOR_TRANSPARENT, TEXT_COLOR_WHITE, TEXT_COLOR_DARK_GRAY};

static const u8 sLabel_Luogo[]     = _("Luogo: ");
static const u8 sText_InCorso[]    = _("In corso");
static const u8 sText_Completata[] = _("Completata");
static const u8 sText_ReturnFor[]  = _("Torna a ");
static const u8 sText_ForPrize[]   = _("per il premio!");
static const u8 sText_CounterSlash[] = _("/");
static const u8 sText_Space[]        = _(" ");
static const u8 sText_FilterAll[]    = _("Tutte");
static const u8 sText_FilterActive[] = _("In corso");
static const u8 sText_FilterReward[] = _("Premio");
static const u8 sText_FilterDone[]   = _("Completate");
// Testi parziali per il suggerimento tasti (uniti a runtime con le icone)
static const u8 sText_HintR[]  = _(":Tipo ");
static const u8 sText_HintSt[] = _(":A-Z");

#define WIN_QUESTLIST   0
#define WIN_QUESTDETAIL 1
#define WIN_HEADER      2

#define FILTER_ALL    0
#define FILTER_ACTIVE 1
#define FILTER_REWARD 2
#define FILTER_DONE   3
#define FILTER_COUNT  4

#define MAX_VISIBLE_QUESTS 4

static u8    sQuestCursorPos;
static u8    sFilterMode;
static u8    sUnlockedQuestCount;
static u8    sUnlockedQuestIds[SIDE_QUEST_COUNT];
static u8    sNpcSpriteId;
static u16   sScrollOffset;
static bool8 sSortAlpha;

static void QuestMenu_MainCB(void);
static void QuestMenu_VBlankCB(void);
static void PrintQuestDetails(void);
static void ShowNpcSprite(void);
static void PrintCategoryHeader(void);

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

static const struct WindowTemplate sQuestMenuWindowTemplates[] =
{
    { // WIN_QUESTLIST
        .bg = 0,
        .tilemapLeft = 1,
        .tilemapTop = 2,
        .width = 22,
        .height = 9,
        .paletteNum = 15,
        .baseBlock = 1,
    },
    { // WIN_QUESTDETAIL  (baseBlock = 22*9+1 = 199)
        .bg = 0,
        .tilemapLeft = 6,
        .tilemapTop = 13,
        .width = 24,
        .height = 6,
        .paletteNum = 14,
        .baseBlock = 199,
    },
    { // WIN_HEADER (baseBlock = 199+144 = 343)
        .bg = 0,
        .tilemapLeft = 1,
        .tilemapTop = 0,
        .width = 28,
        .height = 2,
        .paletteNum = 13,
        .baseBlock = 343,
    },
    DUMMY_WIN_TEMPLATE
};

extern const u8 gText_SelectorArrow2[];

// ---- Helpers ----------------------------------------------------------------

static bool8 QuestMatchesFilter(u8 questId)
{
    bool8 active = CheckQuestUnlocked(questId) && !CheckQuestCompleted(questId);
    bool8 reward = CheckQuestUnlocked(questId) &&  CheckQuestCompleted(questId);
    bool8 done   = CheckQuestRewarded(questId);
    switch (sFilterMode)
    {
    case FILTER_ALL:    return active || reward || done;
    case FILTER_ACTIVE: return active;
    case FILTER_REWARD: return reward;
    case FILTER_DONE:   return done;
    }
    return FALSE;
}

// Selection sort on a sub-range of sUnlockedQuestIds by quest name
static void SortQuestsAlpha(u8 start, u8 count)
{
    u8 i, j, minIdx, tmp;
    if (count <= 1)
        return;
    for (i = start; i < start + count - 1; i++)
    {
        minIdx = i;
        for (j = i + 1; j < start + count; j++)
        {
            if (StringCompare(gSideQuests[sUnlockedQuestIds[j]].name,
                              gSideQuests[sUnlockedQuestIds[minIdx]].name) < 0)
                minIdx = j;
        }
        if (minIdx != i)
        {
            tmp = sUnlockedQuestIds[i];
            sUnlockedQuestIds[i] = sUnlockedQuestIds[minIdx];
            sUnlockedQuestIds[minIdx] = tmp;
        }
    }
}

static void BuildUnlockedQuestList(void)
{
    u8 i;

    sUnlockedQuestCount = 0;
    for (i = 0; i < SIDE_QUEST_COUNT; i++)
    {
        if (QuestMatchesFilter(i))
            sUnlockedQuestIds[sUnlockedQuestCount++] = i;
    }

    if (sSortAlpha)
        SortQuestsAlpha(0, sUnlockedQuestCount);

    if (sQuestCursorPos >= sUnlockedQuestCount)
        sQuestCursorPos = sUnlockedQuestCount > 0 ? sUnlockedQuestCount - 1 : 0;
}

static void AdjustScrollForCursor(void)
{
    if (sQuestCursorPos < sScrollOffset)
        sScrollOffset = sQuestCursorPos;
    else if (sUnlockedQuestCount > MAX_VISIBLE_QUESTS &&
             sQuestCursorPos >= sScrollOffset + MAX_VISIBLE_QUESTS)
        sScrollOffset = sQuestCursorPos - MAX_VISIBLE_QUESTS + 1;
}

// ---- Drawing ----------------------------------------------------------------

void PrintQuestsList(void)
{
    u8 i;
    u32 y;
    u8 questId;
    const u8 *statusText;
    const u8 *statusColor;
    const u8 *nameColor;
    s32 statusX;
    u8 visCount;
    u8 arrowBuf[2];
    const s32 winWidth = 22 * 8;

    AdjustScrollForCursor();
    FillWindowPixelBuffer(WIN_QUESTLIST, PIXEL_FILL(0));

    // ▲ indicator when scrolled down
    if (sScrollOffset > 0)
    {
        arrowBuf[0] = CHAR_UP_ARROW;
        arrowBuf[1] = EOS;
        AddTextPrinterParameterized4(WIN_QUESTLIST, FONT_NARROW, 0, 0, 0, 0, sQuestTextColor, -1, arrowBuf);
    }

    visCount = sUnlockedQuestCount > sScrollOffset ? sUnlockedQuestCount - sScrollOffset : 0;
    if (visCount > MAX_VISIBLE_QUESTS)
        visCount = MAX_VISIBLE_QUESTS;

    y = 0;
    for (i = 0; i < visCount; i++)
    {
        u8 listIdx = (u8)(sScrollOffset + i);
        questId = sUnlockedQuestIds[listIdx];
        nameColor = sQuestTextColor;

        if (listIdx == sQuestCursorPos)
            AddTextPrinterParameterized4(WIN_QUESTLIST, FONT_NARROW, 4, y, 0, 0, nameColor, -1, gText_SelectorArrow2);
        AddTextPrinterParameterized4(WIN_QUESTLIST, FONT_NARROW, 16, y, 0, 0, nameColor, -1, gSideQuests[questId].name);

        if (CheckQuestRewarded(questId))
        {
            statusText  = sText_Completata;
            statusColor = sQuestStatusGreenColor;
        }
        else if (CheckQuestCompleted(questId))
        {
            statusText  = sText_Completata;
            statusColor = sQuestStatusRedColor;
        }
        else
        {
            statusText  = sText_InCorso;
            statusColor = sQuestTextColor;
        }
        statusX = winWidth - GetStringWidth(FONT_NARROW, statusText, 0);
        AddTextPrinterParameterized4(WIN_QUESTLIST, FONT_NARROW, statusX, y, 0, 0, statusColor, -1, statusText);

        y += 16;
    }

    // ▼ indicator when more quests exist below
    if (sScrollOffset + MAX_VISIBLE_QUESTS < sUnlockedQuestCount)
    {
        arrowBuf[0] = CHAR_DOWN_ARROW;
        arrowBuf[1] = EOS;
        AddTextPrinterParameterized4(WIN_QUESTLIST, FONT_NARROW, 0, y, 0, 0, sQuestTextColor, -1, arrowBuf);
    }

    CopyWindowToVram(WIN_QUESTLIST, COPYWIN_FULL);
}

static void PrintQuestDetails(void)
{
    const struct SideQuest *quest;
    u8 questId;

    FillWindowPixelBuffer(WIN_QUESTDETAIL, PIXEL_FILL(0));
    if (sUnlockedQuestCount == 0)
    {
        CopyWindowToVram(WIN_QUESTDETAIL, COPYWIN_FULL);
        return;
    }

    questId = sUnlockedQuestIds[sQuestCursorPos];
    quest   = &gSideQuests[questId];

    // Luogo: [map]
    StringCopy(gStringVar4, sLabel_Luogo);
    StringAppend(gStringVar4, quest->map);
    AddTextPrinterParameterized4(WIN_QUESTDETAIL, FONT_SMALL, 0, 0, 0, 0, sQuestDetailColor, -1, gStringVar4);

    if (CheckQuestRewarded(questId))
    {
        // REWARDED: mostra donedesc, con fallback su desc
        const u8 *desc = (quest->donedesc != NULL) ? quest->donedesc : quest->desc;
        AddTextPrinterParameterized4(WIN_QUESTDETAIL, FONT_SMALL, 0, 14, 0, 0, sQuestDetailColor, -1, desc);
    }
    else if (CheckQuestCompleted(questId))
    {
        // REWARD: obiettivo completato, premio non ancora ritirato
        StringCopy(gStringVar4, sText_ReturnFor);
        StringAppend(gStringVar4, quest->map);
        AddTextPrinterParameterized4(WIN_QUESTDETAIL, FONT_SMALL, 0, 14, 0, 0, sQuestDetailColor, -1, gStringVar4);
        AddTextPrinterParameterized4(WIN_QUESTDETAIL, FONT_SMALL, 0, 28, 0, 0, sQuestDetailColor, -1, sText_ForPrize);
    }
    else
    {
        // ACTIVE: mostra desc
        AddTextPrinterParameterized4(WIN_QUESTDETAIL, FONT_SMALL, 0, 14, 0, 0, sQuestDetailColor, -1, quest->desc);
    }

    CopyWindowToVram(WIN_QUESTDETAIL, COPYWIN_FULL);
}

static void ShowNpcSprite(void)
{
    const struct SideQuest *quest;

    if (sNpcSpriteId < MAX_SPRITES)
    {
        DestroySprite(&gSprites[sNpcSpriteId]);
        sNpcSpriteId = MAX_SPRITES;
    }
    if (sUnlockedQuestCount == 0)
        return;

    quest = &gSideQuests[sUnlockedQuestIds[sQuestCursorPos]];

    if (quest->spritetype == QUEST_SPRITE_PKMN)
    {
        sNpcSpriteId = CreateMonIcon(quest->npcGraphicsId, SpriteCallbackDummy, 18, 132, 0, 0);
    }
    else
    {
        sNpcSpriteId = CreateObjectGraphicsSprite(quest->npcGraphicsId, SpriteCallbackDummy, 18, 132, 0);
        if (sNpcSpriteId < MAX_SPRITES)
        {
            gSprites[sNpcSpriteId].oam.priority = 0;
            StartSpriteAnim(&gSprites[sNpcSpriteId], ANIM_STD_FACE_SOUTH);
        }
    }
}

// Costruisce il buffer "icona:testo icona:testo ..." per i suggerimenti tasti
static void BuildHintString(u8 *buf)
{
    u8 *p = buf;
    const u8 *src;

    *p++ = CHAR_KEYPAD_ICON; *p++ = CHAR_R_BUTTON;
    for (src = sText_HintR;  *src != EOS; src++) *p++ = *src;
    *p++ = CHAR_KEYPAD_ICON; *p++ = CHAR_START_BUTTON;
    for (src = sText_HintSt; *src != EOS; src++) *p++ = *src;
    *p = EOS;
}

static void PrintCategoryHeader(void)
{
    static const u8 *const sFilterLabels[FILTER_COUNT] = {
        sText_FilterAll, sText_FilterActive, sText_FilterReward, sText_FilterDone,
    };
    u8 i, total = 0;
    u8 numBuf[8];
    u8 counterBuf[16];
    u8 hintBuf[48];
    const u8 *label = sFilterLabels[sFilterMode];
    s32 hintX;

    for (i = 0; i < SIDE_QUEST_COUNT; i++)
    {
        if (CheckQuestUnlocked(i) || CheckQuestCompleted(i) || CheckQuestRewarded(i))
            total++;
    }

    // Contatore "x/y"
    ConvertIntToDecimalStringN(numBuf, sUnlockedQuestCount, STR_CONV_MODE_LEFT_ALIGN, 2);
    StringCopy(counterBuf, numBuf);
    StringAppend(counterBuf, sText_CounterSlash);
    ConvertIntToDecimalStringN(numBuf, total, STR_CONV_MODE_LEFT_ALIGN, 2);
    StringAppend(counterBuf, numBuf);

    // Hint con icone tasti
    BuildHintString(hintBuf);
    hintX = 28 * 8 - GetStringWidth(FONT_NARROW, hintBuf, 0) - 1;

    // Compone "NomeFiltro x/y" come unica stringa a sinistra
    StringCopy(gStringVar4, label);
    StringAppend(gStringVar4, sText_Space);
    StringAppend(gStringVar4, counterBuf);

    FillWindowPixelBuffer(WIN_HEADER, PIXEL_FILL(0));
    AddTextPrinterParameterized4(WIN_HEADER, FONT_NARROW, 0,     2, 0, 0, sHeaderTextColor, -1, gStringVar4);
    AddTextPrinterParameterized4(WIN_HEADER, FONT_NARROW, hintX, 2, 1, 0, sHeaderTextColor, -1, hintBuf);
    CopyWindowToVram(WIN_HEADER, COPYWIN_FULL);
}

// ---- Init / Callbacks -------------------------------------------------------

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
            // Slot 15: lista quest
            Menu_LoadStdPalAt(BG_PLTT_ID(15));
            LoadPalette(sBlackColor,  BG_PLTT_ID(15) + 1, sizeof(u16));
            LoadPalette(sRedColor,    BG_PLTT_ID(15) + 4, sizeof(u16));
            LoadPalette(sGreenColor,  BG_PLTT_ID(15) + 5, sizeof(u16));
            // Slot 13: header — idx1=bianco (icone chiare), idx2=nero (testo)
            Menu_LoadStdPalAt(BG_PLTT_ID(13));
            LoadPalette(sBlackColor, BG_PLTT_ID(13) + 2, sizeof(u16));
            // Slot 14: dettagli quest
            Menu_LoadStdPalAt(BG_PLTT_ID(14));
            LoadPalette(sBlackColor,  BG_PLTT_ID(14) + 2, sizeof(u16));
            // Forza il buffer faded a nero prima del VBlank
            BlendPalettes(PALETTES_ALL, 16, RGB_BLACK);

            SetVBlankCallback(QuestMenu_VBlankCB);
            gMain.state++;
            break;

        case 2:
            CopyBgTilemapBufferToVram(1);

            SetGpuReg(REG_OFFSET_BG1VOFS, 0);
            SetGpuReg(REG_OFFSET_BG1HOFS, 0);

            BuildUnlockedQuestList();
            PutWindowTilemap(WIN_QUESTLIST);
            PutWindowTilemap(WIN_QUESTDETAIL);
            PutWindowTilemap(WIN_HEADER);
            PrintCategoryHeader();
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
            if (JOY_NEW(DPAD_UP))
            {
                PlaySE(SE_SELECT);
                if (sUnlockedQuestCount > 0)
                {
                    // Wrap-around: su dal primo → ultimo
                    sQuestCursorPos = (sQuestCursorPos == 0)
                        ? sUnlockedQuestCount - 1
                        : sQuestCursorPos - 1;
                }
                PrintQuestsList();
                PrintQuestDetails();
                ShowNpcSprite();
            }
            else if (JOY_NEW(DPAD_DOWN))
            {
                PlaySE(SE_SELECT);
                if (sUnlockedQuestCount > 0)
                {
                    // Wrap-around: giù dall'ultimo → primo
                    sQuestCursorPos = (sQuestCursorPos >= sUnlockedQuestCount - 1)
                        ? 0
                        : sQuestCursorPos + 1;
                }
                PrintQuestsList();
                PrintQuestDetails();
                ShowNpcSprite();
            }
            else if (JOY_NEW(R_BUTTON))
            {
                PlaySE(SE_SELECT);
                sFilterMode = (sFilterMode + 1) % FILTER_COUNT;
                sQuestCursorPos = 0;
                sScrollOffset   = 0;
                BuildUnlockedQuestList();
                PrintCategoryHeader();
                PrintQuestsList();
                PrintQuestDetails();
                ShowNpcSprite();
            }
            else if (JOY_NEW(START_BUTTON))
            {
                PlaySE(SE_SELECT);
                sSortAlpha = !sSortAlpha;
                BuildUnlockedQuestList();
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
