#pragma once

#include "UIComponent.h"

#include <Siv3D.hpp>

namespace s3d::gui {
	class UIRect : public UIComponent {
	private:
		RoundRect m_rect;

	public:
		using UIComponent::UIComponent;

		const RoundRect& rect() const {
			return m_rect;
		}

		void setCornerRadius(double r);

	protected:
		void updateMouseIntersection() override;

		void updateLayer(const Rect& scissor) override;

		void draw() override;
	};
}
