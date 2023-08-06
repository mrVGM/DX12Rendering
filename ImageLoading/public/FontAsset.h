#pragma once

#include "BaseObject.h"

#include <map>
#include <string>

namespace rendering
{
	class DXTexture;
}

namespace rendering::image_loading
{
	struct CharInfo
	{
		int m_id = -1;
		std::string m_text;
		int m_xOffset = 0;
		int m_yOffset = 0;
		int m_width = 0;
		int m_height = 0;
		int m_basePositionX;
		int m_basePositionY;
	};

	struct FontInfo
	{
		std::map<int, CharInfo> m_charInfo;
	};

	class FontAsset : public BaseObject
	{
		DXTexture* m_texture;
		FontInfo m_fontInfo;

	public:
		FontAsset(const BaseObjectMeta& meta, DXTexture* texture, const std::string& fontInfo);
		virtual ~FontAsset();

		DXTexture* GetFontTexture();
		const FontInfo& GetFontInfo();
	};
}