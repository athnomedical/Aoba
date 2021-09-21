#include <GUIKit/UISlider.hpp>
#include <GUIKit/PixelUnit.hpp>

GUICreateInputEvent(Sliding);

namespace s3d::gui {
	void UISlider::initialize() {
		const double handleRadius = 6_px;

		addEventListener<Sliding>([this, handleRadius] {
			const double pre = m_value;
			m_value = Clamp(m_min + (m_max - m_min) * (Cursor::Pos().x - (layer().left + handleRadius)) / (layer().width - handleRadius * 2), m_min, m_max);

			if (pre != m_value) {
				requestToUpdateLayer();
				if (m_valueChangedHandler) {
					m_valueChangedHandler(m_value);
				}
			}
			}, true);

		addEventListener<MouseEvent::Hovered>([this] {
			m_uiHandle.backgroundColor.highlight(DynamicColor::DefaultBlue);
			}, true);

		addEventListener<MouseEvent::UnHovered>([this] {
			m_uiHandle.backgroundColor.lowlight(DynamicColor::Background);
			}, true);

		addEventListener<MouseEvent::Hovering>([] {
			Cursor::RequestStyle(CursorStyle::Hand);
			}, true);

		const double h = 3.0_px;
		m_uiRailLeft.drawFrame = true;
		m_uiRailLeft.backgroundColor = DynamicColor::DefaultBlue;
		m_uiRailLeft.penetrateMouseEvent = true;
		m_uiRailLeft.setConstraint(LayerDirection::CenterY, m_uiHandle, LayerDirection::CenterY);
		m_uiRailLeft.setConstraint(LayerDirection::Height, h);
		m_uiRailLeft.setConstraint(LayerDirection::Left, *this, LayerDirection::Left, handleRadius);
		m_uiRailLeft.setConstraint(LayerDirection::Right, m_uiHandle, LayerDirection::CenterX);

		m_uiRailRight.drawFrame = true;
		m_uiRailRight.backgroundColor = DynamicColor::BackgroundSecondary;
		m_uiRailRight.penetrateMouseEvent = true;
		m_uiRailRight.setConstraint(LayerDirection::CenterY, m_uiHandle, LayerDirection::CenterY);
		m_uiRailRight.setConstraint(LayerDirection::Height, h);
		m_uiRailRight.setConstraint(LayerDirection::Left, m_uiHandle, LayerDirection::CenterX);
		m_uiRailRight.setConstraint(LayerDirection::Right, *this, LayerDirection::Right, -handleRadius);

		m_uiHandle.drawFrame = true;
		m_uiHandle.backgroundColor = DynamicColor::Background;
		m_uiHandle.penetrateMouseEvent = true;
		m_uiHandle.setConstraint(LayerDirection::Top, *this, LayerDirection::CenterY);
		m_uiHandle.setConstraint(LayerDirection::Height, handleRadius * 2);
		m_uiHandle.setConstraint(LayerDirection::CenterX, [this, handleRadius] {
			return layer().left + handleRadius + (layer().width - handleRadius * 2) * (m_value - m_min) / (m_max - m_min);
			});
		m_uiHandle.setConstraint(LayerDirection::Width, handleRadius * 2);

		uiText.penetrateMouseEvent = true;
		uiText.setConstraint(LayerDirection::Top, *this, LayerDirection::Top);
		uiText.setConstraint(LayerDirection::Bottom, m_uiHandle, LayerDirection::Top);
		uiText.setConstraint(LayerDirection::Left, *this, LayerDirection::Left);
		uiText.setConstraint(LayerDirection::Right, *this, LayerDirection::Right);

		UIView::initialize();
	}

	void UISlider::updateInputEvents() {
		if (mouseCondition().left.down) {
			m_sliding = true;
		}

		if (m_sliding) {
			if (MouseL.up()) {
				m_sliding = false;
			}
			else {
				registerInputEvent(Sliding(this, false));
			}
		}

		UIView::updateInputEvents();
	}
}
