﻿#pragma once

#include "UIView.hpp"
#include "UIButton.hpp"
#include "GUIFactory.hpp"

#include <Siv3D.hpp>

namespace s3d::gui {
	// Called when tab switched or tab view loaded.
	GUICreateInputEvent(TabSwitched);

	class UITabView : public UIView {
		struct Tab {
			size_t index;
			UIView& view;
			UIButton& selector;

			bool enabled = true;

			void show() {
				selector.setFont(UnifiedFontStyle::DefaultBold);
				view.hidden = false;
			}

			void hide() {
				selector.setFont(UnifiedFontStyle::DefaultLight);
				view.hidden = true;
			}
		};

	private:
		UIView& ui_tabView = GUIFactory::Create<UIView>(*this);
		UIView& ui_tabSelectorView = GUIFactory::Create<UIView>(*this);

		Array<Tab> m_tabs;
		size_t m_tabIndex = 0;

	public:
		explicit UITabView() noexcept :
			UIView()
		{}

		virtual ~UITabView() {}

		size_t index() const {
			return m_tabIndex;
		}

		template<class T>
		T& appendTab(const String& name) {
			static_assert(std::is_base_of<UIView, T>::value, "Specified Type does not inherit gui::UIView.");

			auto& view = GUIFactory::Create<T>(ui_tabView);
			auto& selector = GUIFactory::Create<UIButton>(ui_tabSelectorView);

			initializeTab(name, selector, view);

			return view;
		}

		void setTab(size_t index);

		void setTabEnabled(size_t index, bool enabled);

	protected:
		void initialize() override;

	private:
		void initializeTab(const String& name, UIButton& selector, UIView& view);
	};
}
