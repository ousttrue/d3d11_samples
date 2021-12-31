# Physically Based Rendering

[GLSLで物理ベースシェーディング](https://qiita.com/aa_debdeb/items/f813bdcbd8524a66a11b)

```hlsl
float3 color = (1-metallic) * diffuse + metallic * specular;
```

ということでよさそう。

## Diffuse
### 正規化Lambert

<https://github.com/KhronosGroup/glTF-Sample-Viewer/blob/glTF-WebGL-PBR/shaders/pbr-frag.glsl>

```glsl
// Basic Lambertian diffuse
// Implementation from Lambert's Photometria https://archive.org/details/lambertsphotome00lambgoog
// See also [1], Equation 1
vec3 diffuse(PBRInfo pbrInputs)
{
    return pbrInputs.diffuseColor / M_PI;
}

// irradiance x BRDF
dot(S, N) * diffuse(pbrInputs);
```

という具合にに `PI` で割る。
[正規化Lambert](http://www.project-asura.com/program/d3d11/d3d11_004.html)
というらしい。

#### Albedo

ここで `pbrInputs.diffuseColor` が `color` とちょっと違うので `Albedo` と名付けたぽい？

* `sRGB` でなくて `linear` で扱う
* PI で割ることをあらかじめ加味しておく

## Specular: 鏡面反射モデル

[GLSLによるフォンシェーディング](http://www.slis.tsukuba.ac.jp/~fujisawa.makoto.fu/cgi-bin/wiki/index.php?GLSL%A4%CB%A4%E8%A4%EB%A5%D5%A5%A9%A5%F3%A5%B7%A5%A7%A1%BC%A5%C7%A5%A3%A5%F3%A5%B0)

|  | D | G | F |
|--|---|---|---|
|  |   |   |   |
