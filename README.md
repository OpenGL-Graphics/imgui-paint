# How to get code
Clone repo and its submodules:

```console
$ git clone git@github.com:h4k1m0u/imgui-example.git
$ git submodule init
$ git submodule update --init --recursive --remote  # also needed to get new commits from submodule
```


# Imgui
- Source and header files for imgui were copied directly from [imgui-repo] into this project.
- Follow [imgui-example] to include imgui in an opengl/glsw project.

[imgui-repo]: https://github.com/ocornut/imgui/
[imgui-example]: https://github.com/ocornut/imgui/tree/master/examples/example_glfw_opengl3

# Documentation
- [Unofficial API][api].
- [Components example][components-example].
- [Integration with glfw/opengl3 example][imgui-opengl-example].

[api]: https://pthom.github.io/imgui_manual_online/manual/imgui_manual.html
[components-example]: https://github.com/ocornut/imgui/blob/master/imgui_demo.cpp
[imgui-opengl-example]: https://github.com/ocornut/imgui/blob/master/examples/example_glfw_opengl3/main.cpp
