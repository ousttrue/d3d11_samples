# Root

<https://github.com/ousttrue/d3d11_samples>

ツールキット実装ノートになってしまった。

🦍 lib/gorilla

-   window
-   d3d11, device, resource

🍌 lib/banana: 環境(windows, d3d11)に依存しない

-   imgui
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

-   catch2
-   magic_enum
-   [imgui](https://github.com/ocornut/imgui)
-   [nlohmann json](https://github.com/nlohmann/json) (glTFロードなど)
-   DirectXMath
-   [mathfu](https://github.com/google/mathfu) (後でシーンをプラットフォーム独立にする場合に、こちらの方が都合がよさそう)
-   MiKKTSpace
-   stb
-   tinygizmo

## basic

CreateWindow ～ ImGui導入 ～ RenderTargetへの描画

```{toctree}
:maxdepth: 1
basic/index
```

## pipeline

ShaderPipeline 一般

```{toctree}
:maxdepth: 1
pipeline/index
```

## 二次元シーン

```{toctree}
:maxdepth: 1
two_d/index
```

## 三次元シーン

```{toctree}
:maxdepth: 1
three_d/index
```

## Tool

-   <https://renderdoc.org/>

## 参考

-   <https://docs.microsoft.com/en-us/windows/win32/direct3d11/d3d11-graphics-reference-d3d11-core-interfaces>
-   [いまさらDirect3D11入門](https://tositeru.github.io/ImasaraDX11/)

## todo

```{toctree}
todo
```

## index

-   {ref}`genindex`
-   {ref}`modindex`
-   {ref}`search`
