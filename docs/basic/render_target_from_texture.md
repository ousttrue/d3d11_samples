# RTV from Texture

Dockspace の中にはめ込むために、
Texture に対して描画できるようにする。

以降の章では、RTV に対して如何に描画するかについて説明する。
RTV が backbuffer もしくは texture 由来であることを区別しない。

```{image} ./imgui_rtv.jpg
:width: 640px
:height: 480px
```

```{literalinclude} ../../samples/_Basic/ImGuiDockspaceView/main.cpp
:caption:
:language: cpp
```

```{todo} マウスイベントのハンドリング
```
