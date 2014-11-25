#include "HUD.h"
#include "UIRect.h"
#include "D2DResource.h"
#include <wrl/client.h>
#include <boost/property_tree/xml_parser.hpp>
#include <d2d1_1.h>


std::wstring DictContext::GetText(const std::wstring &key)
{
    auto found=m_map.find(key);
    if(found==m_map.end())return L"";

    return found->second;
}

void DictContext::SetText(const std::wstring &key, const std::wstring &value)
{
    m_map[key]=value;
}


HUD::HUD()
    : m_context(new DictContext)
{}

bool HUD::Load(const std::string &path)
{
    boost::property_tree::wptree pt;
	//boost::property_tree::read_json(path.c_str(), pt);
	boost::property_tree::read_xml(path.c_str(), pt);
	if (pt.empty()){
        return false;
    }

	// build tree
	if (auto layout = pt.get_child_optional(L"Rect")){
		m_root = std::make_shared<UIRect>();

        m_root->SetDefault();

		// traverse ui tree
		m_root->Traverse(*layout);
	}

    return true;
}

void HUD::Update(const std::chrono::milliseconds &elapsed)
{
	if (!m_root)return;

    static int counter=0;
    ++counter;

    std::wstringstream ss;
    ss << counter;

    m_context->SetText(L"FPS", ss.str());
}

void HUD::Render(ID2D1DeviceContext *pRenderTarget)
{
	if (!m_root)return;

    pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
    D2D1_SIZE_F rtSize = pRenderTarget->GetSize();
	m_root->Layout(UIRect::Rect(0, 0, rtSize.width, rtSize.height));
    m_root->Render(pRenderTarget, m_context.get());
}

