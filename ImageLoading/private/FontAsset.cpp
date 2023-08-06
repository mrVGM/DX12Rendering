#include "FontAsset.h"

#include "FontAssetMeta.h"
#include "FontAssetLoadedNotificationMeta.h"

#include "DXTexture.h"

#include "Notifications.h"

#include "json.hpp"

#include <sstream>

#define THROW_ERROR(hRes, error) \
if (FAILED(hRes)) {\
    throw error;\
}

namespace
{
	void CacheObjects()
	{
		using namespace rendering;
	}
}

rendering::image_loading::FontAsset::FontAsset(const BaseObjectMeta& meta, DXTexture* texture, const std::string& fontInfo) :
	BaseObject(meta),
	m_texture(texture)
{
	CacheObjects();

	nlohmann::json json = nlohmann::json::parse(fontInfo);

	int texWidth = m_texture->GetTextureDescription().Width;
	int texHeight = m_texture->GetTextureDescription().Height;

	for (auto it = json.begin(); it != json.end(); ++it)
	{
		int id;
		const std::string idStr = it.key();

		std::stringstream ss;
		ss << idStr;
		ss >> id;
		ss.clear();

		nlohmann::json& cur = *it;

		const std::string& text = cur["text"];
		int x = cur["x"];
		int y = texHeight - cur["y"] - 1;
		int width = cur["width"];
		int height = cur["height"];
		int baseline = cur["baseline"];

		CharInfo ci;
		ci.m_id = id;
		ci.m_text = text;
		ci.m_basePositionY = baseline;
		ci.m_basePositionX = x;
		ci.m_xOffset = 0;
		ci.m_yOffset = y - height - baseline;
		ci.m_width = width;
		ci.m_height = height;

		m_fontInfo.m_charInfo[id] = ci;
	}
	notifications::Notify(FontAssetLoadedNotificationMeta::GetInstance());
}

rendering::image_loading::FontAsset::~FontAsset()
{
}

rendering::DXTexture* rendering::image_loading::FontAsset::GetFontTexture()
{
	return m_texture;
}

const rendering::image_loading::FontInfo& rendering::image_loading::FontAsset::GetFontInfo()
{
	return m_fontInfo;
}

#undef THROW_ERROR