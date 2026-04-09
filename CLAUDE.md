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

### BetterBag (8 tasche)
- Ispirato a: https://github.com/AsparagusEduardo/pokeemerald/tree/BetterBag
- File principali: `include/constants/item.h`, `include/global.h`, `src/item.c`, `src/item_menu.c`, `src/item_menu_icons.c`, `src/strings.c`, `src/data/items.h`

#### Tasche (enum Pocket, 0-based)
| Valore | Nome | Costante dimensione |
|---|---|---|
| 0 | ITEMS | `BAG_ITEMS_COUNT` = 30 |
| 1 | MEDICINE | `BAG_MEDICINE_COUNT` = 25 |
| 2 | POKÉ BALLS | `BAG_POKEBALLS_COUNT` = 16 |
| 3 | BATTLE | `BAG_BATTLEITEMS_COUNT` = 20 |
| 4 | BERRIES | `BAG_BERRIES_COUNT` = 46 |
| 5 | TREASURES | `BAG_TREASURES_COUNT` = 25 |
| 6 | TM/HM | `BAG_TMHM_COUNT` = 64 |
| 7 | KEY ITEMS | `BAG_KEYITEMS_COUNT` = 30 |

#### Classificazione automatica degli item
Gli item sono stati riclassificati in `src/data/items.h` in base al campo `.sortType`:
- `MEDICINE` ← `HEALTH_RECOVERY`, `STATUS_RECOVERY`, `PP_RECOVERY`
- `BATTLE` ← `X_ITEM`, `AUX_ITEM`, `BATTLE_ITEM`, `FLUTE`
- `TREASURES` ← `SELLABLE`, `SHARD`, `RELIC`, `FOSSIL`

#### Note tecniche
- `FREE_MYSTERY_EVENT_BUFFERS TRUE` in `include/config/save.h` — libera 1104 byte per compensare le 3 nuove tasche (+280 byte in SaveBlock1)
- I puntini indicatore tasca vengono disegnati tutti all'apertura dello zaino (tutti inattivi), poi quello attivo viene evidenziato; evita il bug dei puntini "comparsi" navigando
- `sBagSpriteAnimTable` in `src/item_menu_icons.c` usa designated initializers: le nuove tasche riusano `sSpriteAnim_Bag_Items` per evitare NULL/corruzione sprite

### Surfable Pokémon
- Ispirato a: https://github.com/Slawter666/pokeemerald/tree/surfable
- File principale: `src/field_effect_helpers.c` (funzione `FldEff_SurfBlob`)
- Usa il sistema `OW_POKEMON_OBJECT_EVENTS TRUE` già presente in pokeemerald-expansion

#### Funzionamento
Quando il giocatore sale sul surf, `FldEff_SurfBlob` cerca nella squadra il primo Pokémon vivo che conosce Surf. Se trovato, crea uno sprite overworld del Pokémon (con palette shiny se è shiny, female se è femmina) usando `CreateObjectGraphicsSpriteWithTag`. Il callback di movimento `UpdateSurfBlobFieldEffect` è lo stesso del blob originale (bobbing, sync posizione/animazione). Se nessun Pokémon con Surf è trovato (caso impossibile in gameplay normale), fa fallback al blob originale.

#### Cosa testare
- Surfa con un Pokémon non-shiny → vedi lo sprite del Pokémon
- Surfa con un Pokémon shiny → palette shiny
- Surfa con una Pokémon femmina con sprite diverso → sprite femmina
- Il Pokémon bobba sull'acqua e si orienta nella direzione corretta

### Lava Surfing
- Ispirato a Pokémon Unbound
- File modificati: `include/constants/metatile_behaviors.h`, `src/metatile_behavior.c`, `include/metatile_behavior.h`, `src/field_player_avatar.c`, `include/field_player_avatar.h`, `src/field_control_avatar.c`

#### Funzionamento
I tile di lava usano il comportamento `MB_LAVA`. Premendo A verso un tile `MB_LAVA`, se il giocatore ha in squadra un Pokémon di tipo Fuoco che conosce Surf, parte il surf normalmente (stessa animazione/HM). Senza un Pokémon Fuoco+Surf, l'interazione non fa nulla.

#### Implementazione
- `MB_LAVA` ha flags `TILE_FLAG_SURFABLE | TILE_FLAG_LAVA` — il giocatore può muoversi sul tile mentre surfa, e il tile è identificabile come lava
- `MetatileBehavior_IsLava(u8)` — controlla `TILE_FLAG_LAVA`
- `IsPlayerFacingLava()` — come `IsPlayerFacingSurfableFishableWater` ma per lava
- `PartyHasFireMonWithSurf()` — cerca nella squadra un Pokémon Fuoco (`IsSpeciesOfType(species, TYPE_FIRE)`) con Surf
- In `GetInteractedWaterScript` (`src/field_control_avatar.c`): se `IsPlayerFacingLava() && PartyHasFireMonWithSurf()` → avvia `EventScript_UseSurf`

#### Come usare in Porymap
Assegna il comportamento `MB_LAVA` ai tile di lava nella mappa. Il tile deve avere elevation 3 (come l'acqua) per far funzionare correttamente il sistema di collisione surf.

#### ⚠️ BUG DESIGN — da rifare prima di testare
L'implementazione attuale richiede un Pokémon Fuoco che conosca Surf (`PartyHasFireMonWithSurf`), ma i Pokémon Fuoco non imparano Surf naturalmente → la feature è inutilizzabile.

**Design corretto da implementare:**
1. Condizione attivazione: `IsFieldMoveUnlocked(FIELD_MOVE_SURF)` + `PartyHasLavaResistantMon()` (tipo Fuoco in squadra, senza richiedere la mossa Surf)
2. Sprite mostrato: primo Pokémon di tipo adatto in squadra (non il Surf-user)

**Modifiche necessarie:**
- `PartyHasFireMonWithSurf()` → sostituire con `PartyHasLavaResistantMon()` che controlla solo il tipo (Fuoco, eventualmente anche Roccia/Terra), non la mossa
- `GetInteractedWaterScript` in `src/field_control_avatar.c`: usare `IsFieldMoveUnlocked(FIELD_MOVE_SURF) && IsPlayerFacingLava() && PartyHasLavaResistantMon()`
- `FldEff_SurfBlob` in `src/field_effect_helpers.c`: quando il giocatore è su tile `MB_LAVA`, cercare il primo Pokémon lava-resistente invece del Surf-user per lo sprite

**Nota sprite surf su lava:** attualmente `FldEff_SurfBlob` mostra il primo Pokémon con Surf, quindi sull'acqua e sulla lava verrebbe mostrato lo stesso Pokémon. Con il fix, sulla lava verrà mostrato il Pokémon Fuoco.

**Nota sprite sovrapposti:** alcuni Pokémon alti (es. Goodra) si sovrappongono bruttamente al personaggio perché `UpdateBobbingEffect` posiziona lo sprite con `y = playerSprite->y + 8`, pensato per sprite bassi/piatti. Kyogre e simili funzionano bene. Fix futuro: usare `graphicsInfo->height` per calcolare un offset y dinamico.

### Item Drop da selvatici
- Ispirato a: https://github.com/AgustinGDLV/pokeemerald-rom-hack/compare/item_drops?expand=1
- Quando un Pokémon selvatico che tiene un oggetto viene KO, l'oggetto va nello zaino del giocatore
- Non funziona contro i trainer (`BATTLE_TYPE_TRAINER | BATTLE_TYPE_FIRST_BATTLE | BATTLE_TYPE_WALLY_TUTORIAL`)

#### Implementazione
- `BS_SaveFaintedBattlerItem` (native) — chiamata in `BattleScript_FaintBattler` subito dopo `printstring STRINGID_BATTLERFAINTED`; salva `gBattleMons[gBattlerFainted].item` in `gBattleHistory->heldItems[gBattlerFainted]`
- `BS_GiveDroppedItems` (native) — chiamata tramite macro `givedroppeditems` in `BattleScript_PayDayMoneyAndPickUpItems` (fine battaglia); itera gli avversari (1 o 2 in doppio), chiama `AddBagItem`, poi esegue `BattleScript_ItemDropped` con `BattleScriptCall`
- `BattleScript_ItemDropped` — suono + `printfromtable gItemDroppedStringIds` (B_MSG_ITEM_DROPPED o B_MSG_BAG_IS_FULL)
- Stringa usa `{B_SCR_NAME_WITH_PREFIX}` (= `gBattleScripting.battler`, impostato a `battler` prima di `BattleScriptCall`) e `{B_LAST_ITEM}`

#### File modificati
- `asm/macros/battle_script.inc` — macro `savefaintedbattleritem` e `givedroppeditems`
- `data/battle_scripts_1.s` — `savefaintedbattleritem` in `BattleScript_FaintBattler`, `givedroppeditems` in `BattleScript_PayDayMoneyAndPickUpItems`, `BattleScript_ItemDropped` aggiunto in fondo
- `include/battle_scripts.h` — `extern BattleScript_ItemDropped[]`
- `include/constants/battle_string_ids.h` — `STRINGID_PKMNDROPPEDITEM`, `STRINGID_BAGISFULL`, enum `ItemDroppedStringID`
- `src/battle_message.c` — stringhe IT + `gItemDroppedStringIds`
- `src/battle_script_commands.c` — `BS_SaveFaintedBattlerItem` e `BS_GiveDroppedItems`

#### Note tecniche
- In pokeemerald-expansion `Cmd_various` è deprecato → usare `callnative` per nuove funzioni
- `B_SCR_ACTIVE_NAME_WITH_PREFIX` non esiste nel charmap → usare `{B_SCR_NAME_WITH_PREFIX}` (token `FD 13`) che legge `gBattleScripting.battler`
- `savebattleritem` esistente usa `gBattlerTarget`, non `gBattlerFainted` → necessaria nuova native

#### Come testare
Trovare un Pokémon selvatico con held item (es. Zigzagoon su Route 101 tiene Potion al 50%, Shroomish in Petalburg Woods tiene Tiny Mushroom). Mandarlo KO → dovrebbe comparire il messaggio e l'item in zaino.

## Gimmick di battaglia attivi
| Gimmick | Stato | Config |
|---|---|---|
| Mega Evoluzioni | **Attive** | `P_MEGA_EVOLUTIONS TRUE` in `include/config/species_enabled.h` |
| Z-Mosse | **Attive** (no toggle globale) | Sempre disponibili se il Pokémon le ha nei dati |
| Dynamax/Gigamax | **Disabilitato in battaglia** | `B_FLAG_DYNAMAX_BATTLE 0` in `include/config/battle.h` — le forme Gigamax esistono nei dati (`P_GIGANTAMAX_FORMS TRUE`) ma il Dynamax non è attivabile in battaglia |

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

### Token battle string nei COMPOUND_STRING
I placeholder `{B_...}` nelle stringhe di battaglia sono risolti da `charmap.txt` (non da codice C). Solo i token definiti lì sono validi. Token utili: `{B_SCR_NAME_WITH_PREFIX}` = `FD 13` (usa `gBattleScripting.battler`), `{B_ATK_NAME_WITH_PREFIX}` = `FD 0F`, `{B_LAST_ITEM}` = `FD 16`, `{WAIT_SE}` = `FC 0A`. `B_SCR_ACTIVE_NAME_WITH_PREFIX` NON esiste nel charmap — usare `B_SCR_NAME_WITH_PREFIX`.

### Bag menu — Sort hint (icona START + "Ordina")
Finestra `WIN_SORT_HINT` in `src/item_menu.c` sopra la lista oggetti (lato destro dello schermo). Palette 15 modificata: idx1=bianco (icona), idx2=nero (testo, override con `RGB_BLACK` dopo `LoadPalette`). Finestra `tilemapTop=0, height=3` con testo a `y=3` per centrare visivamente nella striscia giallo scuro (tile row 1 = screen y=8-15). Allineamento a destra calcolato con `GetStringWidth`.

## Cose da testare

- **Item Drop da selvatici** — trovare un selvatico con held item (es. Zigzagoon su Route 101 tiene Potion al 50%, Shroomish in Petalburg Woods tiene Tiny Mushroom al 50%), mandarlo KO → deve comparire il messaggio "[Pokémon] ha lasciato cadere [oggetto]!" e l'item in zaino
