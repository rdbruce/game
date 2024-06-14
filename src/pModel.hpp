#include <SDL.h>

class pModel
{
    public:
        pModel(SDL_Renderer* gRenderer_from_main, const int wFromMain, const int hFromMain);

        ~pModel();

        //Update the locations of the pendulums
        void updatePendulum(double theta, double theta2);

        //Draw the pendulums
        void drawPendulum();
    private:
        // Pendulum length
        const int pLen = 100;

        // Cartesian pendulum points
        SDL_Point pt0;
        SDL_Point pt1;
        SDL_Point pt2;

        // Renderer pointer
        SDL_Renderer* gRenderer;

        void drawCircle(int32_t centreX, int32_t centreY, int32_t radius);
};