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
#include "sound.h"
#include "constants/songs.h"
#include "constants/rgb.h"

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
    // Queste chiamate aggiornano le componenti visive ogni frame.
    // UpdatePaletteFade() è la chiave per far funzionare la sfumatura.
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
            gMain.state++;
            break;

        case 1:
            LZ77UnCompVram(gQuestMenu_Gfx, (void *)(VRAM + 0x4000)); 
            LZ77UnCompWram(gQuestMenu_Tilemap, GetBgTilemapBuffer(1));
            
            LoadPalette(gQuestMenu_Pal, 0, 32);
            LoadPalette(sQuestMenuTextPalette, 15 * 16, 32);
            BlendPalettes(PALETTES_ALL, 16, RGB_BLACK);

            SetVBlankCallback(QuestMenu_VBlankCB);
            gMain.state++;
            break;

        case 2:
            CopyBgTilemapBufferToVram(1);
            
            // 1. Reset degli offset (fondamentale per evitare che il BG "salti")
            SetGpuReg(REG_OFFSET_BG1VOFS, 0); 
            SetGpuReg(REG_OFFSET_BG1HOFS, 0); 

            PutWindowTilemap(0);
            PrintQuestsList();
            
            // 2. FORZA TUTTO AL NERO prima di mostrare qualsiasi cosa
            // Questo sovrascrive istantaneamente le vecchie palette dell'overworld
            BlendPalettes(PALETTES_ALL, 16, RGB_BLACK);
            
            // 3. Ora puoi rendere visibili i BG e accendere il display.
            // Poiché abbiamo usato BlendPalettes, vedrai solo NERO.
            ShowBg(0);
            ShowBg(1);
            SetGpuReg(REG_OFFSET_DISPCNT, DISPCNT_MODE_0 | DISPCNT_OBJ_1D_MAP | DISPCNT_BG0_ON | DISPCNT_BG1_ON | DISPCNT_OBJ_ON);
            
            // 4. Avvia il Fade-in (da Nero a Colori Normali)
            BeginNormalPaletteFade(PALETTES_ALL, 0, 16, 0, RGB_BLACK);
            
            gMain.state++;
            break;

        case 3:
            // Aspetta che il fade-in sia completato prima di passare allo stato degli input
            if (!gPaletteFade.active)
            {
                gMain.state++;
            }
            break;

        case 4:
            // Gestione Input
            if (JOY_NEW(B_BUTTON))
            {
                PlaySE(SE_SELECT); // Aggiungiamo il suono di sistema standard per l'uscita
                
                // Novità: Inizia la sfumatura da NORMALE a NERO (Fade Out)
                BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
                gMain.state++;
            }
            break;
            
        case 5:
            // Novità: Aspettiamo che il Fade Out sia completamente nero
            if (!gPaletteFade.active)
            {
                Free(GetBgTilemapBuffer(1));
                FreeAllWindowBuffers();
                SetMainCallback2(CB2_ReturnToFieldWithOpenMenu);
            }
            break;
    }
}