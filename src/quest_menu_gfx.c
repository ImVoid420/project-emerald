#include "global.h"

// I file LZ sono compressi, il PNG genera automaticamente la palette
const u32 gQuestMenu_Gfx[] = INCBIN_U32("graphics/quest_menu/menu.4bpp.lz");
const u32 gQuestMenu_Tilemap[] = INCBIN_U32("graphics/quest_menu/menu.bin.lz");

// Questa riga è fondamentale: anche se hai eliminato il file .pal fisico, 
// il Makefile genererà gQuestMenu_Pal estraendolo dal PNG.
const u32 gQuestMenu_Pal[] = INCBIN_U32("graphics/quest_menu/menu.gbapal");