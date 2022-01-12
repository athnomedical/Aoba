﻿#include <Aoba/Core.hpp>
#include <Aoba/Page.hpp>
#include <Aoba/WindowManager.hpp>
#include <Aoba/UnifiedFont.hpp>
#include <Aoba/Factory.hpp>

#include "PageManager/PageManager.hpp"
#include "ParallelTaskManager/ParallelTaskManager.hpp"

#include <thread>

namespace s3d::aoba {
	Core::Core() {
		m_pageManager = new PageManager();
		m_parallelTaskManager = new ParallelTaskManager();

		WindowManager::Initialize();

		UnifiedFont::Initialize();

		Window::SetTitle(U"Aoba Framework");

		Scene::SetResizeMode(ResizeMode::Actual);

		System::SetTerminationTriggers(UserAction::NoAction);

		Core::AddLicense();
	}

	Core::~Core() {
		delete m_pageManager;
		delete m_parallelTaskManager;
	}

	bool Core::IsParallelTaskAlive() {
		return Instance().m_parallelTaskManager->isAlive();
	}

	void Core::SwitchPage(const String& identifier) {
		Instance().m_pageManager->switchPage(identifier);
	}

	void Core::SetColorMode(ColorMode mode) {
		Instance().m_animateColor = true;
		ColorTheme::SetColorMode(mode);
	}

	void Core::ToggleColorMode() {
		Instance().SetColorMode(ColorTheme::CurrentColorMode() == ColorMode::Light ? ColorMode::Dark : ColorMode::Light);
	}

	bool Core::animateColor() {
		static double t = 0.0;
		t += 5.0 * Scene::DeltaTime();

		if (t > 1.0) {
			ColorTheme::Animate(ColorTheme::CurrentColorMode() == ColorMode::Light ? 0.0 : 1.0);
			t = 0.0;
			return false;
		}

		ColorTheme::Animate(ColorTheme::CurrentColorMode() == ColorMode::Light ? 1 - t : t);
		return true;
	}

	void Core::InsertProcessToMainThread(const std::function<void()>& func) {
		std::lock_guard<std::mutex> lock(Instance().m_mainThreadInserterMutex);
		Instance().m_eventsRequestedToRunInMainThread.push_back(func);
	}

	void Core::CreateParallelTask(const std::function<void()>& func, const std::function<void()>& completion) {
		if (completion) {
			Instance().m_parallelTaskManager->createTask(func, [completion] {
				Instance().InsertProcessToMainThread(completion);
				});
		}
		else {
			Instance().m_parallelTaskManager->createTask(func);
		}
	}

	size_t Core::SetTimeout(const std::function<void()>& func, double ms, bool threading) {
		Instance().m_timeouts.push_back(Timeout(func, ms, threading));
		return Instance().m_timeouts[Instance().m_timeouts.size() - 1].id();
	}

	bool Core::StopTimeout(size_t id) {
		for (auto& timeout : Instance().m_timeouts) {
			if (timeout.id() == id) {
				return timeout.stop();
			}
		}
		return false;
	}

	bool Core::RestartTimeout(size_t id) {
		for (auto& timeout : Instance().m_timeouts) {
			if (timeout.id() == id) {
				return timeout.restart();
			}
		}
		return false;
	}

	bool Core::IsTimeoutAlive(size_t id) {
		for (auto& timeout : Instance().m_timeouts) {
			if (timeout.id() == id) {
				return timeout.isAlive();
			}
		}
		return false;
	}

	Page& Core::getPage(const String& identifier) const noexcept {
		return m_pageManager->getPage(identifier);
	}

	void Core::appendPage(const std::shared_ptr<Page>& page) {
		m_pageManager->appendPage(page);
	}

	void Core::AppendIsolatedComponent(const UIComponent& component) {
		Instance().appendIsolatedComponent(Factory::GetComponent(component.id()));
	}

	void Core::appendIsolatedComponent(const std::shared_ptr<UIComponent>& component) {
		m_pageManager->appendIsolatedComponent(component);
	}

	void Core::AddLicense() {
		LicenseInfo licence;
		licence.title = U"Aoba Framework";
		licence.copyright = U"Copyright (c) 2021 Ekyu Kondo";
		licence.text =
UR"(Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.)";

		LicenseManager::AddLicense(licence);
	}
}