# Root

<https://github.com/ousttrue/d3d11_samples>

ツールキット実装ノートになってしまった。

🦍 lib/gorilla

-   window
-   d3d11, device, resource
-   imgui

🍌 lib/banana

-   camera
-   scene
-   glTF

env

-   Windows10
-   vc2019
-   CMake
-   `c++17`
    -   std::string_view
    -   std::filesystem
-   clang-format オプションなし
-   class name: UpperCamel
-   other: snake_case

dependency

-   imgui
-   mathfu

```{toctree}
todo
```

## basic

CreateWindow ～ ImGui導入 ～ RenderTargetへの描画

```{toctree}
:maxdepth: 1
basic/index
```

## pipeline

ShaderPipeline 一般

```{toctree}
pipeline/index
```

## DXSAS

DXSAS

```{toctree}
dxsas/index
```

## 二次元シーン

## 三次元シーン

```{toctree}
three_d/index
gltf/index
gizmo/tinygizmo
```

## Tool

-   <https://renderdoc.org/>

## 参考

-   <https://docs.microsoft.com/en-us/windows/win32/direct3d11/d3d11-graphics-reference-d3d11-core-interfaces>

-   [いまさらDirect3D11入門](https://tositeru.github.io/ImasaraDX11/)

-   {ref}`genindex`

-   {ref}`modindex`

-   {ref}`search`
