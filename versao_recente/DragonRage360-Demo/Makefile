XSDK ?= ../xsdk
.DEFAULT_GOAL := all
TOOLCHAIN ?= ../xsdk-gcc/bin/powerpc-xsdk-elf-
CC := $(TOOLCHAIN)gcc
# Retain real native component APIs for static verification in the diagnostic.
NATIVE_API_ROOTS := -Wl,--undefined=sceMpegInit -Wl,--undefined=sceMpegCreate -Wl,--undefined=sceMpegDelete -Wl,--undefined=sceMpegReset -Wl,--undefined=sceMpegIsEnd -Wl,--undefined=sceMpegAddCallback -Wl,--undefined=sceMpegAddStrCallback -Wl,--undefined=sceMpegDemuxPss -Wl,--undefined=sceMpegGetPicture -Wl,--undefined=sceGsResetPath -Wl,--undefined=sceGsResetGraph -Wl,--undefined=dr_gs_movie_build -Wl,--undefined=dr_gs_movie_send -Wl,--undefined=XbAdx_Clock -Wl,--undefined=ADXT_Init -Wl,--undefined=ADXT_Create -Wl,--undefined=ADXT_SetReloadSct -Wl,--undefined=ADXT_SetSvrFreq -Wl,--undefined=ADXT_StartAfs -Wl,--undefined=ADXT_StartFname -Wl,--undefined=ADXT_Stop -Wl,--undefined=ADXT_Pause -Wl,--undefined=ADXT_SetOutVol -Wl,--undefined=ADXT_GetOutVol -Wl,--undefined=ADXT_SetOutPan -Wl,--undefined=ADXT_SetOutputMono -Wl,--undefined=ADXT_GetStat -Wl,--undefined=XbOverlay_PrepareBuild -Wl,--undefined=XbOverlay_Load -Wl,--undefined=XbMpeg_Open -Wl,--undefined=XbMpeg_Next -Wl,--undefined=XbMpeg_Close
CFLAGS := -mcpu=cell -fno-pic -std=gnu17 -MMD -MP -O2 -g -ffunction-sections -fdata-sections -Wall -Wextra -Werror -I$(XSDK)/include -Isrc -Iengine/graphics -Iengine/capture/port/src/gs -Iengine/platform/platform -DDR_XBOX360
OBJECTS := build/dr_gs_movie.o build/dr_dma_engine.o build/dr_gs_native.o build/dr_dma_producer.o build/dr_gs_stream.o build/gs_draw.o build/gs_core.o build/gs_sony.o build/dr_gs_backend.o build/dr_gs_replay.o build/dr_capture_demo.o build/dr_capture_platform.o build/main.o build/demo_ui.o build/dr_disc.o build/bt3_wire.o build/net_check.o build/lobby.o build/dr_anim.o build/dr_bpe.o build/dr_resource_selftest.o build/dr_container.o build/dr_stage_collision.o build/dr_skeleton.o build/dr_xenos.o build/dr_gs_texture.o build/dr_graphics_demo.o build/dr_body_demo.o build/dr_texfile.o build/dr_body_pipeline.o build/dr_body_model.o build/dr_elf.o build/dr_face_vif.o build/dr_targets.o build/dr_vu1.o build/dr_vu1_selftest.o build/save_unlock.o build/save_export.o
.PHONY: all clean host-test
PLATFORM_SOURCES := $(addprefix engine/platform/platform/,$(shell cat engine/platform/platform-sources.txt))
PLATFORM_OBJECTS := $(patsubst engine/platform/platform/%.c,build/%.o,$(PLATFORM_SOURCES))
OBJECTS += $(PLATFORM_OBJECTS)
$(PLATFORM_OBJECTS): build/%.o: engine/platform/platform/%.c
	$(CC) $(CFLAGS) -DPORT_BIGENDIAN -DXB_USE_NTALLOC -DXB_USE_XAM -DXB_USE_NTDELAY -c $< -o $@
all: build/default.xex
build/%.o: src/%.c
	mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@
build/default.elf: $(OBJECTS) $(XSDK)/build/libxsdk.a
	$(CC) -static -nostdlib -T $(XSDK)/src/xsdk.ld -Wl,--gc-sections,-Map,build/default.map -o $@ $(OBJECTS) $(NATIVE_API_ROOTS) -Wl,--start-group $(XSDK)/build/libxsdk.a -lc -lm -lgcc -Wl,--end-group
build/default.xex: build/default.elf
	python3 $(XSDK)/tools/elf2xex.py --title-id 0x44523630 --title-name "DragonRage360 Demo" $< $@
host-test:
	mkdir -p build
	gcc -std=c17 -Wall -Wextra -Werror -Isrc src/demo_ui.c tests/test_ui.c -o build/test-ui
	./build/test-ui
clean:
	rm -rf build
.PHONY: disc-test
disc-test:
	mkdir -p build
	gcc -std=c99 -Wall -Wextra -Werror -Isrc src/dr_disc.c tests/disc_probe.c -o build/disc-probe
	python3 tests/test_disc.py
.PHONY: save-test
save-test:
	mkdir -p build
	gcc -std=c17 -Wall -Wextra -Werror -Isrc src/save_unlock.c src/save_export.c tests/save/test_export.c -o build/test-save-export
	python3 tests/save/test_save.py build/test-save-export

.PHONY: lobby-test
lobby-test:
	mkdir -p build
	gcc -std=c17 -Wall -Wextra -Werror -Isrc src/lobby.c tests/test_lobby.c -o build/test-lobby
	./build/test-lobby

-include $(OBJECTS:.o=.d)

# Reference VU arithmetic must not become fused multiply-add.
build/dr_vu1.o: CFLAGS += -ffp-contract=off

build/dr_body_pipeline.o: CFLAGS += -ffp-contract=off

build/dr_stage_collision.o: CFLAGS += -mcheck-fpr-alignment

# Additional compile checks; actual GS capture/backend now also enter the title link.
# Combat simulation and full PS2 platform layers remain absent.
.PHONY: engine-backend-check
engine-backend-check: build/dr_gs_backend.experimental.o build/dr_gs_replay.experimental.o
build/dr_gs_backend.experimental.o: engine/graphics/dr_gs_backend.c engine/graphics/dr_gs_backend.h engine/upstream/gs_draw.h
	$(CC) $(CFLAGS) -DDR_XBOX360 -mcheck-fpr-alignment -Iengine/graphics -Iengine/upstream -c $< -o $@
build/dr_gs_replay.experimental.o: engine/graphics/dr_gs_replay.c engine/graphics/dr_gs_replay.h engine/upstream/gs_draw.h
	$(CC) $(CFLAGS) -DDR_XBOX360 -mcheck-fpr-alignment -Iengine/graphics -Iengine/upstream -c $< -o $@

-include build/dr_gs_backend.experimental.d build/dr_gs_replay.experimental.d

build/gs_draw.o: engine/capture/port/src/gs/gs_draw.c
	$(CC) $(CFLAGS) -mcheck-fpr-alignment -c $< -o $@
build/gs_core.o: engine/capture/port/src/gs/gs_core.c
	$(CC) $(CFLAGS) -mcheck-fpr-alignment -c $< -o $@
build/gs_sony.o: engine/capture/port/src/gs/gs_sony.c
	$(CC) $(CFLAGS) -c $< -o $@
build/dr_gs_backend.o: engine/graphics/dr_gs_backend.c
	$(CC) $(CFLAGS) -mcheck-fpr-alignment -c $< -o $@
build/dr_gs_replay.o: engine/graphics/dr_gs_replay.c
	$(CC) $(CFLAGS) -mcheck-fpr-alignment -c $< -o $@

# Strict integration attempt: never overwrites build/default.elf or default.xex.
# This target still fails on missing services/data. No generated stubs.
UPSTREAM ?= ../../dragonrage-upstream
FLOAT_MODE ?= native-ps2
PLUGIN_ARGS ?=
.PHONY: battle-link
battle-link: $(OBJECTS) $(XSDK)/build/libxsdk.a
	python3 engine/tools/link_battle.py $(UPSTREAM) $(CC) $(XSDK) build/battle-native --float-mode $(FLOAT_MODE) --platform $(PLUGIN_ARGS)

build/dr_dma_engine.o: engine/graphics/dr_dma_engine.c engine/graphics/dr_dma_engine.h
	$(CC) $(CFLAGS) -c $< -o $@

build/dr_gs_movie.o: engine/graphics/dr_gs_movie.c engine/graphics/dr_gs_movie.h
	$(CC) $(CFLAGS) -c $< -o $@

# Links the real combat code with a diagnostic entry, never calls the fight.
.PHONY: engine-linked-diagnostic
engine-linked-diagnostic: $(OBJECTS) $(XSDK)/build/libxsdk.a
	python3 engine/tools/link_battle.py $(UPSTREAM) $(CC) $(XSDK) build/battle-native --float-mode $(FLOAT_MODE) --platform --fixed-battle $(PLUGIN_ARGS)
	python3 engine/tools/test_native_data_sections.py build/battle-native build/battle-native/tests
	python3 engine/tools/build_engine_diagnostic.py build/battle-native $(XSDK) build/engine-diagnostic
	python3 tests/check_xex.py build/engine-diagnostic build/engine-diagnostic/VALIDACAO.json

# ISO is an external user-owned input. Extract only its two programs locally;
# never distribute build/typed-input. Partial catalogs never authorize combat.
ISO ?= game.iso
.PHONY: typed-data-diagnostic
typed-data-diagnostic: $(OBJECTS) $(XSDK)/build/libxsdk.a
	python3 engine/tools/link_battle.py $(UPSTREAM) $(CC) $(XSDK) build/typed-base --float-mode $(FLOAT_MODE) --platform --fixed-battle $(PLUGIN_ARGS)
	python3 engine/tools/extract_iso.py $(ISO) build/typed-input --programs-only
	python3 engine/tools/build_typed_manifest.py $(UPSTREAM) build/typed-base build/typed-input/disc build/typed-manifest
	python3 engine/tools/test_typed_manifest.py build/typed-manifest/typed-manifest.json build/typed-input/disc build/typed-test --sanitize
	python3 engine/tools/test_water_pool.py build/typed-input/disc/SLUS_216.78 build/typed-manifest/water-pool-proof.json
	python3 engine/tools/test_reviewed_unused.py build/typed-manifest/typed-manifest.json build/typed-base $(CC) build/unused-native-test
	python3 engine/tools/test_fixed_resources.py $(XSDK) $(ISO) build/fixed-resources --sanitize
	python3 engine/tools/link_battle.py $(UPSTREAM) $(CC) $(XSDK) build/fixed-manifest --float-mode $(FLOAT_MODE) --platform --fixed-battle --typed-catalog build/typed-manifest/typed_catalog.c $(PLUGIN_ARGS)
	python3 engine/tools/build_engine_diagnostic.py build/fixed-manifest $(XSDK) build/typed-diagnostic
	python3 engine/tools/verify_typed_catalog.py build/typed-manifest/typed-manifest.json build/typed-diagnostic/default.elf build/typed-diagnostic/catalog-verification.json
	python3 tests/check_xex.py build/typed-diagnostic build/typed-diagnostic/VALIDACAO.json

# Native stage adapter uses the original typed views; port_compat fixes s64 on PPC32.
build/xb_battle_stage.o: CFLAGS += -I$(UPSTREAM)/include -I$(UPSTREAM)/port/include -mcheck-fpr-alignment

build/xb_battle_fighter.o: CFLAGS += -I$(UPSTREAM)/include -I$(UPSTREAM)/port/include -mcheck-fpr-alignment

build/xb_fighter_registry.o: CFLAGS += -I$(UPSTREAM)/include -I$(UPSTREAM)/port/include -mcheck-fpr-alignment

# Refuse accidental decomp duplicates, compatibility-header shadowing and raw
# binary tables before compiling or linking platform modules.
.PHONY: platform-audit
platform-audit:
	python3 engine/tools/audit_platform_sources.py .
$(OBJECTS): | platform-audit
build/default.elf: | platform-audit

build/xb_model_native.o: CFLAGS += -I$(UPSTREAM)/include -I$(UPSTREAM)/port/include -mcheck-fpr-alignment

# Original roster/common reader layouts used by the bounded D9 probe.
build/xb_battle_consumers.o: CFLAGS += -I$(UPSTREAM)/include -I$(UPSTREAM)/port/include -mcheck-fpr-alignment

build/xb_battle_runtime.o: CFLAGS += -I$(UPSTREAM)/include -I$(UPSTREAM)/port/include -mcheck-fpr-alignment
