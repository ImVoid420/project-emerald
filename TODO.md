# TODO.md - Hoenn Next

Documento operativo per lo sviluppo della hackrom su `pokeemerald-expansion`.
Ogni task deve dire non solo cosa fare, ma anche dove intervenire, come farlo e come verificarlo.

---

## STATO TEST FEATURE CUSTOM (aggiornato 2026-08-30)

Registro di cosa e' stato testato a schermo (non solo verificato nel codice) di
tutto cio' che non fa parte di Pokemon Smeraldo originale. Aggiornare questa
tabella ad ogni sessione di test, cosi' si sa sempre con precisione a che punto
si e'.

| Feature | Stato | Note |
|---|---|---|
| Pokedex (HGSS style, Nazionale da subito) | ✅ OK | Confermato dall'utente 2026-08-30 |
| DexNav | 🔧 Fix applicato, da ritestare | Bug: con l'autorun attivo serviva tenere sia B che A per entrare in "creeping mode" durante la ricerca. Causa: in `src/field_player_avatar.c`, il controllo `DN_FLAG_SEARCHING` veniva dopo il blocco autorun (che faceva `return` prima). Spostato prima, con return anticipato, come gia' avveniva per il caso surf. |
| IV/EV nella schermata Pokemon | 🔧 Fix applicato, da ritestare | IV ok, EV mostrava lettere (F/D/C/B/A/S) invece di numeri. Causa: `P_SUMMARY_SCREEN_IV_EV_VALUES` era FALSE in `include/config/summary_screen.h`. Impostato a TRUE. |
| Move Relearner | ✅ OK, ma da ribilanciare | Spostato dalla Summary Screen al **Party Menu**. Reso **sempre attivo** il 2026-08-30 (rimosso temporaneamente il controllo su `FLAG_SYS_MOVE_TUTOR_ENABLED` in `src/party_menu.c`, commentato per poterlo ripristinare). **Decisioni rimandate:** (1) troppo sgravato a inizio gioco secondo l'utente — va deciso quando/come sbloccarlo via storia; (2) da decidere se permettere di insegnare qualsiasi mossa (`P_ENABLE_ALL_TM_MOVES TRUE`/`P_ENABLE_ALL_LEVEL_UP_MOVES`) o limitarlo. Nessuna scadenza, se ne riparla più avanti. |
| Zaino (8 tasche, grafica custom, sorting) | ✅ OK | Confermato dall'utente 2026-08-30, incluso il fix del colore del prompt ordinamento |
| Sprite Pokemon durante Surf | ⚠️ Funziona con bug noto | Lo sprite del giocatore a volte va sopra quello del Pokemon (soprattutto Pokemon alti). Gia' documentato in `CLAUDE.md` sezione "Surfable Pokemon": `UpdateBobbingEffect` usa un offset Y fisso pensato per sprite bassi. Fix futuro: offset dinamico basato su `graphicsInfo->height`. Non ancora fixato. |
| Surf sulla lava | ⏳ Da testare piu' avanti | Codice verificato corretto (vedi `CLAUDE.md`), non ancora testato a schermo dall'utente |
| Map preview stile FRLG | ℹ️ Chiarito | E' una tabella statica in `src/map_preview.c` (`sMapPreviewScreenData[]`), gia' pensata per essere estesa: per aggiungere una mappa propria serve un nuovo `MPS_<NOME>` in `include/map_preview.h`, una riga nella tabella con `.mapsec = MAPSEC_<TUA_MAPPA>`, `.type` (CAVE/FOREST), e grafica dedicata o riuso di una esistente. Nessuna modifica fatta oggi, solo verificato che si puo' fare. |
| Box PC accessibili ovunque | ✅ OK, confermato dall'utente 2026-08-31 | Storia completa di tentativi: (1) scorciatoia R nel party menu — schermo inconsistente uscendo dai box; (2) oggetto custom `ITEM_PORTABLE_BOX` con chiamata diretta a `ShowPokemonStorageSystemPC` da codice C — colori sfasati e prompt fantasma; (3) `ITEM_POKEMON_BOX_LINK` gia' pronto nel motore, rinominato "Box Portatile" — funzionava ma reso superfluo; (4) icona BOX nel menu Start con `CB2_ReturnToFieldContinueScript` — fade nero indesiderato (stesso di Bag/Party/Pokedex, ma l'utente lo voleva evitare solo qui). **Soluzione finale:** capito che il menu Start (USM) non lascia mai il CB2 di campo (e' un overlay, vedi `Usm_InitStartMenu`), quindi basta `ScriptContext_SetupScript(EventScript_AccessPokemonBoxLink)` **senza alcun `SetMainCallback2`**: zero fade, stesso comportamento di un PC fisico. Icona **BOX**, sbloccata da `FLAG_SYS_BOX_LINK_GET` (2316/0x90C) come DexNav/Quest: grafica dell'oggetto Box Link ricolorata, con pannello forzato sul blu vero della palette (il remap a colore piu' vicino lo confondeva con lo sfondo) e frame grigio/colore corretti per la desaturazione da non-selezionata. Resta piccola nel riquadro 32x32 (M6.0, arte dedicata da fare piu' avanti). L'oggetto `ITEM_POKEMON_BOX_LINK` resta nel gioco ma non serve piu' per l'accesso. |
| "Sposta Pokémon" prima voce nel menu PC | ✅ Gia' cosi' | Verificato 2026-08-30: gia' garantito da `OW_PC_MOVE_ORDER = GEN_LATEST` in `include/config/overworld.h` (ordine "Gen 7+": Sposta Pokémon, Deposita, Preleva, Sposta Oggetti, Esci). Vale sia per il Box Portatile sia per un PC fisico normale, nessuna modifica necessaria. |
| Item drop dai selvatici | ⏳ Non ancora ritestato in questa sessione | Verificato nel codice (vedi sopra), non confermato a schermo dall'utente in questa sessione |
| Quest Menu (stati, filtro R, sprite NPC) | ⏳ Da testare piu' avanti | Non ci sono ancora missioni vere nel gioco, per scelta dell'utente si rimanda |
| Start Menu (Unbound Style, icone riordinabili, QUESTS aggiunto) | ✅ OK, parzialmente testato | Confermato dall'utente 2026-08-30. **Da testare ancora**: comportamento con Flash/buio (limite noto, vedi `CLAUDE.md`), Battle Pyramid, Safari Zone |
| Tutto il resto della checklist iniziale (Mega Evo, Z-Move, Dynamax disattivo, Condividi Exp, repellenti BW, PokeVial, Key Item Wheel, ecc.) | ✅ OK | Non dichiarato come problematico dall'utente 2026-08-30: si assume funzionante finche' non emerge il contrario |

Legenda: ✅ testato e ok · 🔧 bug trovato e gia' corretto, da ritestare · ⚠️ funziona ma con bug noto non ancora risolto · ⏳ non ancora testato · ℹ️ chiarito/spiegato, nessun problema reale · ❓ da chiarire con l'utente

## Stato attuale verificato

- PokeVial: il ripristino PP e gia presente in `src/pokevial.c` tramite `CalculatePPWithBonus`.
- PokeVial: il sistema dose/size esiste gia in `struct Pokevial` dentro `include/global.h`.
- Lava Surf: la condizione corretta e gia implementata: Surf sbloccato + tile lava davanti + Pokemon Fuoco vivo.
- Lava Surf: lo sprite su lava e gia il primo Pokemon Fuoco vivo della squadra.
- Item drop dai selvatici: gia implementato (`BS_SaveFaintedBattlerItem`/`BS_GiveDroppedItems` in `src/battle_script_commands.c`, `BattleScript_ItemDropped` in `data/battle_scripts_1.s`). La checklist OneNote "COSE DA AGGIUNGERE" lo segna ancora come da fare: e da aggiornare li, non nel codice.
- Quest Menu: stati quest e script command sono presenti.
- Quest Menu: `FLAG_QUEST_MENU_ACTIVE` gia gate la voce nello Start Menu (`src/start_menu.c:371`) — vedi M1.1/BUG-001.
- Quest Menu: restano aperti toggle START/A-Z, reset sort, callback di uscita, quest bloccate `????`.
- Start Menu: sostituito con la Unbound Style Start Menu (`src/unbound_start_menu.c`), integrata e mergiata su master il 2026-08-30 dopo build e test in mGBA. Vedi `CLAUDE.md` sezione "Unbound Style Start Menu" per dettagli, limiti noti (icone non visibili nel cerchio di luce di Flash) e come cambiare colori/icone.
- Il vecchio `src/start_menu.c`/`src/menu.c` (barra orizzontale) resta nel repo come codice morto, non richiamato da nessun punto di ingresso: da rimuovere in futuro se la USM si conferma definitiva.

## Story & design

- Il Master Design Document (fazioni, atti, sistema Badge Aegis, ordine Capipalestra) e stato trascritto in [`docs/hoenn_next_story.md`](docs/hoenn_next_story.md), che soddisfa anche M4.1. Contiene una tabella scena-per-scena per l'Atto 1 e una nota da chiarire su Rocco (personaggio Atto 1 vs Capopalestra di Verdeazzupoli).

---

## MILESTONE 0 - Start Menu orizzontale (SUPERATA — vedi nota sopra)

> ⚠️ Il 2026-08-30 lo Start Menu e' stato sostituito dalla Unbound Style Start
> Menu (`src/unbound_start_menu.c`). I task sotto restano come storico ma
> riguardano codice ora non piu' in uso (`src/start_menu.c`/`src/menu.c`): non
> lavorarci sopra a meno di aver deciso di tornare indietro dalla USM.

- [x] M0.1 - Convertire la finestra Start Menu da lista verticale a barra orizzontale bassa
  - Perche: il menu Start deve diventare una UI centrale della hack, piu simile a una barra rapida moderna che a una lista vanilla allungata.
  - File:
    - `src/menu.c`
    - `src/start_menu.c`
  - Come:
    - In `AddStartMenuWindow()`, usare una finestra bassa e larga: `left=1`, `top=14`, `width=28`, `height=5`.
    - Non stampare piu i nomi nella barra.
    - Usare icone come sprite item provvisori, cosi ogni icona mantiene la propria palette.
    - Mostrare fino a 6 icone alla volta e far scorrere la barra se le voci sono piu di 6.
  - Test:
    - Compilare ROM.
    - Aprire menu Start con poche e molte voci.
    - Verificare che la barra rimanga nella parte bassa dello schermo.
    - Verificare che le icone non restino visibili dopo aver aperto altri menu.

- [x] M0.2 - Implementare navigazione orizzontale del menu Start
  - Perche: `InitMenuNormal()` gestisce solo menu verticali; la barra orizzontale richiede un cursore custom.
  - File:
    - `src/start_menu.c`
  - Come:
    - Sostituire `InitMenuNormal()` con un cursore custom.
    - Sinistra/destra muovono di una voce.
    - Il cursore fa wrap tra prima e ultima voce.
    - La voce selezionata viene evidenziata alzando leggermente la sua icona.
  - Test:
    - Sinistra/destra scorrono tutte le voci.
    - Il wrap funziona da prima a ultima voce e viceversa.
    - A apre sempre la voce evidenziata.

- [x] M0.3 - Nascondere `QUESTS` dietro `FLAG_QUEST_MENU_ACTIVE`
  - Perche: il Quest Menu deve apparire solo quando la storia lo sblocca.
  - File:
    - `src/start_menu.c`
    - `include/constants/flags.h`
  - Come:
    - In `BuildNormalStartMenu()`, aggiungere `MENU_ACTION_QUESTS` solo se `FlagGet(FLAG_QUEST_MENU_ACTIVE) == TRUE`.
  - Test:
    - Senza flag, `QUESTS` non appare.
    - Con flag, `QUESTS` appare nella barra.

- [ ] M0.4 - Valutare ordine finale delle voci Start Menu
  - Perche: in una barra orizzontale l'ordine influenza molto la memoria muscolare del player.
  - File:
    - `src/start_menu.c`
  - Come:
    - Proposta ordine normale:
      - `POKEMON`
      - `BAG`
      - `POKEDEX`
      - `DEXNAV`
      - `QUESTS`
      - `POKENAV`
      - `PLAYER`
      - `SAVE`
      - `OPTION`
      - `EXIT`
    - Decidere se mettere `QUESTS` prima o dopo `DEXNAV`.
    - Tenere i menu speciali Safari/Battle Pyramid semplici e verificare caso per caso.
  - Test:
    - Menu leggibile con tutte le feature sbloccate.
    - Menu leggibile a inizio gioco con poche feature sbloccate.

- [ ] M0.5 - Aggiungere polish visivo futuro al menu Start
  - Perche: la barra funzionale e il primo passo; in seguito deve ricevere icone dedicate invece dei placeholder item icon.
  - File:
    - `src/start_menu.c`
    - eventuali nuove grafiche in `graphics/interface/`
  - Come:
    - Sostituire le icone item provvisorie con una spritesheet dedicata.
    - Valutare palette maschio/femmina.
    - Valutare cornice dedicata per barra e namebox.
  - Test:
    - Nessuna icona tagliata.
    - Nessun overlap con finestre Safari/Battle Pyramid.

- [x] M0.7 - Aggiungere namebox sopra a sinistra della barra Start Menu
  - Perche: la barra usa icone; serve un nome leggibile della voce selezionata, come un piccolo namebox sopra il box principale.
  - File:
    - `src/start_menu.c`
  - Come:
    - Creare una finestra piccola a `left=1`, `top=12`.
    - Calcolare la larghezza in base al testo selezionato, con minimo e massimo.
    - Ridisegnare il namebox ogni volta che cambia il cursore.
  - Test:
    - Il nome cambia quando si scorre la barra.
    - Il namebox sta sopra a sinistra della barra.
    - Il namebox sparisce quando si chiude il menu.

- [x] M0.8 - Rifinire sfondo, allineamento e placeholder icone della barra Start Menu
  - Perche: la prima versione aveva il fondo della barra non sempre riempito, icone troppo alte e alcuni placeholder poco chiari.
  - File:
    - `src/menu.c`
    - `src/start_menu.c`
  - Come:
    - Copiare la finestra Start Menu con `COPYWIN_FULL` dopo il draw del frame.
    - Centrare le icone verticalmente nella barra.
    - Mappare DexNav su `ITEM_POKE_RADAR`.
    - Mappare PokeNav su `ITEM_SCANNER`.
    - Mappare trainer/player su `ITEM_RED_CARD`.
    - Mappare Save su `ITEM_TM_CASE`.
    - Mappare Option su `ITEM_DOWSING_MACHINE`.
  - Test:
    - Sfondo barra completamente riempito.
    - Icone centrate verticalmente.
    - Le icone risultano piu leggibili in attesa di asset dedicati.

- [x] M0.6 - Allargare buffer azioni Start Menu a 10 voci
  - Perche: con Pokedex, DexNav, Pokemon, Quests, Bag, PokeNav, Player, Save, Option, Exit il menu normale puo arrivare a 10 voci.
  - File:
    - `src/start_menu.c`
  - Come:
    - Portare `sCurrentStartMenuActions` da 9 a 10 elementi.
  - Test:
    - Compilare ROM.
    - Sbloccare tutte le voci e verificare che il menu si apra senza glitch.

---

## MILESTONE 1 - Stabilizzare i sistemi gia aggiunti

- [x] M1.1 - Nascondere `QUESTS` finche non viene sbloccato
  - Verificato nel codice (2026-08-30): `src/start_menu.c:371` gia controlla `FlagGet(FLAG_QUEST_MENU_ACTIVE) == TRUE` prima di aggiungere `MENU_ACTION_QUESTS` in `BuildNormalStartMenu()`. Resta da collegare `setflag FLAG_QUEST_MENU_ACTIVE` al primo evento storia (Modulo Delta) quando quell'evento verra scritto (MILESTONE 2).

- [ ] M1.2 - Implementare ordinamento A-Z nel Quest Menu con START
  - Perche: `sSortAlpha` esiste gia, ma al momento non viene mai modificato da input.
  - File:
    - `src/quest_menu.c`
  - Come:
    - Nel case input principale del Quest Menu, aggiungere un ramo `JOY_NEW(START_BUTTON)`.
    - Toggle: `sSortAlpha = !sSortAlpha`.
    - Dopo il toggle, resettare `sQuestCursorPos = 0` e `sScrollOffset = 0`.
    - Chiamare `BuildUnlockedQuestList()`, `PrintCategoryHeader()`, `PrintQuestsList()`, `PrintQuestDetails()`, `ShowNpcSprite()`.
    - Aggiungere un feedback sonoro con `PlaySE(SE_SELECT)`.
  - Test:
    - Con almeno 3 quest attive, premere START e verificare ordine alfabetico.
    - Premere START di nuovo e verificare ritorno all'ordine originale.
    - Verificare che filtro R + START funzionino insieme.

- [ ] M1.3 - Mostrare icona START e testo `:A-Z` nell'header Quest Menu
  - Perche: `sText_HintSt` e gia dichiarato ma non viene usato, quindi il player non sa che START ordina.
  - File:
    - `src/quest_menu.c`
  - Come:
    - Aggiornare `BuildHintString()`.
    - Dopo il blocco per R, aggiungere `CHAR_KEYPAD_ICON`, `CHAR_START_BUTTON`, poi copiare `sText_HintSt`.
    - Controllare la larghezza del buffer `hintBuf[48]`; aumentarla se serve.
    - Verificare `hintX` con `GetStringWidth` per non uscire dallo schermo.
  - Test:
    - Header mostra icona R + `:Tipo` e icona START + `:A-Z`.
    - Test su schermo reale/emulatore: nessun testo tagliato.

- [ ] M1.4 - Resettare `sSortAlpha` all'uscita dal Quest Menu
  - Perche: l'ordinamento non deve persistere tra aperture, come nella guida PSF.
  - File:
    - `src/quest_menu.c`
  - Come:
    - Nel ramo di uscita dopo fade out, prima di `SetMainCallback2(...)`, impostare `sSortAlpha = FALSE`.
    - Valutare se resettare anche `sFilterMode`, `sQuestCursorPos` e `sScrollOffset`; consigliato lasciare filtro/cursore solo se c'e una ragione UX precisa.
  - Test:
    - Aprire Quest Menu, premere START, uscire.
    - Riaprire Quest Menu: ordinamento non alfabetico.

- [ ] M1.5 - Usare davvero la callback passata a `QuestMenu_Init`
  - Perche: `QuestMenu_Init(void (*callback)(void))` riceve una callback ma ora la ignora; questo rende fragile l'apertura da Start Menu e da script.
  - File:
    - `src/quest_menu.c`
    - `include/quest_menu.h`
    - `src/start_menu.c`
    - `src/scrcmd.c`
  - Come:
    - Aggiungere una variabile statica `static void (*sQuestMenuExitCallback)(void);`.
    - In `QuestMenu_Init(callback)`, salvare `sQuestMenuExitCallback = callback`.
    - All'uscita usare `SetMainCallback2(sQuestMenuExitCallback)` se non NULL.
    - Da Start Menu continuare a passare `CB2_ReturnToFieldWithOpenMenu`.
    - Da script decidere se passare `CB2_ReturnToField` o una callback specifica.
  - Test:
    - Aprire da Start Menu e uscire: ritorna al campo senza blocchi.
    - Aprire via comando script `questmenu` e uscire: ritorna al campo senza riaprire menu Start.

- [ ] M1.6 - Mostrare quest bloccate come `????`
  - Perche: il menu deve comunicare che esistono obiettivi futuri senza spoilerare contenuti.
  - File:
    - `src/quest_menu.c`
    - opzionale `src/strings.c` o stringa statica locale
  - Come:
    - Aggiungere stringa `static const u8 sText_LockedQuestName[] = _("????");`.
    - Modificare `QuestMatchesFilter()`:
      - filtro `Tutte`: includere tutte le quest, anche mai sbloccate.
      - filtro `In corso`: solo active.
      - filtro `Premio`: solo reward.
      - filtro `Completate`: solo rewarded.
    - In `PrintQuestsList()`, se la quest non e active/reward/rewarded, stampare `????` e non stampare stato.
    - In `PrintQuestDetails()`, se bloccata, mostrare testo minimo tipo `Dettagli non disponibili.` oppure lasciare vuoto.
    - In `ShowNpcSprite()`, non creare sprite per quest bloccate.
  - Test:
    - Con `SIDE_QUEST_COUNT > 1`, le quest non iniziate appaiono come `????` nel filtro Tutte.
    - Nei filtri stato non appaiono le quest bloccate.
    - Se si sblocca una quest, il nome reale appare.

- [ ] M1.7 - Rifinire contatore Quest Menu dopo quest bloccate
  - Perche: il significato di `x/y` cambia se il filtro Tutte include anche quest non scoperte.
  - File:
    - `src/quest_menu.c`
  - Come:
    - Decidere semantica finale:
      - Opzione A: `x/y` = quest nel filtro / quest scoperte.
      - Opzione B: `x/y` = quest nel filtro / totale quest, incluse bloccate.
    - Per Hoenn Next consiglio Opzione B nel filtro Tutte, Perche rafforza il senso di progressione.
    - Aggiornare `PrintCategoryHeader()` di conseguenza.
  - Test:
    - Con 1 quest scoperta e 5 totali: filtro Tutte deve mostrare `05/05` o `01/05` in base alla scelta, ma deve essere intenzionale e documentato.

- [ ] M1.8 - Testare PokeVial end-to-end
  - Perche: il codice e fixato, ma serve conferma in ROM.
  - File:
    - `src/pokevial.c`
    - `src/item_use.c`
    - `data/scripts/pkmn_center_nurse.inc`
  - Come:
    - Preparare una squadra con HP mancanti, status e PP consumati.
    - Usare il PokeVial dalla borsa.
    - Registrarlo nel Key Item Wheel e usarlo da SELECT.
    - Curare al Pokemon Center e verificare ricarica dose.
  - Test:
    - HP massimi.
    - PP massimi su tutte le mosse.
    - Status rimossi.
    - Dose decrementa di 1.
    - Dose torna a Size dopo cura al centro.

- [ ] M1.9 - Pulire vecchio `VAR_POKE_VIAL_CHARGES` se obsoleto
  - Perche: il sistema ora usa `struct Pokevial`; tenere una var non usata puo confondere script futuri.
  - File:
    - `include/constants/vars.h`
    - ricerca globale `VAR_POKE_VIAL_CHARGES`
  - Come:
    - Verificare se la var e ancora referenziata in script o C.
    - Se non usata, lasciare commento `legacy/unused` oppure rimuovere solo se non rompe documentazione o salvataggi.
    - Aggiornare `AGENTS.md` per non indicarla piu come fonte primaria.
  - Test:
    - Build completa.
    - Uso PokeVial invariato.

- [ ] M1.10 - Testare Lava Surf su tile corretti
  - Perche: il codice funziona solo se il tile e configurato bene in Porymap.
  - File:
    - Porymap
    - `include/constants/metatile_behaviors.h`
    - `src/metatile_behavior.c`
  - Come:
    - Aprire una mappa di test in Porymap.
    - Assegnare ai tile lava il comportamento `MB_LAVA`.
    - Impostare elevation 3, come l'acqua surfabile.
    - Posizionare il player davanti al tile lava.
    - Dare Surf sbloccato e almeno un Pokemon Fuoco vivo.
  - Test:
    - Surf sbloccato + Pokemon Fuoco vivo: parte Surf sulla lava.
    - Surf sbloccato + nessun Pokemon Fuoco vivo: non parte.
    - Surf non sbloccato + Pokemon Fuoco vivo: non parte.
    - Su lava appare il Pokemon Fuoco, non il Pokemon con Surf.

- [ ] M1.11 - Aggiungere messaggio dedicato per Lava Surf fallito
  - Perche: se il player preme A sulla lava e non succede nulla, sembra un bug.
  - File:
    - `src/field_control_avatar.c`
    - `data/scripts/field_move_scripts.inc` o nuovo script comune
    - `src/strings.c` se serve stringa C
  - Come:
    - In `GetInteractedWaterScript()`, distinguere tre casi:
      - lava + Surf non sbloccato: messaggio tipo `Non puoi ancora attraversare questa lava.`
      - lava + Surf sbloccato + nessun Pokemon adatto: messaggio tipo `Serve un Pokemon resistente al calore.`
      - lava + requisiti OK: `EventScript_UseSurf`.
    - Creare uno o due script dedicati invece di ritornare `NULL`.
  - Test:
    - Premere A sulla lava senza requisiti mostra messaggio.
    - Il messaggio non appare sull'acqua normale.
    - Con requisiti OK parte Surf senza messaggio errato.

- [ ] M1.12 - Valutare tipi resistenti alla lava
  - Perche: solo Fuoco e semplice, ma Roccia/Terra possono essere coerenti a livello gameplay.
  - File:
    - `src/field_player_avatar.c`
    - `src/field_effect_helpers.c`
  - Come:
    - Creare helper condiviso `IsSpeciesLavaResistant(species)`.
    - Usarlo sia in `PartyHasLavaResistantMon()` sia in `FldEff_SurfBlob()`.
    - Iniziare con solo `TYPE_FIRE`; aggiungere `TYPE_ROCK`/`TYPE_GROUND` solo se deciso nel design.
  - Test:
    - Se solo Fuoco: Numel funziona, Geodude no.
    - Se Fuoco/Roccia/Terra: verificare tutti i tipi previsti.

---

## MILESTONE 2 - Vertical slice Atto 1 (Porto Selcepoli -> Ferrugipoli)

Riscritta il 2026-08-30 per allinearsi al flusso dettagliato flag-per-flag in
[`docs/hoenn_next_story.md`](docs/hoenn_next_story.md) §5 (fonte:
`emerald-timeline/Atto 1.1.drawio.html` e `Atto 1.2.drawio.html`, scritti
dall'autore). I nomi flag qui sotto sono quelli del diagramma: usarli cosi come
sono per restare coerenti tra design doc e codice, salvo necessita tecniche
diverse (in tal caso annotare la mappatura in questo file).

- [ ] M2.1 - Arrivo a Porto Selcepoli e aggancio dei tre Pokemon rari
  - Perche: l'apertura non e un attacco diretto, ma il player viene notato e
    silenziosamente "reclutato" da Beldum/Bagon/Ralts prima ancora di sapere
    perche; il bivio "li segui?" e un momento di scelta reale, non decorativo.
  - File:
    - `src/new_game.c` (posizione iniziale del player)
    - map header / warp iniziali
    - `data/maps/SlateportCity/scripts.inc` (o mappa dedicata Porto Selcepoli, da confermare)
    - object events dei tre Pokemon in Porymap
  - Come:
    - Impostare la mappa iniziale su Porto Selcepoli.
    - Sequenza flag da rispettare: `FLAG_ARRIVAL_PORTO_SELCEPOLI` -> `FLAG_RARE_POKEMON_SIGHTING` -> `FLAG_POKEMON_REQUEST_HELP` -> `FLAG_PLAYER_ACCEPTS_HELP_REQUEST`.
    - Implementare il bivio come multichoice Si/No dopo `FLAG_PLAYER_ACCEPTS_HELP_REQUEST`.
    - Ramo "No": mostrare i titoli di coda (o un loop che ripropone la scelta, da decidere) — non deve essere un vero game over distruttivo.
    - Ramo "Si": prosegue a M2.2.
  - Test:
    - Nuova partita carica Porto Selcepoli e i tre Pokemon compaiono nella sequenza corretta.
    - Rispondere "No" al bivio non causa crash o softlock.
    - Rispondere "Si" prosegue verso il Museo Oceanografico.

- [ ] M2.2 - Museo Oceanografico e laboratorio Devon: irruzione Zenith
  - Perche: e la scena che introduce fisicamente il conflitto Aegis/Zenith e il Modulo Delta.
  - File:
    - mappa Museo Oceanografico / laboratorio Devon (nuova o riadattata, da definire in Porymap)
    - object events custode museo, Ricercatore Devon, Grunt Zenith
    - trainer data per Grunt Zenith
  - Come:
    - Sequenza flag: `FLAG_APPROACH_OCEAN_MUSEUM` -> `FLAG_MUSEUM_INTERNAL_ALERT` -> `FLAG_ENTER_OCEAN_MUSEUM` -> `FLAG_POKEMON_SHOW_DISTRESS` -> `FLAG_ENTER_DEVON_LAB` -> `FLAG_RESEARCHER_NOTICES_PLAYER` -> `FLAG_ZENITH_BREAK_IN` -> `FLAG_LAB_CHAOS` -> `FLAG_DELTA_MODULE_VISIBLE`.
    - Il Modulo Delta si attiva "parzialmente" qui: mostrare solo un effetto (vibrazione/animazione), non ancora consegnarlo come item.
    - Nota testo: il ricercatore non e sorpreso di vedere il player con i tre Pokemon insieme, ma che lo abbiano coinvolto.
    - Nota testo: la reazione al Modulo Delta e instabilita tecnologica, non reazione dei Pokemon (serve a piantare il seme per Groudon/Kyogre in Atto 2).
  - Test:
    - La sequenza di flag si attiva in ordine, non saltabile.
    - Il player non puo uscire dal laboratorio prima dell'irruzione Zenith.
    - Nessun softlock se il player tenta di riparlare agli NPC durante la scena.

- [ ] M2.3 - Consegna emergenziale di un solo Pokemon (starter)
  - Perche: a differenza di una scelta libera da menu, qui e il ricercatore che
    affida un solo Pokemon per necessita, dopo aver spiegato di non essere un
    allenatore; il dialogo `FLAG_RESEARCHER_FRUSTRATION` e tematicamente
    centrale per tutto il progetto (stessa logica di Petri verso la Stasi
    Tecnologica) e va scritto con cura.
  - File:
    - script laboratorio Devon (stesso file di M2.2)
    - stringhe dedicate in `src/strings.c` o script locale
  - Come:
    - Sequenza flag: `FLAG_RESEARCHER_LIMITS_REVEALED` -> `FLAG_RESEARCHER_ROLE_EXPLAINED` -> `FLAG_RESEARCHER_FRUSTRATION` -> `FLAG_ZENITH_ADVANCE` -> `FLAG_EMERGENCY_REALIZATION` -> `FLAG_EMERGENCY_POKEMON_HANDOVER`.
    - Multichoice tra Beldum/Bagon/Ralts, ma **un solo** Pokemon assegnato con `givepokemon` (livello consigliato 5); gli altri due restano NPC/non ottenibili in questa scena.
    - Bloccare ripetizione con flag gia settato dopo la consegna.
  - Test:
    - Ogni scelta assegna il Pokemon corretto e uno soltanto.
    - Riparlare al ricercatore dopo la consegna non da un secondo Pokemon.
    - Il dialogo `FLAG_RESEARCHER_FRUSTRATION` e leggibile e non tagliato a schermo.

- [ ] M2.4 - Prima battaglia Zenith, uscita dal museo, Pokedex
  - Perche: chiude l'apertura e stabilisce che la Devon SpA ora conosce il player.
  - File:
    - trainer data per Grunt Zenith (stesso trainer di M2.2 o dedicato)
    - script laboratorio/museo
  - Come:
    - Sequenza flag: `FLAG_FIRST_ZENITH_BATTLE` -> `FLAG_FIRST_BATTLE_WON` -> `FLAG_POST_BATTLE_SILENCE` -> `FLAG_RESEARCHER_EVALUATES_PLAYER` -> `FLAG_POST_MUSEUM_CONSEQUENCES` -> `FLAG_EXIT_MUSEUM` -> `FLAG_POKEDEX_OBTAINED`.
    - Team Grunt Zenith: 1 Pokemon livello 4-5, non punitivo contro nessuno dei tre starter.
    - Il Pokedex viene dato qui dal ricercatore, non all'inizio come in Emerald vanilla.
  - Test:
    - Sconfitta gestita senza softlock (retry o game over standard).
    - Vittoria prosegue alla valutazione del ricercatore e all'uscita dal museo.
    - Pokedex compare nell'inventario/menu dopo la scena.

- [ ] M2.5 - Creare Modulo Delta come Key Item
  - Perche: e il MacGuffin che collega Devon, Zenith e Deoxys; a differenza della
    vecchia versione di questo task, ora sappiamo che si "attiva parzialmente"
    gia dentro il laboratorio (M2.2) prima di essere formalmente un item.
  - File:
    - `include/constants/items.h`
    - `src/data/items.h`
    - grafica icona item se necessaria
    - stringhe item
  - Come:
    - Aggiungere `ITEM_DELTA_MODULE` o nome equivalente.
    - Configurarlo come Key Item senza uso attivo per ora.
    - Dare l'item nello script dopo `FLAG_EXIT_MUSEUM` (M2.4), non durante l'irruzione.
    - Se la grafica manca, usare temporaneamente icona placeholder coerente.
  - Test:
    - Item appare nella tasca Strumenti Base dopo l'uscita dal museo.
    - Non e consumabile.
    - Non rompe Key Item Wheel.

- [ ] M2.6 - Creare prima quest principale `Il Modulo Delta`
  - Perche: il Quest Menu deve diventare parte della narrazione, non solo una feature isolata.
  - File:
    - `include/constants/quests.h`
    - `include/quests.h`
    - `src/quests.c`
    - `src/strings.c` o stringhe quest dedicate
    - script Porto Selcepoli
  - Come:
    - Aggiungere `MAIN_QUEST_DELTA_MODULE` o usare naming separato da side quest se si vuole distinguere main/side in futuro.
    - Compilare name, desc, donedesc, map, reward.
    - `startquest MAIN_QUEST_DELTA_MODULE` dopo `FLAG_EXIT_MUSEUM` (M2.4).
    - `completequest`/`rewardquest` dopo l'incontro con Petri (M2.12).
  - Test:
    - Quest appare nel menu dopo l'uscita dal museo.
    - Descrizione cambia correttamente quando completata.
    - Se premiata, appare come completata definitiva.

- [ ] M2.7 - Verso Ciclanova: favore di Walter e generatore Zenith
  - Perche: e il primo incarico che il Capopalestra affida al player, e introduce
    il fatto che il Capo Zenith stesso (in incognito) lo sta manipolando verso
    il giro palestre.
  - File:
    - mappa Percorso 110 / dintorni Porto Selcepoli (Porymap)
    - mappa Ciclanova (grotta/generatore, da creare in Porymap)
    - trainer data: recluta Zenith ingresso, 3 reclute interne, vice comandante
  - Come:
    - Sequenza flag: `FLAG_ROUTE_GUIDANCE_MYSTERY` (Osservatore Misterioso = Capo Zenith in incognito, **non rivelare l'identita a schermo**) -> `FLAG_ROUTE_BATTLES`/`FLAG_ROUTE_CIVILI` (Percorso 110) -> `FLAG_DEPART_PORTO_SELCEPOLI` -> `FLAG_MEET_WALTER` -> `FLAG_WALTER_FAVOR_CICLANOVA` -> `FLAG_ZENITH_RECRUIT_CHALLENGE` -> `FLAG_BATTLE_ZENITH_RECRUIT` -> `FLAG_ARRIVE_CICLANOVA` -> `FLAG_ZENITH_RECRUITS_INSIDE` -> `FLAG_VICE_AT_GENERATOR` -> `FLAG_VICE_LEAVES` -> `FLAG_INVESTIGATE_PC` -> `FLAG_RETURN_CICLAMIPOLI_AFTER_PC`.
    - Il PC lasciato dal vice comandante contiene dati sui Leggendari con "file piu importanti criptati" — lasciare volutamente incompleto come gancio per Atto 2, non risolverlo qui.
    - Le 3 reclute interne sono mini-battaglie opzionali, non obbligatorie per proseguire.
  - Test:
    - L'Osservatore Misterioso non rivela mai testualmente di essere il Capo Zenith.
    - Il player puo evitare le 3 reclute interne opzionali senza bloccarsi.
    - Dopo `FLAG_INVESTIGATE_PC`, il player puo tornare a Ciclamipoli.

- [ ] M2.8 - Ciclamipoli: ringraziamento di Walter e prima medaglia
  - Perche: chiude l'arco di apertura con la prima Palestra standard.
  - File:
    - mappa Ciclamipoli (Porymap)
    - trainer data Walter (gia referenziato in `docs/hoenn_next_story.md` §7)
  - Come:
    - Sequenza flag: `FLAG_WALTER_THANKS` -> `FLAG_ARRIVE_CICLAMIPOLI` -> `FLAG_WALTER_BATTLE` -> `FLAG_WALTER_MEDAL`.
    - `FLAG_WALTER_THANKS` avviene appena il player rientra dalla grotta di Ciclanova, prima di arrivare fisicamente in citta.
  - Test:
    - La battaglia in palestra e disponibile solo dopo `FLAG_ARRIVE_CICLAMIPOLI`.
    - La medaglia viene assegnata una sola volta.
    - Rivincita contro Walter dopo la medaglia non ridà la medaglia.

- [ ] M2.9 - Uscita palestra: PokeNav e chiamata di Petri
  - Perche: e il primo aggancio diretto tra Petri e il player, e introduce lo strumento PokeNav.
  - File:
    - script uscita palestra Ciclamipoli
    - eventuale logica PokeNav esistente (verificare se serve codice nuovo o solo script)
  - Come:
    - Sequenza flag: `FLAG_EXIT_GYM_INTERCEPT` -> `FLAG_RECEIVE_POKENAV` -> `FLAG_PETRI_CALL`.
    - Il PokeNav squilla immediatamente dopo la consegna, senza attesa del player.
  - Test:
    - L'intercettazione avviene solo dopo aver ottenuto la medaglia di Walter.
    - Il PokeNav appare nel menu Start dopo la scena (verificare gating con `FLAG_SYS_POKENAV_GET` in `src/start_menu.c`).
    - La chiamata di Petri parte una sola volta.

- [ ] M2.10 - Viaggio verso Ferrugipoli: Mentania e Rocco
  - Perche: e l'unico avvertimento che il player riceve prima di incontrare Petri, e stabilisce Rocco come personaggio a se (non il Capopalestra di Verdeazzupoli, vedi `docs/hoenn_next_story.md` §3.2).
  - File:
    - mappa Percorso 117, Mentania, Tunnel Menferro, Percorso 116 (Porymap, da creare/adattare)
    - object event Rocco a Mentania
  - Come:
    - Sequenza flag: `FLAG_ROUTE_CIVILI`/`FLAG_ROUTE_BATTLES` (Percorso 117) -> `FLAG_MENTANIA_ROCCO` -> `FLAG_ROUTE_CIVILI` (Tunnel Menferro) -> `FLAG_ROUTE_CIVILI`/`FLAG_ROUTE_BATTLES` (Percorso 116).
    - Rocco non deve combattere qui (vedi `TODO.md` M4.4): solo dialogo di avvertimento.
  - Test:
    - `FLAG_MENTANIA_ROCCO` si attiva una sola volta.
    - Il dialogo di Rocco non rivela i piani reali di Petri, solo un avvertimento generico.

- [ ] M2.11 - Evento pubblico Aegis a Ferrugipoli
  - Perche: prima apparizione pubblica del Consorzio, stabilisce il tono "futuro senza eroi" prima dell'incontro privato con Petri.
  - File:
    - mappa Ferrugipoli (Porymap)
    - object events Membri Aegis / Annunciatore
  - Come:
    - Sequenza flag: `FLAG_AEGIS_PUBLIC` -> `FLAG_FERRUGIPOLI_EXPLORE`.
    - Testo dell'annunciatore e dei due membri Aegis gia scritto in `docs/hoenn_next_story.md` §5 Atto 1.2 nodo 8 — riusarlo verbatim o adattarlo, non riscriverlo da zero.
    - **Da decidere prima di scrivere lo script:** identita di "Adriano" e del "ragazzo di Albanova" citati come eroi del passato.
    - Dopo l'evento, il player puo esplorare liberamente Ferrugipoli (venditori, allenatori, cittadini) prima di andare da Petri.
  - Test:
    - L'evento pubblico parte una sola volta all'arrivo a Ferrugipoli.
    - Il player puo esplorare la citta liberamente dopo l'evento, senza essere forzato subito da Petri.

- [ ] M2.12 - Incontro con il Sig. Petri e avvio del percorso Badge Aegis
  - Perche: chiude l'Atto 1.2 e formalizza il sistema della Doppia Sfida (Medaglie + Badge Aegis).
  - File:
    - mappa uffici Devon a Ferrugipoli
    - script locale
  - Come:
    - Sequenza flag: `FLAG_PETRI_MEETING` -> `FLAG_PETRI_INIT_AEGIS`.
    - Dialogo chiave gia scritto in `docs/hoenn_next_story.md` §5 Atto 1.2 nodo 11: Petri avverte che gli Zenith riconosceranno il player dopo gli eventi di Porto Selcepoli.
    - Sbloccare qui `FLAG_QUEST_MENU_ACTIVE` se non gia fatto altrove.
    - Non risolvere qui la quest `MAIN_QUEST_DELTA_MODULE` (M2.6): decidere se questo e il punto di `completequest` o `rewardquest`.
  - Test:
    - Dialogo leggibile e non troppo lungo.
    - Flag storia impostati correttamente, incluso `FLAG_QUEST_MENU_ACTIVE`.
    - Quest `Il Modulo Delta` aggiornata coerentemente dopo il dialogo.

---

## MILESTONE 3 - Sistema Badge Aegis

- [ ] M3.1 - Definire i flag dei Badge Aegis
  - Perche: servono per gating aree sensibili e progressione parallela alle medaglie standard.
  - File:
    - `include/constants/flags.h`
  - Come:
    - Aggiungere 8 flag consecutivi custom:
      - `FLAG_AEGIS_BADGE_1`
      - `FLAG_AEGIS_BADGE_2`
      - fino a `FLAG_AEGIS_BADGE_8`.
    - Usare spazio flag libero gia vicino alle feature custom, facendo attenzione a non sovrapporsi ai flag QoL field moves.
    - Non aggiungere dati a SaveBlock: i flag bastano.
  - Test:
    - Script con `setflag FLAG_AEGIS_BADGE_1` compila.
    - `checkflag FLAG_AEGIS_BADGE_1` funziona.

- [ ] M3.2 - Creare macro script per Badge Aegis
  - Perche: gli script mappa devono restare leggibili.
  - File:
    - `asm/macros/event.inc`
  - Come:
    - Aggiungere macro leggere, ad esempio:
      - `giveaegisbadge badgeFlag`
      - `goto_if_aegisbadge badgeFlag, dest`
    - In alternativa usare direttamente `setflag` e `goto_if_set` se si vuole evitare astrazione prematura.
  - Test:
    - Compilare uno script di prova con le macro.
    - Verificare che generino gli opcode corretti.

- [ ] M3.3 - Creare primo gate Aegis
  - Perche: il player deve percepire subito che Aegis controlla l'accesso alle aree sensibili.
  - File:
    - mappa scelta, consigliato Devon/Ferrugipoli o area sensibile a Porto Selcepoli
    - script locale
  - Come:
    - Aggiungere NPC guardia Devon/Aegis davanti all'accesso.
    - Script:
      - se `FLAG_AEGIS_BADGE_1` e settato, lascia passare.
      - altrimenti blocca con dialogo istituzionale.
    - Usare movement script per far arretrare il player se necessario.
  - Test:
    - Senza badge: accesso negato.
    - Con badge: accesso consentito.
    - Nessun softlock davanti alla porta.

- [ ] M3.4 - Creare prima Sfida Aegis legata a una palestra
  - Perche: il doppio sistema Medaglia/Badge Aegis e la meccanica narrativa centrale.
  - File:
    - script palestra scelta
    - trainer data capopalestra rematch
    - flag badge Aegis
  - Come:
    - Scegliere palestra pilota, consigliata Petra per coerenza Devon/roccia/fossili.
    - Lasciare intatta la sfida standard per la medaglia.
    - Aggiungere dialogo post-medaglia o NPC Aegis che propone la prova speciale.
    - Creare rematch con team piu forte o restrizione semplice.
    - Alla vittoria: `setflag FLAG_AEGIS_BADGE_1`.
  - Test:
    - Medaglia standard ottenibile normalmente.
    - Sfida Aegis disponibile al momento deciso.
    - Badge Aegis ottenuto una sola volta.
    - Gate Aegis si apre dopo la vittoria.

---

## MILESTONE 4 - Atto 1 giocabile

- [x] M4.1 - Scrivere outline Atto 1 direttamente in script notes
  - Fatto: [`docs/hoenn_next_story.md`](docs/hoenn_next_story.md) §5 contiene la tabella scena-per-scena per l'Atto 1 (mappa/NPC/flag ingresso-uscita). Atto 2/3 hanno solo outline di alto livello: da dettagliare con lo stesso formato quando si arriva a MILESTONE 7/8.

- [ ] M4.2 - Introdurre propaganda Aegis nel mondo
  - Perche: la politica della hack deve essere visibile in citta, non solo nei dialoghi principali.
  - File:
    - script NPC cittadini
    - TV/signpost se usati
  - Come:
    - Aggiungere 3-5 NPC che parlano di sicurezza post-cataclismi.
    - Evitare monologhi lunghi: una frase per NPC.
    - Alternare cittadini favorevoli, dubbiosi e rassegnati.
  - Test:
    - Il tono e maturo ma non pesante.
    - Il player capisce Perche Aegis ha consenso pubblico.

- [ ] M4.3 - Introdurre Team Zenith senza renderlo Team Magma/Idro 2.0
  - Perche: Zenith deve avere identita propria: evoluzione guidata, adattamento, controllo del DNA.
  - File:
    - trainer class/name
    - dialoghi reclute
    - eventuale grafica sprite futura
  - Come:
    - Scrivere dialoghi con lessico scientifico/pragmatico.
    - Evitare frasi da cattivo generico.
    - Far capire che usano Groudon/Kyogre come esca, non come obiettivo finale.
  - Test:
    - Dopo Atto 1, il player deve intuire che Deoxys e collegato ma non sapere tutto.

- [ ] M4.4 - Inserire Rocco come presenza indiretta
  - Perche: Rocco deve essere il ponte emotivo tra Petri e il player.
  - File:
    - script evento o messaggio
  - Come:
    - In Atto 1 non serve farlo combattere.
    - puo comparire con una frase breve o lasciare un messaggio al ricercatore Devon.
    - Deve suggerire che non e pienamente allineato al padre.
  - Test:
    - Il cameo non ruba la scena al player.

---

## MILESTONE 5 - Sidequest e ricompense

- [ ] M5.1 - Creare template pratico per nuove quest
  - Perche: aggiungere quest deve diventare ripetibile senza reinventare il flusso ogni volta.
  - File:
    - `include/constants/quests.h`
    - `src/quests.c`
    - `src/strings.c`
    - script mappa interessata
  - Come:
    - Per ogni quest definire:
      - costante quest
      - name
      - desc
      - donedesc
      - map
      - reward
      - npcGraphicsId
      - spritetype
    - Script standard:
      - `checkquest QUEST, 2` -> gia premiata
      - `checkquest QUEST, 1` -> consegna premio
      - `checkquest QUEST, 0` -> quest attiva
      - altrimenti startquest
  - Test:
    - Ogni stato deve essere raggiungibile e non ripetere reward.

- [ ] M5.2 - Usare PokeVial size come ricompensa quest
  - Perche: il sistema size esiste gia e puo dare progressione QOL senza rompere il bilanciamento.
  - File:
    - `include/pokevial.h`
    - script quest
    - eventuale special C se serve
  - Come:
    - Creare special script per chiamare `PokevialSizeUp(amount)` se non gia esposto.
    - Ricompensa esempio: +1 capacita dopo una sidequest medica/Devon.
    - Dopo aumento size, chiamare eventualmente `PokevialRefill()` o lasciare dose attuale invariata; scegliere e documentare.
  - Test:
    - Size aumenta.
    - Dose non supera size.
    - Salvataggio mantiene il nuovo valore.

---

## MILESTONE 6 - UI e identita visiva

- [ ] M6.0 - Sostituire le icone placeholder/piccole nella Start Menu
  - Perche: due icone aggiunte il 2026-08-30/31 sono provvisorie:
    - QUESTS duplica `debug.png` solo per avere una voce funzionante da testare; e' visivamente identica all'icona del Debug menu.
    - BOX riusa la grafica reale dell'oggetto Box Link (24x24) ricolorata e centrata in un frame 32x32, ma risulta piccola/poco leggibile nel riquadro rispetto alle altre icone a piena dimensione.
  - File:
    - `graphics/unbound_start_menu/sprites/quest.png` (sorgente, 32x64: due frame 32x32)
    - `graphics/unbound_start_menu/sprites/box.png` (idem)
  - Come:
    - Disegnare icone dedicate a piena dimensione (es. una pergamena/lista per Quest, un box/armadietto per Box) mantenendo formato 32x64 e la palette condivisa `icons.pal`.
    - Ricompilare con `make` (nessun `touch` extra necessario, vedi `CLAUDE.md`).
  - Test:
    - QUESTS e DEBUG (se attivo) risultano visivamente distinguibili nella barra.
    - BOX risulta leggibile quanto le altre icone, non piu' piccola/centrata in un riquadro vuoto.

- [ ] M6.1 - Estendere colori maschio/femmina al Quest Menu
  - Perche: lo zaino ha gia identita cromatica per genere; il Quest Menu deve sembrare parte dello stesso gioco.
  - File:
    - `src/quest_menu.c`
  - Come:
    - Dopo `LoadPalette`, sovrascrivere a runtime gli indici critici come gia fatto nello zaino.
    - Maschio: primary `RGB(7, 13, 17)`, dark `RGB(5, 10, 13)`.
    - Femmina: primary `RGB(22, 9, 27)`, dark `RGB(16, 3, 21)`, shadow `RGB(31, 3, 21)`.
    - Non toccare indici speciali se usati dalla grafica base.
  - Test:
    - Player maschio: palette blu.
    - Player femmina: palette viola/rosa.
    - Test fade in: nessun flash colore.

- [ ] M6.2 - Definire stile visivo Aegis
  - Perche: Aegis deve comunicare controllo, ordine, tecnologia e istituzione.
  - File:
    - sprite NPC futuri
    - palette UI future
    - docs stile
  - Come:
    - Palette fredda, pulita, aziendale.
    - Dialoghi formali e misurati.
    - Evitare estetica da Team villain.
  - Test:
    - Un NPC Aegis deve sembrare autorita pubblica/privata, non recluta criminale.

- [ ] M6.3 - Definire stile visivo Zenith
  - Perche: Zenith deve distinguersi da Magma/Idro e da Aegis.
  - File:
    - sprite NPC futuri
    - trainer class
    - dialoghi
  - Come:
    - Stile clandestino/scientifico.
    - Lessico: adattamento, mutazione, resilienza, forma, sopravvivenza.
    - Non usare troppi riferimenti diretti a Magma/Idro nelle prime scene.
  - Test:
    - Il player capisce che Zenith e nuovo, anche se nasce da vecchie ferite di Hoenn.

---

## MILESTONE 7 - Atto 2 e sistemi mid-game

- [ ] M7.1 - Pianificare anomalie climatiche controllate
  - Perche: Zenith manipola il clima per attirare Rayquaza/Deoxys, non per ripetere Emerald.
  - File:
    - `docs/hoenn_next_story.md`
    - map scripts delle zone coinvolte
  - Come:
    - Scegliere 3 zone con anomalie diverse.
    - Ogni anomalia deve avere gameplay: blocco percorso, trainer, puzzle, Lava Surf, quest.
    - Collegare ogni anomalia a un indizio Zenith.
  - Test:
    - Ogni anomalia ha inizio, soluzione e conseguenza narrativa.

- [ ] M7.2 - Usare Lava Surf come gating mid-game
  - Perche: e una feature distintiva e coerente con Hoenn cataclismica.
  - File:
    - Porymap
    - script mappa scelta
  - Come:
    - Creare area con lava attraversabile solo dopo Surf + Pokemon adatto.
    - Inserire reward opzionale o progresso storia.
    - Evitare di bloccare il player se non puo tornare indietro.
  - Test:
    - Il player puo capire il requisito.
    - Nessun softlock se il Pokemon Fuoco viene depositato/KO.

---

## MILESTONE 8 - Finale Deoxys

- [ ] M8.1 - Prototipare Deoxys a cambio forma dinamico
  - Perche: e il climax meccanico della filosofia Zenith: adattamento forzato.
  - File:
    - battle script custom
    - trainer/boss data
    - eventuali funzioni native battle script
  - Come:
    - Prima versione semplice:
      - sopra 66% HP: forma Normale
      - sotto 66%: forma Attacco
      - sotto 33%: forma Difesa o Velocite
    - Versione avanzata futura: cambio in base a comportamento player.
    - Non implementare finche Atto 1 non e giocabile.
  - Test:
    - Forma cambia una sola volta per soglia.
    - Stat e sprite aggiornati correttamente.
    - Nessun crash in battaglia.

- [ ] M8.2 - Cutscene Deoxys rompe la Poke Ball
  - Perche: chiude il tema del progetto: il controllo assoluto fallisce.
  - File:
    - script finale Centro Spaziale
    - eventuale animazione/sound
  - Come:
    - Dopo la boss fight, mostrare Deoxys che rifiuta il controllo.
    - Zenith perde Perche la sua teoria e incompleta, non Perche e semplicemente cattivo.
    - Petri deve ridimensionarsi, non redimersi magicamente.
  - Test:
    - Cutscene chiara e non troppo lunga.
    - Flag finale impostati correttamente.

---

## BUG

- [x] BUG-001 - `QUESTS` sempre visibile nello Start Menu
  - File: `src/start_menu.c`
  - Fix: gia gated con `FLAG_QUEST_MENU_ACTIVE` (verificato 2026-08-30, riga 371).
  - Test: nuova partita senza flag non mostra la voce.

- [ ] BUG-002 - `sSortAlpha` non e collegato a START
  - File: `src/quest_menu.c`
  - Fix: aggiungere gestione `JOY_NEW(START_BUTTON)`.
  - Test: START alterna ordine normale/A-Z.

- [ ] BUG-003 - Hint `:A-Z` dichiarato ma non mostrato
  - File: `src/quest_menu.c`
  - Fix: aggiornare `BuildHintString()`.
  - Test: header mostra icona START + `:A-Z`.

- [ ] BUG-004 - `QuestMenu_Init(callback)` ignora `callback`
  - File: `src/quest_menu.c`
  - Fix: salvare callback statica e usarla all'uscita.
  - Test: apertura da Start Menu e da script tornano entrambe correttamente.

- [ ] BUG-005 - Lava Surf senza requisiti non comunica nulla al player
  - File: `src/field_control_avatar.c`
  - Fix: ritornare script messaggio dedicato quando il player interagisce con lava ma mancano requisiti.
  - Test: A su lava senza requisiti mostra messaggio.

- [ ] BUG-006 - Sprite Surf/Lava Surf alti possono sovrapporsi al player
  - File: `src/field_effect_helpers.c`
  - Fix futuro: offset Y dinamico in base ad altezza sprite/graphicsInfo.
  - Test: provare Pokemon bassi, medi e alti.

---

## VERIFICHE MANUALI

- [ ] V1 - PokeVial da borsa: HP, status e PP ripristinati.
- [ ] V2 - PokeVial da SELECT/key item wheel: HP, status e PP ripristinati.
- [ ] V3 - PokeVial a dose 0: mostra messaggio vuoto.
- [ ] V4 - Pokemon Center: ricarica dose a size massimo.
- [ ] V5 - Quest Menu senza flag: voce `QUESTS` non visibile.
- [ ] V6 - Quest Menu con flag: voce `QUESTS` visibile.
- [ ] V7 - Quest Menu: ACTIVE, REWARD, REWARDED visualizzati correttamente.
- [ ] V8 - Quest Menu: filtro R funziona su tutte le categorie.
- [ ] V9 - Quest Menu: START ordina A-Z dopo implementazione.
- [ ] V10 - Quest Menu: uscita e riapertura resetta ordinamento.
- [ ] V11 - Lava Surf: Surf sbloccato + Pokemon Fuoco vivo -> parte.
- [ ] V12 - Lava Surf: Surf non sbloccato + Pokemon Fuoco vivo -> non parte e mostra messaggio.
- [ ] V13 - Lava Surf: Surf sbloccato + nessun Pokemon Fuoco vivo -> non parte e mostra messaggio.
- [ ] V14 - Lava Surf: sprite su lava = primo Pokemon Fuoco vivo.
- [ ] V15 - Surf normale: sprite su acqua = primo Pokemon vivo che conosce Surf.
- [ ] V16 - Item drop selvatici: KO selvatico con held item aggiunge item allo zaino.
- [ ] V17 - Item drop selvatici: zaino pieno mostra messaggio corretto.
- [ ] V18 - Vertical slice Porto Selcepoli: nuova partita -> starter -> battaglia -> Modulo Delta -> quest -> Petri.

---

## NOTE OPERATIVE

- Prima di aggiungere nuove feature grosse, completare M1 e una vertical slice M2.
- Non aggiungere campi a SaveBlock2 per le quest: il sistema 2-bit attuale evita overflow.
- Per nuove quest, preferire script command gia presenti: `startquest`, `completequest`, `checkquest`, `rewardquest`.
- Per Lava Surf, ricordare che Porymap deve usare `MB_LAVA` ed elevation 3.
- Dopo modifiche grafiche `.gbapal` o `.png`, fare `touch src/graphics.c && make -j$(nproc)`.
- Tenere `AGENTS.md` aggiornato quando un TODO vecchio viene completato o superato dal codice.
