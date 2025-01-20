#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>

// Forward declarations
struct Texture {
    SDL_Texture* sdlTexture;
    int width;
    int height;
};

struct Font {
    TTF_Font* sdlFont;
    int size;
};

struct Sound {
    Mix_Chunk* sdlChunk;
};


// Define sound IDs
enum SoundID {
    SOUND_NONE = 0,
    SOUND_HIT,
    SOUND_BACKGROUND_MUSIC,
    SOUND_HELICOPTER,
    SOUND_WIND,
    SOUND_CLOUD_HIT,
    SOUND_CLOUD_BOUNCE,
    SOUND_CHOMP,
    SOUND_VICTORY,
    SOUND_MAX
};

// Define font IDs
enum FontID {
    FONT_NONE = 0,
    FONT_FPS,
    FONT_MAX
};

// Resource definitions
struct SoundResource {
    const char* path;
    SoundID id;
};

struct FontResource {
    const char* path;
    int size;
    FontID id;
};

// Define texture IDs
enum TextureID {
    TEXTURE_NONE = 0,
    TEXTURE_BOX,
    TEXTURE_WALL,
    TEXTURE_BOX_ANIM_SHEET,
    TEXTURE_SQUIRREL_SITTING,
    TEXTURE_SQUIRREL_OPEN,
    TEXTURE_SQUIRREL_OPENING,
    TEXTURE_SQUIRREL_CLOSED,
    TEXTURE_CLOUD_1,
    TEXTURE_BLACK_CLOUD_SMALL,
    TEXTURE_HELICOPTER,
    TEXTURE_BACKGROUND_MIDDLE,
    TEXTURE_BACKGROUND_BOTTOM,
    TEXTURE_PEANUT,
    TEXTURE_SHIELD_PEANUT,
    TEXTURE_SUPER_PEANUT,
    TEXTURE_WHITE_CLOUDE_SMALL,
    TEXTURE_WHITE_CLOUDE_MEDIUM,
    TEXTURE_WHITE_CLOUDE_LARGE,
    TEXTURE_ARROW,
    TEXTURE_BACKGROUND_BOTTOM_2,
    TEXTURE_MAX
};


struct TextureResource {
    const char* path;
    TextureID id;
};

// Global resource definitions
static const TextureResource GAME_TEXTURES[] = {
    {"assets/box.png", TEXTURE_BOX},
    {"assets/wall.png", TEXTURE_WALL},
    {"assets/box_sheet.png", TEXTURE_BOX_ANIM_SHEET},
    {"assets/sprites/squirrel/squirrel_sitting.png", TEXTURE_SQUIRREL_SITTING},
    {"assets/sprites/squirrel/squirrel_open.png", TEXTURE_SQUIRREL_OPEN},
    {"assets/sprites/squirrel/squirrel_opening.png", TEXTURE_SQUIRREL_OPENING},
    {"assets/sprites/squirrel/squirrel_closed.png", TEXTURE_SQUIRREL_CLOSED},
    {"assets/sprites/clouds/cloud_1.png", TEXTURE_CLOUD_1},
    {"assets/sprites/clouds/cloud_black_small.png", TEXTURE_BLACK_CLOUD_SMALL},
    {"assets/sprites/helicopter.png", TEXTURE_HELICOPTER},
    {"assets/background/background_middle.png", TEXTURE_BACKGROUND_MIDDLE},
    {"assets/background/background_bottom.png", TEXTURE_BACKGROUND_BOTTOM},
    {"assets/sprites/peanuts/peanut_boost.png", TEXTURE_PEANUT},
    {"assets/sprites/peanuts/peanut_shield.png", TEXTURE_SHIELD_PEANUT},
    {"assets/sprites/peanuts/peanut_super.png", TEXTURE_SUPER_PEANUT},
    {"assets/sprites/clouds/cloud_white_small.png", TEXTURE_WHITE_CLOUDE_SMALL},
    {"assets/sprites/clouds/cloud_white_medium.png", TEXTURE_WHITE_CLOUDE_MEDIUM},
    {"assets/sprites/clouds/cloud_white_large.png", TEXTURE_WHITE_CLOUDE_LARGE},
    {"assets/sprites/arrow.png", TEXTURE_ARROW},
    {"assets/background/background_bottom_2.png", TEXTURE_BACKGROUND_BOTTOM_2},
    // Add new textures here
};




static const SoundResource GAME_SOUNDS[] = {
    {"assets/sounds/hit.wav", SOUND_HIT},
    {"assets/sounds/music.mp3", SOUND_BACKGROUND_MUSIC},
    {"assets/sounds/helicopter.wav", SOUND_HELICOPTER}, // 49483__lorenzosu__helicopterraw_30sec.wav
    {"assets/sounds/wind.mp3", SOUND_WIND}, // 420301__tonik-95__wind18.m4a
    {"assets/sounds/hit.wav", SOUND_CLOUD_HIT},      // Add this
    {"assets/sounds/Cartoon Boing Sound Effect.mp3", SOUND_CLOUD_BOUNCE}, // https://www.youtube.com/watch?v=F7oMDw2zKog&list=PLM3YufdBArxhztFAvw_d26CsAEJqAK_TB
    {"assets/sounds/chomp.wav", SOUND_CHOMP}, // https://freesound.org/people/maugusto_sfx/sounds/521253/
    {"assets/sounds/victory.wav", SOUND_VICTORY}, // https://freesound.org/people/maugusto_sfx/sounds/521253/
    // Add new sounds here
};

static const FontResource GAME_FONTS[] = {
    {"assets/fonts/VCR_OSD_MONO_1.001.ttf", 16, FONT_FPS},
    // Add new fonts here
};

struct ResourceManager {
    static void Cleanup();
    
    // Texture management
    static Texture* LoadTexture(const char* path);
    static void UnloadTexture(Texture* texture);
    static void RenderTexture(Texture* texture, int x, int y);

    // New methods for ID-based resources
    static Texture* LoadTexture(const char* path, TextureID id);
    static Texture* GetTexture(TextureID id);
    static void UnloadTexture(TextureID id);

    // Font management
    static Font* LoadFont(const char* path, int size);
    static void UnloadFont(Font* font);
    static Texture *GetTextTexture(Font *font, const char *text, SDL_Color color);
    static int RenderTextAlignedTopRight(Font* font, const char* text, SDL_Color color, int x, int y);
    static void RenderTextAlignedCenter(Font* font, const char* text, SDL_Color color, int x, int y);

    // Sound management
    static Sound* LoadSound(const char* path);
    static void UnloadSound(Sound* sound);

    // New methods for ID-based resources
    static Sound* LoadSound(const char* path, SoundID id);
    static Sound* GetSound(SoundID id);
    static void UnloadSound(SoundID id);

    static Font* LoadFont(const char* path, int size, FontID id);
    static Font* GetFont(FontID id);
    static void UnloadFont(FontID id);

    // New initialization methods
    static bool InitAllResources();
    static void UnloadAllResources();

    // Music playback
    static void PlayMusic(SoundID id, int loops = -1);
    static void StopMusic();
    static void SetMusicVolume(int volume); // 0-128

private:
    // Fixed-size arrays for resources
    static Texture* textures[TEXTURE_MAX];
    static Sound* sounds[SOUND_MAX];
    static Font* fonts[FONT_MAX];

    // Helper methods for initialization
    static bool InitTextures();
    static bool InitSounds();
    static bool InitFonts();
}; 