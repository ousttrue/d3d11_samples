# MinShaderPipeline
シェーダーパイプラインはロジックと入出力がセットとなるためひとつずつ要素を導入することができないが、
なるべく小さいサンプルから始める。
このサンプルのパイプラインは以下の要素から構成される。

## 入力: InputAssembler Stage
### VertexBuffer + IndexBuffer
各頂点が、位置(必須)と色で構成された三角形。

## ロジック
### VertexShader Stage + VertexLayout
InputAssembler Stageから入力された頂点をそのまま無加工で後続(Rasterizer Stage)に渡す。
頂点のフォーマットも定義する。

### PixelShader Stage
Rasterizer Stageから入力されたピクセルをそのまま無加工で後続(OutputMerger Stage)に渡す。

## 出力: OutputMerger Stage
### RTV(RenderTargetView)
スワップチェインから生成したレンダーターゲット。
ビューポートも定義する。

[Graphics Pipeline](https://msdn.microsoft.com/en-us/library/windows/desktop/ff476882(v=vs.85).aspx)

