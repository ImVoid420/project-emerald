#include "global.h"
#include "pokevial.h"
#include "pokemon.h"
#include "constants/items.h"
#include "constants/moves.h"

static void PokevialFixDoseOverflow(void);

static void PokevialInit(void)
{
    if (gSaveBlock1Ptr->pokevial.Size < VIAL_MIN_SIZE)
    {
        gSaveBlock1Ptr->pokevial.Size = VIAL_MIN_SIZE;
        gSaveBlock1Ptr->pokevial.Dose = VIAL_MIN_SIZE;
    }
}

u32 PokevialGetDose(void)
{
    PokevialInit();
    return gSaveBlock1Ptr->pokevial.Dose;
}

u32 PokevialGetSize(void)
{
    PokevialInit();
    return gSaveBlock1Ptr->pokevial.Size;
}

void PokevialSizeUp(u8 sizeIncrease)
{
    u32 newSize = PokevialGetSize() + sizeIncrease;
    gSaveBlock1Ptr->pokevial.Size = (newSize > VIAL_MAX_SIZE) ? VIAL_MAX_SIZE : newSize;
}

void PokevialDoseUp(u8 doseIncrease)
{
    u32 newDose = PokevialGetDose() + doseIncrease;
    gSaveBlock1Ptr->pokevial.Dose = (newDose > gSaveBlock1Ptr->pokevial.Size) ? gSaveBlock1Ptr->pokevial.Size : newDose;
}

void PokevialSizeDown(u8 sizeDecrease)
{
    u32 cur = PokevialGetSize();
    gSaveBlock1Ptr->pokevial.Size = (sizeDecrease >= cur) ? VIAL_MIN_SIZE : (cur - sizeDecrease);
    PokevialFixDoseOverflow();
}

void PokevialDoseDown(u8 doseDecrease)
{
    u32 cur = PokevialGetDose();
    gSaveBlock1Ptr->pokevial.Dose = (doseDecrease >= cur) ? EMPTY_VIAL : (cur - doseDecrease);
}

static void PokevialFixDoseOverflow(void)
{
    PokevialDoseUp(0);
}

bool32 PokevialRefill(void)
{
    if (PokevialGetDose() == PokevialGetSize())
        return FALSE;
    gSaveBlock1Ptr->pokevial.Dose = gSaveBlock1Ptr->pokevial.Size;
    return TRUE;
}

// Inizializza il vial con 5 cariche (chiamato una sola volta dal Pokécenter alla prima consegna)
void PokevialInitDefault(void)
{
    gSaveBlock1Ptr->pokevial.Size = 5;
    gSaveBlock1Ptr->pokevial.Dose = 5;
}

void Pokevial_HealPlayerParty(void)
{
    u8 i, j;
    u8 ppBonuses;
    u8 arg[4];

    for (i = 0; i < gPlayerPartyCount; i++)
    {
        u16 maxHP = GetMonData(&gPlayerParty[i], MON_DATA_MAX_HP);
        arg[0] = maxHP;
        arg[1] = maxHP >> 8;
        SetMonData(&gPlayerParty[i], MON_DATA_HP, arg);

        ppBonuses = GetMonData(&gPlayerParty[i], MON_DATA_PP_BONUSES);
        for (j = 0; j < MAX_MON_MOVES; j++)
        {
            arg[0] = CalculatePPWithBonus(GetMonData(&gPlayerParty[i], MON_DATA_MOVE1 + j), ppBonuses, j);
            SetMonData(&gPlayerParty[i], MON_DATA_PP1 + j, arg);
        }

        arg[0] = 0; arg[1] = 0; arg[2] = 0; arg[3] = 0;
        SetMonData(&gPlayerParty[i], MON_DATA_STATUS, arg);
    }
}
