#pragma once
#include <string>
#include <memory>
#include <chrono>
#include <hash_map>
#include "TextContext.h"


class DictContext: public ITextContext
{
    std::hash_map<std::wstring, std::wstring> m_map;

public:
    std::wstring GetText(const std::wstring &key)override;

    void SetText(const std::wstring &key, const std::wstring &value);
};


///
/// HUDレイアウト
///
class HUD
{
    std::shared_ptr<class UIRect> m_root;
    std::shared_ptr<DictContext> m_context;

public:
    HUD();
    bool Load(const std::string &path);
    void Update(const std::chrono::milliseconds &elapsed);
    void Render(struct ID2D1DeviceContext *pContext);
    std::shared_ptr<DictContext> GetDict(){ return m_context; }
};

