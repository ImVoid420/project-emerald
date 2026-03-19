# Project Emerald — pokeemerald-expansion

## Base repository
- **pokeemerald-expansion** by rh-hideout: https://github.com/rh-hideout/pokeemerald-expansion
- Fork di pokeemerald (GBA Pokémon Emerald decompilation) con molte feature aggiuntive

## Custom features aggiunte

### Quest Menu
- Implementato seguendo la guida: https://github.com/PokemonSanFran/pokeemerald/wiki/Unbound-Quest-Menu
- File principali: `src/quest_menu.c`, `include/quest_menu.h`, `src/quests.c`, `include/quests.h`, `include/constants/quests.h`
- Accessibile dal menu Start con la voce "QUESTS" (`MENU_ACTION_QUESTS` in `src/start_menu.c`)
- La logica di unlock delle quest usa `CheckQuestUnlocked(i)` e `gSideQuests[]`
- Grafica: `gQuestMenu_Gfx`, `gQuestMenu_Tilemap`, `gQuestMenu_Pal` (risorse esterne)

## Note tecniche

### Palette fade (quest menu)
`LoadPalette` scrive sia in `gPlttBufferUnfaded` che in `gPlttBufferFaded`. Se si attiva il VBlank callback dopo `LoadPalette`, il buffer faded (con i colori reali) viene trasferito all'hardware PLTT prima che `BlendPalettes` lo sovrascriva con il nero. Questo causa un flash di colore visibile quando il display viene riacceso. **Fix**: chiamare `BlendPalettes(PALETTES_ALL, 16, RGB_BLACK)` subito dopo `LoadPalette` e prima di `SetVBlankCallback`.
