# Root

d3d11 の備忘録。

## 準備

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
basic/basic_pipeline
basic/constant_buffer
basic/texture
basic/input_assembler
basic/mip_map
basic/post_effect
```

## Projection の導入と三次元化

```{toctree}
three_d/orbit_camera
three_d/depth_test
```

## Shading

* ADS(Ambient, Diffuse, Specular)
* NormalMap
* Cook-Torrance

```{toctree}
shading/pbr
```

## SceneGraph

```{toctree}
three_d/scene
three_d/gltf
```

# Gizmo

* grid
* aabb pick (ray intersection)

# 参考

* <https://docs.microsoft.com/en-us/windows/win32/direct3d11/d3d11-graphics-reference-d3d11-core-interfaces>
* <https://tositeru.github.io/ImasaraDX11/>

# Tool

* <https://renderdoc.org/>

# glTF

* <https://github.com/KhronosGroup/glTF>
* <https://github.com/KhronosGroup/glTF-Sample-Models>

# Indices and tables

* {ref}`genindex`
* {ref}`modindex`
* {ref}`search`
