.section .rodata

    .align 2
gQuestMenu_Gfx::
    .incbin "graphics/quest_menu/menu.4bpp.lz" @ Assicurati che il percorso sia corretto

    .align 2
gQuestMenu_Tilemap::
    .incbin "graphics/quest_menu/menu.bin.lz"

    .align 2
gQuestMenu_Pal::
    .incbin "graphics/quest_menu/menu.pal"