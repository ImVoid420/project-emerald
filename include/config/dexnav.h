#ifndef GUARD_CONFIG_DEXNAV_H
#define GUARD_CONFIG_DEXNAV_H

#define DEXNAV_ENABLED                TRUE  // DexNav attivo
#define USE_DEXNAV_SEARCH_LEVELS      TRUE  /* ATTENZIONE: Se il make dà errore "Saveblock space exceeded", metti FALSE */

// Flag/var defines - I nomi "puliti" che abbiamo creato in flags.h e vars.h
#define DN_FLAG_SEARCHING             FLAG_SYS_DEXNAV_SEARCHING 
#define DN_FLAG_DEXNAV_GET            FLAG_SYS_DEXNAV_GET       
#define DN_FLAG_DETECTOR_MODE         FLAG_SYS_DETECTOR_MODE    
#define DN_VAR_SPECIES                VAR_DEXNAV_SPECIES        
#define DN_VAR_STEP_COUNTER           VAR_DEXNAV_STEP_COUNTER   

// Search parameters
#define DEXNAV_TIMEOUT                  15  
#define SNEAKING_PROXIMITY              4   
#define CREEPING_PROXIMITY              2
#define MAX_PROXIMITY                   20
#define DEXNAV_CHAIN_MAX                100 

// Hidden pokemon options
#define HIDDEN_MON_STEP_COUNT       100  
#define HIDDEN_MON_SEARCH_RATE      25   
#define HIDDEN_MON_PROBABILTY       15   

//// SEARCH PROBABILITIES
// Chance of encountering egg move at search levels
#define SEARCHLEVEL0_MOVECHANCE         0
#define SEARCHLEVEL5_MOVECHANCE         21
#define SEARCHLEVEL10_MOVECHANCE        46
#define SEARCHLEVEL25_MOVECHANCE        58
#define SEARCHLEVEL50_MOVECHANCE        63
#define SEARCHLEVEL100_MOVECHANCE       83

// Chance of encountering Hidden Abilities at search levels
#define SEARCHLEVEL0_ABILITYCHANCE      0
#define SEARCHLEVEL5_ABILITYCHANCE      0
#define SEARCHLEVEL10_ABILITYCHANCE     5
#define SEARCHLEVEL25_ABILITYCHANCE     15
#define SEARCHLEVEL50_ABILITYCHANCE     20
#define SEARCHLEVEL100_ABILITYCHANCE    23

// Chance of encountering held item
#define SEARCHLEVEL0_ITEM               0
#define SEARCHLEVEL5_ITEM               0
#define SEARCHLEVEL10_ITEM              1
#define SEARCHLEVEL25_ITEM              7
#define SEARCHLEVEL50_ITEM              6
#define SEARCHLEVEL100_ITEM             12

// Chance of encountering one star potential
#define SEARCHLEVEL0_ONESTAR            0
#define SEARCHLEVEL5_ONESTAR            14
#define SEARCHLEVEL10_ONESTAR           17
#define SEARCHLEVEL25_ONESTAR           17
#define SEARCHLEVEL50_ONESTAR           15
#define SEARCHLEVEL100_ONESTAR          8

// Chance of encountering two star potential
#define SEARCHLEVEL0_TWOSTAR            0
#define SEARCHLEVEL5_TWOSTAR            1
#define SEARCHLEVEL10_TWOSTAR           9
#define SEARCHLEVEL25_TWOSTAR           16
#define SEARCHLEVEL50_TWOSTAR           17
#define SEARCHLEVEL100_TWOSTAR          24

// Chance of encountering three star potential
#define SEARCHLEVEL0_THREESTAR          0
#define SEARCHLEVEL5_THREESTAR          0
#define SEARCHLEVEL10_THREESTAR         1
#define SEARCHLEVEL25_THREESTAR         7
#define SEARCHLEVEL50_THREESTAR         6
#define SEARCHLEVEL100_THREESTAR        12

#endif // GUARD_CONFIG_DEXNAV_H