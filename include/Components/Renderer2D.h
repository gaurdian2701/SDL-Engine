#pragma once
#include "SDL3/SDL_render.h"

namespace Components
{
    struct Renderer2D
    {
        //Default Constructor
        Renderer2D() = default;
        ~Renderer2D()
        {
            RenderTexture = nullptr;
        }

        SDL_FColor Color = SDL_FColor(255, 255, 255, 255);
        SDL_FRect RenderRectangle = SDL_FRect(0, 0, 5, 5);
        SDL_Texture* RenderTexture = nullptr;
    };
}
