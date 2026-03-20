#ifndef GUARD_POKEVIAL_H
#define GUARD_POKEVIAL_H

#include "constants/pokevial.h"

u32 PokevialGetDose(void);
u32 PokevialGetSize(void);

void PokevialSizeUp(u8 amount);
void PokevialDoseUp(u8 amount);
void PokevialSizeDown(u8 amount);
void PokevialDoseDown(u8 amount);

bool32 PokevialRefill(void);
void PokevialInitDefault(void);
void Pokevial_HealPlayerParty(void);

// Se FALSE: apre il party menu con l'animazione di cura (come al Pokécenter)
// Se TRUE:  cura istantaneamente e mostra solo il messaggio
#define POKEVIAL_SKIP_CUTSCENE FALSE

#endif // GUARD_POKEVIAL_H
