# 仕様決め

glTF を中心に取り扱いたいので、仕様はそれに合わせる。


## Viewport

```
  (y)
  +1
   ^
-1-+>+1(x)
   |
  -1
```

## UV

左上原点

```
  0
 0+->+1(x)
  |
  v
 +1
 (y)
```

## World Space

右手系Y-UP

`DirectX::XMMatrixPerspectiveFovRH`

```
  (y)
   ^ /
   |/
 --+-->(x)
  /|
 L |
(z)
```

## 三角形の表

```
D3D11_RASTERIZER_DESC rs_desc = {};
rs_desc.CullMode = D3D11_CULL_BACK;
rs_desc.FrontCounterClockwise = true;
```

## Constant Buffer

変数名で解決することにした。
各 `HLSL` は、あらかじめ決まった変数名を使う。

```c++
commands.push_back(commands::SetVariable{"MVP", m * viewprojection});
commands.push_back(commands::SetVariable{"VP", viewprojection});
commands.push_back(commands::SetVariable{"ModelViewMatrix", view});
commands.push_back(commands::SetVariable{"NormalMatrix", normal_matrix});
commands.push_back(commands::SetVariable{"CameraPosition", camera_position});
```
