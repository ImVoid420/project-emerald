#include "global.h"
#include "menu.h"
#include "quest_menu.h"
#include "task.h"

// Questa è la funzione che il gioco chiama per aprire il menu
void QuestMenu_Init(u8 taskId)
{
    // Per ora facciamo solo uscire dal menu immediatamente
    // Così il gioco non crasha e possiamo testare se il tasto funziona
    DestroyTask(taskId);
}