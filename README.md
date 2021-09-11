# D3D11 Samples

久しぶりに d3d11 やったら、忘れているし検索で得られる情報も無くなっていたり、
サンプルが dxut とか d3dx に依存していて改造が必要だったりするので、
小さいサンプルを維持する必要性を感じる。

## update

* 20210911: 履歴破棄
* [ ] sphinx4 で gh-pages に記事作ってみる(myst)

## sphinx

* pip install sphinx
* mkdir docs
* cd docs
* sphinx-quickstart
* rm Makefile, make.bat
* task: docs$ sphinx-build . ../public

* [actions](./.github/workflows/sphinx.yml)

* [Sphinx で使える Markdown 方言 'MyST'](https://qiita.com/Tachy_Pochy/items/53866eea43d0ad93ea1d)



## old

D3D11(vs2013)による三角形描画サンプル。

# 更新
* 20180531 CMakeに書き換える

#Tutorial
##[x]01.CreateDevice & ClearRenderTarget
ID3D11Deviceを生成してWindowを単色でクリアするまで。

##[ ]02.D2D_HUD
D2Dで文字情報を描画する。

###ToDo
* ロガーのHUD表示
* HUDの簡易UI

##[x]03.MinShaderPipeline
シェーダーパイプライン事始め。

##[ ]04.Texture
テクスチャの導入。

##[ ]05.GeometryShader
GeometryShaderの導入。

##[ ]06.ConstantsBuffer
定数バッファの導入。

##[ ]07.ConstantsBuffer
定数バッファを更新してアニメーションする。

##[ ]08.Projection
投影を取り入れて立体になる。

##[ ]09.Camera & DepthStencil
カメラを取り入れる。
深度バッファ。

##[ ]10.PmdModel
まとめとしてPmdモデルから頂点バッファを入力する。

