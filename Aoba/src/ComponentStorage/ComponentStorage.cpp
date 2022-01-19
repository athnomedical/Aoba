#include "ComponentStorage.hpp"

namespace s3d::aoba {
	ComponentStorage& ComponentStorage::Instance() {
		static ComponentStorage instance;
		return instance;
	}

	void ComponentStorage::Store(const std::shared_ptr<UIComponent>& component) {
		component->validate();
		Instance().releaseComponentsIfNeed();
		Instance().m_components.push_back(component);
	}

	void ComponentStorage::Release(size_t id) {
		for (auto& component : Instance().m_components) {
			if (component && component->id() == id) {
#if SIV3D_BUILD(DEBUG)
				Logger << U"[Aoba](Destroy) " + Unicode::Widen(std::string(typeid(*component).name())) + U" " + ToString(component->id());
#endif
				component.reset();
				break;
			}
		}
	}

	std::shared_ptr<UIComponent>& ComponentStorage::Get(size_t id) {
		for (auto& component : Instance().m_components) {
			if (component && component->id() == id) {
				return component;
			}
		}

		throw Error{ U"A component with identifier \"{}\" not found."_fmt(id) };
	}

	void ComponentStorage::releaseUnusedComponents() {
		m_components.remove_if([](const std::shared_ptr<UIComponent>& component) {
			return !component;
			});
	}

	void ComponentStorage::releaseComponentsIfNeed() {
		if (m_releaseCounter++; m_releaseCounter == 100) {
			releaseUnusedComponents();

			if (m_components.capacity() != m_components.size()) {
				m_components.shrink_to_fit();
			}

			m_releaseCounter = 0;
		}
	}
}