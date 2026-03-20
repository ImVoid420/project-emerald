# Project Emerald — pokeemerald-expansion

## Base repository
- **pokeemerald-expansion** by rh-hideout: https://github.com/rh-hideout/pokeemerald-expansion
- Fork di pokeemerald (GBA Pokémon Emerald decompilation) con molte feature aggiuntive

## Custom features aggiunte

### Quest Menu
- Implementato seguendo la guida: https://github.com/PokemonSanFran/pokeemerald/wiki/Unbound-Quest-Menu
- File principali: `src/quest_menu.c`, `include/quest_menu.h`, `src/quests.c`, `include/quests.h`, `include/constants/quests.h`
- Accessibile dal menu Start con la voce "QUESTS" (`MENU_ACTION_QUESTS` in `src/start_menu.c`)
- Grafica: `gQuestMenu_Gfx`, `gQuestMenu_Tilemap`, `gQuestMenu_Pal` (risorse esterne)

#### Stati delle quest (3 stati, 0 byte extra in SaveBlock2)
Codifica a 2 bit usando due array già esistenti (`unlockedQuests[]`, `completedQuests[]`):
| Stato | unlocked | completed | Descrizione |
|---|---|---|---|
| ACTIVE | 1 | 0 | Quest in corso |
| REWARD | 1 | 1 | Obiettivo completato, premio non ritirato |
| REWARDED | 0 | 1 | Premio ritirato (completata definitivamente) |

`FLAG_GET_REWARDED` = `completedQuests=1 AND unlockedQuests=0`
`FLAG_SET_REWARDED` = setta completed, azzera unlocked

#### Script commands (`src/scrcmd.c`, `asm/macros/event.inc`)
| Opcode | Macro | Effetto |
|---|---|---|
| 0xe7 | `startquest QUEST` | FLAG_SET_UNLOCKED |
| 0xe8 | `completequest QUEST` | FLAG_SET_COMPLETED |
| 0xe9 | `checkquest QUEST, mode` | 0=unlocked, 1=completed, 2=rewarded → gSpecialVar_Result |
| 0xea | `rewardquest QUEST` | FLAG_SET_REWARDED |

Esempio flusso script (Oldale Town Pokemon Center):
```
checkquest SIDE_QUEST_1, 2  → se rewarded → QuestDone
checkquest SIDE_QUEST_1, 1  → se completed → RewardQuest (rewardquest)
checkquest SIDE_QUEST_1, 0  → se active → QuestActive (completequest)
altrimenti                  → startquest
```

#### Funzionalità implementate
- Scroll ▲/▼ con max 4 quest visibili (`sScrollOffset`, `MAX_VISIBLE_QUESTS=4`)
- Contatore `x/y` nell'header (quest nel filtro / totale con qualsiasi stato)
- Navigazione wrap-around (su dal primo → ultimo, giù dall'ultimo → primo)
- Filtro per stato con tasto R (Tutte / In corso / Premio / Completate)
- Ordinamento A-Z con tasto Start (`sSortAlpha`)
- Testo premio nel pannello dettagli ("Torna a [map] / per il premio!") in stato REWARD
- Fallback su `desc` se `donedesc` è NULL
- Sprite NPC overworld (`QUEST_SPRITE_OBJECT`) e icona Pokémon (`QUEST_SPRITE_PKMN`) nel riquadro bianco
- Etichette stato colorate a destra del nome: **In corso** (nero), **Completata** (rosso = REWARD, verde = REWARDED)
- Icone tasti R e Start nell'header tramite `CHAR_KEYPAD_ICON` + `CHAR_R_BUTTON`/`CHAR_START_BUTTON`

#### Palette layout
| Slot | Uso | Note |
|---|---|---|
| 15 | Lista quest (WIN_QUESTLIST) | idx1=nero (testo), idx4=rosso, idx5=verde |
| 14 | Dettagli quest (WIN_QUESTDETAIL) | testo bianco su sfondo blu |
| 13 | Header (WIN_HEADER) | idx1=bianco (icone tasti chiare), idx2=nero (testo) |

#### Struct SideQuest (`include/quests.h`)
```c
struct SideQuest {
    const u8* name;
    const u8* desc;       // descrizione mentre la quest è attiva
    const u8* donedesc;   // descrizione dopo aver completato l'obiettivo (può essere NULL)
    const u8* poc;
    const u8* map;
    const u8* reward;
    u16 npcGraphicsId;    // OBJ_EVENT_GFX_* oppure species ID se QUEST_SPRITE_PKMN
    u8 spritetype;        // QUEST_SPRITE_OBJECT o QUEST_SPRITE_PKMN
};
```

#### TODO futuri (da implementare)
- [ ] **Quest bloccate come `????`** — mostrare TUTTE le quest nel menu con `????` finché non vengono sbloccate (attualmente mostriamo solo quelle attive/completate)
- [ ] **Subquest** — quest con sotto-obiettivi
- [ ] **`FLAG_SYS_QUEST_MENU_GET`** — flag per mostrare/nascondere la voce nel menu Start
- [ ] **Macro script di convenienza** — `goto_if_quest_active`, `goto_if_quest_complete`, `returnqueststate` ecc.
- [ ] **Reset `sSortAlpha` all'uscita** — attualmente l'ordinamento A-Z persiste tra aperture; nella repo PSF si resetta all'uscita

### PokéVial
- Riferimento: https://www.pokecommunity.com/threads/pok%C3%A9vial.500216/
- File principali: `src/item_use.c`, `data/scripts/field_move_scripts.inc`, `data/scripts/pkmn_center_nurse.inc`
- Item ID: `ITEM_POKE_VIAL` (vedere `include/constants/items.h`)
- Cariche gestite tramite `VAR_POKE_VIAL_CHARGES` (`0x40F9`)
- Il Pokécenter dà l'oggetto e ricarica la variabile (`data/scripts/pkmn_center_nurse.inc`)

#### Funzionamento attuale
- Usabile da borsa e da SELECT (key item wheel)
- Cura HP e status di tutti i Pokémon della squadra
- Se carica = 0: mostra messaggio "Il PokéVial è vuoto..."
- Decrementa la variabile ad ogni uso e mostra le cariche rimanenti

#### TODO — da fixare/migliorare
- [ ] **PP non ripristinati** — bug funzionale: il loop in `ItemUseOutOfBattle_PokeVial` cura solo HP+status, mancano le mosse. Fix: aggiungere loop su `MON_DATA_PP` con `CalculatePPWithBonus`
- [ ] **Animazione di cura** — l'originale PSF fa fade + party menu animation come al Pokécenter; attualmente si mostra solo un messaggio
- [ ] **Sistema dose/size** — l'originale distingue "cariche attuali" (dose) e "capacità massima" (size, aumentabile con ricompense); noi abbiamo solo un numero fisso
- [ ] **Icona dinamica** — l'originale ha 11 stati grafici (0-100% pieno); richiede nuova grafica

### Party Menu — Shortcut
- **SELECT** su un Pokémon → lo sposta direttamente in prima posizione (via `SwitchSelectedMons`)
- **L** su un Pokémon → entra in modalità sposta senza aprire il mini-menu; **L di nuovo** → conferma lo spostamento
- Implementato in `Task_HandleChooseMonInput` (`src/party_menu.c`)

### Key Item Wheel (ORAS-style)
- Riferimento: https://www.pokecommunity.com/threads/oras-style-key-item-wheel.498877/
- File principali: `src/item_menu.c`, `src/item_icon.c`, `include/item_icon.h`, `include/item_menu.h`
- Grafica: `graphics/bag/key_item_box.png/.gbapal`, `graphics/bag/select_button_right/down/left.png`

#### Architettura
- Fino a 4 key item registrabili simultaneamente in `gSaveBlock1Ptr->registeredItems[4]`
- `registeredItemCompat` mantiene compat con il vecchio slot singolo
- `MAX_REMATCH_ENTRIES` ridotto 100→92 per fare spazio agli 8 byte di `registeredItems[4]`
- `RegisteredItemIndex(item)` — restituisce lo slot (0-3) di un item, o -1 se non registrato
- `CountRegisteredItems()` — conta gli item registrati con migrazione automatica da `registeredItemCompat`

#### Registrazione
- Da borsa → menu contestuale → "REGISTER": se già registrato → deregistra; se 0 item registrati → slot 0 automatico; altrimenti → mostra prompt "Press any D-pad key" → `Task_RegisterUsingDpad`
- Icone direzionali mostrate nella lista accanto all'item registrato (↑ slot 0, → slot 1, ↓ slot 2, ← slot 3)

#### Uso (SELECT in overworld)
- 1 item registrato → usa direttamente
- 2-4 item registrati → apre il wheel overlay (croce centrata sullo schermo)
- Il wheel usa HBlank scanline trick per la palette del 4° item (`sKeyItemWheelExtraPalette`, palette 13)
- In dark cave: attiva OBJWIN sprites per mostrare le box

#### SaveBlock1 layout
| Offset | Campo | Note |
|---|---|---|
| 0x496 | `registeredItemCompat` | vecchio slot singolo, mantenuto per compat |
| 0x9CA | `trainerRematches[92]` | ridotto da 100 (78 usati, margine sufficiente) |
| 0xA26 | `registeredItems[4]` | 4 slot × u16 = 8 byte |

## Tool disponibili
- **Porymap** — editor visuale per mappe, eventi, connessioni, wild encounters
- **Tilemap Studio** — editor per tilemap/grafica
- **HexManiac Advance** — editor esadecimale per la ROM

## Note tecniche

### Palette fade (quest menu)
`LoadPalette` scrive sia in `gPlttBufferUnfaded` che in `gPlttBufferFaded`. Se si attiva il VBlank callback dopo `LoadPalette`, il buffer faded (con i colori reali) viene trasferito all'hardware PLTT prima che `BlendPalettes` lo sovrascriva con il nero. Questo causa un flash di colore visibile quando il display viene riacceso. **Fix**: chiamare `BlendPalettes(PALETTES_ALL, 16, RGB_BLACK)` subito dopo `LoadPalette` e prima di `SetVBlankCallback`.

### SaveBlock2 overflow
Aggiungere array extra a `SaveBlock2` può causare `size of array 'SaveBlock2FreeSpace' is negative`. Il sistema quest usa encoding a 2 bit sui due array esistenti (`unlockedQuests[]`, `completedQuests[]`) per evitare di aggiungere byte.

### Icone tasto keypad nel testo
Per mostrare icone grafiche dei tasti GBA in una stringa, serve la sequenza a due byte: `CHAR_KEYPAD_ICON (0xF8)` seguito dall'ID icona (`CHAR_R_BUTTON=0x03`, `CHAR_START_BUTTON=0x04`, `CHAR_SELECT_BUTTON=0x05`). Funzione: `DrawKeypadIcon` in `src/text.c`. Altezza icone: 12px (A/B=8px wide, L/R=16px, Start/Select=24px).
