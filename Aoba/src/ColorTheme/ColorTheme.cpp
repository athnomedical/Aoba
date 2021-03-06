#include "Aoba/ColorTheme.hpp"

namespace s3d::aoba {
    ColorMode CurrentMode;
    double T = 0.0;

    Color ColorTheme::color() const noexcept {
        if (m_isTransition) {
            m_transitionTimer += static_cast<uint64>(Scene::DeltaTime() * 1000);
            const double k = m_transitionTime == 0 ? 1.0 : static_cast<double>(m_transitionTimer) / static_cast<double>(m_transitionTime);
            if (k >= 1.0) {
                light          = m_transitionLight;
                dark           = m_transitionDark;
                m_isTransition = false;
                return CurrentMode == ColorMode::Light ? light : dark;
            }

            return (CurrentMode == ColorMode::Light ? light : dark)
                .lerp(CurrentMode == ColorMode::Light ? m_transitionLight : m_transitionDark, k);
        } else {
            return light.lerp(dark, T);
        }
    }

    void ColorTheme::setColor(const Color& lightColor, const Color& darkColor, uint64 transitionTimeMs) noexcept {
        if (m_isTransition) {
            const auto c = color();
            light        = c;
            dark         = c;
        }

        if (transitionTimeMs == 0) {
            light = lightColor;
            dark  = darkColor;
        }

        if (light.a == 0) {
            light.setRGB(lightColor.r, lightColor.g, lightColor.b);
        }

        if (dark.a == 0) {
            dark.setRGB(darkColor.r, darkColor.g, darkColor.b);
        }

        m_transitionLight = lightColor.a == 0 ? Color(light, 0) : lightColor;
        m_transitionDark  = darkColor.a == 0 ? Color(dark, 0) : darkColor;

        m_transitionTime  = transitionTimeMs;
        m_transitionTimer = 0;
        m_isTransition    = true;
    }

    ColorMode ColorTheme::CurrentColorMode() noexcept {
        return CurrentMode;
    }

    void ColorTheme::SetColorMode(ColorMode mode) noexcept {
        CurrentMode = mode;
    }

    void ColorTheme::Animate(double t) noexcept {
        T = Clamp(t, 0.0, 1.0);
    }
}
