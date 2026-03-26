#include "global.h"
#include "event_data.h"
#include "field_move.h"
#include "item_menu.h"
#include "party_menu.h"
#include "pokemon.h"
#include "qol_field_moves.h"
#include "constants/flags.h"
#include "constants/items.h"

// Controlla se la specie può imparare la mossa tramite level-up
static bool8 SpeciesCanLearnMoveByLevelUp(u16 species, u16 move)
{
    const struct LevelUpMove *learnset = GetSpeciesLevelUpLearnset(species);
    for (u32 i = 0; learnset[i].move != LEVEL_UP_MOVE_END; i++)
    {
        if (learnset[i].move == move)
            return TRUE;
    }
    return FALSE;
}

// Ritorna lo slot del primo Pokémon che PUÒ IMPARARE la field move
// (TM/Tutor o level-up, esclude chi la conosce già).
// Ritorna PARTY_SIZE se nessuno può impararla.
u8 QoL_PartyHasMonCanLearnFieldMove(enum FieldMove fieldMove)
{
    u16 move = FieldMove_GetMoveId(fieldMove);

    for (u32 i = 0; i < PARTY_SIZE; i++)
    {
        u16 species = GetMonData(&gPlayerParty[i], MON_DATA_SPECIES, NULL);
        if (!species)
            break;
        if (GetMonData(&gPlayerParty[i], MON_DATA_IS_EGG))
            continue;
        if (MonKnowsMove(&gPlayerParty[i], move))
            continue; // Già la conosce → gestita da checkfieldmove

        if (CanLearnTeachableMove(species, move) || SpeciesCanLearnMoveByLevelUp(species, move))
        {
            gSpecialVar_0x8004 = species;
            return (u8)i;
        }
    }
    return PARTY_SIZE;
}

// Controlla se almeno un Pokémon conosce la mossa (senza badge check)
static bool8 PartyHasMonThatKnowsMove(u16 move)
{
    for (u32 i = 0; i < PARTY_SIZE; i++)
    {
        u16 species = GetMonData(&gPlayerParty[i], MON_DATA_SPECIES, NULL);
        if (!species)
            break;
        if (!GetMonData(&gPlayerParty[i], MON_DATA_IS_EGG) && MonKnowsMove(&gPlayerParty[i], move))
            return TRUE;
    }
    return FALSE;
}

// ---- CanUse functions -------------------------------------------------------
// TRUE se il giocatore può usare la mossa (tool, conosce, o può imparare)

bool8 QoL_CanUseCut(void)
{
    u16 move = FieldMove_GetMoveId(FIELD_MOVE_CUT);
    if (CheckBagHasItem(ITEM_CUT_TOOL, 1))
        return TRUE;
    if (!IsFieldMoveUnlocked(FIELD_MOVE_CUT))
        return FALSE;
    if (PartyHasMonThatKnowsMove(move))
        return TRUE;
    return QoL_PartyHasMonCanLearnFieldMove(FIELD_MOVE_CUT) != PARTY_SIZE;
}

bool8 QoL_CanUseSurf(void)
{
    u16 move = FieldMove_GetMoveId(FIELD_MOVE_SURF);
    if (CheckBagHasItem(ITEM_SURF_TOOL, 1))
        return TRUE;
    if (!IsFieldMoveUnlocked(FIELD_MOVE_SURF))
        return FALSE;
    if (PartyHasMonThatKnowsMove(move))
        return TRUE;
    return QoL_PartyHasMonCanLearnFieldMove(FIELD_MOVE_SURF) != PARTY_SIZE;
}

bool8 QoL_CanUseStrength(void)
{
    u16 move = FieldMove_GetMoveId(FIELD_MOVE_STRENGTH);
    if (CheckBagHasItem(ITEM_STRENGTH_TOOL, 1))
        return TRUE;
    if (!IsFieldMoveUnlocked(FIELD_MOVE_STRENGTH))
        return FALSE;
    if (PartyHasMonThatKnowsMove(move))
        return TRUE;
    return QoL_PartyHasMonCanLearnFieldMove(FIELD_MOVE_STRENGTH) != PARTY_SIZE;
}

bool8 QoL_CanUseRockSmash(void)
{
    u16 move = FieldMove_GetMoveId(FIELD_MOVE_ROCK_SMASH);
    if (CheckBagHasItem(ITEM_ROCK_SMASH_TOOL, 1))
        return TRUE;
    if (!IsFieldMoveUnlocked(FIELD_MOVE_ROCK_SMASH))
        return FALSE;
    if (PartyHasMonThatKnowsMove(move))
        return TRUE;
    return QoL_PartyHasMonCanLearnFieldMove(FIELD_MOVE_ROCK_SMASH) != PARTY_SIZE;
}

bool8 QoL_CanUseWaterfall(void)
{
    u16 move = FieldMove_GetMoveId(FIELD_MOVE_WATERFALL);
    if (CheckBagHasItem(ITEM_WATERFALL_TOOL, 1))
        return TRUE;
    if (!IsFieldMoveUnlocked(FIELD_MOVE_WATERFALL))
        return FALSE;
    if (PartyHasMonThatKnowsMove(move))
        return TRUE;
    return QoL_PartyHasMonCanLearnFieldMove(FIELD_MOVE_WATERFALL) != PARTY_SIZE;
}

bool8 QoL_CanUseDive(void)
{
    u16 move = FieldMove_GetMoveId(FIELD_MOVE_DIVE);
    if (CheckBagHasItem(ITEM_DIVE_TOOL, 1))
        return TRUE;
    if (!IsFieldMoveUnlocked(FIELD_MOVE_DIVE))
        return FALSE;
    if (PartyHasMonThatKnowsMove(move))
        return TRUE;
    return QoL_PartyHasMonCanLearnFieldMove(FIELD_MOVE_DIVE) != PARTY_SIZE;
}

bool8 QoL_CanUseFlash(void)
{
    u16 move = FieldMove_GetMoveId(FIELD_MOVE_FLASH);
    if (CheckBagHasItem(ITEM_FLASH_TOOL, 1))
        return TRUE;
    if (!IsFieldMoveUnlocked(FIELD_MOVE_FLASH))
        return FALSE;
    if (PartyHasMonThatKnowsMove(move))
        return TRUE;
    return QoL_PartyHasMonCanLearnFieldMove(FIELD_MOVE_FLASH) != PARTY_SIZE;
}

// ---- Flag management --------------------------------------------------------

void QoL_ClearFieldMoveFlags(void)
{
    FlagClear(FLAG_SYS_USE_CUT);
    FlagClear(FLAG_SYS_USE_SURF);
    FlagClear(FLAG_SYS_USE_ROCK_SMASH);
    FlagClear(FLAG_SYS_USE_WATERFALL);
}
