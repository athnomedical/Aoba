﻿#pragma once

#include "UnifiedFont.hpp"
#include "UIRect.hpp"

namespace s3d::gui {
	enum class TextDirection {
		LeftTop,
		LeftCenter,
		LeftBottom,
		CenterTop,
		Center,
		CenterBottom,
		RightTop,
		RightCenter,
		RightBottom
	};

	class UIText : public UIRect {
	public:
		ColorTheme textColor = DynamicColor::Text;

	private:
		String m_text = U"";
		Font m_font = UnifiedFont::Get();
		TextDirection m_direction = TextDirection::LeftCenter;
		RectF m_textRegion = RectF();
		double paddingTop = 0.0, paddingBottom = 0.0, paddingLeft = 0.0, paddingRight = 0.0;

	public:
		explicit UIText(const ColorTheme& backgroundColor = DynamicColor::Clear, const ColorTheme& _textColor = DynamicColor::Text) noexcept :
			UIRect(backgroundColor)
		{
			textColor = _textColor;
		}

		const Font& font() const {
			return m_font;
		}

		const String& text() const {
			return m_text;
		}
		const RectF& textRegion() const {
			return m_textRegion;
		}

		void setPadding(double top, double bottom, double left, double right);

		void setFont(UnifiedFontStyle style);

		void setText(const String& text);

		void setDirection(TextDirection direction);

		void setFont(const Font& font) {
			m_font = font;
		}

	protected:
		void updateLayer(const Rect& scissor) override;

		void draw() override;

		virtual void updateDrawableText(bool updateField = false);

	private:
		void updateTextRegion();

		void fitTextRegionToRect();
	};
}
