# RTV from SwapChain

swapchain から backbuffer(`ID3D11Texture2D`) を得てRTV を作る。
RTV に対して操作する

* clear する
* pipeline にセットする

ことで RTV の元になった ID3D11Texture2D を更新できる。

| surface | format                        | bind                                                  | target                      | source                        |
| ------- | ----------------------------- | ----------------------------------------------------- | --------------------------- | ----------------------------- |
|         | DXGI_FORMAT_R8G8B8A8_UNORM    | D3D11_BIND_RENDER_TARGET + D3D11_BIND_SHADER_RESOURCE | ID3D11RenderTargetView(RTV) | ID3D11ShaderResourceView(SRV) |

```{literalinclude} ../../lib/gorilla/render_target.cpp
:language: cpp
:caption:
```
