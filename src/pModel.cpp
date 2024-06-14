#include <SDL.h>
#include <cmath>

#include "pModel.hpp"

pModel::pModel(SDL_Renderer* gRenderer_from_main, const int wFromMain, const int hFromMain)
{
    //Pass ptrs
	gRenderer = gRenderer_from_main;

   //Set center
   pt0 = { wFromMain / 2, hFromMain / 2 };
}

pModel::~pModel()
{

}

void pModel::updatePendulum(double theta, double theta2)
{
   //Polar to cartesian conversion
   pt1 = { (int)(pt0.x + pLen*sin(theta) + 0.5), (int)(pt0.y + pLen*cos(theta) + 0.5) };
   pt2 = { (int)(pt1.x + pLen*sin(theta2) + 0.5), (int)(pt1.y + pLen*cos(theta2) + 0.5) };
}

void pModel::drawCircle(int32_t centreX, int32_t centreY, int32_t radius)
{
   const int32_t diameter = (radius * 2);

   int32_t x = (radius - 1);
   int32_t y = 0;
   int32_t tx = 1;
   int32_t ty = 1;
   int32_t error = (tx - diameter);

   while (x >= y)
   {
      //  Each of the following renders an octant of the circle
      SDL_RenderDrawPoint(gRenderer, centreX + x, centreY - y);
      SDL_RenderDrawPoint(gRenderer, centreX + x, centreY + y);
      SDL_RenderDrawPoint(gRenderer, centreX - x, centreY - y);
      SDL_RenderDrawPoint(gRenderer, centreX - x, centreY + y);
      SDL_RenderDrawPoint(gRenderer, centreX + y, centreY - x);
      SDL_RenderDrawPoint(gRenderer, centreX + y, centreY + x);
      SDL_RenderDrawPoint(gRenderer, centreX - y, centreY - x);
      SDL_RenderDrawPoint(gRenderer, centreX - y, centreY + x);

      if (error <= 0)
      {
         ++y;
         error += ty;
         ty += 2;
      }

      if (error > 0)
      {
         --x;
         tx += 2;
         error += (tx - diameter);
      }
   }
}

void pModel::drawPendulum()
{
   //Draw blue horizontal line for pendulum segments
   SDL_SetRenderDrawColor( gRenderer, 0x00, 0x00, 0xFF, 0xFF );		
   SDL_RenderDrawLine( gRenderer, pt0.x, pt0.y, pt1.x, pt1.y );
   drawCircle(pt1.x, pt1.y, 10);
   SDL_RenderDrawLine( gRenderer, pt1.x, pt1.y, pt2.x, pt2.y );
   drawCircle(pt2.x, pt2.y, 10);
}


