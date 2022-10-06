#ifndef APP_HPP_
#define APP_HPP_

#include <iostream>
#include <SDL2/SDL.h>

#define REPORT(X) std::cout << #X << " in " << __FILE__ << " @ line " << __LINE__ << ", SDL Error: " << SDL_GetError() << std::endl;
#define ASSERT(X) if(!(X)) {REPORT(X); exit(-1);}

class Application
{
public:
    Application(const std::string &title, uint32_t width, uint32_t height);
    ~Application();
    virtual void Update();
    virtual void Render();
    virtual void HandleEvents();
    virtual uint32_t GetWidth() const { return this->WindowWidth; }
    virtual uint32_t GetHeight() const { return this->WindowHeight; }
    virtual bool IsRunning() const { return this->Running; }

protected:
    Application(uint32_t width, uint32_t height);
    SDL_Window *Window = nullptr;
    SDL_Renderer *Renderer = nullptr;
    uint32_t WindowWidth;
    uint32_t WindowHeight;
    bool Running = false;
    SDL_Event WindowEvent;
};

#endif // APP_HPP_