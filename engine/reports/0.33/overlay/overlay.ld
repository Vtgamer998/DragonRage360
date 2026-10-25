/* Lays a title out for elf2xex: read-only, code and writable data each start on their own 64 KB page. */
OUTPUT_FORMAT("elf32-powerpc")
OUTPUT_ARCH(powerpc:common)
ENTRY(xsdk_start)
/* It lives in libxsdk.a, which only the entry point refers to. */
EXTERN(xsdk_start)

SECTIONS
{
	/* Titles load at 0x82000000; elf2xex puts the PE headers in the first 4 KB. */
	. = 0x82000000 + 0x1000;
	.rdata : {
		*(.rodata .rodata.* .gnu.linkonce.r.*)
		*(.sdata2 .sdata2.* .sbss2 .sbss2.*)
		*(.gcc_except_table .gcc_except_table.*)
	}
	/* start.c registers this with libgcc's unwinder; the zero word ends the list, as crtend.o would. */
	.eh_frame : {
		__eh_frame_start = .;
		KEEP(*(.eh_frame))
		LONG(0)
	}

	. = ALIGN(0x10000);
	.text : {
		/* Imports first: Xenia runs a tail call's target as the caller (xenia:src/xenia/cpu/ppc/ppc_scanner.cc:204). */
		*(.text.xsdk_thunk_*)
		*(.text .text.*)
	}

	/* The constructor and destructor lists are writable sections, so they go with the data. */
	. = ALIGN(0x10000);
	.preinit_array : {
		__preinit_array_start = .;
		KEEP(*(.preinit_array))
		__preinit_array_end = .;
	}
	.init_array : {
		__init_array_start = .;
		KEEP(*(SORT_BY_INIT_PRIORITY(.init_array.*)))
		KEEP(*(.init_array))
		__init_array_end = .;
	}
	.fini_array : {
		__fini_array_start = .;
		KEEP(*(SORT_BY_INIT_PRIORITY(.fini_array.*)))
		KEEP(*(.fini_array))
		__fini_array_end = .;
	}
	.data : {
		__xbo_data_begin = .;
 . = ALIGN(4); __xbo_start_32 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_menu_e_b_c.o"(.data.sPortStageRows)) __xbo_end_32 = .;
 . = ALIGN(4); __xbo_start_53 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_menu_o_d_c.o"(.data.gMisSelFaceTex)) __xbo_end_53 = .;
 . = ALIGN(4); __xbo_start_58 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_menu_r_b_c.o"(.data.gSimEvent)) __xbo_end_58 = .;
 . = ALIGN(4); __xbo_start_60 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_menu_r_d_c.o"(.sdata.gSimTopIdleVoice)) __xbo_end_60 = .;
 . = ALIGN(4); __xbo_start_61 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_menu_r_d_c.o"(.sdata.gSimTopUnused)) __xbo_end_61 = .;
 . = ALIGN(4); __xbo_start_62 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_menu_r_d_c.o"(.data.gSimTopFaceTex)) __xbo_end_62 = .;
 . = ALIGN(1); __xbo_start_70 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_menu_z_d_c.o"(.data.gDcPassKeys)) __xbo_end_70 = .;
 . = ALIGN(1); __xbo_start_71 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_menu_za_b_c.o"(.data.gPassKeys)) __xbo_end_71 = .;
 . = ALIGN(4); __xbo_start_83 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_port_screen_c.o"(.data.sPsPh)) __xbo_end_83 = .;
 . = ALIGN(4); __xbo_start_84 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_port_screen_c.o"(.data.sPsPw)) __xbo_end_84 = .;
 . = ALIGN(4); __xbo_start_98 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_port_tbo_c.o"(.data.cur.12)) __xbo_end_98 = .;
 . = ALIGN(4); __xbo_start_106 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_port_tbo_c.o"(.data.mode.22)) __xbo_end_106 = .;
 . = ALIGN(4); __xbo_start_110 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_port_tbo_c.o"(.data.sTbTitle)) __xbo_end_110 = .;
 . = ALIGN(1); __xbo_start_111 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_port_tbo_c.o"(.data.sTbNameKeys)) __xbo_end_111 = .;
 . = ALIGN(1); __xbo_start_112 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_port_tbo_c.o"(.data.sTbKeys)) __xbo_end_112 = .;
		*(.data .data.* .gnu.linkonce.d.*)
		*(.sdata .sdata.*)
	}
	.bss : {
 . = ALIGN(8); __xbo_start_0 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_menu_a_c.o"(.bss.index.0)) __xbo_end_0 = .;
 . = ALIGN(4); __xbo_start_1 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_menu_a_c.o"(.sbss.gMainMenu)) __xbo_end_1 = .;
 . = ALIGN(4); __xbo_start_2 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_menu_a_b_c.o"(.sbss.gMenuArc12)) __xbo_end_2 = .;
 . = ALIGN(4); __xbo_start_3 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_menu_a_b_c.o"(.sbss.gMenuArc11)) __xbo_end_3 = .;
 . = ALIGN(4); __xbo_start_4 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_menu_a_b_c.o"(.sbss.gMenuArc10)) __xbo_end_4 = .;
 . = ALIGN(4); __xbo_start_5 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_menu_a_b_c.o"(.sbss.gMenuArc9)) __xbo_end_5 = .;
 . = ALIGN(4); __xbo_start_6 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_menu_a_b_c.o"(.sbss.gMenuArc8)) __xbo_end_6 = .;
 . = ALIGN(4); __xbo_start_7 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_menu_a_b_c.o"(.sbss.gMenuArc7)) __xbo_end_7 = .;
 . = ALIGN(4); __xbo_start_8 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_menu_a_b_c.o"(.sbss.gMenuArc6)) __xbo_end_8 = .;
 . = ALIGN(4); __xbo_start_9 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_menu_a_b_c.o"(.sbss.gMenuArc5)) __xbo_end_9 = .;
 . = ALIGN(4); __xbo_start_10 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_menu_a_b_c.o"(.sbss.gMenuArc4)) __xbo_end_10 = .;
 . = ALIGN(4); __xbo_start_11 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_menu_a_b_c.o"(.sbss.gMenuArc3)) __xbo_end_11 = .;
 . = ALIGN(4); __xbo_start_12 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_menu_a_b_c.o"(.sbss.gMenuArc2)) __xbo_end_12 = .;
 . = ALIGN(4); __xbo_start_13 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_menu_a_b_c.o"(.sbss.gMenuArc1)) __xbo_end_13 = .;
 . = ALIGN(4); __xbo_start_14 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_menu_a_b_c.o"(.sbss.gMenuArc0)) __xbo_end_14 = .;
 . = ALIGN(4); __xbo_start_15 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_menu_a_c_c.o"(.sbss.gTitle)) __xbo_end_15 = .;
 . = ALIGN(4); __xbo_start_16 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_menu_a_d_c.o"(.sbss.gModeMenu)) __xbo_end_16 = .;
 . = ALIGN(4); __xbo_start_17 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_menu_b_b_c.o"(.sbss.gModeBg)) __xbo_end_17 = .;
 . = ALIGN(4); __xbo_start_18 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_menu_b_c_c.o"(.sbss.gHistOutro)) __xbo_end_18 = .;
 . = ALIGN(4); __xbo_start_19 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_menu_b_d_c.o"(.sbss.gHistSel)) __xbo_end_19 = .;
 . = ALIGN(4); __xbo_start_20 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_menu_c_c_c.o"(.sbss.gHistResult)) __xbo_end_20 = .;
 . = ALIGN(4); __xbo_start_21 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_menu_c_d_c.o"(.sbss.gHistSave)) __xbo_end_21 = .;
 . = ALIGN(4); __xbo_start_22 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_menu_c_e_c.o"(.bss.sBgmIds.0)) __xbo_end_22 = .;
 . = ALIGN(4); __xbo_start_23 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_menu_c_e_c.o"(.bss.sAllIds.1)) __xbo_end_23 = .;
 . = ALIGN(4); __xbo_start_24 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_menu_c_e_c.o"(.sbss.gCharSel)) __xbo_end_24 = .;
 . = ALIGN(4); __xbo_start_25 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_menu_c_e_c.o"(.bss.sPortStageNameHidden)) __xbo_end_25 = .;
 . = ALIGN(4); __xbo_start_26 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_menu_c_e_c.o"(.bss.sPortSongNameHidden)) __xbo_end_26 = .;
 . = ALIGN(4); __xbo_start_27 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_menu_c_e_c.o"(.bss.sPortSongsAdded)) __xbo_end_27 = .;
 . = ALIGN(4); __xbo_start_28 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_menu_c_e_c.o"(.bss.sPortStagesAdded)) __xbo_end_28 = .;
 . = ALIGN(4); __xbo_start_29 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_menu_e_b_c.o"(.bss.sBgmIds.0)) __xbo_end_29 = .;
 . = ALIGN(4); __xbo_start_30 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_menu_e_b_c.o"(.bss.sAllIds.1)) __xbo_end_30 = .;
 . = ALIGN(4); __xbo_start_31 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_menu_e_b_c.o"(.sbss.gTeamSel)) __xbo_end_31 = .;
 . = ALIGN(4); __xbo_start_33 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_menu_e_b_c.o"(.bss.sPortSongsAdded)) __xbo_end_33 = .;
 . = ALIGN(4); __xbo_start_34 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_menu_e_b_c.o"(.bss.sPortStagesAdded)) __xbo_end_34 = .;
 . = ALIGN(4); __xbo_start_35 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_menu_e_b_c.o"(.bss.sPortStageNameHidden)) __xbo_end_35 = .;
 . = ALIGN(4); __xbo_start_36 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_menu_e_b_c.o"(.bss.sPortSongNameHidden)) __xbo_end_36 = .;
 . = ALIGN(4); __xbo_start_37 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_menu_g_c.o"(.sbss.gItemPanel)) __xbo_end_37 = .;
 . = ALIGN(4); __xbo_start_38 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_menu_g_c_c.o"(.sbss.gDuelMenu)) __xbo_end_38 = .;
 . = ALIGN(4); __xbo_start_39 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_menu_h_b_c.o"(.sbss.gCharRef)) __xbo_end_39 = .;
 . = ALIGN(4); __xbo_start_40 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_menu_h_d_c.o"(.sbss.gTrain)) __xbo_end_40 = .;
 . = ALIGN(4); __xbo_start_41 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_menu_i_b_c.o"(.sbss.gBootCard)) __xbo_end_41 = .;
 . = ALIGN(4); __xbo_start_42 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_menu_i_c_c.o"(.sbss.gLogo)) __xbo_end_42 = .;
 . = ALIGN(4); __xbo_start_43 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_menu_i_d_c.o"(.sbss.gEntrySel)) __xbo_end_43 = .;
 . = ALIGN(4); __xbo_start_44 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_menu_j_b_c.o"(.sbss.gTourMenu)) __xbo_end_44 = .;
 . = ALIGN(4); __xbo_start_45 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_menu_k_b_c.o"(.sbss.gBracket)) __xbo_end_45 = .;
 . = ALIGN(4); __xbo_start_46 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_menu_k_d_c.o"(.sbss.gTourBg)) __xbo_end_46 = .;
 . = ALIGN(4); __xbo_start_47 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_menu_l_b_c.o"(.sbss.gSoloSel)) __xbo_end_47 = .;
 . = ALIGN(4); __xbo_start_48 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_menu_m_b_c.o"(.sbss.gUbTeamSel)) __xbo_end_48 = .;
 . = ALIGN(4); __xbo_start_49 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_menu_n_b_c.o"(.sbss.gUbzSel)) __xbo_end_49 = .;
 . = ALIGN(4); __xbo_start_50 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_menu_n_c_c.o"(.sbss.gUbRank)) __xbo_end_50 = .;
 . = ALIGN(4); __xbo_start_51 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_menu_n_d_c.o"(.sbss.gUbResult)) __xbo_end_51 = .;
 . = ALIGN(4); __xbo_start_52 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_menu_o_b_c.o"(.sbss.gDiscFusion)) __xbo_end_52 = .;
 . = ALIGN(4); __xbo_start_54 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_menu_o_d_c.o"(.sbss.gMisSel)) __xbo_end_54 = .;
 . = ALIGN(4); __xbo_start_55 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_menu_p_b_c.o"(.sbss.gMisResult)) __xbo_end_55 = .;
 . = ALIGN(4); __xbo_start_56 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_menu_p_c_c.o"(.sbss.gUbMenu)) __xbo_end_56 = .;
 . = ALIGN(4); __xbo_start_57 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_menu_q_b_c.o"(.sbss.gSimDay)) __xbo_end_57 = .;
 . = ALIGN(4); __xbo_start_59 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_menu_r_c_c.o"(.sbss.gSimResult)) __xbo_end_59 = .;
 . = ALIGN(4); __xbo_start_63 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_menu_r_d_c.o"(.sbss.gSimTop)) __xbo_end_63 = .;
 . = ALIGN(4); __xbo_start_64 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_menu_s_b_c.o"(.sbss.gSurvSel)) __xbo_end_64 = .;
 . = ALIGN(4); __xbo_start_65 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_menu_s_c_c.o"(.sbss.gSurvResult)) __xbo_end_65 = .;
 . = ALIGN(4); __xbo_start_66 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_menu_u_g_c.o"(.sbss.gEvoZ)) __xbo_end_66 = .;
 . = ALIGN(4); __xbo_start_67 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_menu_v_c_c.o"(.sbss.gItemHelp)) __xbo_end_67 = .;
 . = ALIGN(4); __xbo_start_68 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_menu_v_d_c.o"(.sbss.gShop)) __xbo_end_68 = .;
 . = ALIGN(4); __xbo_start_69 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_menu_x_c_c.o"(.sbss.gOption)) __xbo_end_69 = .;
 . = ALIGN(4); __xbo_start_72 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_menu_za_b_c.o"(.sbss.gPassWin)) __xbo_end_72 = .;
 . = ALIGN(4); __xbo_start_73 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_menu_za_d_c.o"(.sbss.gReplayMenu)) __xbo_end_73 = .;
 . = ALIGN(4); __xbo_start_74 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_menu_za_e_c.o"(.sbss.gDcSave)) __xbo_end_74 = .;
 . = ALIGN(4); __xbo_start_75 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_port_language_c.o"(.bss.sLanguageArt)) __xbo_end_75 = .;
 . = ALIGN(1); __xbo_start_76 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_port_language_c.o"(.bss.sLanguageDisplay)) __xbo_end_76 = .;
 . = ALIGN(4); __xbo_start_77 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_port_language_c.o"(.bss.sLanguage)) __xbo_end_77 = .;
 . = ALIGN(4); __xbo_start_78 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_port_screen_c.o"(.bss.sPsPromptCount)) __xbo_end_78 = .;
 . = ALIGN(4); __xbo_start_79 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_port_screen_c.o"(.bss.sPsPromptLabel)) __xbo_end_79 = .;
 . = ALIGN(4); __xbo_start_80 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_port_screen_c.o"(.bss.sPsPromptKind)) __xbo_end_80 = .;
 . = ALIGN(4); __xbo_start_81 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_port_screen_c.o"(.bss.sPsShift)) __xbo_end_81 = .;
 . = ALIGN(4); __xbo_start_82 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_port_screen_c.o"(.bss.sPsAlign)) __xbo_end_82 = .;
 . = ALIGN(4); __xbo_start_85 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_port_screen_c.o"(.bss.sPsPy)) __xbo_end_85 = .;
 . = ALIGN(4); __xbo_start_86 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_port_screen_c.o"(.bss.sPsPx)) __xbo_end_86 = .;
 __xbo_start_87 = gPortScreen; __xbo_end_87 = gPortScreen+4;
 . = ALIGN(8); __xbo_start_88 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_port_tbo_c.o"(.bss.line.0)) __xbo_end_88 = .;
 . = ALIGN(8); __xbo_start_89 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_port_tbo_c.o"(.bss.line.1)) __xbo_end_89 = .;
 . = ALIGN(8); __xbo_start_90 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_port_tbo_c.o"(.bss.parts.2)) __xbo_end_90 = .;
 . = ALIGN(1); __xbo_start_91 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_port_tbo_c.o"(.bss.value.4)) __xbo_end_91 = .;
 . = ALIGN(8); __xbo_start_92 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_port_tbo_c.o"(.bss.line.5)) __xbo_end_92 = .;
 . = ALIGN(8); __xbo_start_93 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_port_tbo_c.o"(.bss.line.6)) __xbo_end_93 = .;
 . = ALIGN(8); __xbo_start_94 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_port_tbo_c.o"(.bss.line.7)) __xbo_end_94 = .;
 . = ALIGN(1); __xbo_start_95 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_port_tbo_c.o"(.bss.value.9)) __xbo_end_95 = .;
 . = ALIGN(2); __xbo_start_96 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_port_tbo_c.o"(.bss.buf.10)) __xbo_end_96 = .;
 . = ALIGN(8); __xbo_start_97 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_port_tbo_c.o"(.bss.said.11)) __xbo_end_97 = .;
 . = ALIGN(1); __xbo_start_99 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_port_tbo_c.o"(.bss.name.14)) __xbo_end_99 = .;
 . = ALIGN(8); __xbo_start_100 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_port_tbo_c.o"(.bss.got.15)) __xbo_end_100 = .;
 . = ALIGN(4); __xbo_start_101 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_port_tbo_c.o"(.bss.noted.16)) __xbo_end_101 = .;
 . = ALIGN(8); __xbo_start_102 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_port_tbo_c.o"(.bss.line.17)) __xbo_end_102 = .;
 . = ALIGN(8); __xbo_start_103 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_port_tbo_c.o"(.bss.said.18)) __xbo_end_103 = .;
 . = ALIGN(8); __xbo_start_104 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_port_tbo_c.o"(.bss.line.19)) __xbo_end_104 = .;
 . = ALIGN(8); __xbo_start_105 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_port_tbo_c.o"(.bss.typed.21)) __xbo_end_105 = .;
 . = ALIGN(8); __xbo_start_107 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_port_tbo_c.o"(.bss.noItems.23)) __xbo_end_107 = .;
 . = ALIGN(4); __xbo_start_108 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_port_tbo_c.o"(.bss.sTbSignFrames)) __xbo_end_108 = .;
 . = ALIGN(4); __xbo_start_109 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_port_tbo_c.o"(.bss.sTbWinRes)) __xbo_end_109 = .;
 . = ALIGN(8); __xbo_start_113 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_port_tbo_c.o"(.bss.sTbBgCode)) __xbo_end_113 = .;
 . = ALIGN(4); __xbo_start_114 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_port_tbo_c.o"(.bss.sTbBgFrames)) __xbo_end_114 = .;
 . = ALIGN(4); __xbo_start_115 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_port_tbo_c.o"(.bss.sTbBgQueue)) __xbo_end_115 = .;
 . = ALIGN(4); __xbo_start_116 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_port_tbo_c.o"(.bss.sTbBgKind)) __xbo_end_116 = .;
 . = ALIGN(4); __xbo_start_117 = .; KEEP("/workspace/scratch/0e938d660765/work-030/DragonRage360-Demo/build/boot-033/objects/src_menu_port_tbo_c.o"(.bss.sTb)) __xbo_end_117 = .;
 __xbo_start_118 = gPortTboSpar; __xbo_end_118 = gPortTboSpar+4;
		*(.sbss .sbss.* .bss .bss.* COMMON)
		__xbo_data_end = .;
	}

	/* Executable memory for a recompiler: its own 64 KB-aligned pages, which elf2xex marks Code (milestone 55,
	   docs/scope/55-windows-threads-and-memory.md). A title defines a .jit input section of the size it wants; without
	   one this is empty and costs nothing. */
	. = ALIGN(0x10000);
	.jit : {
		xsdk_jit_start = .;
		KEEP(*(.jit .jit.*))
		xsdk_jit_end = .;
	}

	.xsdk.imports 0 : {
		KEEP(*(.xsdk.imports))
	}

	/DISCARD/ : {
		*(.comment .note.* .gnu.attributes)
	}
}
