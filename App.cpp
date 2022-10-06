#include "App.hpp"

Application::Application(const std::string &title, uint32_t width, uint32_t height)
    :WindowWidth(width),
     WindowHeight(height)
{
    ASSERT(!SDL_Init(SDL_INIT_VIDEO));
    ASSERT(this->Window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN));
    ASSERT(this->Renderer = SDL_CreateRenderer(this->Window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC));
    this->Running = true;
}

Application::Application(uint32_t width, uint32_t height)
    :WindowWidth(width),
    WindowHeight(height)
{}

Application::~Application()
{
    SDL_DestroyWindow(this->Window);
    SDL_DestroyRenderer(this->Renderer);
    SDL_Quit();
}

void Application::Render()
{}

void Application::Update()
{}

void Application::HandleEvents()
{}