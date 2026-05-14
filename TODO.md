# TODO.md - Hoenn Next

Documento operativo per lo sviluppo della hackrom su `pokeemerald-expansion`.
Ogni task deve dire non solo cosa fare, ma anche dove intervenire, come farlo e come verificarlo.

---

## Stato attuale verificato

- PokeVial: il ripristino PP e gia presente in `src/pokevial.c` tramite `CalculatePPWithBonus`.
- PokeVial: il sistema dose/size esiste gia in `struct Pokevial` dentro `include/global.h`.
- Lava Surf: la condizione corretta e gia implementata: Surf sbloccato + tile lava davanti + Pokemon Fuoco vivo.
- Lava Surf: lo sprite su lava e gia il primo Pokemon Fuoco vivo della squadra.
- Quest Menu: stati quest e script command sono presenti.
- Quest Menu: restano aperti gating della voce Start Menu, toggle START/A-Z, reset sort, quest bloccate `????`.
- Start Menu: conversione iniziale a due colonne in stile HGSS/BW avviata in `src/start_menu.c` e `src/menu.c`.

---

## MILESTONE 0 - Start Menu HGSS/BW

- [x] M0.1 - Convertire la finestra Start Menu da lista verticale a griglia a due colonne
  - Perche: il menu Start deve diventare una UI centrale della hack, capace di ospitare DexNav, Quest Menu e sistemi custom senza sembrare una lista vanilla allungata.
  - File:
    - `src/menu.c`
    - `src/start_menu.c`
  - Come:
    - In `AddStartMenuWindow()`, usare una finestra piu larga: 14 tile invece di 7.
    - Calcolare le righe come `(numActions + 1) / 2`.
    - In `PrintStartMenuActions()`, stampare ogni voce con:
      - colonna = `index % 2`
      - riga = `index / 2`
    - Usare una larghezza logica di 56 px per colonna.
  - Test:
    - Compilare ROM.
    - Aprire menu Start con numero pari e dispari di voci.
    - Verificare che i testi non escano dalla finestra.

- [x] M0.2 - Implementare navigazione D-pad a griglia
  - Perche: `InitMenuNormal()` gestisce solo menu verticali; con due colonne serve input custom.
  - File:
    - `src/start_menu.c`
  - Come:
    - Sostituire `InitMenuNormal()` con un cursore custom.
    - Su/Giu muovono di 2 posizioni.
    - Sinistra/Destra muovono tra colonne quando la cella esiste.
    - Il wrap gestisce anche l'ultima riga incompleta.
  - Test:
    - Su/Giu fanno wrap corretto.
    - Sinistra/Destra non vanno su celle vuote.
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
    - Con flag, `QUESTS` appare nella griglia.

- [ ] M0.4 - Valutare ordine finale delle voci Start Menu
  - Perche: in due colonne l'ordine influenza molto la memoria muscolare del player.
  - File:
    - `src/start_menu.c`
  - Come:
    - Proposta ordine normale:
      - `POKEDEX` / `DEXNAV`
      - `POKEMON` / `BAG`
      - `QUESTS` / `POKENAV`
      - `PLAYER` / `SAVE`
      - `OPTION` / `EXIT`
    - Decidere se `QUESTS` deve stare accanto a `DEXNAV` oppure sotto `BAG`.
    - Tenere i menu speciali Safari/Battle Pyramid semplici e verificare caso per caso.
  - Test:
    - Menu leggibile con tutte le feature sbloccate.
    - Menu leggibile a inizio gioco con poche feature sbloccate.

- [ ] M0.5 - Aggiungere polish visivo futuro al menu Start
  - Perche: la griglia funzionale e il primo passo; in seguito puo diventare una UI piu simile a HGSS/BW.
  - File:
    - `src/start_menu.c`
    - eventuali nuove grafiche in `graphics/interface/`
  - Come:
    - Valutare icone accanto alle voci.
    - Valutare palette maschio/femmina.
    - Valutare box piu largo/centrato invece del classico frame vanilla.
  - Test:
    - Nessun testo tagliato.
    - Nessun overlap con finestre Safari/Battle Pyramid.

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

- [ ] M1.1 - Nascondere `QUESTS` finche non viene sbloccato
  - Perche: all'inizio della storia il Quest Menu non deve essere disponibile; deve apparire dopo l'evento Devon/Zenith.
  - File:
    - `include/constants/flags.h`
    - `src/start_menu.c`
    - script evento introduttivo futuro, probabilmente `data/maps/SlateportCity/scripts.inc`
  - Come:
    - Usare il flag gia esistente `FLAG_QUEST_MENU_ACTIVE`.
    - In `BuildNormalStartMenu()` sostituire l'aggiunta incondizionata di `MENU_ACTION_QUESTS` con un controllo `FlagGet(FLAG_QUEST_MENU_ACTIVE)`.
    - Nel primo evento storia, dopo la consegna/attivazione del Modulo Delta, chiamare `setflag FLAG_QUEST_MENU_ACTIVE`.
    - Se si preferisce coerenza col naming vanilla, rinominare il flag in `FLAG_SYS_QUEST_MENU_GET`, ma farlo in un commit separato o con ricerca globale.
  - Test:
    - Nuova partita: `QUESTS` non appare nel menu Start.
    - Dopo `setflag FLAG_QUEST_MENU_ACTIVE`: `QUESTS` appare.
    - Entrare e uscire dal Quest Menu senza rompere il ritorno al campo.

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

## MILESTONE 2 - Vertical slice iniziale a Porto Selcepoli

- [ ] M2.1 - Spostare l'inizio gioco a Porto Selcepoli
  - Perche: Hoenn Next parte con il player che sbarca e viene coinvolto per caso, non con il classico avvio in casa.
  - File:
    - `src/new_game.c`
    - map header / warp iniziali
    - `data/maps/SlateportCity/scripts.inc`
  - Come:
    - Individuare dove viene impostata la posizione iniziale del player.
    - Impostare mappa iniziale su Porto Selcepoli o su una piccola mappa nave/molo.
    - Aggiungere evento autorun iniziale controllato da flag.
    - Evitare modifiche grandi a Littleroot finche la vertical slice non e stabile.
  - Test:
    - Nuova partita carica Porto Selcepoli.
    - Il player non puo saltare l'evento introduttivo.
    - Salvataggio e ricarica funzionano dopo l'intro.

- [ ] M2.2 - Creare evento attacco Zenith al ricercatore Devon
  - Perche: e l'incidente casuale che mette il protagonista dentro la trama.
  - File:
    - `data/maps/SlateportCity/scripts.inc`
    - object events della mappa in Porymap
    - trainer data per recluta Zenith
  - Come:
    - Aggiungere ricercatore Devon e recluta Zenith vicino al porto.
    - Creare script autorun: blocca movimento, mostra dialogo, muove NPC, avvia scelta starter.
    - Usare flag tipo `FLAG_STORY_SLATEPORT_ZENITH_ATTACK_DONE` per non ripetere l'evento.
  - Test:
    - L'evento parte solo una volta.
    - Gli NPC hanno posizione corretta dopo la scena.
    - Il player non resta bloccato se perde/vince la battaglia.

- [ ] M2.3 - Creare scelta starter emergenziale Beldum/Bagon/Ralts
  - Perche: lo starter raro comunica subito che questa non e Emerald vanilla.
  - File:
    - `data/maps/SlateportCity/scripts.inc`
    - eventuali stringhe in `src/strings.c` o script locale
  - Come:
    - Creare multichoice con tre opzioni: Beldum, Bagon, Ralts.
    - Dopo la scelta usare `givepokemon` con livello iniziale deciso, consigliato 5.
    - Salvare scelta in una variabile o flag se serve per dialoghi futuri.
    - Bloccare ripetizione con flag dedicato.
  - Test:
    - Ogni scelta assegna il Pokemon corretto.
    - Il Pokemon appare in squadra.
    - Riparlare agli NPC non assegna un secondo starter.

- [ ] M2.4 - Creare prima battaglia contro recluta Zenith
  - Perche: il player deve usare subito lo starter in una situazione di emergenza.
  - File:
    - trainer data in `src/data/trainers.h` o file trainer della base expansion
    - party trainer in `src/data/trainer_parties.h`
    - `data/maps/SlateportCity/scripts.inc`
  - Come:
    - Creare classe/nome provvisorio per recluta Zenith se non esiste ancora.
    - Team consigliato: 1 Pokemon livello 4-5, non troppo punitivo contro nessuno dei tre starter.
    - Avviare trainerbattle dallo script dopo la scelta starter.
    - Dopo vittoria, far fuggire la recluta o interrompere la scena con arrivo Devon.
  - Test:
    - Battaglia parte dopo la scelta.
    - Sconfitta gestita senza softlock.
    - Vittoria prosegue allo step Modulo Delta.

- [ ] M2.5 - Creare Modulo Delta come Key Item
  - Perche: e il MacGuffin iniziale e serve a collegare Devon, Zenith e Deoxys.
  - File:
    - `include/constants/items.h`
    - `src/data/items.h`
    - grafica icona item se necessaria
    - stringhe item
  - Come:
    - Aggiungere `ITEM_DELTA_MODULE` o nome equivalente.
    - Configurarlo come Key Item senza uso attivo per ora.
    - Dare l'item nello script dopo l'attacco Zenith.
    - Se la grafica manca, usare temporaneamente icona placeholder coerente.
  - Test:
    - Item appare nella tasca Strumenti Base.
    - non e consumabile.
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
    - Dopo l'attacco Zenith: `startquest MAIN_QUEST_DELTA_MODULE`.
    - Dopo incontro con Petri: `completequest` o `rewardquest` a seconda del flusso.
  - Test:
    - Quest appare nel menu dopo l'intro.
    - Descrizione cambia correttamente quando completata.
    - Se premiata, appare come completata definitiva.

- [ ] M2.7 - Creare primo incontro con Sig. Petri
  - Perche: Petri deve impostare il conflitto sicurezza/liberta senza sembrare un villain piatto.
  - File:
    - mappa Devon/Porto Selcepoli o cutscene dedicata
    - script locale
  - Come:
    - Dopo l'incidente, Petri riconosce che il player e un testimone utile ma rischioso.
    - Dialogo chiave: ringrazia gli eroi passati, ma dice che Hoenn non puo piu dipendere da bambini coraggiosi.
    - Introduce i Badge Aegis come percorso di monitoraggio/autorizzazione.
    - Sblocca Quest Menu se non gia fatto.
  - Test:
    - Dialogo leggibile e non troppo lungo.
    - Flag storia impostati correttamente.
    - Quest aggiornata dopo il dialogo.

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

- [ ] M4.1 - Scrivere outline Atto 1 direttamente in script notes
  - Perche: prima di espandere mappe e dialoghi serve una scaletta concreta.
  - File:
    - nuovo file consigliato `docs/hoenn_next_story.md`
  - Come:
    - Dividere in sequenze giocabili:
      - arrivo a Porto Selcepoli
      - attacco Zenith
      - starter emergenziale
      - Modulo Delta
      - Petri
      - prima palestra
      - prima Sfida Aegis
      - primo gate Aegis
    - Per ogni sequenza scrivere: mappa, NPC, flag ingresso, flag uscita.
  - Test:
    - Ogni sequenza deve avere una condizione di inizio e una di fine.

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

- [ ] BUG-001 - `QUESTS` sempre visibile nello Start Menu
  - File: `src/start_menu.c`
  - Fix: gate con `FLAG_QUEST_MENU_ACTIVE`.
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


