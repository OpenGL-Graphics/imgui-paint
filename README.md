# Screenshot
![Screenshot](/screenshots/screenshot-2022-03-27.png)

# Building
Clone repo and its submodules:

```console
# clone repo with its submodules
$ git clone --recursive https://github.com/OpenGL-Graphics/imgui-paint

# build & run
$ cd imgui-paint
$ mkdir build && cd build
$ cmake .. && make -j && ./main

# to get new commits from submodules
$ git submodule update --init --recursive --remote
```

# Dependencies
- **ImGui:** used to render UI.
- **NanoVG:** To draw on texture.

# TODOs
- This [blog post][drawing-techniques] about implementing a brush tool on html5 canvas needs to be checked, because the brush tools still gives discontinuous lines/circles (maybe interpolate the path drawn by user).
- Bug: Painting at bottom (after scrolling) of image doesn't draw at the right location (y-coord is wrong at bottom, check with tooltip)

[drawing-techniques]: http://perfectionkills.com/exploring-canvas-drawing-techniques/

## Not used anymore
Currently, shapes (lines & circles) are painted directly on the opengl texture (i.e. on the gpu) with NanoVG, so Cairo & GDK aren't needed anymore.

- **Cairo:** used to be used draw shapes on the image, and needs to be installed beforehand (see [this example][gist-cairo]).
- **GDK:** used to be used to convert edited Cairo image surface to pixel data before rendering (avoids costly writing to disk eachtime a shape is drawn).

Both dependencies were installed on Ubuntu:

```console
$ apt install libglfw3-dev libcairo2-dev libgtk-3-dev
```

[gist-cairo]: https://gist.github.com/h4k1m0u/703a8c1afd4f256fd32f5446b8e6dae6

# Test
This app was tested on the following drivers:
- Opengl-3.0 & GLSL-1.30
- Opengl-4.6 & GLSL-4.60

---

# Bundled dependencies
## Imgui
- Source and header files for imgui were copied directly from [imgui-repo] into this project.
- Follow [imgui-example] to include imgui in an opengl/glsw project.

[imgui-repo]: https://github.com/ocornut/imgui/
[imgui-example]: https://github.com/ocornut/imgui/tree/master/examples/example_glfw_opengl3

### Documentation
- [Unofficial API][api].
- [Components example][components-example].
- [Integration with glfw/opengl3 example][imgui-opengl-example].

[api]: https://pthom.github.io/imgui_manual_online/manual/imgui_manual.html
[components-example]: https://github.com/ocornut/imgui/blob/master/imgui_demo.cpp
[imgui-opengl-example]: https://github.com/ocornut/imgui/blob/master/examples/example_glfw_opengl3/main.cpp

## ImGuiFileDialog
Downloaded from [repo][imgui-filedialog-repo] and installed following the [tutorial][imgui-filedialog-tutorial] found on the same page.

[imgui-filedialog-repo]: https://github.com/aiekick/ImGuiFileDialog
[imgui-filedialog-tutorial]: https://github.com/aiekick/ImGuiFileDialog#structure

## IconFontCppHeaders
Downloaded from [repo][icon-font-repo]

[icon-font-repo]: https://github.com/juliettef/IconFontCppHeaders
