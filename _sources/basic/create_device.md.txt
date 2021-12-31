# CreateDevice

```{literalinclude} ../../lib/gorilla/device.cpp
:language: cpp
:caption:
```

## CreateDevice flag
* `D3D11_CREATE_DEVICE_DEBUG` があるとアプリケーション終了時に COM の Release 忘れが Console に表示される
* `D3D11_CREATE_DEVICE_BGRA_SUPPORT` は、D2D デバイスを作る場合は必要

## Microsoft::WRL::ComPtr<T>

IUnknown を std::shared_ptr のように扱えるようにする。

```c++
#include <wrl/client.h>
template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;
```

`->` 経由でポインタのように扱えばよいが、初期化、生ポインタへの変換、COM Interface の取得などで固有の操作がある。

### 初期化

```{todo} ComPtr init
```

### get

### getaddresof

### cast
