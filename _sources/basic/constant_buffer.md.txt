# Constant Buffer

マウス座標と ViewPort サイズを ConstantBuffer 経由で送り込んでカーソル位置に三角形を描画する。

```{literalinclude} ../../assets/constant.hlsl
:language: hlsl
:caption:
```

```{literalinclude} ../../lib/gorilla/constant_buffer.cpp
:language: cpp
:caption:
```

## CPU側のメモリレイアウト

16バイトアライメント。
例えば、hlsl の `float3x3` は,

```c
struct Mat3 {
float _11, _12, _13, padding,
_21, _22, _23, padding,
_31, _32, _33, padding,
};
```

になることに注意する。
DirectX::FLOAT3X3 ではなく DirectX::FLOAT3X4 に格納する必要がある。
