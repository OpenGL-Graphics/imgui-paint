cmake_minimum_required(VERSION 3.10)
project(ImGUI-Example)

# use C++17 (in case std::fs needed)
set(CMAKE_CXX_STANDARD 17)

# autocomplete with YCM & debug with gdb
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Release)
# set(CMAKE_BUILD_TYPE Debug)
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=leak")

# copy assets folder
file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})

# imgui library (preprocessor definition to force it to use glad)
add_compile_definitions(IMGUI_IMPL_OPENGL_LOADER_GLAD)
file(GLOB IMGUI_SRC "src/imgui/*.cpp")
add_library(imgui SHARED ${IMGUI_SRC})
target_include_directories(imgui PUBLIC include/imgui include)

# filedialog extension for imgui
add_library(file_dialog SHARED src/ImGuiFileDialog/ImGuiFileDialog.cpp)
target_include_directories(file_dialog PUBLIC include/ImGuiFileDialog include/imgui)

# header-only font icons library
add_library(icon_font INTERFACE)
target_include_directories(icon_font INTERFACE include/IconFontCppHeaders)

# nanosvg shared library (prevent from loading file multiple times)
# add_compile_definitions(NANOVG_GL3_IMPLEMENTATION)
add_library(nanovg SHARED
  "src/nanovg/nanovg.c"
)
target_include_directories(nanovg PUBLIC include/nanovg)

# glfw window & opengl utils (texture, shader, fbo) libraries from submodule
add_subdirectory(glfw-window)
add_subdirectory(opengl-utils)

# main executable
file(GLOB SRC
  "src/ui/*.cpp"
  "src/ui/enumerations/*.cpp"
  "src/ui/tooltips/*.cpp"
  "src/ui/listeners/*.cpp"
  "src/ui/globals/*.cpp"
  "src/image/*.cpp"
  "src/fonts/*.cpp"
  "src/profiling/*.cpp"
  "src/main.cpp"
)
add_executable(main ${SRC})
target_link_libraries(main
  nanovg
  imgui
  file_dialog
  icon_font

  glfw_window
  opengl_utils
)
