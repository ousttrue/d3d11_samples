# Basic Pipeline

Sahder を使う最小構成。

頂点バッファを省略することができるので2次元の `GeometryShader` から入る。
空の頂点バッファからダミーの Point Primitive を入力して、GeometryShader で 三角形を出力する。

## 頂点バッファを省略

```{literalinclude} ../../samples/BasicPipeline/gorilla/pipeline.cpp
:caption:
:language: cpp
:lines: 70-74
:linenos:
```

何もしない頂点シェーダー。
`point DummyInput input[1]: POSITION` に対応して `D3D11_PRIMITIVE_TOPOLOGY_POINTLIST` を使う。
空の頂点シェーダーをキックすることで、間接的に GeometryShader を始動する。

## GeometryShader

```{literalinclude} ../../assets/basic.hlsl
:caption:
:language: hlsl
:emphasize-lines: 15-16,19-20,23-24
:linenos:
```

## ビューポート座標系

```{image} ./basic_pipeline.jpg
:width: 482px
:height: 528px
```

第1頂点が (-1, -1) かつ赤。
第2頂点が (-1, +1) かつ緑。
第3頂点が (+1, +1) かつ青。

ということから

```
2(-1, +1) 3(+1, +1)
     +---+
     |   |
     +---+
1(-1, -1)
```

という座標系であることがわかる。
三角形は時計回り。
試しに、第２頂点の符号を逆にすると見えなくなる。

`ID3D11RasterizerState` のデフォルトが 

```
D3D11_RASTERIZER_DESC::CullMode = D3D11_CULL_BACK;
D3D11_RASTERIZER_DESC::FrontCounterClockwise = false;
```

であることがわかる。
