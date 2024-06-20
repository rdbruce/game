#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <string>
#include <sstream>

#include "SDLHolder.hpp"
#include "LTexture.hpp"
#include "LTimer.hpp"
#include "pModel.hpp"
#include "pMath.hpp"

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <functional>
#include <memory>
#include <vector>

//Generic thread wrapper
class Simulator {
public:
    using Callback = std::function<void ()>;
private:
    Callback callback;
    bool die;
    std::thread tid;
    std::string name;
public:
	//Construct std function from provided callable
    Simulator(Callback _callback) : callback(_callback), die(false)
    {
		//Create thread
        tid = std::thread([this]() { this->worker(); });
    }

    ~Simulator()
    {
        {
            die = true;
        }
        tid.join();
    }

	//Generic run loop that pMath exists in
    void worker()
    {
        using namespace std;
        for (;;) {
            callback();
            if (die) return;
        }
    }
};

//Generic SafeSharedPtr
template <typename T>
class SafeSharedPtr {
    std::mutex  mtx;
    std::shared_ptr<T>  ptr;
public:
    SafeSharedPtr() : ptr(nullptr) {}

    void store(std::shared_ptr<T> arg)
    {
        std::unique_lock<std::mutex> lock(mtx);
        ptr = arg;
    }

    std::shared_ptr<T> load()
    {
        std::unique_lock<std::mutex> lock(mtx);
        return ptr;
    }
};

int main( 
	// int argc, char* args[] 
	)
{
	//Screen dimension constants
	const int SCREEN_FPS = 60;
	const int SCREEN_TICK_PER_FRAME = 1000 / SCREEN_FPS;
	const int SCREEN_WIDTH = 640;
	const int SCREEN_HEIGHT = 480;

	//Initialize SDL
	SDLHolder gHolder(SCREEN_WIDTH, SCREEN_HEIGHT);

	//Scene textures
	LTexture gFPSTextTexture(gHolder.gRenderer, gHolder.gFont);

	//Main loop flag
	bool quit = false;

	//Event handler
	SDL_Event e;

	//Set text color as black
	SDL_Color textColor = { 0, 0, 0, 255 };

	//The frames per second timer
	LTimer fpsTimer;

	//The frames per second cap timer
	LTimer capTimer;

	//In memory text stream
	std::stringstream timeText;

	//Start counting frames per second
	int countedFrames = 0;
	fpsTimer.start();

	//Initialize double pendulum 1
	//Initialize pendulum render object
	pModel ourP(gHolder.gRenderer, SCREEN_WIDTH, SCREEN_HEIGHT);
	//Initialize pendulum position structure pointer
	SafeSharedPtr<pData> latch;
	//Initialize mathematical model 
	pMath ourPm(3.1415, 3.1415); //Input thetas
	//Call generic thread creator and initialize with our callable
    Simulator s([&]() { auto ptr = ourPm.simulate(); latch.store(ptr); });

	//Initialize double pendulum 2
	//Initialize pendulum render object
	pModel ourP2(gHolder.gRenderer, SCREEN_WIDTH, SCREEN_HEIGHT);
	//Initialize pendulum position structure pointer
	SafeSharedPtr<pData> latch2;
	//Initialize mathematical model 
	//Notice initial condition similarity
	pMath ourPm2(3.141, 3.141); //Input thetas
	//Call generic thread creator and initialize with our callable
    Simulator s2([&]() { auto ptr2 = ourPm2.simulate(); latch2.store(ptr2); });

	//While application is running
	while( !quit )
	{
		//Start cap timer
		capTimer.start();

		//Handle events on queue
		while( SDL_PollEvent( &e ) != 0 )
		{
			//User requests quit
			if( e.type == SDL_QUIT )
			{
				quit = true;
			}
		}

		//Calculate and correct fps
		float avgFPS = countedFrames / ( fpsTimer.getTicks() / 1000.f );
		if( avgFPS > 2000000 )
		{
			avgFPS = 0;
		}

		//Set text to be rendered
		timeText.str( "" );
		timeText << "Average Frames Per Second (With Cap) " << avgFPS; 

		//Render text
		if( !gFPSTextTexture.loadFromRenderedText( timeText.str().c_str(), textColor ) )
		{
			printf( "Unable to render FPS texture!\n" );
		}

		//Clear screen
		SDL_SetRenderDrawColor( gHolder.gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
		SDL_RenderClear( gHolder.gRenderer );

		//Fetch pendulum position structure pointers
		auto ptr = latch.load();
		auto ptr2 = latch2.load();

		// printf("Theta %f Phi %f \n", ptr->t, ptr->dt);

		//Render textures
		gFPSTextTexture.render( 0, 0 );
		//Update pendulum render objects with current mathematical positions
		ourP.updatePendulum(ptr->t, ptr->t2);
		ourP2.updatePendulum(ptr2->t, ptr2->t2);
		//Draw pendulums
		ourP.drawPendulum();
		ourP2.drawPendulum();

		//Update screen
		SDL_RenderPresent( gHolder.gRenderer );
		++countedFrames;

		//If frame finished early
		int frameTicks = capTimer.getTicks();
		if( frameTicks < SCREEN_TICK_PER_FRAME )
		{
			//Wait remaining time
			SDL_Delay( SCREEN_TICK_PER_FRAME - frameTicks );
		}
	}

	//Free loaded images
	gFPSTextTexture.free();

	return 0;
}
