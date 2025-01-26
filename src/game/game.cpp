#include "game.h"
#include "../core/resource_manager.h"
#include "../core/window.h"
#include "../core/input.h"
#include "cloud_init.h"
#include "peanut_init.h"
#include <math.h>

Game g_Game;

#define DEBUG_COLLISION 1



bool Game::Init() {
    // Initialize systems
    // renderSystem.Init();
    // squirrelSystem.Init();
    // cameraSystem.Init();
    // cloudSystem.Init();
    // backgroundSystem.Init();
    // peanutSystem.Init();
    // collisionSystem.Init();
    // musicSystem.Init();
    
    
    g_Engine.systemManager.RegisterSystem(&backgroundSystem);
    g_Engine.systemManager.RegisterSystem(&renderSystem);
    //g_Engine.systemManager.RegisterSystem(&squirrelSystem);
    g_Engine.systemManager.RegisterSystem(&cameraSystem);
    g_Engine.systemManager.RegisterSystem(&cloudSystem);
    g_Engine.systemManager.RegisterSystem(&peanutSystem);
    g_Engine.systemManager.RegisterSystem(&collisionSystem);
    g_Engine.systemManager.RegisterSystem(&musicSystem);
    g_Engine.systemManager.RegisterSystem(new WASDControllerSystem());
    g_Engine.systemManager.RegisterSystem(new IcePhysicsSystem());
    g_Engine.systemManager.RegisterSystem(new ShooterSystem());
    g_Engine.systemManager.RegisterSystem(new BalloonSystem());
    g_Engine.systemManager.RegisterSystem(&waveSystem);

    // Create background
    backgroundEntity = g_Engine.entityManager.CreateEntity();
    Texture* backgroundTexture = ResourceManager::GetTexture(TEXTURE_BACKGROUND_MIDDLE);
    ADD_TRANSFORM(backgroundEntity, -600.0f, 0.0f, 0.0f, 1.0f);
    ADD_SPRITE(backgroundEntity, backgroundTexture);
    ADD_BACKGROUND(backgroundEntity, 0.5f);  // 0.5 parallax factor for medium depth


    // Create squirrel entity
    squirrelEntity = g_Engine.entityManager.CreateEntity();
    
    Texture* squirrelTexture = ResourceManager::GetTexture(TEXTURE_SQUIRREL_OPEN);    
    // Add basic components
    ADD_TRANSFORM(squirrelEntity, 1200.0f, 100.0f, 0.0f, 1.0f);  // Center-top of screen
    ADD_PHYSICS(squirrelEntity, 100, 2);
    ADD_SPRITE(squirrelEntity, squirrelTexture);
    ADD_WASD_CONTROLLER(squirrelEntity, 400, 1);
    ADD_COLLIDER(squirrelEntity, 32, 32, 0, 0);
    ADD_SHOOTER(squirrelEntity, 150.0f, 300.0f, 1.0f);

    // create camera
    cameraEntity = g_Engine.entityManager.CreateEntity();
    ADD_TRANSFORM(cameraEntity, 1200.0f, 100.0f, 0.0f, 1.0f);
    ADD_CAMERA(cameraEntity, WINDOW_WIDTH, WINDOW_HEIGHT, squirrelEntity);

    // Store IDs for later use
    hitSoundID = SOUND_HIT;
    fpsFontID = FONT_FPS;
    

    gameTimer = 0.0f;


    // Start in START state instead of PLAYING
    gameState = GAME_STATE_START;
    bestTime = 0.0f;
    isNewRecord = false;

    // Create arrow entity
    arrowEntity = g_Engine.entityManager.CreateEntity();
    Texture* arrowTexture = ResourceManager::GetTexture(TEXTURE_ARROW);
    ADD_TRANSFORM(arrowEntity, 0.0f, 0.0f, 0.0f, 1.0f);
    ADD_SPRITE(arrowEntity, arrowTexture);

    return true;
}

// logic related to inputs on the game should go here
void Game::HandleInput(){
    // Check for game start inputs when in start state
    if (gameState == GAME_STATE_START) {
        if (Input::IsKeyPressed(SDL_SCANCODE_W) || 
            Input::IsKeyPressed(SDL_SCANCODE_A) || 
            Input::IsKeyPressed(SDL_SCANCODE_S) || 
            Input::IsKeyPressed(SDL_SCANCODE_D) || 
            Input::IsKeyPressed(SDL_SCANCODE_SPACE) ||
            Input::mouseButtonsPressed[0]) {
            
            gameState = GAME_STATE_PLAYING;
            return;
        }
    }

    // Play sound on mouse click
    if (Input::mouseButtonsPressed[0]) {  // Left click
        Sound* hitSound = ResourceManager::GetSound(SOUND_HIT);
        if (hitSound) {
            Mix_PlayChannel(-1, hitSound->sdlChunk, 0);
        }
    }
    
    // Add reset on 'R' key press
    if (Input::IsKeyPressed(SDL_SCANCODE_R)) {
        Reset();
    }
}

void Game::Update(float deltaTime) {
    HandleInput();

    // Only update timer if game is being played
    if (gameState == GAME_STATE_PLAYING) {
        gameTimer += deltaTime;
    }

    // Get squirrel position
    TransformComponent* squirrelTransform =
        (TransformComponent*)g_Engine.componentArrays.GetComponentData(squirrelEntity, COMPONENT_TRANSFORM);

    if (squirrelTransform) {
        // Check if squirrel is outside arena boundary (radius 800)
        float dx = squirrelTransform->x;
        float dy = squirrelTransform->y;
        float distanceFromCenter = sqrt(dx*dx + dy*dy);
        
        if (distanceFromCenter > 800.0f) {
            // Reset position to center if outside boundary
            squirrelTransform->x = 0.0f;
            squirrelTransform->y = 0.0f;
            
            // Reset velocity
            PhysicsComponent* physics = 
                (PhysicsComponent*)g_Engine.componentArrays.GetComponentData(squirrelEntity, COMPONENT_PHYSICS);
            if (physics) {
                physics->velocityX = 0.0f;
                physics->velocityY = 0.0f;
            }

            Reset();
        }
    }
}

void Game::Render() {
    // Always render game world
    g_Engine.systemManager.UpdateSystems(g_Engine.deltaTime, &g_Engine.entityManager, &g_Engine.componentArrays);
    
    // Get camera position
    CameraComponent* camera = nullptr;
    for (EntityID entity = 1; entity < MAX_ENTITIES; entity++) {
        if (g_Engine.entityManager.HasComponent(entity, COMPONENT_CAMERA)) {
            camera = (CameraComponent*)g_Engine.componentArrays.GetComponentData(entity, COMPONENT_CAMERA);
            break;
        }
    }
    
    if (!camera) return;

    // Draw arena boundary circle
    SDL_SetRenderDrawColor(g_Engine.window->renderer, 255, 0, 0, 255);  // Red color
    const int SEGMENTS = 32;  // Number of line segments to draw the circle
    const float RADIUS = 800.0f;
    
    for (int i = 0; i < SEGMENTS; i++) {
        float angle1 = (float)i * 2.0f * 3.14159f / SEGMENTS;
        float angle2 = (float)(i + 1) * 2.0f * 3.14159f / SEGMENTS;
        
        // Calculate world positions
        int x1 = (int)(RADIUS * cos(angle1));
        int y1 = (int)(RADIUS * sin(angle1));
        int x2 = (int)(RADIUS * cos(angle2));
        int y2 = (int)(RADIUS * sin(angle2));
        
        // Adjust for camera position
        x1 -= (int)camera->x;
        y1 -= (int)camera->y;
        x2 -= (int)camera->x;
        y2 -= (int)camera->y;
        
        SDL_RenderDrawLine(g_Engine.window->renderer, x1, y1, x2, y2);
    }

    // Draw collision boundaries
    SDL_SetRenderDrawColor(g_Engine.window->renderer, 255, 0, 0, 128);  // Semi-transparent red
    
    // Loop through all entities with colliders
    if (DEBUG_COLLISION) // can be disabled by changing here
    for (EntityID entity = 1; entity < MAX_ENTITIES; entity++) {
        if (g_Engine.entityManager.HasComponent(entity, COMPONENT_COLLIDER | COMPONENT_TRANSFORM)) {
            TransformComponent* transform = 
                (TransformComponent*)g_Engine.componentArrays.GetComponentData(entity, COMPONENT_TRANSFORM);
            ColliderComponent* collider = 
                (ColliderComponent*)g_Engine.componentArrays.GetComponentData(entity, COMPONENT_COLLIDER);
                
            if (transform && collider) {
                // Calculate screen position (adjust for camera)
                SDL_Rect rect;
                rect.x = (int)(transform->x - camera->x - collider->width/2);  // Center horizontally
                rect.y = (int)(transform->y - camera->y - collider->height/2); // Center vertically
                rect.w = (int)collider->width;
                rect.h = (int)collider->height;
                
                // Draw rectangle outline
                SDL_RenderDrawRect(g_Engine.window->renderer, &rect);
                
                // For triggers, draw an X
                if (collider->isTrigger) {
                    SDL_RenderDrawLine(g_Engine.window->renderer, 
                        rect.x, rect.y, 
                        rect.x + rect.w, rect.y + rect.h);
                    SDL_RenderDrawLine(g_Engine.window->renderer, 
                        rect.x + rect.w, rect.y, 
                        rect.x, rect.y + rect.h);
                }
            }
        }
    }

    // Draw damage percentage
    PhysicsComponent* porcupinePhysics = 
        (PhysicsComponent*)g_Engine.componentArrays.GetComponentData(squirrelEntity, COMPONENT_PHYSICS);
    
    if (porcupinePhysics) {
        // Create text for damage display
        char damageText[32];
        snprintf(damageText, sizeof(damageText), "Damage: %.0f%%", porcupinePhysics->damagePercent);
        
        // Get font
        Font* font = ResourceManager::GetFont(fpsFontID);
        if (font) {
            // Create text surface
            SDL_Color textColor = {255, 50, 50, 255};  // Red color for damage
            SDL_Surface* textSurface = TTF_RenderText_Solid(font->sdlFont, damageText, textColor);
            
            if (textSurface) {
                // Create texture from surface
                SDL_Texture* textTexture = SDL_CreateTextureFromSurface(g_Engine.window->renderer, textSurface);
                
                if (textTexture) {
                    // Position text at bottom center of screen
                    SDL_Rect destRect;
                    destRect.w = textSurface->w;
                    destRect.h = textSurface->h;
                    destRect.x = (WINDOW_WIDTH - destRect.w) / 2;  // Center horizontally
                    destRect.y = WINDOW_HEIGHT - destRect.h - 20;  // 20 pixels from bottom
                    
                    // Render text
                    SDL_RenderCopy(g_Engine.window->renderer, textTexture, NULL, &destRect);
                    
                    // Clean up texture
                    SDL_DestroyTexture(textTexture);
                }
                
                // Clean up surface
                SDL_FreeSurface(textSurface);
            }
        }
    }

    // Draw score if game is being played or just ended
    if (gameTimer > 0) {
        // Get font
        Font* font = ResourceManager::GetFont(fpsFontID);
        if (font) {
            // Create score text
            char scoreText[64];
            snprintf(scoreText, sizeof(scoreText), "Time: %.1f seconds", gameTimer);
            
            if (isNewRecord) {
                snprintf(scoreText, sizeof(scoreText), "New Record! %.1f seconds", gameTimer);
            }
            
            // Create text surface
            SDL_Color textColor = {255, 255, 255, 255};
            SDL_Surface* textSurface = TTF_RenderText_Solid(font->sdlFont, scoreText, textColor);
            
            if (textSurface) {
                SDL_Texture* textTexture = SDL_CreateTextureFromSurface(g_Engine.window->renderer, textSurface);
                if (textTexture) {
                    SDL_Rect destRect;
                    destRect.w = textSurface->w;
                    destRect.h = textSurface->h;
                    destRect.x = (WINDOW_WIDTH - destRect.w) / 2;  // Center horizontally
                    destRect.y = 20;  // Near top of screen
                    
                    SDL_RenderCopy(g_Engine.window->renderer, textTexture, NULL, &destRect);
                    SDL_DestroyTexture(textTexture);
                }
                SDL_FreeSurface(textSurface);
            }
        }
    }

    // Modify start screen to show best time
    if (gameState == GAME_STATE_START) {
        Font* font = ResourceManager::GetFont(fpsFontID);
        if (font) {
            SDL_Color textColor = {255, 255, 255, 255};
            
            const char* controls[] = {
                "WASD for movement",
                "Left click to shoot quills",
                "",
                "",
                bestTime > 0 ? "Best Time: %.1f seconds" : "",
                "",
                "Press any key to start"
            };
            
            // Render each line of text
            for (int i = 0; i < 7; i++) {
                char lineBuffer[64];
                if (i == 4 && bestTime > 0) {
                    snprintf(lineBuffer, sizeof(lineBuffer), controls[i], bestTime);
                } else {
                    strncpy(lineBuffer, controls[i], sizeof(lineBuffer));
                }
                
                SDL_Surface* textSurface = TTF_RenderText_Solid(font->sdlFont, lineBuffer, textColor);
                if (textSurface) {
                    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(g_Engine.window->renderer, textSurface);
                    if (textTexture) {
                        SDL_Rect destRect;
                        destRect.w = textSurface->w;
                        destRect.h = textSurface->h;
                        destRect.x = (WINDOW_WIDTH - destRect.w) / 2;
                        destRect.y = (WINDOW_HEIGHT / 2) - (120 - i * 40);
                        
                        SDL_RenderCopy(g_Engine.window->renderer, textTexture, NULL, &destRect);
                        SDL_DestroyTexture(textTexture);
                    }
                    SDL_FreeSurface(textSurface);
                }
            }
        }
    }
}

void Game::Cleanup() {
    
    // Resources will be cleaned up by ResourceManager
}

void Game::Reset() {
    // Check for new record
    if (gameTimer > bestTime) {
        bestTime = gameTimer;
        isNewRecord = true;
    } else {
        isNewRecord = false;
    }
    
    // Clean up all balloons and their projectiles
    for (EntityID entity = 1; entity < MAX_ENTITIES; entity++) {
        // Remove all balloons
        if (g_Engine.entityManager.HasComponent(entity, COMPONENT_BALLOON)) {
            g_Engine.entityManager.DestroyEntity(entity);
            continue;
        }
        
        // Remove all projectiles (both balloon shots and porcupine quills)
        if (g_Engine.entityManager.HasComponent(entity, COMPONENT_PROJECTILE)) {
            g_Engine.entityManager.DestroyEntity(entity);
        }
    }
    
    // Reset wave system
    waveSystem.ResetWaves();

    // Reset to start screen
    gameState = GAME_STATE_START;
    gameTimer = 0.0f;
}

