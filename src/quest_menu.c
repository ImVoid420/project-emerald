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
#include "quests.h" // Importante per la struttura SideQuest

// Risorse grafiche
extern const u32 gQuestMenu_Gfx[];      
extern const u32 gQuestMenu_Tilemap[];  
extern const u32 gQuestMenu_Pal[];    

// Palette di sistema per i testi (Nero su Trasparente)
static const u16 sQuestMenuTextPalette[] = {
    0x7FFF, // 0: Trasparente
    0x0000, // 1: Nero (Testo)
    0x318C, // 2: Grigio (Ombra)
    0x7FFF, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15
};

static void QuestMenu_MainCB(void);
static void QuestMenu_VBlankCB(void);

// 1. Definizione dei Background
static const struct BgTemplate sQuestMenuBgTemplates[] =
{
    {
        .bg = 0, // Layer Testi
        .charBaseIndex = 0,
        .mapBaseIndex = 31,
        .screenSize = 0,
        .paletteMode = 0,
        .priority = 0,
        .baseTile = 0
    },
    {
        .bg = 1, // Layer Diario (PNG 128x32)
        .charBaseIndex = 1,
        .mapBaseIndex = 30,
        .screenSize = 0,
        .paletteMode = 0,
        .priority = 1,
        .baseTile = 0
    }
};

// 2. Finestra centrata sulla grafica 128x32
static const struct WindowTemplate sQuestMenuWindowTemplates[] =
{
    {
        .bg = 0,
        .tilemapLeft = 2,   // Margine sinistro (2 tile = 16px)
        .tilemapTop = 4,    // Margine superiore (regolalo per centrare il testo nel diario)
        .width = 26,        // Occupa quasi tutto lo schermo in larghezza
        .height = 12,       // Spazio per circa 6 missioni
        .paletteNum = 15,
        .baseBlock = 1,
    },
    DUMMY_WIN_TEMPLATE
};

// 3. Funzione stampa: ora usa gSideQuests e CheckQuestUnlocked
void PrintQuestsList(void)
{
    u8 i;
    u32 y = 0;
    static const u8 color[3] = {0, 1, 2}; 

    FillWindowPixelBuffer(0, PIXEL_FILL(0));

    for (i = 0; i < SIDE_QUEST_COUNT; i++)
    {
        // Usa la funzione definita in quests.c
        if (CheckQuestUnlocked(i))
        {
            AddTextPrinterParameterized4(0, 1, 0, y, 0, 0, color, -1, gSideQuests[i].name);
            y += 16;
        }
    }
    CopyWindowToVram(0, 3);
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
    switch (gMain.state)
    {
        case 0:
            SetVBlankCallback(NULL);
            SetGpuReg(REG_OFFSET_DISPCNT, 0);
            
            // RESET TOTALE OFFSET: La tilemap (.bin) gestisce la posizione dei tile
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
            gMain.state++;
            break;

        case 1:
            // Caricamento Tileset e Tilemap
            LZ77UnCompVram(gQuestMenu_Gfx, (void *)(VRAM + 0x4000)); 
            LZ77UnCompWram(gQuestMenu_Tilemap, GetBgTilemapBuffer(1));
            
            LoadPalette(gQuestMenu_Pal, 0, 32); 
            LoadPalette(sQuestMenuTextPalette, 15 * 16, 32);
            
            SetVBlankCallback(QuestMenu_VBlankCB);
            gMain.state++;
            break;

        case 2:
            CopyBgTilemapBufferToVram(1);
            
            // Assicuriamoci che gli offset rimangano a 0 prima di mostrare i BG
            SetGpuReg(REG_OFFSET_BG1VOFS, 0); 
            SetGpuReg(REG_OFFSET_BG1HOFS, 0); 

            PutWindowTilemap(0);
            PrintQuestsList();
            
            ShowBg(0);
            ShowBg(1);
            
            // Attivazione display
            SetGpuReg(REG_OFFSET_DISPCNT, DISPCNT_MODE_0 | DISPCNT_OBJ_1D_MAP | DISPCNT_BG0_ON | DISPCNT_BG1_ON | DISPCNT_OBJ_ON);
            gMain.state++;
            break;

        case 3:
            if (JOY_NEW(B_BUTTON))
            {
                Free(GetBgTilemapBuffer(1));
                FreeAllWindowBuffers();
                SetMainCallback2(CB2_ReturnToFieldWithOpenMenu);
            }
            break;
    }
}