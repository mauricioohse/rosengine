#include "game.h"
#include "../core/resource_manager.h"
#include "../core/window.h"
#include "../core/input.h"
#include "cloud_init.h"
#include "peanut_init.h"
#include <math.h>
#include <algorithm>

Game g_Game;

#define DEBUG_COLLISION 1

// Define the static constants
const float Game::COMBO_TIME_LIMIT = 3.0f;
const float Game::COMBO_MULTIPLIER_INCREASE = 0.5f;
const float Game::MAX_COMBO_MULTIPLIER = 4.0f;
const int Game::SCORE_RED_BALLOON = 100;
const int Game::SCORE_GREEN_BALLOON = 200;
const int Game::SCORE_BLUE_BALLOON = 300;

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
    g_Engine.systemManager.RegisterSystem(&icePhysicsSystem);
    g_Engine.systemManager.RegisterSystem(&shooterSystem);
    g_Engine.systemManager.RegisterSystem(new BalloonSystem());
    g_Engine.systemManager.RegisterSystem(&waveSystem);

    // Create background
    backgroundEntity = g_Engine.entityManager.CreateEntity();
    Texture* backgroundTexture = ResourceManager::GetTexture(TEXTURE_BACKGROUND_MIDDLE);
    ADD_TRANSFORM(backgroundEntity, 0.0f, 0.0f, 0.0f, 1.0f);
    ADD_SPRITE(backgroundEntity, backgroundTexture);
    ADD_BACKGROUND(backgroundEntity, 0.5f);  // 0.5 parallax factor for medium depth


    // Create squirrel entity
    squirrelEntity = g_Engine.entityManager.CreateEntity();
    
    Texture* squirrelTexture = ResourceManager::GetTexture(TEXTURE_PORCUPINE_LEFT);    
    // Add basic components
    ADD_TRANSFORM(squirrelEntity, 1200.0f, 100.0f, 0.0f, 1.0f);  // Center-top of screen
    ADD_PHYSICS(squirrelEntity, 50, 2, 1,  750.0f);
    ADD_SPRITE(squirrelEntity, squirrelTexture);
    ADD_WASD_CONTROLLER(squirrelEntity, 600, 1);
    ADD_COLLIDER(squirrelEntity, 32, 32, 0, 0);
    ADD_SHOOTER(squirrelEntity, 800.0f, 200.0f, 1.0f);

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

    // Initialize scoring system
    currentScore = 0;
    bestScore = 0;
    comboMultiplier = 1.0f;
    comboTimer = 0.0f;
    
    return true;
}

// logic related to inputs on the game should go here
void Game::HandleInput(){
    // Handle pause toggle with Enter key
    if (Input::IsKeyPressed(SDL_SCANCODE_ESCAPE)) {
        if (gameState == GAME_STATE_PLAYING) {
            gameState = GAME_STATE_PAUSED;
            return;
        } else if (gameState == GAME_STATE_PAUSED) {
            gameState = GAME_STATE_PLAYING;
            return;
        }
    }

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

    // Debug: Force upgrade selection with ESC
#ifdef DEBUG
    if (Input::IsKeyPressed(SDL_SCANCODE_T) && gameState == GAME_STATE_PLAYING) {
        waveSystem.GenerateUpgradeChoices();
        waveSystem.SetAwaitingUpgradeChoice(true);
    }
#endif

    // Add reset on 'R' key press
    if (Input::IsKeyPressed(SDL_SCANCODE_R)) {
        Reset();
    }
}

void Game::Update(float deltaTime) {
    if (waveSystem.IsAwaitingUpgradeChoice()) {
        // Handle upgrade selection input
        if (Input::IsKeyPressed(SDL_SCANCODE_1)) {
            waveSystem.SelectUpgrade(0);
            PrintUpgradeStats();  // Add debug output after selection
        } else if (Input::IsKeyPressed(SDL_SCANCODE_2)) {
            waveSystem.SelectUpgrade(1);
            PrintUpgradeStats();  // Add debug output after selection
        } else if (Input::IsKeyPressed(SDL_SCANCODE_3)) {
            waveSystem.SelectUpgrade(2);
            PrintUpgradeStats();  // Add debug output after selection
        }
        return;  // Pause game while choosing
    }
    
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


            Reset();
        }
    }

    // Update combo timer
    if (comboTimer > 0) {
        comboTimer -= deltaTime;
        if (comboTimer <= 0) {
            comboMultiplier = 1.0f;
            printf("Combo reset!\n");  // Debug message
        }
    }

    // Update porcupine sprite based on movement and damage
    WASDControllerComponent* controller = 
        (WASDControllerComponent*)g_Engine.componentArrays.GetComponentData(squirrelEntity, COMPONENT_WASD_CONTROLLER);
    SpriteComponent* sprite = 
        (SpriteComponent*)g_Engine.componentArrays.GetComponentData(squirrelEntity, COMPONENT_SPRITE);
    PhysicsComponent* physics = 
        (PhysicsComponent*)g_Engine.componentArrays.GetComponentData(squirrelEntity, COMPONENT_PHYSICS);

    if (controller && sprite && physics) {
                
        if (g_Porcupine_is_hit){
            g_Porcuopine_hit_anim_timer = 1.0f;
            g_Porcupine_is_hit = 0;
        }

        // Update sprite based on movement direction and hit state
        if (g_Porcuopine_hit_anim_timer > 0)
        {
            // Hit animations
            sprite->texture = ResourceManager::GetTexture(TEXTURE_PORCUPINE_IS_HIT);
            g_Porcuopine_hit_anim_timer -= deltaTime;
        }
        else
        {
            // Normal movement animations
            if (controller->moveX >= 0) sprite->texture = ResourceManager::GetTexture(TEXTURE_PORCUPINE_RIGHT);
            else if (controller->moveX < 0) sprite->texture = ResourceManager::GetTexture(TEXTURE_PORCUPINE_LEFT);
            // If not moving, keep current sprite
        }
    }
}

void Game::PrintUpgradeStats() {
    // Get system pointers

    printf("\n=== Current Stats ===\n");
    
    // Shooter stats
    printf("Shooter:\n");
    printf("- Fire Rate Multiplier: %.2fx\n", shooterSystem.GetFireRateMultiplier());
    printf("- Extra Projectiles: %d\n", shooterSystem.GetExtraProjectiles());
    printf("- Damage Multiplier: %.2fx\n", shooterSystem.GetDamageMultiplier());
    
    // Movement stats
    printf("\nMovement:\n");
    printf("- Speed Multiplier: %.2fx\n", wasdSystem.GetSpeedMultiplier());
    
    // Physics stats
    printf("\nPhysics:\n");
    printf("- Grip Multiplier: %.2fx\n", icePhysicsSystem.GetGripMultiplier());
    printf("- Knockback Resistance: %.2fx\n", icePhysicsSystem.GetKnockbackResistance());
    
    // Wave system stats
    printf("\nDifficulty:\n");
    printf("- Red Balloon Multiplier: %.2fx\n", waveSystem.GetRedBalloonMultiplier());
    printf("- Blue Balloon Multiplier: %.2fx\n", waveSystem.GetBlueBalloonMultiplier());
    printf("- Green Balloon Multiplier: %.2fx\n", waveSystem.GetGreenBalloonMultiplier());
    printf("- Balloon Speed Multiplier: %.2fx\n", waveSystem.GetBalloonSpeedMultiplier());
    printf("- Blue Projectile Speed: %.2fx\n", waveSystem.GetBlueProjectileSpeedMultiplier());
    printf("- Green Projectile Speed: %.2fx\n", waveSystem.GetGreenProjectileSpeedMultiplier());
    printf("- Damage Taken Multiplier: %.2fx\n", waveSystem.GetDamageMultiplier());
    printf("=====================\n\n");
}

void Game::RenderUpgradeUI(){
    // Draw upgrade UI when awaiting choice
    if (waveSystem.IsAwaitingUpgradeChoice()) {
        Font* font = ResourceManager::GetFont(fpsFontID);
        if (font) {
            SDL_Color titleColor = {255, 255, 255, 255};  // White
            SDL_Color upgradeColor = {150, 255, 150, 255}; // Light green
            SDL_Color difficultyColor = {255, 150, 150, 255}; // Light red
            
            // Draw title
            const char* titleText = "Choose Your Upgrade";
            SDL_Surface* titleSurface = TTF_RenderText_Solid(font->sdlFont, titleText, titleColor);
            if (titleSurface) {
                SDL_Texture* titleTexture = SDL_CreateTextureFromSurface(g_Engine.window->renderer, titleSurface);
                if (titleTexture) {
                    SDL_Rect titleRect = {
                        (WINDOW_WIDTH - (titleSurface->w * 2)) / 2,  // Center horizontally, accounting for 2x scale
                        150,  // Top position
                        titleSurface->w * 2,  // Double the width
                        titleSurface->h * 2   // Double the height
                    };
                    SDL_RenderCopy(g_Engine.window->renderer, titleTexture, NULL, &titleRect);
                    SDL_DestroyTexture(titleTexture);
                }
                SDL_FreeSurface(titleSurface);
            }

            // Get upgrade choices
            const UpgradeChoice* choices = waveSystem.GetCurrentUpgradeChoices();
            
            // Print difficulties once when choices are retrieved
            static bool printedChoices = false;
            if (!printedChoices) {
                printf("\nUpgrade choices difficulties:\n");
                for (int i = 0; i < 3; i++) {
                    printf("Choice %d: %s (%.2fx)\n", i+1, 
                           choices[i].difficulty.name,
                           choices[i].difficulty.value);
                }
                printf("\n");
                printedChoices = true;
            }
            
            // Draw each upgrade choice
            for (int i = 0; i < 3; i++) {
                int boxX = WINDOW_WIDTH/4 + (i * WINDOW_WIDTH/4) - 150;  // Spread boxes horizontally
                int boxY = (i == 1) ? 450 : 200;  // Middle option is lower
                
                // Draw box background
                SDL_Rect boxRect = {boxX, boxY, 300, 200};
                SDL_SetRenderDrawColor(g_Engine.window->renderer, 50, 50, 50, 200);
                SDL_RenderFillRect(g_Engine.window->renderer, &boxRect);
                
                // Draw selection number
                char numText[2] = {'1' + i, '\0'};
                SDL_Surface* numSurface = TTF_RenderText_Solid(font->sdlFont, numText, titleColor);
                if (numSurface) {
                    SDL_Texture* numTexture = SDL_CreateTextureFromSurface(g_Engine.window->renderer, numSurface);
                    if (numTexture) {
                        SDL_Rect numRect = {
                            boxX + 10,
                            boxY + 10,
                            numSurface->w,
                            numSurface->h
                        };
                        SDL_RenderCopy(g_Engine.window->renderer, numTexture, NULL, &numRect);
                        SDL_DestroyTexture(numTexture);
                    }
                    SDL_FreeSurface(numSurface);
                }

                // Draw upgrade info
                SDL_Surface* upgradeSurface = TTF_RenderText_Solid(font->sdlFont, 
                    choices[i].upgrade.name, upgradeColor);
                SDL_Surface* upgradeDescSurface = TTF_RenderText_Solid(font->sdlFont, 
                    choices[i].upgrade.description, upgradeColor);
                
                // Draw difficulty info
                char difficultyLine1[64];
                char difficultyLine2[64];
                char difficultyLine3[64];
                const int MAX_LINE_LENGTH = 30;  // Maximum characters per line

                snprintf(difficultyLine1, sizeof(difficultyLine1), "%s", choices[i].difficulty.name);

                // Split description into two lines
                const char* desc = choices[i].difficulty.description;
                int descLength = strlen(desc);
                
                if (descLength > MAX_LINE_LENGTH) {
                    // Find the last space before MAX_LINE_LENGTH
                    int splitPoint = MAX_LINE_LENGTH;
                    while (splitPoint > 0 && desc[splitPoint] != ' ') {
                        splitPoint--;
                    }
                    if (splitPoint == 0) splitPoint = MAX_LINE_LENGTH; // No space found

                    // Copy first line
                    strncpy(difficultyLine2, desc, splitPoint);
                    difficultyLine2[splitPoint] = '\0';
                    
                    // Copy second line
                    strncpy(difficultyLine3, desc + splitPoint + 1, sizeof(difficultyLine3) - 1);
                } else {
                    // Description fits on one line
                    strncpy(difficultyLine2, desc, sizeof(difficultyLine2) - 1);
                    difficultyLine3[0] = '\0';
                }

                // Add multiplier value to the last line
                char multiplierText[16];
                snprintf(multiplierText, sizeof(multiplierText), " (%.2fx)", choices[i].difficulty.value);
                strncat(difficultyLine3, multiplierText, sizeof(difficultyLine3) - strlen(difficultyLine3) - 1);
                
                SDL_Surface* diffSurface1 = TTF_RenderText_Solid(font->sdlFont, difficultyLine1, difficultyColor);
                SDL_Surface* diffSurface2 = TTF_RenderText_Solid(font->sdlFont, difficultyLine2, difficultyColor);
                SDL_Surface* diffSurface3 = TTF_RenderText_Solid(font->sdlFont, difficultyLine3, difficultyColor);
                
                // Render all text surfaces
                if (upgradeSurface && upgradeDescSurface && diffSurface1 && diffSurface2 && diffSurface3) {
                    SDL_Texture* upgradeTexture = SDL_CreateTextureFromSurface(g_Engine.window->renderer, upgradeSurface);
                    SDL_Texture* upgradeDescTexture = SDL_CreateTextureFromSurface(g_Engine.window->renderer, upgradeDescSurface);
                    SDL_Texture* diffTexture1 = SDL_CreateTextureFromSurface(g_Engine.window->renderer, diffSurface1);
                    SDL_Texture* diffTexture2 = SDL_CreateTextureFromSurface(g_Engine.window->renderer, diffSurface2);
                    SDL_Texture* diffTexture3 = SDL_CreateTextureFromSurface(g_Engine.window->renderer, diffSurface3);
                    
                    if (upgradeTexture && upgradeDescTexture && diffTexture1 && diffTexture2 && diffTexture3) {
                        // Position all elements
                        SDL_Rect upgradeRect = {boxX + 20, boxY + 50, upgradeSurface->w, upgradeSurface->h};
                        SDL_Rect upgradeDescRect = {boxX + 20, boxY + 80, upgradeDescSurface->w, upgradeDescSurface->h};
                        SDL_Rect diffRect1 = {boxX + 20, boxY + 120, diffSurface1->w, diffSurface1->h};
                        SDL_Rect diffRect2 = {boxX + 20, boxY + 140, diffSurface2->w, diffSurface2->h};
                        SDL_Rect diffRect3 = {boxX + 20, boxY + 160, diffSurface3->w, diffSurface3->h};
                        
                        // Render all elements
                        SDL_RenderCopy(g_Engine.window->renderer, upgradeTexture, NULL, &upgradeRect);
                        SDL_RenderCopy(g_Engine.window->renderer, upgradeDescTexture, NULL, &upgradeDescRect);
                        SDL_RenderCopy(g_Engine.window->renderer, diffTexture1, NULL, &diffRect1);
                        SDL_RenderCopy(g_Engine.window->renderer, diffTexture2, NULL, &diffRect2);
                        SDL_RenderCopy(g_Engine.window->renderer, diffTexture3, NULL, &diffRect3);
                        
                        // Cleanup textures
                        SDL_DestroyTexture(upgradeTexture);
                        SDL_DestroyTexture(upgradeDescTexture);
                        SDL_DestroyTexture(diffTexture1);
                        SDL_DestroyTexture(diffTexture2);
                        SDL_DestroyTexture(diffTexture3);
                    }
                }
                
                // Cleanup surfaces
                SDL_FreeSurface(upgradeSurface);
                SDL_FreeSurface(upgradeDescSurface);
                SDL_FreeSurface(diffSurface1);
                SDL_FreeSurface(diffSurface2);
                SDL_FreeSurface(diffSurface3);
            }
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

    // Draw score, combo and wave number
    Font* font = ResourceManager::GetFont(fpsFontID);
    if (font) {
        SDL_Color textColor = {255, 255, 255, 255};  // Default white color
        
        // Create score and wave text
        char scoreText[128];
        char comboText[32] = "";  // New separate text for combo
        
        // Format wave and score text
        snprintf(scoreText, sizeof(scoreText), "Score: %d - Wave %d-%d", 
                currentScore, 
                waveSystem.currentCycle + 1, waveSystem.currentWave + 1);
        
        // Create text surface for score and wave
        SDL_Surface* textSurface = TTF_RenderText_Solid(font->sdlFont, scoreText, textColor);
        if (textSurface) {
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(g_Engine.window->renderer, textSurface);
            if (textTexture) {
                SDL_Rect destRect;
                // Make score text 1.5x larger
                destRect.w = (int)(textSurface->w * 1.5f);
                destRect.h = (int)(textSurface->h * 1.5f);
                destRect.x = (WINDOW_WIDTH - destRect.w) / 2;  // Center horizontally
                destRect.y = 20;  // Near top of screen
                
                SDL_RenderCopy(g_Engine.window->renderer, textTexture, NULL, &destRect);
                SDL_DestroyTexture(textTexture);
            }
            SDL_FreeSurface(textSurface);
        }

        // Render combo multiplier if active
        if (comboMultiplier > 1.0f) {
            // Calculate color intensity based on combo (from white to red)
            int redIntensity = 255;
            int otherIntensity = (int)(255 * (2.0f - comboMultiplier / 2.0f));
            otherIntensity = std::max(0, std::min(255, otherIntensity));
            
            SDL_Color comboColor = {
                (Uint8)redIntensity,
                (Uint8)otherIntensity,
                (Uint8)otherIntensity,
                255
            };

            snprintf(comboText, sizeof(comboText), "%.1fx", comboMultiplier);
            
            // Calculate scale factor based on combo (1.0 to 2.0)
            float scale = 1.0f + (comboMultiplier - 1.0f) / 3.0f;  // Scales up with combo
            
            // Calculate wiggle based on combo and time
            float wiggleIntensity = (comboMultiplier - 1.0f) * 5.0f;  // Increases with combo
            float wiggleX = sin(gameTimer * 10.0f) * wiggleIntensity;
            float wiggleY = cos(gameTimer * 8.0f) * wiggleIntensity;
            
            SDL_Surface* comboSurface = TTF_RenderText_Solid(font->sdlFont, comboText, comboColor);
            if (comboSurface) {
                SDL_Texture* comboTexture = SDL_CreateTextureFromSurface(g_Engine.window->renderer, comboSurface);
                if (comboTexture) {
                    SDL_Rect comboRect;
                    comboRect.w = (int)(comboSurface->w * scale);
                    comboRect.h = (int)(comboSurface->h * scale);
                    comboRect.x = (WINDOW_WIDTH - comboRect.w) / 2 + (int)wiggleX;  // Add wiggle to position
                    comboRect.y = 60 + (int)wiggleY;  // Add wiggle to position
                    
                    SDL_RenderCopy(g_Engine.window->renderer, comboTexture, NULL, &comboRect);
                    SDL_DestroyTexture(comboTexture);
                }
                SDL_FreeSurface(comboSurface);
            }
        }
    }

    // Modify start screen to show best score
    if (gameState == GAME_STATE_START) {
        Font* font = ResourceManager::GetFont(fpsFontID);
        if (font) {
            SDL_Color textColor = {255, 255, 255, 255};
            
            const char* controls[] = {
                "WASD for movement",
                "Left click to shoot quills",
                "",
                bestScore > 0 ? "Best Score: %d" : "",
                "",
                "Press any key to start"
            };
            
            // Render each line of text
            for (int i = 0; i < 6; i++) {
                char lineBuffer[64];
                if (i == 3 && bestScore > 0) {
                    snprintf(lineBuffer, sizeof(lineBuffer), controls[i], bestScore);
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

    // Draw indicators for off-screen balloons
    const float TRIANGLE_SIZE = 15.0f;    // Length of the triangle
    const float TRIANGLE_WIDTH = 10.0f;   // Width of triangle base
    const int INDICATOR_MARGIN = 40;      // Distance from screen edge
    
    for (EntityID entity = 1; entity < MAX_ENTITIES; entity++) {
        if (g_Engine.entityManager.HasComponent(entity, COMPONENT_BALLOON | COMPONENT_TRANSFORM)) {
            TransformComponent* balloonTransform = 
                (TransformComponent*)g_Engine.componentArrays.GetComponentData(entity, COMPONENT_TRANSFORM);
            BalloonComponent* balloon = 
                (BalloonComponent*)g_Engine.componentArrays.GetComponentData(entity, COMPONENT_BALLOON);
            
            if (!balloonTransform || !balloon) continue;
            
            // Get balloon's screen position (relative to camera)
            float screenX = balloonTransform->x - camera->x;
            float screenY = balloonTransform->y - camera->y;
            
            // Check if balloon is off screen
            if (screenX < 0 || screenX > WINDOW_WIDTH || 
                screenY < 0 || screenY > WINDOW_HEIGHT) {
                
                // Set color based on balloon type
                switch (balloon->type) {
                    case BALLOON_RED:
                        SDL_SetRenderDrawColor(g_Engine.window->renderer, 255, 50, 50, 255);
                        break;
                    case BALLOON_GREEN:
                        SDL_SetRenderDrawColor(g_Engine.window->renderer, 50, 255, 50, 255);
                        break;
                    case BALLOON_BLUE:
                        SDL_SetRenderDrawColor(g_Engine.window->renderer, 50, 50, 255, 255);
                        break;
                    default:
                        SDL_SetRenderDrawColor(g_Engine.window->renderer, 255, 255, 255, 255);
                }
                
                // Calculate angle to balloon from screen center
                float dx = screenX - WINDOW_WIDTH/2;
                float dy = screenY - WINDOW_HEIGHT/2;
                float angle = atan2(dy, dx);
                
                // Calculate indicator position on screen edge
                float indicatorX = WINDOW_WIDTH/2 + cos(angle) * (WINDOW_WIDTH/2 - INDICATOR_MARGIN);
                float indicatorY = WINDOW_HEIGHT/2 + sin(angle) * (WINDOW_HEIGHT/2 - INDICATOR_MARGIN);
                
                // Clamp to screen bounds
                indicatorX = std::max(INDICATOR_MARGIN, std::min(WINDOW_WIDTH - INDICATOR_MARGIN, (int)indicatorX));
                indicatorY = std::max(INDICATOR_MARGIN, std::min(WINDOW_HEIGHT - INDICATOR_MARGIN, (int)indicatorY));
                

                // Calculate triangle points
                float tipX = indicatorX + cos(angle) * TRIANGLE_SIZE;
                float tipY = indicatorY + sin(angle) * TRIANGLE_SIZE;
                float base1X = indicatorX + cos(angle - M_PI/2) * TRIANGLE_WIDTH;
                float base1Y = indicatorY + sin(angle - M_PI/2) * TRIANGLE_WIDTH;
                float base2X = indicatorX + cos(angle + M_PI/2) * TRIANGLE_WIDTH;
                float base2Y = indicatorY + sin(angle + M_PI/2) * TRIANGLE_WIDTH;

                // Fill the triangle using horizontal scan lines
                int minY = (int)std::min({tipY, base1Y, base2Y});
                int maxY = (int)std::max({tipY, base1Y, base2Y});

                for (int y = minY; y <= maxY; y++) {
                    // For each horizontal line, find intersections with triangle edges
                    std::vector<float> intersections;
                    
                    // Check each edge of the triangle
                    auto addIntersection = [&intersections, y](float x1, float y1, float x2, float y2) {
                        if ((y1 <= y && y2 > y) || (y2 <= y && y1 > y)) {
                            float x = x1 + (y - y1) * (x2 - x1) / (y2 - y1);
                            intersections.push_back(x);
                        }
                    };

                    addIntersection(tipX, tipY, base1X, base1Y);
                    addIntersection(base1X, base1Y, base2X, base2Y);
                    addIntersection(base2X, base2Y, tipX, tipY);

                    if (intersections.size() >= 2) {
                        std::sort(intersections.begin(), intersections.end());
                        SDL_RenderDrawLine(g_Engine.window->renderer, 
                            (int)intersections[0], y, 
                            (int)intersections[1], y);
                    }
                }
            }
        }
    }

    if (waveSystem.IsAwaitingUpgradeChoice()) {
        RenderUpgradeUI();
    }

    // Add pause menu rendering at the end of the Render function:
    if (gameState == GAME_STATE_PAUSED && !waveSystem.IsAwaitingUpgradeChoice()) {
        Font* font = ResourceManager::GetFont(fpsFontID);
        if (font) {
            SDL_Color textColor = {255, 255, 255, 255};
            
            // Create semi-transparent dark overlay
            SDL_SetRenderDrawBlendMode(g_Engine.window->renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(g_Engine.window->renderer, 0, 0, 0, 128);
            SDL_Rect overlay = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
            SDL_RenderFillRect(g_Engine.window->renderer, &overlay);
            
            const char* pauseText = "PAUSED";
            const char* continueText = "Press ENTER to continue";
            
            // Render PAUSED text
            SDL_Surface* pauseSurface = TTF_RenderText_Solid(font->sdlFont, pauseText, textColor);
            if (pauseSurface) {
                SDL_Texture* pauseTexture = SDL_CreateTextureFromSurface(g_Engine.window->renderer, pauseSurface);
                if (pauseTexture) {
                    SDL_Rect pauseRect;
                    pauseRect.w = pauseSurface->w * 2;  // Make text larger
                    pauseRect.h = pauseSurface->h * 2;
                    pauseRect.x = (WINDOW_WIDTH - pauseRect.w) / 2;
                    pauseRect.y = (WINDOW_HEIGHT - pauseRect.h) / 2 - 40;
                    
                    SDL_RenderCopy(g_Engine.window->renderer, pauseTexture, NULL, &pauseRect);
                    SDL_DestroyTexture(pauseTexture);
                }
                SDL_FreeSurface(pauseSurface);
            }
            
            // Render continue text
            SDL_Surface* continueSurface = TTF_RenderText_Solid(font->sdlFont, continueText, textColor);
            if (continueSurface) {
                SDL_Texture* continueTexture = SDL_CreateTextureFromSurface(g_Engine.window->renderer, continueSurface);
                if (continueTexture) {
                    SDL_Rect continueRect;
                    continueRect.w = continueSurface->w;
                    continueRect.h = continueSurface->h;
                    continueRect.x = (WINDOW_WIDTH - continueRect.w) / 2;
                    continueRect.y = (WINDOW_HEIGHT - continueRect.h) / 2 + 40;
                    
                    SDL_RenderCopy(g_Engine.window->renderer, continueTexture, NULL, &continueRect);
                    SDL_DestroyTexture(continueTexture);
                }
                SDL_FreeSurface(continueSurface);
            }
        }
    }
}

void Game::Cleanup() {
    
    // Resources will be cleaned up by ResourceManager
}

void Game::Reset() {
    // Check for new record
    if (currentScore > bestScore) {
        bestScore = currentScore;
        isNewRecord = true;
    } else {
        isNewRecord = false;
    }
    
    // Reset scoring
    currentScore = 0;
    comboMultiplier = 1.0f;
    comboTimer = 0.0f;
    
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

    // Reset position to center if outside boundary
    TransformComponent* squirrelTransform = 
        (TransformComponent*)g_Engine.componentArrays.GetComponentData(squirrelEntity, COMPONENT_TRANSFORM);
    if (squirrelTransform) {
        squirrelTransform->x = 0.0f;
        squirrelTransform->y = 0.0f;
    }

    // Reset velocity
    PhysicsComponent *physics =
        (PhysicsComponent *)g_Engine.componentArrays.GetComponentData(squirrelEntity, COMPONENT_PHYSICS);
    if (physics)
    {
        physics->velocityX = 0.0f;
        physics->velocityY = 0.0f;
        physics->damagePercent=0;
        physics->knockbackMultiplier=1.0f;
    }

    // Reset wave system
    waveSystem.ResetWaves();

    // resets upgrades
    shooterSystem.Init();
    icePhysicsSystem.Init();

    // Reset to start screen
    gameState = GAME_STATE_START;
    gameTimer = 0.0f;
}

void Game::AddScore(BalloonType balloonType) {
    // Calculate base score based on balloon type
    int baseScore = 0;
    switch (balloonType) {
        case BALLOON_RED:
            baseScore = SCORE_RED_BALLOON;
            break;
        case BALLOON_GREEN:
            baseScore = SCORE_GREEN_BALLOON;
            break;
        case BALLOON_BLUE:
            baseScore = SCORE_BLUE_BALLOON;
            break;
    }
    
    // Apply combo multiplier
    currentScore += (int)(baseScore * comboMultiplier);
    
    // Increase combo
    comboMultiplier = std::min(comboMultiplier + COMBO_MULTIPLIER_INCREASE, MAX_COMBO_MULTIPLIER);
    comboTimer = COMBO_TIME_LIMIT;  // Reset combo timer

    printf("Score added: %d (base: %d, combo: %.1fx)\n", 
           (int)(baseScore * comboMultiplier), baseScore, comboMultiplier);
}

