from conan import ConanFile
from conan.tools.cmake import cmake_layout


class ExampleRecipe(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps", "CMakeToolchain"

    def requirements(self):
        self.requires("sdl/[~2.30]")
        # self.requires("sdl_ttf/2.0.18")
        # self.requires("sdl_image/2.0.5")

    def layout(self):
        cmake_layout(self)