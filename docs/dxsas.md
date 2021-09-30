# DXSAS

`Direct X Standard Annotations and Semantics`

<https://docs.microsoft.com/en-us/windows/win32/direct3d9/dx9-graphics-reference-effects-dxsas>

廃れてしまった `D3D9` 時代の仕様なのだけど、便利なので部分的に実装する。

* <https://docs.microsoft.com/en-us/windows/win32/direct3d9/data-binding>
## Implementations

* <https://www.nvidia.com/en-us/drivers/using-sas/>
    * <http://download.nvidia.com/developer/presentations/2005/GDC/Sponsored_Day/Writing_w_FXComposer.pdf>
* [MMEリファレンス‎ > ‎2. パラメータのセマンティクスとアノテーション‎ > ‎2-1. ジオメトリ変換](https://sites.google.com/site/mmereference/home/Annotations-and-Semantics-of-the-parameter/2-1-geometry-translation)
* [VVVV チュートリアル ジオメトリモーフィング GPU1](https://vvvv.org/documentation/%E3%83%81%E3%83%A5%E3%83%BC%E3%83%88%E3%83%AA%E3%82%A2%E3%83%AB-%E3%82%B8%E3%82%AA%E3%83%A1%E3%83%88%E3%83%AA%E3%83%A2%E3%83%BC%E3%83%95%E3%82%A3%E3%83%B3%E3%82%B0-gpu1)

## Bindings

| SEMANTIC            | type     | comment |
|---------------------|----------|---------|
| WORLD               | float4x4 |         |
| VIEW                | float4x4 |         |
| PROJECTION          | float4x4 |         |
| WORLDVIEW           | float4x4 |         |
| VIEWPROJECTION      | float4x4 |         |
| WORLDVIEWPROJECTION | float4x4 |         |

### Common Material and Light Characteristics

* POSITION
* DIRECTION
* DIFFUSE
* SPECULAR
* AMBIENT
* POWER
* SPECULARPOWER
* CONSTANTATTENUATION
* LINEARATTENUATION
* QUADRATICATTENUATION
* FALLOFFANGLE
* FALLOFFEXPONENT
* EMISSION
* EMISSIVE
* OPACITY
* REFRACTION

### Texture-Related

* RENDERDEPTHSTENCILTARGET
* RENDERCOLORTARGET
* VIEWPORTPIXELSIZE
* DIFFUSEMAP
* SPECULARMAP
* NORMAL
* ENVIRONMENT
* ENVMAP
* ENVIRONMENTNORMAL

### Transforms & Locations

* WORLD
* VIEW
* PROJECTION
* WORLDVIEW
* VIEW PROJECTION
* WORLDVIEWPROJECTION
* WORLDINVERSE
* VIEWINVERSE
* PROJECTIONINVERSE
* WORLDVIEWINVERSE
* VIEW PROJECTIONINVERSE
* WORLDVIEWPROJECTIONINVERSE
* WORLDTRANSPOSE
* VIEWTRANSPOSE
* PROJECTIONTRANSPOSE
* WORLDVIEWTRANSPOSE
* VIEW PROJECTIONTRANSPOSE
* WORLDVIEWPROJECTIONTRANSPOSE
* WORLDINVERSETRANSPOSE
* VIEWINVERSETRANSPOSE
* PROJECTIONINVERSETRANSPOSE
* WORLDVIEWINVERSETRANSPOSE
* VIEW PROJECTIONINVERSETRANSPOSE
* WORLDVIEWPROJECTIONINVERSETRANSPOSE
* TRANSFORM
* LIGHTPOSITION

### Others

* STANDARDSGLOBAL
* HEIGHT
* UNITSSCALE
