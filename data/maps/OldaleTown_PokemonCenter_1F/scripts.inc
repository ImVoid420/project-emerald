OldaleTown_PokemonCenter_1F_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, OldaleTown_PokemonCenter_1F_OnTransition
	map_script MAP_SCRIPT_ON_RESUME, CableClub_OnResume
	.byte 0

OldaleTown_PokemonCenter_1F_OnTransition:
	setrespawn HEAL_LOCATION_OLDALE_TOWN
	call Common_EventScript_UpdateBrineyLocation
	end

OldaleTown_PokemonCenter_1F_EventScript_Nurse::
	setvar VAR_0x800B, LOCALID_OLDALE_NURSE
	call Common_EventScript_PkmnCenterNurse
	waitmessage
	waitbuttonpress
	release
	end

OldaleTown_PokemonCenter_1F_EventScript_Gentleman::
	msgbox OldaleTown_PokemonCenter_1F_Text_TrainersCanUsePC, MSGBOX_NPC
	end

OldaleTown_PokemonCenter_1F_EventScript_Boy::
	msgbox OldaleTown_PokemonCenter_1F_Text_PokemonCentersAreGreat, MSGBOX_NPC
	end

OldaleTown_PokemonCenter_1F_EventScript_Girl::
	lock
	faceplayer
	goto_if_set FLAG_SYS_POKEDEX_GET, OldaleTown_PokemonCenter_1F_EventScript_WirelessClubAvailable
	msgbox OldaleTown_PokemonCenter_1F_Text_WirelessClubNotAvailable, MSGBOX_DEFAULT
	release
	end

OldaleTown_PokemonCenter_1F_EventScript_WirelessClubAvailable::
	msgbox OldaleTown_PokemonCenter_1F_Text_TradedInWirelessClub, MSGBOX_DEFAULT
	release
	end

OldaleTown_PokemonCenter_1F_Text_TrainersCanUsePC:
	.string "That PC in the corner there is\n"
	.string "for any POKéMON TRAINER to use.\p"
	.string "Naturally, that means you're welcome\n"
	.string "to use it, too.$"

OldaleTown_PokemonCenter_1F_Text_PokemonCentersAreGreat:
	.string "POKéMON CENTERS are great!\p"
	.string "You can use their services as much\n"
	.string "as you like, and it's all for free.\l"
	.string "You never have to worry!$"

OldaleTown_PokemonCenter_1F_Text_WirelessClubNotAvailable:
	.string "The POKéMON WIRELESS CLUB on\n"
	.string "the second floor was built recently.\p"
	.string "But they say they're still making\n"
	.string "adjustments.$"

OldaleTown_PokemonCenter_1F_Text_TradedInWirelessClub:
	.string "The POKéMON WIRELESS CLUB on\n"
	.string "the second floor was built recently.\p"
	.string "I traded POKéMON right away.$"

OldaleCenter_QuestNPC::
	lock
	faceplayer
	@ Premio già ritirato → missione conclusa
	checkquest SIDE_QUEST_1, 2
	compare VAR_RESULT, 1
	goto_if_eq OldaleCenter_QuestDone
	@ Obiettivo completato, ritira il premio
	checkquest SIDE_QUEST_1, 1
	compare VAR_RESULT, 1
	goto_if_eq OldaleCenter_RewardQuest
	@ Quest attiva → non ancora completata
	checkquest SIDE_QUEST_1, 0
	compare VAR_RESULT, 1
	goto_if_eq OldaleCenter_QuestActive
	@ Non ancora iniziata → dai la quest
	startquest SIDE_QUEST_1
	msgbox OldaleCenter_Text_Started, MSGBOX_DEFAULT
	release
	end

OldaleCenter_QuestActive:
	@ Qui l'obiettivo viene raggiunto (in questo test parli di nuovo con Flannery)
	completequest SIDE_QUEST_1
	msgbox OldaleCenter_Text_ObjectiveDone, MSGBOX_DEFAULT
	release
	end

OldaleCenter_RewardQuest:
	rewardquest SIDE_QUEST_1
	msgbox OldaleCenter_Text_Rewarded, MSGBOX_DEFAULT
	release
	end

OldaleCenter_QuestDone:
	msgbox OldaleCenter_Text_Finished, MSGBOX_DEFAULT
	release
	end

OldaleCenter_Text_Started:
	.string "Ciao! Ho una missione per te.\nVieni a parlarmi ancora!$"

OldaleCenter_Text_ObjectiveDone:
	.string "Ottimo! Ora torna da me\nper ritirare il premio.$"

OldaleCenter_Text_Rewarded:
	.string "Ecco il tuo premio!\nGrazie per l'aiuto.$"

OldaleCenter_Text_Finished:
	.string "Grazie ancora per l'aiuto!$"
