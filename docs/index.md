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
  0
 0+->+1(x)
  |
  v
 +1
 (y)
```

```hlsl
cbuffer model {
  row_major float4 BaseColor;
}
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

glTF の座標系を採用する。右手系Y-UP

```{toctree}
  (y)
   ^ /
   |/
 --+-->(x)
  /|
 L |
(z)
```

```hlsl
cbuffer object {
  row_major matrix MVP;
  // TODO: Light
}
```

```{toctree}
three_d/orbit_camera
three_d/depth_test
```

## Lighting

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
