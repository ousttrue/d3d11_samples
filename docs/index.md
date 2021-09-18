# Root

d3d11 の備忘録。

## Source

```{toctree}
source
```

## Screen Space

以下の座標系

```
  (y)
  +1
   ^
-1-+>+1(x)
   |
  -1
```

UV は、左上原点

```
  (y)
   0
   |
 0-+>+1(x)
   V
  +1
```

```{toctree}
basic/create_window
basic/create_device
basic/create_swapchain
basic/render_target
basic/compile_shader
basic/basic_pipeline
basic/shader_reflection
basic/constant_buffer
basic/texture
basic/input_assembler
```

## Object Space

```{toctree}
three_d/orbit_camera
three_d/depth_test
```

## SceneGraph

```{toctree}
three_d/scene
```

# 参考

* <https://docs.microsoft.com/en-us/windows/win32/direct3d11/d3d11-graphics-reference-d3d11-core-interfaces>
* <https://tositeru.github.io/ImasaraDX11/>

# Indices and tables

* {ref}`genindex`
* {ref}`modindex`
* {ref}`search`
