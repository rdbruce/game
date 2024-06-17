# Double Pendulum Project

![final_pendulum](pendulum.PNG)

This is a personal project written in C++ that models and displays a double pendulum. I wrote this program to practice C++ threading, shared pointers, and standard functions. Reading the book Chaos: Making a New Science Book by James Gleick made me want to play with a dynamical system, giving me an excuse to practice C++.

The bones for the SDL implimentation were built upon the frame limiter example that can be found at [lazyfoo](https://lazyfoo.net/tutorials/SDL/). Author: Amine B. Hassouna [@aminosbh](https://gitlab.com/aminosbh)

## Explination

### final_pendulum

This is our main file. Here we define both our math and render loops. Keeping these loops seperate via threading is one way of making an accurate and fast model, otherwise the math and rendering could block one another.

Simulator is a generic thread wrapper class that takes a callable to run continuously in a thread. Simulator will run our math model loop.

SafeSharedPtr is a shared pointer wrapper class that gives us a way to safely store and load our math model output pointer by locking it with a mutex, otherwise simulantious storing and accessing could corrupt the program.

Now comes main(). Before we get to the render loop boilerplate variable and class initialization must be done, including the SDL objects, math model, and render model. In the main render loop we first create render our framerate as text. An explination can be found here [lazyfoo]((https://lazyfoo.net/tutorials/SDL/25_capping_frame_rate/index.php)). Second, we fetch the math model output with our load() function defined in SafeSharedPtr. Third, we update the render model positions with the updatePosition() function defined in pModel (our render model class). Fourth, we draw the pendulum with our drawPendulum() method also definied in pModel. Last in the loop is refreshing the screen and correcting for framerate.

### pModel

### pMath

### SDLHolder LTexture and LTimer


## Dependencies

- [Git][]
- C++ Compiler (gcc, ...)
- [CMake][]
- [SDL2][SDL] library

**On Debian/Ubuntu based distributions, use the following command:**

```sh
sudo apt install git build-essential pkg-config cmake cmake-data libsdl2-dev
```

**Optional packages:**

- [SDL2_image][] library
- [SDL2_ttf][] library
- [SDL2_net][] library
- [SDL2_mixer][] library
- [SDL2_gfx][] library

```sh
sudo apt install libsdl2-image-dev libsdl2-ttf-dev libsdl2-net-dev libsdl2-mixer-dev libsdl2-gfx-dev
```

## Build instructions

```sh
# Clone this repo
git clone https://github.com/rdbruce/final_pendulum.git
cd final_pendulum

# Navigate to build folder
cd build

# Build
cmake ..
make

# Run
./final-pendulum-project
```

***Note:*** To use SDL2_image, SDL2_ttf, SDL2_net, SDL2_mixer or SDL2_gfx, you
should uncomment some instructions in the CMakeLists.txt file and re-execute
the `make` command.


[SDL]: https://www.libsdl.org
[CMake]: https://cmake.org
[Git]: https://git-scm.com
[SDL2_image]: https://www.libsdl.org/projects/SDL_image
[SDL2_ttf]: https://www.libsdl.org/projects/SDL_ttf
[SDL2_net]: https://www.libsdl.org/projects/SDL_net
[SDL2_mixer]: https://www.libsdl.org/projects/SDL_mixer
[SDL2_gfx]: http://www.ferzkopp.net/wordpress/2016/01/02/sdl_gfx-sdl2_gfx
