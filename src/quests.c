#include "global.h"
#include "quests.h"
#include "strings.h"
#include "item_icon.h"
#include "sprite.h"
#include "constants/event_objects.h"

// Logica per gestire le missioni (Sbloccate/Completate)
s8 GetSetQuestFlag(u8 quest, u8 caseId)
{
    u8 index = quest / 8;
    u8 bit = quest % 8;
    u8 mask = 1 << bit;

    switch (caseId)
    {
    case FLAG_GET_UNLOCKED:
        return (gSaveBlock2Ptr->unlockedQuests[index] & mask) != 0;
    case FLAG_SET_UNLOCKED:
        gSaveBlock2Ptr->unlockedQuests[index] |= mask;
        return 1;
    case FLAG_GET_COMPLETED:
        return (gSaveBlock2Ptr->completedQuests[index] & mask) != 0;
    case FLAG_SET_COMPLETED:
        gSaveBlock2Ptr->completedQuests[index] |= mask;
        return 1;
    }
    return -1;
}

// L'elenco delle tue missioni
const struct SideQuest gSideQuests[SIDE_QUEST_COUNT] = 
{
    [SIDE_QUEST_1] = {
        .name = gText_SideQuestName_1,
        .desc = gText_SideQuestDesc_1,
        .poc = gText_SideQuestPOC_1,
        .map = gText_SideQuestMap_1,
        .reward = gText_SideQuestReward_1,
        .npcGraphicsId = OBJ_EVENT_GFX_FLANNERY,
    },
};

bool8 CheckQuestUnlocked(u8 questId)
{
    return GetSetQuestFlag(questId, FLAG_GET_UNLOCKED) == 1;
}

bool8 CheckQuestCompleted(u8 questId)
{
    return GetSetQuestFlag(questId, FLAG_GET_COMPLETED) == 1;
}