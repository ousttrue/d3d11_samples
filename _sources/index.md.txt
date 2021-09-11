# d3d11 samples

d3d11 の備忘録。

* Windows10
* vc2019
* CMake
* `c++17`
* clang-format オプションなし
* class name: UpperCamel
* other: snake_case

で記述する。

```{toctree}
:maxdepth: 2

create_window
create_device
swapchain
compile_shader
simple_pipeline
constant_buffer
texture
vertex_buffer
```


## old

D3D11(vs2013)による三角形描画サンプル。

# 更新
* 20180531 CMakeに書き換える

#Tutorial

## [x]01.CreateDevice & ClearRenderTarget

ID3D11Deviceを生成してWindowを単色でクリアするまで。

## [ ]02.D2D_HUD
D2Dで文字情報を描画する。

### ToDo
* ロガーのHUD表示
* HUDの簡易UI

## [x]03.MinShaderPipeline
シェーダーパイプライン事始め。

##[ ]04.Texture
テクスチャの導入。

## [ ]05.GeometryShader
GeometryShaderの導入。

## [ ]06.ConstantsBuffer
定数バッファの導入。

## [ ]07.ConstantsBuffer
定数バッファを更新してアニメーションする。

## [ ]08.Projection
投影を取り入れて立体になる。

## [ ]09.Camera & DepthStencil
カメラを取り入れる。
深度バッファ。

## [ ]10.PmdModel
まとめとしてPmdモデルから頂点バッファを入力する。

# Indices and tables

* {ref}`genindex`
* {ref}`modindex`
* {ref}`search`
