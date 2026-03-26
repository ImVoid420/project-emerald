EventScript_UseSurf::
	lockall
	checkfieldmove FIELD_MOVE_SURF, TRUE
	goto_if_eq VAR_RESULT, PARTY_SIZE, EventScript_SurfNoMon
	setfieldeffectargument 0, VAR_RESULT    @ mon knows Surf → use, no dialog
	dofieldeffect FLDEFF_USE_SURF
	goto EventScript_ReleaseUseSurf

EventScript_SurfNoMon:
	checkfieldmoveunlocked FIELD_MOVE_SURF
	goto_if_eq VAR_RESULT, FALSE, EventScript_ReleaseUseSurf
	setfieldeffectargument 0, PARTY_SIZE    @ no mon (field_effect.c checks tMonId)
	dofieldeffect FLDEFF_USE_SURF
EventScript_ReleaseUseSurf::
	releaseall
EventScript_EndUseSurf::
	end
