TARGET_EXEC := doom_matrix

BUILD_DIR := build
SRC_DIRS := src
AUDIO ?= 1
UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Darwin)
$(error doom-matrix requires Linux/Raspberry Pi userspace; macOS cannot build libs/rpi-rgb-led-matrix. Build this project on a Raspberry Pi or Linux target instead)
endif

DOOMGENERIC_DIR := libs/doomgeneric/doomgeneric
SRCS_DOOM = dummy.c am_map.c doomdef.c doomstat.c dstrings.c d_event.c d_items.c d_iwad.c d_loop.c d_main.c d_mode.c d_net.c f_finale.c f_wipe.c g_game.c hu_lib.c hu_stuff.c info.c i_cdmus.c i_endoom.c i_joystick.c i_scale.c i_sound.c i_system.c i_timer.c memio.c m_argv.c m_bbox.c m_cheat.c m_config.c m_controls.c m_fixed.c m_menu.c m_misc.c m_random.c p_ceilng.c p_doors.c p_enemy.c p_floor.c p_inter.c p_lights.c p_map.c p_maputl.c p_mobj.c p_plats.c p_pspr.c p_saveg.c p_setup.c p_sight.c p_spec.c p_switch.c p_telept.c p_tick.c p_user.c r_bsp.c r_data.c r_draw.c r_main.c r_plane.c r_segs.c r_sky.c r_things.c sha1.c sounds.c statdump.c st_lib.c st_stuff.c s_sound.c tables.c v_video.c wi_stuff.c w_checksum.c w_file.c w_main.c w_wad.c z_zone.c w_file_stdc.c i_input.c i_video.c mus2mid.c doomgeneric.c
ifeq ($(AUDIO),1)
SRCS_DOOM += i_sdlmusic.c i_sdlsound.c
endif
SRCS_DOOM := $(addprefix $(DOOMGENERIC_DIR)/,$(SRCS_DOOM))
OBJS := $(patsubst %.c,$(BUILD_DIR)/%.o,$(SRCS_DOOM))

RGB_LIB_DISTRIBUTION=libs/rpi-rgb-led-matrix
RGB_LIBDIR=$(RGB_LIB_DISTRIBUTION)/lib
RGB_LIBRARY_NAME=rgbmatrix
RGB_LIBRARY=$(RGB_LIBDIR)/lib$(RGB_LIBRARY_NAME).a
RGB_LIBRARY_INCLUDE=$(RGB_LIB_DISTRIBUTION)/include

ifeq ($(AUDIO),1)
SDL2_CONFIG ?= sdl2-config
SDL_CFLAGS ?= $(shell $(SDL2_CONFIG) --cflags)
SDL_LDLIBS ?= $(shell $(SDL2_CONFIG) --libs)
SDL_MIXER_CFLAGS ?= $(shell pkg-config --cflags SDL2_mixer 2>/dev/null)
SDL_MIXER_LDLIBS ?= $(shell pkg-config --libs SDL2_mixer 2>/dev/null)
endif

OPTFLAGS ?= -O3 -DNDEBUG
LTOFLAGS ?= -flto
ARCHFLAGS ?= -march=native -mtune=native

LDFLAGS += -L$(RGB_LIBDIR) $(LTOFLAGS)
LDLIBS += -l$(RGB_LIBRARY_NAME) -lrt -lm -lc -lpthread
ifeq ($(AUDIO),1)
LDLIBS += $(SDL_LDLIBS) $(SDL_MIXER_LDLIBS)
endif

SRCS := $(shell find $(SRC_DIRS) -name '*.cpp' -or -name '*.c')
OBJS += $(patsubst %.c,$(BUILD_DIR)/%.o,$(filter %.c,$(SRCS)))
OBJS += $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(filter %.cpp,$(SRCS)))

INC_DIRS := $(shell find $(SRC_DIRS) -type d) $(DOOMGENERIC_DIR) $(RGB_LIBRARY_INCLUDE)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))
ifeq ($(AUDIO),1)
INC_FLAGS += $(SDL_CFLAGS)
INC_FLAGS += $(SDL_MIXER_CFLAGS)
CFLAGS += -DFEATURE_SOUND
endif
CFLAGS += -Wall $(OPTFLAGS) $(ARCHFLAGS) $(LTOFLAGS)
CPPFLAGS := $(INC_FLAGS) -MMD -MP

$(TARGET_EXEC): $(OBJS) $(RGB_LIBRARY)
	$(CXX) $(shell find $(BUILD_DIR) -name '*.o') -o $@ $(LDFLAGS) $(LDLIBS)

$(RGB_LIBRARY): FORCE
	$(MAKE) -C $(RGB_LIBDIR) CC="$(CC)" CXX="$(CXX)" AR="$(AR)"

$(BUILD_DIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@ $(LIBS)

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)
	rm -f $(TARGET_EXEC)
	$(MAKE) -C $(RGB_LIBDIR) clean

FORCE:
.PHONY: FORCE
