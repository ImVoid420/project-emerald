# Hoenn Next — Story & Design Document

Trascrizione operativa del Master Design Document (OneNote, v9.2 — 26/01/2026) più
outline scena-per-scena richiesto da `TODO.md` M4.1. Questo file è la fonte di
verità narrativa; `TODO.md` resta il documento operativo per l'implementazione.

---

## 1. Visione Generale del Progetto

- **Concept:** Sequel narrativo di Pokémon Smeraldo/ORAS. La regione di Hoenn è la
  stessa, ma il tessuto sociale e politico è stato stravolto dalle conseguenze dei
  cataclismi passati.
- **Tono:** Maturo e realistico. Si esplora il confine tra sicurezza e libertà: ogni
  fazione agisce convinta di proteggere il futuro di Hoenn.
- **Filosofia:** Rifiuto dell'eroismo predestinato. Il protagonista è un giovane
  (16-18 anni) coinvolto per puro caso ("posto giusto al momento giusto") che deve
  guadagnarsi il proprio ruolo sul campo.

## 2. Contesto Storico: L'Eredità di ORAS

- **Memoria e Trauma:** la popolazione vive con un senso di insicurezza latente. Gli
  eventi di Groudon e Kyogre non sono leggende, ma ferite aperte che hanno generato
  una domanda collettiva di protezione assoluta.
- **La Svolta:** la consapevolezza della Mega Evoluzione e della minaccia di Deoxys
  (vissuta in prima persona dai leader ai tempi di Smeraldo/ORAS) ha portato a un
  cambio di paradigma: la natura non va più assecondata, ma contenuta.

## 3. Fazioni e Leadership

### 3.1 Consorzio Aegis (fazione grigia / istituzionale)

- **Leader:** Sig. Petri (Presidente della Devon SpA).
- **Psicologia:** profondamente segnato dall'aver visto suo figlio Rocco e un
  bambino di 10 anni (Brendan) rischiare la vita per salvare il pianeta. Considera
  il sistema degli allenatori "miracoli statistici" troppo rischiosi: non vuole più
  affidare il destino del mondo al coraggio individuale, ma alla stabilità della
  tecnologia.
- **Propaganda:** comunicazione stile Team Plasma ma in chiave business/aziendale.
  Messaggio chiave: *"Ringraziamo gli eroi del passato, ma costruiamo un futuro dove
  non ci sia più bisogno di eroi."*
- **Obiettivo:** la "Stasi Tecnologica" — usare l'Energia Infinita e le risorse
  Devon per sigillare i leggendari, neutralizzando ogni variabile climatica.
- **Rapporto con il giocatore:** Petri non vede il protagonista come un nemico, ma
  come un "asset pericoloso" che sa troppo. Lo costringe al percorso dei Badge
  Aegis per monitorarlo e valutare se sia una minaccia o una risorsa.

### 3.2 Team Zenith (antagonista principale)

- **Profilo:** rete clandestina di ex ricercatori Magma/Idro.
- **Leader:** figura fredda e carismatica che agisce nell'ombra, apparendo
  pubblicamente come un dirigente legato al mondo industriale, spesso visto in
  contesti ufficiali dell'Aegis (usati come copertura). Non faceva parte né del
  Team Idro né del Team Magma.
- **Obiettivo:** "Evoluzione Guidata" — sbloccare il DNA di Deoxys per permettere ai
  Pokémon di mutare forma (Attacco/Difesa/Velocità) a comando, rendendoli superiori
  a ogni cataclisma naturale.
- **Metodo:** manipolare il clima tramite Groudon e Kyogre non per risvegliarli, ma
  per usarli come "esca" per attirare Rayquaza e, di conseguenza, Deoxys.

> ✅ **Chiarito (2026-08-30):** i Capipalestra di Verdeazzupoli sono **Tell e Pat**
> (coppia di gemelli, equivalente di Tate & Liza), non Rocco. Rocco Petri (figlio
> del Sig. Petri, equivalente narrativo di Steven) resta un personaggio a sé,
> incontrato a Mentania in Atto 1 e attivo come mediatore in Atto 2 — nessuna
> sovrapposizione con la palestra.

## 4. Sistema di Progressione: la Doppia Sfida

A differenza degli altri allenatori, che seguono il percorso classico, il
protagonista affronta un sistema duale per ordine del Sig. Petri.

- **Medaglie Standard:** le classiche 8 medaglie di Hoenn, necessarie per la Lega e
  per il riconoscimento pubblico.
- **Badge Aegis:** accessibili solo al protagonista. Petri li impone come prova di
  fiducia. Senza questi badge, il Consorzio blocca l'accesso alle aree sensibili
  (siti storici, Centro Spaziale, ecc.).
- **Il Doppio Incontro:** per ogni palestra il giocatore affronta il Capopalestra
  due volte (non necessariamente nello stesso ordine):
  1. **Sfida Standard** → Medaglia.
  2. **Sfida Aegis** → secondo match (team più forte, regole speciali o
     restrizioni) → Badge del Consorzio.

Implementazione tecnica in `TODO.md` MILESTONE 3 (`FLAG_AEGIS_BADGE_1..8`, gate
Aegis, prima Sfida Aegis pilota a Ferrugipoli/Petra).

## 5. Struttura della Storia

### Atto 1 — Lo Squilibrio

> ✅ **Fonte primaria aggiornata (2026-08-30):** l'autore ha già scriptato in
> dettaglio l'Atto 1 in due diagrammi separati, `emerald-timeline/Atto 1.1.drawio.html`
> e `emerald-timeline/Atto 1.2.drawio.html` (cartella `project-emerald`, non dentro
> la repo). Questi **sostituiscono** sia il riassunto ad alto livello tratto da
> OneNote sia il vecchio flowchart `project emerald timeline.drawio.html`, che
> descriveva solo un abbozzo iniziale della stessa scena. Le tabelle qui sotto
> trascrivono i due diagrammi flag-per-flag, in ordine narrativo.

#### Atto 1.1 — Intro, scelta starter, prima missione, prima palestra

**Apertura a Porto Selcepoli — Museo Oceanografico / Laboratorio Devon**

| Ordine | Nodo | Flag | Evento | NPC | Dialogo chiave |
|---|---|---|---|---|---|
| 1 | Arrivo Porto Selcepoli | `FLAG_ARRIVAL_PORTO_SELCEPOLI` | Difesa ricercatore Devon | Ricercatore, primi nemici Zenith | "Aiuto! Lo Zenith sta attaccando!" |
| 2 | Osservazione Pokémon Insoliti | `FLAG_RARE_POKEMON_SIGHTING` | Tre Pokémon rari osservano il protagonista | Beldum, Bagon, Ralts | "Non sono Pokémon comuni… e sembrano interessati a te." |
| 3 | Richiesta Silenziosa | `FLAG_POKEMON_REQUEST_HELP` | I Pokémon comunicano urgenza | Beldum, Bagon, Ralts | "Percepisci chiaramente che hanno bisogno di te. Subito." |
| 4 | Decisione Consapevole | `FLAG_PLAYER_ACCEPTS_HELP_REQUEST` | Il protagonista accetta di seguirli | — | "Qualcosa non va. E sei l'unico a cui possono chiederlo." |
| 5 | **Bivio: "Li segui?"** | — | **Si** → continua sotto. **No** → **Titoli di coda** (finale forzato/scherzo: rifiutare i Pokémon non è un'opzione giocabile reale) | — | — |
| 6 | Avvicinamento al Museo Oceanografico | `FLAG_APPROACH_OCEAN_MUSEUM` | I Pokémon accelerano il passo | Civili | "Ehi… State attenti a dove correte!" |
| 7 | Allarme Interno Museo | `FLAG_MUSEUM_INTERNAL_ALERT` | Segnali di emergenza attivi | Custode museo, Ricercatore Devon | "Non dovrebbero esserci visitatori… oggi il laboratorio è chiuso." |
| 8 | Ingresso al Museo Oceanografico | `FLAG_ENTER_OCEAN_MUSEUM` | Accesso anticipato al museo | Custode museo | "Aspetta! Quei Pokémon—!" |
| 9 | Reazione dei Pokémon | `FLAG_POKEMON_SHOW_DISTRESS` | I Pokémon percepiscono un pericolo imminente | Beldum, Bagon, Ralts | "I Pokémon si agitano. Qualcosa sta andando storto." |
| 10 | Ingresso Laboratorio Devon | `FLAG_ENTER_DEVON_LAB` | Accesso all'area di ricerca | Ricercatore Devon | "Voi tre… dovevate restare nascosti." *(il ricercatore non è sorpreso di vederli insieme, è sorpreso che abbiano coinvolto il giocatore)* |
| 11 | Riconoscimento del Protagonista | `FLAG_RESEARCHER_NOTICES_PLAYER` | Il ricercatore osserva il protagonista | Ricercatore Devon | "Tu non sei registrato. Come sei entrato qui?" |
| 12 | Interruzione Brutale | `FLAG_ZENITH_BREAK_IN` | Esplosione porta di sicurezza | Grunt Zenith | "Obiettivo confermato. Recuperate il Modulo." |
| 13 | Caos nel Laboratorio | `FLAG_LAB_CHAOS` | Battaglia evitabile / fuga NPC | Ricercatore Devon, Zenith | "Come hanno fatto a trovarci così presto?!" |
| 14 | Modulo Delta in Vista | `FLAG_DELTA_MODULE_VISIBLE` | Il Modulo Delta si attiva parzialmente | — | "Il dispositivo emette una vibrazione irregolare." *(**non** è reazione dei Pokémon, solo instabilità tecnologica — sta piantando il seme per Groudon/Kyogre più avanti)* |
| 15 | Ammissione del Ricercatore | `FLAG_RESEARCHER_LIMITS_REVEALED` | Il ricercatore chiarisce il suo ruolo | Ricercatore Devon | "Quei Pokémon non sono addestrati per il combattimento." |
| 16 | Spiegazione Tecnica | `FLAG_RESEARCHER_ROLE_EXPLAINED` | Chiarimento sul possesso dei Pokémon | Ricercatore Devon | "Sono soggetti di studio. Li osservo, li proteggo… ma non sono un allenatore." |
| 17 | Conflitto Interiore | `FLAG_RESEARCHER_FRUSTRATION` | Il ricercatore realizza la propria impotenza | Ricercatore Devon | "La conoscenza non serve a niente se non sai difenderla." *(frase **tematicamente centrale** per tutto Hoenn Next — stessa logica che spinge Petri verso la Stasi Tecnologica)* |
| 18 | Pressione Zenith | `FLAG_ZENITH_ADVANCE` | I Grunt si avvicinano | Grunt Zenith | "Fine della dimostrazione scientifica." |
| 19 | Decisione Forzata | `FLAG_EMERGENCY_REALIZATION` | Il ricercatore prende atto della necessità | Ricercatore Devon | "Io non sono un allenatore. Questa situazione richiede qualcuno che sappia combattere, ma non posso metterli tutti in pericolo. **Puoi prenderne solo uno.**" |
| 20 | Consegna Temporanea | `FLAG_EMERGENCY_POKEMON_HANDOVER` | Affidamento di **un solo** Pokémon tra Beldum/Bagon/Ralts | Ricercatore Devon | "Io li studio. Tu… li tieni in piedi." |
| 21 | Inizio Prima Battaglia Zenith | `FLAG_FIRST_ZENITH_BATTLE` | Battaglia obbligatoria 1v1 | Grunt Zenith | "Vediamo quanto reggi." |
| 22 | Vittoria del Protagonista | `FLAG_FIRST_BATTLE_WON` | Grunt Zenith sconfitto e si ritira | Grunt Zenith | "Tsk… un errore di valutazione." |
| 23 | Silenzio Post-Scontro | `FLAG_POST_BATTLE_SILENCE` | Laboratorio torna calmo (riferito al fatto di essere stati trovati dallo Zenith) | Ricercatore Devon | "Non doveva andare così." |
| 24 | Valutazione del Protagonista | `FLAG_RESEARCHER_EVALUATES_PLAYER` | Il ricercatore osserva il protagonista dopo la battaglia | Ricercatore Devon | "Quei Pokémon ti hanno guidato… ma non ti sei limitato a seguirli. Hai scelto di agire. Questo… fa la differenza." |
| 25 | Prime Conseguenze | `FLAG_POST_MUSEUM_CONSEQUENCES` | La Devon SpA ora conosce il protagonista | Ricercatore Devon | "Ora la Devon Spa sa che esisti. Il Sig. Petri vorrà parlarti… prima o poi." |
| 26 | Decisione Successiva – Uscita Museo | `FLAG_EXIT_MUSEUM` | Il protagonista deve lasciare il laboratorio e il museo | Custode museo, Ricercatore Devon | "Non puoi restare qui… e non puoi tornare indietro." |
| 27 | Ottenimento Pokédex | `FLAG_POKEDEX_OBTAINED` | Il ricercatore dà anche il Pokédex, non solo il Pokémon | Ricercatore Devon | "Prendi anche questo, ti servirà." |

**Verso Ciclanova e prima palestra**

| Ordine | Nodo | Flag | Evento | NPC | Dialogo chiave |
|---|---|---|---|---|---|
| 28 | Consiglio sul Percorso – Osservatore Misterioso | `FLAG_ROUTE_GUIDANCE_MYSTERY` | Un personaggio autorevole consiglia il giro palestre | **Osservatore Misterioso = il Capo Zenith stesso, senza rivelare identità** | "Hai gestito bene la tua prima lotta… Perché non fai il giro delle palestre per vedere quanto puoi diventare forte?" |
| 29 | Mini-battaglie opzionali / Incontro Civili | `FLAG_ROUTE_BATTLES` / `FLAG_ROUTE_CIVILI` | Allenatori e civili lungo il Percorso 110 | Allenatori, civili, pescatori | "Pensi di poter passare senza provarci?" |
| 30 | Partenza da Porto Selcepoli | `FLAG_DEPART_PORTO_SELCEPOLI` | Il protagonista lascia il porto per seguire il consiglio | — | — |
| 31 | Incontro con Walter | `FLAG_MEET_WALTER` | Capopalestra Walter intercetta il protagonista | Walter | "Ah, stai facendo il giro delle palestre, vero? Prima però ho bisogno di un favore…" |
| 32 | Richiesta Favore – Andare a Ciclanova | `FLAG_WALTER_FAVOR_CICLANOVA` | Walter manda il protagonista a controllare anomalie | Walter | "Ci sono state strane anomalie a Ciclanova… io non posso lasciare la palestra, ma tu puoi dare un'occhiata." |
| 33 | Recluta Zenith all'ingresso | `FLAG_ZENITH_RECRUIT_CHALLENGE` | Una recluta Zenith blocca l'ingresso | Recluta Zenith | "Non sai con chi hai a che fare. Fatti gli affari tuoi o avrai problemi!" |
| 34 | Mini-conflitto / Lotta Recluta | `FLAG_BATTLE_ZENITH_RECRUIT` | Scontro con la recluta per avanzare | Recluta Zenith | "Non pensare di entrare qui senza sapere con chi hai a che fare!" |
| 35 | Arrivo a Ciclanova – Grotta | `FLAG_ARRIVE_CICLANOVA` | Il protagonista raggiunge la grotta sotto la pista ciclabile | Civili | "Da quando quei tipi strani sono arrivati qui, la città ha iniziato a subire cali di corrente…" |
| 36 | Reclute Zenith all'interno | `FLAG_ZENITH_RECRUITS_INSIDE` | 3 reclute, mini-battaglie opzionali vicino al generatore | Reclute Zenith | "Non penserai mica che un ragazzino ci faccia paura!" |
| 37 | Vice Comandante al generatore | `FLAG_VICE_AT_GENERATOR` | Un ex vice Magma/Idro sta operando su generatore e PC | Vice Comandante Zenith | "Fai attenzione… non ficcare il naso nei nostri affari o te ne pentirai." |
| 38 | Uscita del Vice | `FLAG_VICE_LEAVES` | Il vice se ne va subito dopo l'avvertimento | Vice Comandante Zenith | — |
| 39 | Esplorazione del PC | `FLAG_INVESTIGATE_PC` | Il giocatore usa il PC lasciato dal vice | — | "Il PC contiene informazioni su alcuni Pokémon leggendari, ma i file più importanti sono criptati…" |
| 40 | Ritorno a Ciclamipoli | `FLAG_RETURN_CICLAMIPOLI_AFTER_PC` | Il protagonista esce dalla grotta e torna in città | — | — |
| 41 | Ringraziamento di Walter | `FLAG_WALTER_THANKS` | Walter apprezza come il protagonista ha gestito Ciclanova | Walter | "Davvero... mi chiedo che cosa abbiano in mente... In ogni caso, grazie per il tuo aiuto. Ti aspetto in palestra… voglio vedere con i miei occhi quanto sei forte." |
| 42 | Arrivo a Ciclamipoli | `FLAG_ARRIVE_CICLAMIPOLI` | Il protagonista arriva in città | Civili locali | "Benvenuto a Ciclamipoli, la tua prima palestra ti aspetta." |
| 43 | Battaglia Palestra – Walter | `FLAG_WALTER_BATTLE` | Sfida in palestra | Walter | "Allora sei venuto... Vediamo di cosa sei capace! Preparati!" |
| 44 | Consegna Medaglia – Walter | `FLAG_WALTER_MEDAL` | Walter consegna la medaglia dopo la vittoria | Walter | "Ecco la tua medaglia! Te la sei meritata. Mostra a tutti quanto sei cresciuto come allenatore." |

> ⚠️ **Nota su Ciclanova (M1.4 in `TODO.md`):** questo flowchart descrive
> un'anomalia elettrica/tecnologica (cali di corrente, generatore, vice
> Zenith su un PC), **non** un'anomalia climatica come lasciava intendere il
> vecchio riassunto OneNote. Il PC con dati sui Leggendari ma "file criptati"
> è un gancio narrativo lasciato volutamente aperto (probabile pagamento
> futuro in Atto 2). Il **Osservatore Misterioso** al nodo 28 è una rivelazione
> importante da tenere segreta finché non serve: è il Capo Zenith in incognito
> che spinge attivamente il giocatore verso il percorso palestre — utile per
> capire la sua psicologia (manipola, non recluta con la forza).

#### Atto 1.2 — Post prima palestra, primo incontro con Petri e il Consorzio Aegis

| Ordine | Nodo | Flag | Evento | NPC | Dialogo chiave |
|---|---|---|---|---|---|
| 1 | Uscita Palestra – Intercettazione Ricercatore | `FLAG_EXIT_GYM_INTERCEPT` | Un ricercatore Devon intercetta il giocatore dopo la vittoria su Walter | Ricercatore Devon | "Salve! Il Sig. Petri sarebbe lieto di fare due chiacchiere con te. Ti prego di accettare questo." |
| 2 | Consegna PokéNav | `FLAG_RECEIVE_POKENAV` | Il ricercatore consegna il PokéNav | Ricercatore Devon | "Ecco il tuo PokéNav. Ti sarà molto utile durante il viaggio." *(azione: il PokéNav squilla subito)* |
| 3 | Chiamata Sig. Petri | `FLAG_PETRI_CALL` | Petri chiama al PokéNav appena ricevuto | Sig. Petri | "Ciao! Vorrei parlarti di una questione importante. Ti aspetto nella sede Devon a Ferrugipoli." |
| 4 | Incontro Civili / Mini-battaglie – Percorso 117 | `FLAG_ROUTE_CIVILI` / `FLAG_ROUTE_BATTLES` | Dialoghi e lotte lungo il percorso | Civili, pescatori, allenatori | "Pensi di poter passare senza provarci?" |
| 5 | **Incontro Mentania – Rocco** | `FLAG_MENTANIA_ROCCO` | Il protagonista incontra Rocco a Mentania | Rocco | "Ehi, stai andando verso Ferrugipoli, vero? So che incontrerai mio padre… Non posso dirti cosa ti dirà, ma fai attenzione a come ti muovi. A volte le intenzioni sembrano semplici, ma le conseguenze possono non esserlo." |
| 6 | Incontro Civili – Tunnel Menferro | `FLAG_ROUTE_CIVILI` | Dialoghi/lotte nel tunnel | Allenatori | — |
| 7 | Incontro Civili / Mini-battaglie – Percorso 116 | `FLAG_ROUTE_CIVILI` / `FLAG_ROUTE_BATTLES` | idem | Civili, pescatori, allenatori | "Pensi di poter passare senza provarci?" |
| 8 | Evento pubblico Aegis – Ferrugipoli | `FLAG_AEGIS_PUBLIC` | Prima apparizione pubblica del Consorzio | Membri Aegis / Annunciatore | Annunciatore: *"Cittadini di Hoenn, il Consorzio Aegis è qui per garantire un futuro stabile e sicuro. La tecnologia e la ricerca ci permettono di prevenire catastrofi come quelle causate da Groudon e Kyogre 5 anni fa."* Membro Aegis: *"Ringraziamo gli eroi del passato, **Adriano, Rocco e quel fortissimo ragazzo di Albanova** per il loro coraggio. Tuttavia, il mondo non può più essere affidato al coraggio individuale."* Membro Aegis 2: *"Il Sig. Petri guida questo progetto con la convinzione che la protezione dei Pokémon e dell'ambiente richieda decisioni calcolate."* |
| 9 | Esplorazione Ferrugipoli | `FLAG_FERRUGIPOLI_EXPLORE` | Libera esplorazione dopo l'evento | Cittadini, allenatori, venditori | "Hai visto l'evento del Consorzio?" / "Vuoi fare una lotta prima di andare dal Sig. Petri?" |
| 10 | Incontro con il Sig. Petri – Devon | `FLAG_PETRI_MEETING` | Il protagonista entra negli uffici Devon | Sig. Petri | "Hai visto l'evento del Consorzio? Bene. Voglio parlarti di un progetto importante per Hoenn." |
| 11 | Inizio Percorso Badge Aegis | `FLAG_PETRI_INIT_AEGIS` | Petri impone il doppio percorso dei Badge | Sig. Petri | "Da questo momento sei al centro della storia: dovrai seguire il percorso dei Badge Aegis. Gli Zenith ti riconosceranno e sapranno chi sei dopo gli avvenimenti di Porto Selcepoli. Preparati." |

> 📝 **Nota di lore (nodo 8):** la battuta del Membro Aegis nomina esplicitamente
> tre "eroi del passato" — **Adriano**, **Rocco** e *"quel fortissimo ragazzo di
> Albanova"* — riferimenti agli eventi di ORAS/Smeraldo 5 anni prima. Va deciso
> chi sono "Adriano" e il ragazzo di Albanova nel canon di Hoenn Next (probabili
> equivalenti di Archie/Maxie o di un rivale, e il protagonista di ORAS). Utile
> annotarlo per coerenza quando si scriveranno altri riferimenti a questo evento.

Da qui in poi (fine Atto 1.2 in avanti) resta valido solo l'outline di alto
livello descritto in Atto 2/Atto 3 più sotto — non ancora scriptato in un
diagramma dedicato.

### Atto 2 — Il Conflitto

| # | Scena | Note |
|---|-------|------|
| 2.1 | **Escalation climatica:** lo Zenith provoca anomalie mirate. Petri reagisce schierando le forze di sicurezza Devon. | Collegare a MILESTONE 7 (anomalie climatiche controllate, Lava Surf come gating mid-game). |
| 2.2 | **Il ruolo di Rocco (Steven):** mediatore tormentato tra la visione autoritaria del padre e il rispetto per la natura dei Pokémon; aiuta segretamente il giocatore. | Va tenuto ambiguo fino alla rivelazione finale, se Rocco = leader di Verdeazzupoli (vedi nota §3.2). |

### Atto 3 — L'Equilibrio

| # | Scena | Note |
|---|-------|------|
| 3.1 | **Verdeazzupoli:** lo Zenith occupa il Centro Spaziale e usa il Modulo Delta per attirare Deoxys. | |
| 3.2 | **Boss fight finale** contro il Capo Zenith che usa Deoxys, con cambio forma dinamico (Attacco/Difesa/Velocità) durante il match. | Corrisponde a `TODO.md` M8.1. |
| 3.3 | **Risoluzione:** Deoxys distrugge la Poké Ball dall'interno, rifiutando il controllo umano. Lo Zenith cade, l'Aegis è costretta a ridimensionare il proprio controllo sulla regione. | Corrisponde a `TODO.md` M8.2. |

## 6. Meccaniche e QOL — stato verificato nel codice

Il motore è **pokeemerald-expansion**. Legenda: ✅ verificato nel codice, ⚠️
checklist OneNote disallineata dal codice, ⬜ non ancora implementato.

| Voce | Stato OneNote | Stato reale nel codice | Nota |
|---|---|---|---|
| Split fisico/speciale, Tipo Folletto, dati fino a Gen 9 | ✅ | ✅ | Feature native di pokeemerald-expansion |
| Mega Evoluzioni | ✅ | ✅ | `P_MEGA_EVOLUTIONS TRUE` (`include/config/species_enabled.h`) — vedi `CLAUDE.md` |
| Z-Move | ✅ | ✅ | Sempre disponibili se il Pokémon li ha nei dati |
| Dynamax/Gigamax | ✅ | ⚠️ | Disabilitato **in battaglia** (`B_FLAG_DYNAMAX_BATTLE 0`); le forme Gigamax esistono nei dati. Da decidere se riattivarlo o lasciarlo solo come dato. |
| Teracristal | ✅ | da confermare | Non verificato in questa sessione — controllare `include/config/battle.h` / `P_TERASTALLIZATION` |
| Item drop dai selvatici | ⬜ (checklist dice "temo che la repo non sia più disponibile") | ✅ **già implementato** | `BS_SaveFaintedBattlerItem`/`BS_GiveDroppedItems` in `src/battle_script_commands.c`, `BattleScript_ItemDropped` in `data/battle_scripts_1.s`. Vedi `CLAUDE.md` sezione "Item Drop da selvatici". **Aggiornare la checklist OneNote: la feature è fatta.** |
| Volo nei percorsi stile LPE | ⬜ | ⬜ | Non iniziato |
| $B$ fuga, $R$ Poké Ball, Auto-run, corsa negli edifici, $L$/$Select$ per riordinare la squadra, MT/MN infinite, Poké Healer | ✅ | ✅ | Comandi QoL standard di pokeemerald-expansion, confermati attivi |
| Surf su lava (Pokémon Fuoco) | ✅ | ✅ (con bug di design noto) | Vedi `CLAUDE.md` "Lava Surf" — **nota**: `TODO.md` lo segna come "condizione corretta già implementata" (Fuoco vivo, non serve la mossa Surf), mentre `CLAUDE.md` descrive ancora il vecchio bug `PartyHasFireMonWithSurf`. Verificare quale versione è quella attuale prima di continuare M1.10/M1.11. |
| Day/Night, Box ovunque, NavDex, Menù Quest dedicato | ✅ | ✅ (Quest Menu solo parzialmente completo, vedi sotto) | |
| Sistema repellenti BW, no evoluzioni per scambio, ricerca strumenti ORAS, Condividi Exp Gen 6+, indicatore super/poco efficace in lotta, Pokémon al seguito HGSS, 4 strumenti stile BW con grafica ORAS, Map preview FRLG, MN senza prompt, più tasche zaino, sprite overworld per Pokémon che sanno Surf | ✅ | ✅ | Nessuna discrepanza rilevata |
| Mining minigame stile DPPt | ⬜ | ⬜ | Non iniziato |
| Menù costumi (in dubbio, giustificazione narrativa come cosplay) | ⬜ | ⬜ | Decisione di design ancora aperta |
| Puzzle delle rovine d'alfa | ⬜ | ⬜ | Non iniziato |

### Quest Menu — dettaglio stato reale (verificato ora nel codice)

- `FLAG_QUEST_MENU_ACTIVE` **è già usato per nascondere/mostrare `QUESTS` nello
  Start Menu** (`src/start_menu.c:371`). `TODO.md` M1.1 e `BUG-001` lo segnano
  ancora come aperto: **sono da chiudere**, la funzionalità è già a posto.
- `sSortAlpha` esiste ma **non è ancora collegato al tasto START** (nessun
  `JOY_NEW(START_BUTTON)` in `src/quest_menu.c`) → M1.2/M1.3/BUG-002/BUG-003
  restano validi e da fare.
- `QuestMenu_Init(callback)` **ignora ancora il parametro `callback`** e usa
  sempre `CB2_ReturnToFieldWithOpenMenu` in uscita → M1.5/BUG-004 restano validi.
- Nessuna stringa `????` per quest bloccate trovata → M1.6 resta valido.

## 7. Ordine Capipalestra

1. Ciclamipoli — Walter
2. Ferrugipoli — Petra
3. Cuordilava — Fiammetta
4. Petalipoli — Norman
5. Bluruvia — Rudy
6. Forestopoli — Alice
7. Ceneride — Rodolfo
8. Verdeazzupoli — Tell e Pat

> Non ancora presente nella repo (`src/data/trainers.h` / trainer parties): da
> creare come parte di MILESTONE 2/3/4 man mano che le singole palestre vengono
> costruite.

## 8. Prossimi passi consigliati

1. **Chiudere i due item stale in `TODO.md`** (M1.1, BUG-001 — Quest Menu gating
   già fatto) per evitare di rilavorarci per errore. ✅ Fatto.
2. **Aggiornare la checklist OneNote "COSE DA AGGIUNGERE"**: spuntare "item drop
   dai selvatici", è già implementato e documentato.
3. ~~Chiarire la questione Rocco/Verdeazzupoli~~ ✅ Chiarito: Verdeazzupoli ha
   Tell e Pat, Rocco Petri è un personaggio separato incontrato a Mentania.
4. **Verificare lo stato reale di Lava Surf** (bug vecchio vs fix nuovo) — ✅
   verificato: il fix è già applicato, `CLAUDE.md` aggiornato di conseguenza.
5. **Aggiornare `TODO.md` MILESTONE 2** (vertical slice Porto Selcepoli) per
   riflettere il flusso dettagliato di `emerald-timeline/Atto 1.1` e `Atto 1.2`:
   i task attuali (M2.1-M2.7) sono ancora a livello di sintesi e non riportano
   né il bivio "Li segui?", né lo starter unico (non scelta tra 3), né
   l'Osservatore Misterioso, né la tappa Ciclanova/generatore, né il
   PokéNav/chiamata Petri. Vale la pena riscriverli come sotto-task allineati
   alla sequenza di flag qui sopra prima di iniziare a scriptare.
6. **Decidere l'identità di "Adriano" e del "ragazzo di Albanova"** citati nel
   discorso pubblico Aegis (Atto 1.2, nodo 8) per coerenza futura.
7. Procedere con `TODO.md` MILESTONE 1 (M1.2–M1.9) per stabilizzare Quest Menu e
   PokeVial, poi con la MILESTONE 2 aggiornata (punto 5) prima di aprire il
   codice.
