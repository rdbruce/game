# Double Pendulum Project

![final_pendulum](pendulum.PNG)

This is a personal project that models and displays a double pendulum. The bones for the SDL implimentation can be found at [lazyfoo](https://lazyfoo.net/tutorials/SDL/).

## Dependencies

- [Git][]
- C Compiler (gcc, ...)
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

### Open the project with an IDE under Linux

See [IDE_USAGE.md](IDE_USAGE.md) for details.

## License

Author: Amine B. Hassouna [@aminosbh](https://gitlab.com/aminosbh)

This project is distributed under the terms of the MIT license
[&lt;LICENSE&gt;](LICENSE).



[SDL]: https://www.libsdl.org
[CMake]: https://cmake.org
[Git]: https://git-scm.com
[SDL2_image]: https://www.libsdl.org/projects/SDL_image
[SDL2_ttf]: https://www.libsdl.org/projects/SDL_ttf
[SDL2_net]: https://www.libsdl.org/projects/SDL_net
[SDL2_mixer]: https://www.libsdl.org/projects/SDL_mixer
[SDL2_gfx]: http://www.ferzkopp.net/wordpress/2016/01/02/sdl_gfx-sdl2_gfx
