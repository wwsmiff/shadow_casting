#include "ShadowCasting.hpp"

int main(void)
{
    ShadowCasting shadowCasting("Shadow Casting", 800, 600);
    while(shadowCasting.IsRunning())
    {
        shadowCasting.HandleEvents();
        shadowCasting.Render();
        shadowCasting.Update();
    } 

    return EXIT_SUCCESS;
}