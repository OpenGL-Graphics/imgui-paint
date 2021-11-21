cmake_minimum_required(VERSION 3.10)
project(ImGUI-Example)

# use C++17 (in case std::fs needed)
set(CMAKE_CXX_STANDARD 17)

# autocomplete with YCM & debug with gdb
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Release)
# set(CMAKE_BUILD_TYPE Debug)

# glad shared library (opengl)
add_library(glad SHARED src/glad/glad.c)
target_include_directories(glad PUBLIC include)

# imgui library (preprocessor definition to force it to use glad)
add_compile_definitions(IMGUI_IMPL_OPENGL_LOADER_GLAD)
file(GLOB IMGUI_SRC "src/imgui/*.cpp")
add_library(imgui SHARED ${IMGUI_SRC})
target_include_directories(imgui PUBLIC include/imgui include)

# filedialog extension for imgui
add_library(file_dialog SHARED src/ImGuiFileDialog/ImGuiFileDialog.cpp)
target_include_directories(file_dialog PUBLIC include/ImGuiFileDialog include/imgui)

# glfw window & shader (with texture & glad) libraries from submodule
add_subdirectory(glfw-window)
add_subdirectory(opengl-shader)

# main executable
file(GLOB SRC "src/*.cpp")
add_executable(main ${SRC})
target_link_libraries(main
  glad
  imgui
  glfw_window
  opengl_shader
  file_dialog
)
