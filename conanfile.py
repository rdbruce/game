from conan import ConanFile
from conan.tools.cmake import cmake_layout


class ExampleRecipe(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps", "CMakeToolchain"

    def requirements(self):
        self.requires("sdl/2.30.4")
        # self.requires("sdl/2.28.3")
        # self.requires("sdl_ttf/2.22.0")
        # self.requires("sdl_image/2.6.3")

    def layout(self):
        cmake_layout(self)