# TinyGizmo

Gizmo ライブラリとしては小さくて改造しやすい [tinygizmo](https://github.com/ddiakopoulos/tinygizmo) (OpenGL3 + GLFW) を d3d11 に移植した。

<https://github.com/ousttrue/tinygizmo>

## 仕組み

1. Update: 毎フレーム Gizmo にカメラパラメーター(projection + view)とマウスイベント(move, click, drag)を与える
2. Transform: TRS を引数に 関数を呼ぶ
3. Draw: Gizmo のビジュアルを描画する vertex buffer を得る

もう少し詳細に見ると、カメラから マウス座標に対して Ray を生成して衝突した Gizmo をドラッグする。

## Translation

## Rotation

## Scale
