@ QoL Field Move Tool Scripts
@ Called from ItemUseOnFieldCB_*Tool functions in item_use.c.
@ These scripts bypass dialog and mon-selection for items that replace field moves.

@ ---- CUT TOOL ----------------------------------------------------------------
@ C code already verified a cuttable tree is in front (and set VAR_LAST_TALKED).
EventScript_CutToolMode::
	lockall
	setfieldeffectargument 0, PARTY_SIZE  @ tool mode: no mon
	setfieldeffectargument 3, 2           @ skip player pose + mon animation
	dofieldeffect FLDEFF_USE_CUT_ON_TREE
	waitstate
	setflag FLAG_SYS_USE_CUT
	applymovement VAR_LAST_TALKED, Movement_CutTreeDown
	waitmovement 0
	removeobject VAR_LAST_TALKED
	releaseall
	end

@ ---- SURF TOOL ---------------------------------------------------------------
@ C code already verified surfable water is in front.
EventScript_SurfToolMode::
	lockall
	setfieldeffectargument 0, PARTY_SIZE  @ tool mode: tMonId = PARTY_SIZE → skip anim
	dofieldeffect FLDEFF_USE_SURF
	releaseall
	end

@ ---- STRENGTH TOOL -----------------------------------------------------------
EventScript_StrengthToolMode::
	lockall
	goto_if_set FLAG_SYS_USE_STRENGTH, EventScript_CheckActivatedBoulder
	dofieldeffect FLDEFF_USE_STRENGTH
	waitstate
	setflag FLAG_SYS_USE_STRENGTH
	msgbox Text_StrengthActivated, MSGBOX_DEFAULT
	releaseall
	end

@ ---- FLASH TOOL --------------------------------------------------------------
@ C code already played SE and set FLAG_SYS_USE_FLASH before calling this script.
EventScript_FlashToolMode::
	lockall
	animateflash 1
	setflashlevel 1
	releaseall
	end

@ ---- ROCK SMASH TOOL ---------------------------------------------------------
@ C code already verified a smashable rock is in front (and set VAR_LAST_TALKED).
EventScript_RockSmashToolMode::
	lockall
	setfieldeffectargument 0, PARTY_SIZE  @ tool mode: no mon
	setfieldeffectargument 3, 2           @ skip player pose + mon animation
	dofieldeffect FLDEFF_USE_ROCK_SMASH
	waitstate
	setflag FLAG_SYS_USE_ROCK_SMASH
	setflag FLAG_SAFE_FOLLOWER_MOVEMENT
	applymovement VAR_LAST_TALKED, Movement_SmashRock
	waitmovement 0
	removeobject VAR_LAST_TALKED
	specialvar VAR_RESULT, TryUpdateRusturfTunnelState
	goto_if_eq VAR_RESULT, TRUE, EventScript_EndSmash
	special RockSmashWildEncounter
	goto_if_eq VAR_RESULT, FALSE, EventScript_EndSmash
	waitstate
	releaseall
	end

@ ---- WATERFALL TOOL ----------------------------------------------------------
@ C code already verified a waterfall metatile is in front and player is surfing north.
EventScript_WaterfallToolMode::
	lockall
	setfieldeffectargument 0, PARTY_SIZE  @ tool mode: tMonId = PARTY_SIZE → skip anim
	hidefollowernpc
	dofieldeffect FLDEFF_USE_WATERFALL
	callnative FollowerNPC_WarpSetEnd
	setflag FLAG_SYS_USE_WATERFALL
	releaseall
	end

@ ---- DIVE TOOL (diving down) -------------------------------------------------
@ C code already verified a deep-water metatile is below.
EventScript_DiveToolMode::
	lockall
	setfieldeffectargument 0, PARTY_SIZE  @ tool mode: data[15] = PARTY_SIZE → skip anim
	setfieldeffectargument 1, 1
	hidefollowernpc
	dofieldeffect FLDEFF_USE_DIVE
	releaseall
	end

@ ---- DIVE TOOL (surfacing from underwater) -----------------------------------
EventScript_DiveUnderwaterToolMode::
	lockall
	setfieldeffectargument 0, PARTY_SIZE  @ tool mode: data[15] = PARTY_SIZE → skip anim
	setfieldeffectargument 1, 1
	hidefollowernpc
	dofieldeffect FLDEFF_USE_DIVE
	callnative SetFollowerNPCSurfSpriteAfterDive
	releaseall
	end
