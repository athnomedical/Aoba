#pragma once

#include "UIComponent.h"

#include <Siv3D.hpp>

namespace s3d::gui {
	class UIRect : public UIComponent {
	protected:
		RoundRect m_rect;

	public:
		using UIComponent::UIComponent;

		RoundRect rect() const {
			return m_rect;
		}

		void setCornerRadius(double r);

	protected:
		void updateMouseIntersection() override;

		void draw() override;

		void updateLayer() override;
	};
}