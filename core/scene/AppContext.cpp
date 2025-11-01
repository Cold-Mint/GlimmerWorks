//
// Created by Cold-Mint on 2025/10/10.
//
#include "AppContext.h"

std::string* glimmer::AppContext::GetLanguage() const
{
    return language_;
}

void glimmer::AppContext::SetWindow(SDL_Window* window)
{
    this->window_ = window;
}

SDL_Window* glimmer::AppContext::GetWindow() const
{
    return window_;
}
