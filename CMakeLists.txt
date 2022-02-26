cmake_minimum_required(VERSION 3.10)
project(ImGUI-Example)

# use C++17 (in case std::fs needed)
set(CMAKE_CXX_STANDARD 17)

# autocomplete with YCM & debug with gdb
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Release)
# set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=leak")

# fill in Cairo/GDK headers & lib locations variable from /usr/lib/x86_64-linux-gnu/pkgconfig/{cairo.pc, gdk-3.0.pc}
find_package(PkgConfig REQUIRED)
pkg_check_modules(CAIRO REQUIRED cairo)
pkg_check_modules(GDK REQUIRED gdk-3.0)

# copy assets folder
file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})

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

# header-only library
add_library(icon_font INTERFACE)
target_include_directories(icon_font INTERFACE include/IconFontCppHeaders)

# glfw window & shader (with texture & glad) libraries from submodule
add_subdirectory(glfw-window)
add_subdirectory(opengl-shader)

# main executable
file(GLOB SRC
  "src/ui/*.cpp"
  "src/ui/enumerations/*.cpp"
  "src/ui/tooltips/*.cpp"
  "src/ui/listeners/*.cpp"
  "src/ui/constants/*.cpp"
  "src/image/*.cpp"
  "src/fonts/*.cpp"
  "src/profiling/*.cpp"
  "src/main.cpp"
)
add_executable(main ${SRC})
target_include_directories(main PRIVATE
  ${CAIRO_INCLUDE_DIRS}
  ${GDK_INCLUDE_DIRS}
)
target_link_libraries(main
  glad
  imgui
  glfw_window
  opengl_shader
  file_dialog
  icon_font
  ${CAIRO_LIBRARIES}
  ${GDK_LIBRARIES}
)
