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

{doc}`DXSAS </pipeline/dxsas>` のセマンティクス経由で実装する予定。
