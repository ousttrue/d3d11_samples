# Basic Pipeline

Sahder を使う最小構成。

```{image} ./basic_pipeline.jpg
:width: 320px
:height: 320px
```

頂点バッファを省略することができるので2次元の `GeometryShader` から入る。
空の頂点バッファからダミーの Point Primitive を入力して、GeometryShader で 三角形を出力する。

## 頂点バッファ

```{literalinclude} ../../samples/BasicPipeline/pipeline.cpp
:caption:
:language: cpp
:lines: 75-79
:linenos:
```

何もしない頂点シェーダー。
`point DummyInput input[1]: POSITION` に対応して `D3D11_PRIMITIVE_TOPOLOGY_POINTLIST` を使う。

## GeometryShader

```{literalinclude} ../../samples/BasicPipeline/basic.hlsl
:caption:
:language: hlsl
:emphasize-lines: 15-16,19-20,23-24
:linenos:
```

第1頂点が (-1, -1) かつ赤。
第2頂点が (-1, -1) かつ緑。
第3頂点が (1, 1) かつ青。

ということから

```
         (1, 1)
     +---+
     |   |
     +---+
(-1, -1)
```

という座標系であることがわかる。
