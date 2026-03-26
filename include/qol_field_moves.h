#ifndef GUARD_QOL_FIELD_MOVES_H
#define GUARD_QOL_FIELD_MOVES_H

#include "field_move.h"

// Controlla se almeno un Pokémon della squadra può imparare la mossa
// (tramite TM/Tutor o level-up). Non controlla se la conosce già.
// Ritorna lo slot del primo Pokémon valido, o PARTY_SIZE se nessuno.
u8 QoL_PartyHasMonCanLearnFieldMove(enum FieldMove fieldMove);

// CanUse functions: TRUE se il giocatore può usare la mossa
// (tool presente, o conosce la mossa, o può impararla)
bool8 QoL_CanUseCut(void);
bool8 QoL_CanUseSurf(void);
bool8 QoL_CanUseStrength(void);
bool8 QoL_CanUseRockSmash(void);
bool8 QoL_CanUseWaterfall(void);
bool8 QoL_CanUseDive(void);
bool8 QoL_CanUseFlash(void);

// Pulisce i flag "prima volta usato su questa mappa"
void QoL_ClearFieldMoveFlags(void);

#endif // GUARD_QOL_FIELD_MOVES_H
