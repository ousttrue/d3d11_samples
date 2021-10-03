# Tangent Space Lighting

Tangent 空間で法線(n)と光源方向(s)の内積をする。

<http://www.mikktspace.com/>

# tangent

tangent は、頂点バッファ(position, normal, tex)から mikktspace アルゴリズムにより得る。

```{literalinclude} ../../_external\MikkTSpace\mikktspace.h
:language: hlsl
:caption:
```

## vs

```{literalinclude} ../../assets/lighting/tspace.hlsl
:language: hlsl
:caption:
```

Light ベクトル を `tspace` に変形しておく。
tspace は、頂点の normal と tangent から得ることができる。

## ps

uv から法線マップにアクセスして法線を得る。
この法線は、 `tspace` の法線。

## NormalMap を作る方法

* Substance Painter がデファクトらしい(使ったことない)。
* [Blender ノーマルマップ作成・出力方法](https://www.tomog-storage.com/entry/Blender-Beginner-ExportNomarlMapByBlender)
