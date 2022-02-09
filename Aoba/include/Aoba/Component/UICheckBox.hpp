﻿#pragma once

#include "Aoba/Component/UIButton.hpp"
#include "Aoba/Component/UIText.hpp"
#include "Aoba/PixelUnit.hpp"

namespace s3d::aoba {
    class UICheckBox : public UIButton {
    private:
        const Texture m_checkdIcon = Texture(Icon(0xf00c), 13_px);

        bool m_checked = false;

    public:
        using UIButton::UIButton;

        bool isChecked() const {
            return m_checked;
        }

        void setChecked(bool checked);

    protected:
        void initialize() override;
    };
}