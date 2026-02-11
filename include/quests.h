#ifndef GUARD_QUESTS_H
#define GUARD_QUESTS_H

#include "global.h"
#include "main.h"

enum QuestCases
{
    FLAG_GET_UNLOCKED,
    FLAG_SET_UNLOCKED,
    FLAG_GET_COMPLETED,
    FLAG_SET_COMPLETED,
};

struct SideQuest 
{
    const u8* name;
    const u8* desc;
    const u8* poc;
    const u8* map;
    const u8* reward;
};

extern const struct SideQuest gSideQuests[SIDE_QUEST_COUNT];

// Qui aggiungi i prototipi delle funzioni che hai visto nel commit
void QuestMenu_Init(u8 a0, MainCallback callback);
s8 GetSetQuestFlag(u8 quest, u8 caseId);

#endif