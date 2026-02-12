#ifndef GUARD_QUEST_MENU_H
#define GUARD_QUEST_MENU_H

#include "global.h"

struct Quest {
    const u8 *name;
    const u8 *description;
    const u8 *briefing;
    const u8 *moreInfo;
    u16 flag;
};

// Funzioni del menu
void QuestMenu_Init(void (*callback)(void));
u8 GetQuestState(u8 questId);

// Dichiarazione delle funzioni dei flag (che sono in src/quests.c)
bool8 CheckQuestUnlocked(u8 questId);
bool8 CheckQuestCompleted(u8 questId);

#endif // GUARD_QUEST_MENU_H