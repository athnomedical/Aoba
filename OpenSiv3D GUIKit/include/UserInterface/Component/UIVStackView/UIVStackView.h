#pragma once

#include "../UIView/UIView.h"

namespace s3d::gui {
	enum class LeadingDirection {
		Top,
		Bottom
	};

	class UIVStackView : public UIView {
	public:
		bool scrollingEnabled = true;

	private:
		Array<UIComponent*> m_deletableComponents;
		size_t m_maxStackCount = 0;
		double m_rowHeight = 0.0;
		double m_currentRowHeight = 0.0, m_currentRowsHeight = 0.0;
		double m_topPositionConstant = 0.0;
		bool m_constraintsApplied = false;
		LeadingDirection m_leadingDirection = LeadingDirection::Top;

	public:
		using UIView::UIView;

		~UIVStackView();

		virtual void release();

		void appendComponent(UIComponent& component) override {
			m_userInterfaces.push_back(&component);
			m_constraintsApplied = false;
			requestToUpdateLayer();
		}

		template<class T>
		void appendTemporaryComponent(const T& component) {
			T* cmp = new T(component);
			UIView::appendComponent(*cmp);
			m_deletableComponents.push_back(cmp);
			m_constraintsApplied = false;
			requestToUpdateLayer();
		}

		void releaseDeletableComponents();

		void setMaxStackCount(size_t count) {
			m_maxStackCount = count;
		}

		void setRowHeight(double h) {
			m_rowHeight = h;
		}

		void setLeadingDirection(LeadingDirection direction) {
			m_leadingDirection = direction;
		}

		Array<UIComponent*> components() const {
			return m_userInterfaces;
		}

	protected:
		void updateLayer() override;

		bool mouseWheel() override;

	private:
		void setChildConstraints(size_t index);

		void calcCurrentRowHeight();

		void adjustRowsBottomToViewBottom();
	};
}