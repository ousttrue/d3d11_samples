# Root

d3d11 の備忘録。

## 準備

* <https://docs.microsoft.com/en-us/windows/win32/direct3d11/d3d11-graphics-reference-d3d11-core-interfaces>
* <https://tositeru.github.io/ImasaraDX11/>

```{toctree}
:maxdepth: 2

source
spec
basic/create_window
basic/create_device
basic/create_swapchain
basic/render_target
basic/compile_shader
basic/shader_reflection
```

## パイプラインとバッファーの取り扱い

```{toctree}
:maxdepth: 2

basic/basic_pipeline
basic/constant_buffer
basic/texture
basic/input_assembler
basic/rasterizer_state
basic/blend_state
basic/mip_map
```

## DXSAS

```{toctree}
:maxdepth: 2

dxsas
```

## Projection の導入と三次元化

```{toctree}
:maxdepth: 2

three_d/orbit_camera
three_d/depth_test
```

## Shading

* Cook-Torrance

```{toctree}
:maxdepth: 2

shading/ads
shading/tangent_space
shading/pbr
```

## SceneGraph

```{toctree}
:maxdepth: 2

three_d/scene
three_d/gltf
```

# Gizmo

* grid
* aabb pick (ray intersection)

```{toctree}
:maxdepth: 2

tinygizmo
```

# Tool

* <https://renderdoc.org/>

# glTF

* <https://github.com/KhronosGroup/glTF>
* <https://github.com/KhronosGroup/glTF-Sample-Models>

# Indices and tables

* {ref}`genindex`
* {ref}`modindex`
* {ref}`search`
