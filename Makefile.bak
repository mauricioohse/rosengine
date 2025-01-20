# Common variables
CXX_WINDOWS = g++
CXX_WEB = emcc
CXXFLAGS = -Wall
INCLUDES = -I./include/SDL2

# Source files
SOURCES = src/main.cpp
BUILD_DIR = bin
WEB_DIR = web

# Output targets
DEBUG_DIR = $(BUILD_DIR)/debug
RELEASE_DIR = $(BUILD_DIR)/release
DEBUG_TARGET = $(DEBUG_DIR)/game.exe
RELEASE_TARGET = $(RELEASE_DIR)/game.exe
WEB_TARGET = $(WEB_DIR)/index.html

# DLL files to copy (using wildcard to get all DLLs)
DLLS = $(wildcard dll/*.dll)

# Debug-specific
DEBUG_FLAGS = -g -DDEBUG
DEBUG_LIBS = -L./lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer

# Release-specific (with static linking)
RELEASE_FLAGS = -O2 -DNDEBUG
RELEASE_LIBS = -L./lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -lwinmm -lusp10 -lgdi32 \
    -static -static-libgcc -static-libstdc++ \
    -lole32 -loleaut32 -limm32 -lversion -lsetupapi -lcfgmgr32 -lrpcrt4 \
    -mwindows

# Web-specific
WEB_FLAGS = -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s USE_SDL_TTF=2 -s USE_SDL_MIXER=2 \
    -s SDL2_IMAGE_FORMATS='["png"]' \
    -s SDL2_MIXER_FORMATS='["wav","mp3"]' \
    --preload-file assets \
    -s ALLOW_MEMORY_GROWTH=1 \
    -s INITIAL_MEMORY=67108864

# Create build directories
$(shell mkdir -p $(DEBUG_DIR) $(RELEASE_DIR) $(WEB_DIR))

# Default target
help:
	@echo "Please specify a target: debug, release, or web"
	@echo "Usage:"
	@echo "  make debug   - Build debug version with DLLs"
	@echo "  make release - Build release version (standalone)"
	@echo "  make web     - Build web version"
	@echo "  make zip     - Create release zip package"
	@echo "  make all     - Build everything (debug + release + web + zip)"

# Debug build
debug: $(DEBUG_TARGET) copy_dlls_debug copy_assets_debug

$(DEBUG_TARGET): $(SOURCES)
	$(CXX_WINDOWS) $(SOURCES) $(INCLUDES) $(CXXFLAGS) $(DEBUG_FLAGS) $(DEBUG_LIBS) -o $(DEBUG_TARGET)
	@echo "Debug build complete: $(DEBUG_TARGET)"

copy_dlls_debug:
	@echo "Copying DLLs to debug directory..."
	@for dll in $(DLLS); do \
		cp $$dll $(DEBUG_DIR)/; \
	done

copy_assets_debug:
	@echo "Copying assets to debug directory..."
	@rm -rf $(DEBUG_DIR)/assets
	@cp -r assets $(DEBUG_DIR)/

# Release build
release: $(RELEASE_TARGET) copy_assets_release

$(RELEASE_TARGET): $(SOURCES)
	$(CXX_WINDOWS) $(SOURCES) $(INCLUDES) $(CXXFLAGS) $(RELEASE_FLAGS) $(RELEASE_LIBS) -o $(RELEASE_TARGET)
	@echo "Release build complete: $(RELEASE_TARGET)"

copy_assets_release:
	@echo "Copying assets to release directory..."
	@rm -rf $(RELEASE_DIR)/assets
	@cp -r assets $(RELEASE_DIR)/

# Web build
web: $(WEB_TARGET)

$(WEB_TARGET): $(SOURCES)
	$(CXX_WEB) $(SOURCES) $(INCLUDES) $(CXXFLAGS) $(WEB_FLAGS) -o $(WEB_TARGET)
	@echo "Web build complete: $(WEB_TARGET)"

# Create release package
zip: release web
	@echo "Creating release packages..."
	@mkdir -p releases
	@rm -f releases/cuckoo-launch-win.zip releases/cuckoo-launch-web.zip  # Remove existing zips if they exist
	cd $(RELEASE_DIR) && 7z a -tzip ../../releases/cuckoo-launch-win.zip ./*
	cd $(WEB_DIR) && 7z a -tzip ../releases/cuckoo-launch-web.zip ./*
	@echo "Release packages created in releases folder"

# actually make all
all: debug release web zip

# Clean specific builds
clean-debug:
	rm -rf $(DEBUG_DIR)

clean-release:
	rm -rf $(RELEASE_DIR)

clean-web:
	rm -f $(WEB_DIR)/index.js $(WEB_DIR)/index.data $(WEB_DIR)/index.wasm

# Clean all builds
clean: clean-debug clean-release clean-web
	rm -f game-release.zip

# Make help the default target
.DEFAULT_GOAL := help

.PHONY: all debug release web zip alll clean clean-debug clean-release clean-web copy_dlls_debug copy_assets_debug copy_assets_release
