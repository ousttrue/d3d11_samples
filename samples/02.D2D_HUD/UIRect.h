#pragma once
#include <Windows.h>
#include <functional>
#include <memory>
#include <string>
#include <list>
#include <boost/property_tree/ptree.hpp>


class ITextContext;
class D2DSolidColorBrush;
class D2DTextFormat;
class UIRect
{
    enum LAYOUT_TYPE
    {
        LAYOUT_STACK_HORIZONTAL,
        LAYOUT_STACK_VERTICAL,
        LAYOUT_WINDOW,
    };
    LAYOUT_TYPE m_layout;

    // 要求サイズ
    boost::optional<float> m_left;
    boost::optional<float> m_top;
    boost::optional<float> m_right;
    boost::optional<float> m_bottom;
	float m_width;
    float m_height;

    // 描画文字列
    typedef std::function<std::wstring(ITextContext*)> TextFunc;
    TextFunc m_text;

    // Layout()で計算する値
public:
    struct Rect
    {
        float left;
        float top;
        float right;
        float bottom;

        Rect(float l, float t, float r, float b)
            : left(l), top(t), right(r), bottom(b)
        {}
    };
private:
    Rect m_rect;

    // 子ノード
    std::list<std::shared_ptr<UIRect>> m_children;

    // 描画ブラシ
    std::shared_ptr<D2DTextFormat> m_textformat;
	std::shared_ptr<D2DSolidColorBrush> m_bg;
	std::shared_ptr<D2DSolidColorBrush> m_fg;

public:
	UIRect();
    void SetDefault();
    void SetLayout(LAYOUT_TYPE layout){ m_layout=layout; }

    void SetLeft(float left){ m_left=left; }
    boost::optional<float> GetLeft()const{ return m_left; }
    void SetTop(float top){ m_top=top; }
    boost::optional<float> GetTop()const{ return m_top; }
    void SetRight(float right){ m_right=right; }
    boost::optional<float> GetRight()const{ return m_right; }
    void SetBottom(float bottom){ m_bottom=bottom; }
    boost::optional<float> GetBottom()const{ return m_bottom; }

    void SetWidth(float width){ m_width=width; }
    float GetWidth()const{ return m_width; }
    void SetHeight(float height){ m_height=height; }
    float GetHeight()const{ return m_height; } 
    void SetText(const TextFunc &text){ m_text=text; }
    void SetBG(const std::shared_ptr<D2DSolidColorBrush> &bg){ m_bg=bg; }
    void SetFG(const std::shared_ptr<D2DSolidColorBrush> &fg){ m_fg=fg; }
    void SetTextFormat(const std::shared_ptr<D2DTextFormat> &textformat){ m_textformat=textformat; }
    void AddChild(const std::shared_ptr<UIRect> &child) { m_children.push_back(child); }
    void Traverse(const boost::property_tree::wptree &pt);
    void Layout(const Rect &rect);
    void Render(struct ID2D1DeviceContext *pRenderTarget
		, ITextContext *pContext
		, struct IDWriteTextFormat *pTextFormat = nullptr
        , struct ID2D1Brush *pBG=nullptr
        , struct ID2D1Brush *pFG=nullptr
        );

private:
    void RenderSelf(struct ID2D1DeviceContext *pRenderTarget
		, ITextContext *pContext
		, struct IDWriteTextFormat *pTextFormat
        , struct ID2D1Brush *pBG
        , struct ID2D1Brush *pFG
        );
};

