# Copyright (c) 2020-present Caps Collective & contributors
# Originally authored by Jonathan Moallem (@jonjondev) & Aryeh Zinn (@Raelr)
#
# This code is released under an unmodified zlib license.
# For conditions of distribution and use, please see:
#     https://opensource.org/licenses/Zlib

# Define custom functions
rwildcard = $(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))
platformpth = $(subst /,$(PATHSEP),$1)

# Set global macros
buildDir := bin
executable := app
target := $(buildDir)/$(executable)
sources := $(call rwildcard,src/,*.cpp)
objects := $(patsubst src/%, $(buildDir)/%, $(patsubst %.cpp, %.o, $(sources)))
depends := $(patsubst %.o, %.d, $(objects))
compileFlags := -std=c++17 -isystem include -O2 -Wall -Wextra
linkFlags = -L lib/$(platform) -l raylib

# ── Font embedding ───────────────────────────────────
FONT_SUBSET := assets/fonts/game-font.ttf
FONT_SRC := $(firstword $(filter-out $(FONT_SUBSET),$(wildcard assets/fonts/*.ttf)))
FONT_HEADER := src/FontData.h
FONT_CHARS := assets/fonts/chars.txt

# Check for Windows
ifeq ($(OS), Windows_NT)
	# Set Windows macros
	platform := Windows
	CXX ?= g++
	linkFlags += -Wl,--allow-multiple-definition -pthread -lopengl32 -lgdi32 -lwinmm -mwindows -static -static-libgcc -static-libstdc++
	THEN := &&
	PATHSEP := \$(BLANK)
	MKDIR := -mkdir -p
	RM := -del /q
	COPY = -robocopy "$(call platformpth,$1)" "$(call platformpth,$2)" $3
else
	# Check for MacOS/Linux
	UNAMEOS := $(shell uname)
	ifeq ($(UNAMEOS), Linux)
		# Set Linux macros
		platform := Linux
		CXX ?= g++
		linkFlags += -l GL -l m -l pthread -l dl -l rt -l X11
	endif
	ifeq ($(UNAMEOS), Darwin)
		# Set macOS macros
		platform := macOS
		CXX ?= clang++
		linkFlags += -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL
	endif

	# Set UNIX macros
	THEN := ;
	PATHSEP := /
	MKDIR := mkdir -p
	RM := rm -rf
	COPY = cp $1$(PATHSEP)$3 $2
endif

# Lists phony targets for Makefile
.PHONY: all setup submodules execute clean setup-font font-clean

# Default target, compiles, executes and cleans
all: $(target) execute

# Sets up the project for compiling, generates includes and libs
setup: submodules include lib
	@if [ -z "$(FONT_SRC)" ]; then \
		echo ""; \
		echo "  ⚠  No .ttf font found in assets/fonts/"; \
		echo "  →  Download a CJK-capable font (e.g. Noto Sans SC) and place it in assets/fonts/"; \
		echo "  →  Then run: make setup-font"; \
		echo ""; \
	else \
		echo "  Font found: $(FONT_SRC)"; \
		echo "  →  Run 'make setup-font' to embed it into the binary"; \
	fi

# ── Font subsetting & embedding ─────────────────────

# pyftsubset: creates a minimal font containing only used characters
$(FONT_SUBSET): $(FONT_SRC) $(FONT_CHARS)
	@echo "Subsetting font to game characters only..."
	@if command -v pyftsubset >/dev/null 2>&1; then \
		pyftsubset "$<" --text-file="$(FONT_CHARS)" --output-file="$@"; \
		echo "  Subset font created: $(FONT_SUBSET)"; \
	else \
		echo "  ERROR: pyftsubset not found. Install fonttools: pip install fonttools"; \
		exit 1; \
	fi

# xxd: embeds the subset font as a C byte array
$(FONT_HEADER): $(FONT_SUBSET)
	@echo "Embedding font into C header..."
	xxd -i -n FontData_ttf "$<" > "$@"
	@echo "  Font header generated: $(FONT_HEADER)"

# One-shot target: download font → subset → embed
setup-font: $(FONT_HEADER)
	@echo "Font embedding complete."

# Clean font artifacts
font-clean:
	$(RM) $(call platformpth, $(FONT_SUBSET))
	$(RM) $(call platformpth, $(FONT_HEADER))

# Pull and update the the build submodules
submodules:
	git submodule update --init --recursive --depth 1

# Copy the relevant header files into includes
include:
	$(MKDIR) $(call platformpth, ./include)
	$(call COPY,vendor/raylib/src,./include,raylib.h)
	$(call COPY,vendor/raylib/src,./include,raymath.h)
	$(call COPY,vendor/raylib/src,./include,rlgl.h)
	$(call COPY,vendor/raylib-cpp/include,./include,*.hpp)

# Build the raylib static library file and copy it into lib
lib:
	cd vendor/raylib/src $(THEN) "$(MAKE)" PLATFORM=PLATFORM_DESKTOP
	$(MKDIR) $(call platformpth, lib/$(platform))
	$(call COPY,vendor/raylib/src,lib/$(platform),libraylib.a)

# Link the program and create the executable
$(target): $(objects)
	$(CXX) $(objects) -o $(target) $(linkFlags)

# Add all rules from dependency files
-include $(depends)

# main.o depends on embedded font header (only if font source exists)
ifneq ($(FONT_SRC),)
bin/main.o: $(FONT_HEADER)
endif

# Compile objects to the build directory
$(buildDir)/%.o: src/%.cpp Makefile
	$(MKDIR) $(call platformpth, $(@D))
	$(CXX) -MMD -MP -c $(compileFlags) $< -o $@ $(CXXFLAGS)

# Run the executable
execute: $(target)
	$(target) $(ARGS)

# Clean up all relevant files
clean:
	$(RM) $(call platformpth, $(buildDir)/*)
