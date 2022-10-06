#include "ShadowCasting.hpp"

ShadowCasting::ShadowCasting(const std::string &title, uint32_t width, uint32_t height)
    :Application(width, height)
{
    ASSERT(!SDL_Init(SDL_INIT_VIDEO));
    ASSERT(this->Window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, this->WindowWidth, this->WindowHeight, SDL_WINDOW_SHOWN));
    ASSERT(this->Renderer = SDL_CreateRenderer(this->Window, -1, SDL_RENDERER_ACCELERATED));
    this->Running = true;

    for(uint32_t y = 0; y < this->WorldHeight; ++y)
    {
        for(uint32_t x = 0; x < this->WorldWidth; ++x)
        {
            this->World[y][x].exists = 0;
        }
    }

    for(int32_t x = 1; x < this->WorldWidth - 1; ++x)
    {
        this->World[1][x].exists = true;
        this->World[this->WorldHeight - 2][x].exists = true;
    }

    for(int32_t y = 1; y < this->WorldHeight - 1; ++y)
    {
        this->World[y][1].exists = true;
        this->World[y][this->WorldWidth - 2].exists = true;
    }
}

ShadowCasting::~ShadowCasting()
{
    SDL_DestroyWindow(this->Window);
    SDL_DestroyRenderer(this->Renderer);
    SDL_Quit();
}

void ShadowCasting::Update()
{
    SDL_GetMouseState(&(this->WindowMouseX), &(this->WindowMouseY));
    this->ConvertCellsToPolygons(0, 0, this->WorldWidth, this->WorldHeight, this->BlockSize);
    CalculateVisibility(this->WindowMouseX, this->WindowMouseY, 1000.0f);
    std::cout << "Number of rays casted: " << this->VisibilityPoints.size() << std::endl;
}

void ShadowCasting::Render()
{
    SDL_SetRenderDrawColor(this->Renderer, 0, 0, 0, 255);
    SDL_RenderClear(this->Renderer);

    SDL_SetRenderDrawColor(this->Renderer, 75, 69, 255, 255);
    for(auto y = 0; y < this->WorldHeight; ++y)
    {
        for(auto x = 0; x < this->WorldWidth; ++x)
        {
            if(this->World[y][x].exists)
            {
                SDL_Rect tmp = {(x * this->BlockSize) , (y * this->BlockSize), BlockSize, BlockSize};
                SDL_RenderFillRect(this->Renderer, &tmp);
            }
        }
    }

    if(SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON_RMASK && this->VisibilityPoints.size() > 0)
    {
        for(int i = 0; i < this->VisibilityPoints.size() - 1; ++i)
        {
            SDL_SetRenderDrawColor(this->Renderer, 255, 255, 255, 255);
            SDL_RenderDrawLine(this->Renderer, this->WindowMouseX, this->WindowMouseY, std::get<1>(this->VisibilityPoints[i]), std::get<2>(this->VisibilityPoints[i]));
            // std::vector<SDL_Vertex> triangle = {
            //                                     {SDL_FPoint{static_cast<float>(this->WindowMouseX), static_cast<float>(this->WindowMouseY)}, SDL_Color{255, 255, 255, 255}, SDL_FPoint{0}},
            //                                     {SDL_FPoint{std::get<1>(this->VisibilityPoints[i]), std::get<2>(this->VisibilityPoints[i])}, SDL_Color{255, 255, 255, 255}, SDL_FPoint{0}},
            //                                     {SDL_FPoint{std::get<1>(this->VisibilityPoints[i + 1]), std::get<2>(this->VisibilityPoints[i + 1])}, SDL_Color{255, 255, 255, 255}, SDL_FPoint{0}}
            //                                    };

            // SDL_RenderGeometry(this->Renderer, nullptr, triangle.data(), triangle.size(), nullptr, 0);
        }

        // std::vector<SDL_Vertex> anotherTriangle = {
        //     {SDL_FPoint{static_cast<float>(this->WindowMouseX), static_cast<float>(this->WindowMouseY)}, SDL_Color{255, 255, 255, 255}, SDL_FPoint{0}},
        //     {SDL_FPoint{std::get<1>(this->VisibilityPoints[this->VisibilityPoints.size() - 1]), std::get<2>(this->VisibilityPoints[this->VisibilityPoints.size() - 1])}, SDL_Color{255, 255, 255, 255}, SDL_FPoint{0}},
        //     {SDL_FPoint{std::get<1>(this->VisibilityPoints[0]), std::get<2>(this->VisibilityPoints[0])}, SDL_Color{255, 255, 255, 255}, SDL_FPoint{0}}
        // };
        // SDL_RenderGeometry(this->Renderer, nullptr, anotherTriangle.data(), anotherTriangle.size(), nullptr, 0);

    }

    for(const auto &edge : this->Edges)
    {
        SDL_SetRenderDrawColor(this->Renderer, 255, 0, 0, 255);
        SDL_RenderDrawLineF(this->Renderer, edge.start_x, edge.start_y, edge.end_x, edge.end_y);
    }

    SDL_RenderPresent(this->Renderer);
}

void ShadowCasting::HandleEvents()
{
    while(SDL_PollEvent(&(this->WindowEvent)) != 0)
    {
        if(this->WindowEvent.type == SDL_QUIT) this->Running = false;
        if(this->WindowEvent.type == SDL_MOUSEBUTTONUP) 
        {
            if(this->WindowEvent.button.button == SDL_BUTTON_LEFT)
            {
                int32_t x = static_cast<int32_t>((this->WindowMouseX) / (this->BlockSize));
                int32_t y = static_cast<int32_t>((this->WindowMouseY) / (this->BlockSize));
                this->World[y][x].exists = !(this->World[y][x].exists);
            }
        }
    }
}

void ShadowCasting::ConvertCellsToPolygons(int32_t startX, int32_t startY, int32_t width, int32_t height, int32_t blockSize)
{
    this->Edges.clear();
    for(int32_t y = 0; y < height; ++y)
    {
        for(int32_t x = 0; x < width; ++x)
        {
            for(int32_t i = 0; i < 4; ++i)
            {
                this->World[(y + startY)][(x + startX)].edge_exists[i] = 0;
                this->World[(y + startY)][(x + startX)].edge_id[i] = 0;
            }
        }
    }

#define current_cell this->World[currentY][currentX]
#define north_cell   this->World[northY][northX]
#define east_cell    this->World[eastY][eastX]
#define west_cell    this->World[westY][westX] 
#define south_cell   this->World[southY][southX]    

    for(int32_t x = 1; x < width - 1; ++x)
    {
        for(int32_t y = 1; y < height - 1; ++y)
        {
            int32_t currentX = (x + startX), currentY = (y + startY);
            int32_t northX = (x + startX), northY = (y + startY - 1);
            int32_t southX = (x + startX), southY = (y + startY + 1);
            int32_t eastX = (x + startX + 1), eastY = (y + startY);
            int32_t westX = (x + startX - 1), westY = (y + startY);

            if(current_cell.exists)
            {
                if(!west_cell.exists)
                {
                    if(north_cell.edge_exists[WEST])
                    {
                        this->Edges[north_cell.edge_id[WEST]].end_y += blockSize;
                        current_cell.edge_id[WEST] = north_cell.edge_id[WEST];
                        current_cell.edge_exists[WEST] = true;
                    }
                    else
                    {
                        Edge edge;
                        edge.start_x = (startX + x) * blockSize;
                        edge.start_y = (startY + y) * blockSize;
                        edge.end_x = edge.start_x;
                        edge.end_y = edge.start_y + blockSize;

                        int edge_id = this->Edges.size();
                        this->Edges.push_back(edge);

                        current_cell.edge_id[WEST] = edge_id;
                        current_cell.edge_exists[WEST] = true;
                    }
                }

                if (!east_cell.exists)
                {
                    if (north_cell.edge_exists[EAST])
                    {
                        this->Edges[north_cell.edge_id[EAST]].end_y += blockSize;
                        current_cell.edge_id[EAST] = north_cell.edge_id[EAST];
                        current_cell.edge_exists[EAST] = true;
                    }
                    else
                    {
                        Edge edge;
                        edge.start_x = (startX + x + 1) * blockSize; 
                        edge.start_y = (startY + y) * blockSize;
                        edge.end_x = edge.start_x; 
                        edge.end_y = edge.start_y + blockSize;

                        int edge_id = this->Edges.size();
                        this->Edges.push_back(edge);

                        current_cell.edge_id[EAST] = edge_id;
                        current_cell.edge_exists[EAST] = true;
                    }
                }

                if (!north_cell.exists)
                {
                    if (west_cell.edge_exists[NORTH])
                    {
                        this->Edges[west_cell.edge_id[NORTH]].end_x += blockSize;
                        current_cell.edge_id[NORTH] = west_cell.edge_id[NORTH];
                        current_cell.edge_exists[NORTH] = true;
                    }
                    else
                    {
                        Edge edge;
                        edge.start_x = (startX + x) * blockSize; 
                        edge.start_y = (startY + y) * blockSize;
                        edge.end_x = edge.start_x + blockSize;
                        edge.end_y = edge.start_y;

                        int edge_id = this->Edges.size();
                        this->Edges.push_back(edge);

                        current_cell.edge_id[NORTH] = edge_id;
                        current_cell.edge_exists[NORTH] = true;
                    }
                }

                if (!south_cell.exists)
                {
                    if (west_cell.edge_exists[SOUTH])
                    {
                        this->Edges[west_cell.edge_id[SOUTH]].end_x += blockSize;
                        current_cell.edge_id[SOUTH] = west_cell.edge_id[SOUTH];
                        current_cell.edge_exists[SOUTH] = true;
                    }
                    else
                    {
                        Edge edge;
                        edge.start_x = (startX + x) * blockSize; 
                        edge.start_y = (startX + y + 1) * blockSize;
                        edge.end_x = edge.start_x + blockSize; 
                        edge.end_y = edge.start_y;

                        int edge_id = this->Edges.size();
                        this->Edges.push_back(edge);

                        current_cell.edge_id[SOUTH] = edge_id;
                        current_cell.edge_exists[SOUTH] = true;
                    }
                }
            }
        }
    }
}

void ShadowCasting::CalculateVisibility(float sourceX, float sourceY, float radius)
{
    this->VisibilityPoints.clear();
    
    for(const auto &edge : this->Edges)
    {
        for(int32_t i = 0; i < 2; ++i)
        {
            float rayX, rayY;
            if(i == 0)
            {
               rayX = edge.start_x - sourceX;
               rayY = edge.start_y - sourceY;
            }
            else 
            {
                rayX = edge.end_x - sourceX; 
                rayY = edge.end_y - sourceY;
            }

            float theta = atan2f(rayX, rayY);
            float castingAngle = 0.0f;

            for(int32_t j = 0; j < this->NumberOfRays; ++j)
            {
                // if(j == 0) castingAngle = theta - 0.0001f; // Slight negative deviance 
                // if(j == 1) castingAngle = theta + 0.0f; // 0 deviance
                // if(j == 2) castingAngle = theta + 0.0001f; // Slight positive deviance

                if(j < (this->NumberOfRays / 2) - 1) castingAngle = theta - (j * 0.2f);
                if(j == this->NumberOfRays / 2) castingAngle = theta;
                if(j > (this->NumberOfRays / 2) + 1) castingAngle = theta + (j * 0.2f);

                rayX = radius * cosf(castingAngle);
                rayY = radius * sinf(castingAngle);
                
                float minimumT1Distance = INFINITY;
                float minimumIntersectionX, minimumIntersectionY, minimumTheta;

                bool isValid = false;

                for(const auto &edge1 : this->Edges)
                {
                    // Intersection
                    float edgeX = edge1.end_x - edge1.start_x;
                    float edgeY = edge1.end_y - edge1.start_y;

                    if(fabs(edgeX - rayX) > 0.0f && fabs(edgeY - rayY) > 0.0f)
                    {
                        float t2 = (rayX * (edge1.start_y - sourceY) + (rayY * (sourceX - edge1.start_x))) / (edgeX * rayY - edgeY * rayX);
                        float t1 = (edge1.start_x + edgeX * t2 - sourceX) / rayX;

                        if(t1 > 0 && t2 >= 0 && t2 <= 1.0f)
                        {
                            if(t1 < minimumT1Distance)
                            {
                                minimumT1Distance = t1;
                                minimumIntersectionX = (rayX * t1) + sourceX;
                                minimumIntersectionY = (rayY * t1) + sourceY;
                                minimumTheta = atan2f(minimumIntersectionX, minimumIntersectionY);
                                isValid = true;
                            }
                        }
                    }
                }

                if(isValid)
                {
                    this->VisibilityPoints.push_back({minimumTheta, minimumIntersectionX, minimumIntersectionY});
                }
            }
        }
    }

    // std::sort(this->VisibilityPoints.begin(), this->VisibilityPoints.end(), [&](const std::tuple<float, float, float> &t1,
    //                                                                             const std::tuple<float, float, float> &t2)
    // {
    //     return std::get<0>(t1) < std::get<0>(t2); // Comparing theta     
    // });

    // auto it = std::unique(this->VisibilityPoints.begin(), this->VisibilityPoints.end(), [](const std::tuple<float, float, float> &t1,
    //                                                                                 const std::tuple<float, float, float> &t2){

        
    //     return fabs(std::get<1>(t1) - std::get<1>(t2)) < 0.4f && fabs(std::get<2>(t1) && std::get<2>(t2)) < 0.4f;
    // });

    // this->VisibilityPoints.erase(it, this->VisibilityPoints.end());
}