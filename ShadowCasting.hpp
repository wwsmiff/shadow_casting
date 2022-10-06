#ifndef SHADOW_CASTING_HPP_
#define SHADOW_CASTING_HPP_

#include <vector>
#include <tuple>
#include <algorithm>
#include <string>

#include "Edge.hpp"
#include "Cell.hpp"
#include "App.hpp"

class ShadowCasting : public Application
{
public:
    ShadowCasting(const std::string &title, uint32_t width, uint32_t height);
    ~ShadowCasting();
    void Update();
    void Render();
    void HandleEvents();
    void ConvertCellsToPolygons(int32_t startX, int32_t startY, int32_t width, int32_t height, int32_t blockSize);
    void CalculateVisibility(float sourceX, float sourceY, float radius);

protected:
    int32_t WorldWidth = 40;
    int32_t WorldHeight = 30;
    std::array<std::array<Cell, 40>, 30> World;
    int32_t BlockSize = this->WindowWidth / this->WorldWidth;
    int32_t WindowMouseX, WindowMouseY;
    std::vector<Edge> Edges;
    std::vector<std::tuple<float, float, float>> VisibilityPoints;
    //                     theta    x      y
    //  theta - angle of ray casted from the source point.
    //  x - `x axis` of the end of the ray.
    //  y - `y axis` of the end of the ray.
    bool LightOn = false;
    uint32_t NumberOfRays = 100;
};

#endif // SHADOW_CASTING_HPP_