#include "DXOverlayUpdater.h"

#include "DXOverlayUpdaterMeta.h"

#include "CoreUtils.h"
#include "utils.h"

namespace
{
	rendering::overlay::DXOverlayRP* m_overlayRP = nullptr;
	rendering::DXMutableBuffer* m_quadInstanceBuffer = nullptr;
	rendering::image_loading::FontAsset* m_fontAsset = nullptr;
	rendering::Window* m_wnd = nullptr;

	void CacheObjects()
	{
		using namespace rendering;

		if (!m_overlayRP)
		{
			m_overlayRP = overlay::GetOverlayRP();
		}

		if (!m_quadInstanceBuffer)
		{
			m_quadInstanceBuffer = overlay::GetQuadInstanceBuffer();
		}

		if (!m_fontAsset)
		{
			m_fontAsset = overlay::GetFontAsset();
		}

		if (!m_wnd)
		{
			m_wnd = core::utils::GetWindow();
		}
	}
}

rendering::overlay::DXOverlayUpdater::DXOverlayUpdater() :
	TickUpdater(DXOverlayUpdaterMeta::GetInstance())
{
	CacheObjects();
}

rendering::overlay::DXOverlayUpdater::~DXOverlayUpdater()
{
}

int rendering::overlay::DXOverlayUpdater::GetPriority()
{
	return 0;
}

void rendering::overlay::DXOverlayUpdater::Update(double dt)
{
	std::list<TextPanel>& textPanels = m_overlayRP->GetTextPannels();

	void* data = m_quadInstanceBuffer->GetUploadBuffer()->Map();
	CharInfo* charInfoData = static_cast<CharInfo*>(data);

	int index = 0;

	for (auto it = textPanels.begin(); it != textPanels.end(); ++it)
	{
		TextPanel& curPanel = *it;

		int cursorX = curPanel.m_screenPosX;
		int cursorY = curPanel.m_screenPosY;

		for (auto charIt = curPanel.m_text.begin(); charIt != curPanel.m_text.end(); ++charIt)
		{
			char curChar = *charIt;
			const image_loading::FontInfo& fontInfo = m_fontAsset->GetFontInfo();
			auto curCharInfoIt = fontInfo.m_charInfo.find(curChar);
			if (curCharInfoIt == fontInfo.m_charInfo.end())
			{
				curCharInfoIt = fontInfo.m_charInfo.find(32);
			}
			const image_loading::CharInfo& curCharInfo = curCharInfoIt->second;

			CharInfo& ci = charInfoData[index];
			ci.m_position[0] = cursorX / (float)m_wnd->m_width;
			ci.m_position[1] = cursorY / (float)m_wnd->m_height;
			ci.m_position[2] = (cursorX + curCharInfo.m_width) / (float)m_wnd->m_width;
			ci.m_position[3] = (cursorY + curCharInfo.m_height) / (float)m_wnd->m_height;

			for (int i = 0; i < 4; ++i)
			{
				ci.m_position[i] = 2 * ci.m_position[i] - 1;
			}

			cursorX += curCharInfo.m_width;
			++index;

			if (index >= m_overlayRP->GetMaxCharacters())
			{
				break;
			}
		}

		if (index >= m_overlayRP->GetMaxCharacters())
		{
			break;
		}
	}

	m_quadInstanceBuffer->GetUploadBuffer()->Unmap();

	m_quadInstanceBuffer->SetDirty();
}
