#ifndef GUARD_QUESTS_H
#define GUARD_QUESTS_H

#include "global.h"
#include "main.h"

enum QuestCases
{
    FLAG_GET_UNLOCKED,   // quest attiva (in corso)
    FLAG_SET_UNLOCKED,
    FLAG_GET_COMPLETED,  // obiettivo completato, premio non ancora ritirato
    FLAG_SET_COMPLETED,
    FLAG_GET_REWARDED,   // premio ritirato (completata definitivamente)
    FLAG_SET_REWARDED,
};

// Tipi di sprite NPC nel riquadro bianco
#define QUEST_SPRITE_OBJECT 1   // sprite overworld NPC
#define QUEST_SPRITE_PKMN   3   // icona Pokémon

struct SideQuest
{
    const u8* name;
    const u8* desc;       // descrizione mentre la quest è attiva
    const u8* donedesc;   // descrizione dopo aver completato l'obiettivo
    const u8* poc;
    const u8* map;
    const u8* reward;
    u16 npcGraphicsId;
    u8 spritetype;        // QUEST_SPRITE_OBJECT o QUEST_SPRITE_PKMN
};

extern const struct SideQuest gSideQuests[SIDE_QUEST_COUNT];

// Qui aggiungi i prototipi delle funzioni che hai visto nel commit
void QuestMenu_Init(void (*callback)(void));
s8 GetSetQuestFlag(u8 quest, u8 caseId);
bool8 CheckQuestUnlocked(u8 questId);
bool8 CheckQuestCompleted(u8 questId);
bool8 CheckQuestRewarded(u8 questId);

#endif