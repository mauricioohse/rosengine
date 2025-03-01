#include "render_system.h"
#include <stdio.h>

void RenderSystem::Init() {
    printf("RenderSystem initialized\n");
    cameraX = 0.0f;
    cameraY = 0.0f;
}

void RenderSystem::Update(float deltaTime, std::vector<EntityID> entities, ComponentArrays* components) {
    // Find the active camera (assuming only one camera for now)
    CameraComponent* camera = nullptr;
    for ( EntityID entity : entities) {
        if (g_Engine.entityManager.HasComponent(entity, COMPONENT_CAMERA)) {
            camera = &components->cameras[entity];
            // printf("Active camera found at entity %d\n", entity);
            break;
        }
    }

    // Render all entities with transform and sprite components
    for (EntityID entity : entities)
    {
        if (g_Engine.entityManager.HasComponent(entity, COMPONENT_TRANSFORM | COMPONENT_SPRITE)) {
            TransformComponent* transform = 
                (TransformComponent*)components->GetComponentData(entity, COMPONENT_TRANSFORM);
            SpriteComponent* sprite = 
                (SpriteComponent*)components->GetComponentData(entity, COMPONENT_SPRITE);

            if (!transform || !sprite || !sprite->texture || !sprite->isVisible) continue;

            // Calculate screen position (with camera offset if camera exists)
            float screenX = transform->x;
            float screenY = transform->y;
            
            if (camera) {
                screenX -= camera->x;
                screenY -= camera->y;
            }

            SDL_Rect destRect = {
                (int)screenX - sprite->width/2,
                (int)screenY - sprite->height/2,
                sprite->width,
                sprite->height
            };

            SDL_RenderCopyEx(
                g_Engine.window->renderer,
                sprite->texture->sdlTexture,
                &sprite->srcRect,
                &destRect,
                transform->rotation,
                NULL,
                SDL_FLIP_NONE
            );
        }

        // Handle text rendering
        if (g_Engine.entityManager.HasComponent(entity, COMPONENT_TRANSFORM | COMPONENT_TEXT)) {
            TransformComponent* transform = 
                (TransformComponent*)components->GetComponentData(entity, COMPONENT_TRANSFORM);
            TextComponent* text = 
                (TextComponent*)components->GetComponentData(entity, COMPONENT_TEXT);

            if (!transform || !text || !text->texture) continue;

            float screenX = transform->x;
            float screenY = transform->y;
            
            if (camera) {
                screenX -= camera->x;
                screenY -= camera->y;
            }

            // Adjust position based on alignment
            switch (text->alignment) {
                case TEXT_CENTER:
                    screenX -= text->texture->width / 2;
                    screenY -= text->texture->height / 2;
                    break;
                case TEXT_RIGHT:
                    screenX -= text->texture->width;
                    break;
            }

            SDL_Rect destRect = {
                (int)screenX,
                (int)screenY,
                text->texture->width,
                text->texture->height
            };

            SDL_RenderCopy(
                g_Engine.window->renderer,
                text->texture->sdlTexture,
                NULL,
                &destRect
            );
        }
    }
}

void RenderSystem::RenderEntity(TransformComponent* transform, SpriteComponent* sprite) {
    if (!sprite->texture || !sprite->texture->sdlTexture) return;

    // Calculate screen position (applying camera offset)
    int screenX = (int)(transform->x - cameraX);
    int screenY = (int)(transform->y - cameraY);
    
    // Create destination rectangle
    SDL_Rect destRect;
    destRect.x = screenX;
    destRect.y = screenY;
    destRect.w = sprite->width * transform->scale;
    destRect.h = sprite->height * transform->scale;
    
    // Create source rectangle (for sprite sheets)
    SDL_Rect* srcRect = (sprite->srcRect.w > 0) ? &sprite->srcRect : NULL;
    
    // Calculate rotation center
    SDL_Point center = {
        (int)(sprite->width * transform->scale / 2),
        (int)(sprite->height * transform->scale / 2)
    };
    
    // Render the sprite
    SDL_RenderCopyEx(
        g_Engine.window->renderer,
        sprite->texture->sdlTexture,
        srcRect,
        &destRect,
        transform->rotation,
        &center,
        SDL_FLIP_NONE
    );
}

void RenderSystem::RenderAnimatedEntity(TransformComponent* transform, AnimationComponent* anim) {
    if (!anim->spriteSheet || !anim->spriteSheet->sdlTexture) return;

    // Calculate screen position (applying camera offset)
    int screenX = (int)(transform->x - cameraX);
    int screenY = (int)(transform->y - cameraY);
    
    // Create destination rectangle
    SDL_Rect destRect;
    destRect.x = screenX;
    destRect.y = screenY;
    destRect.w = anim->frameWidth * transform->scale;
    destRect.h = anim->frameHeight * transform->scale;
    
    // Calculate rotation center
    SDL_Point center = {
        (int)(anim->frameWidth * transform->scale / 2),
        (int)(anim->frameHeight * transform->scale / 2)
    };
    
    // Render the sprite
    SDL_RenderCopyEx(
        g_Engine.window->renderer,
        anim->spriteSheet->sdlTexture,
        &anim->frameRect,
        &destRect,
        transform->rotation,
        &center,
        SDL_FLIP_NONE
    );
}

void RenderSystem::Destroy() {
    printf("RenderSystem destroyed\n");
} 