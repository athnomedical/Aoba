﻿#pragma once

#include "UIRect.hpp"

namespace s3d::aoba {
    class UIImageView : public UIRect {
    public:
        bool manualScalingEnabled = true;

    private:
        // Textures
        Array<DynamicTexture> m_textures;
        Array<double> m_alphas;
        RectF m_textureRegion = RectF();

        // Position
        Vec2 m_drawingCenterPos = Vec2();
        Point m_cursoredPixel = Point(), m_preCursoredPixel = Point();

        // Scaling
        const uint32 m_minPixel = 50;
        uint32 m_maxPixel       = 0;
        double m_scale = 1.0, m_minScale = 1.0, m_maxScale = 1.0, m_scaleRate = 0.0;

        // Rotation
        RectF m_rotatedTextureRegion  = RectF();
        Vec2 m_baseRotatedTextureSize = Vec2(), m_rotatedTextureSize = Vec2();
        double m_angle = 0.0;

    public:
        using UIRect::UIRect;

        const Point& currentPixel() const noexcept {
            return m_cursoredPixel;
        }

        const Point& prePixel() const noexcept {
            return m_preCursoredPixel;
        }

        const RectF& textureRegion() const noexcept {
            return m_textureRegion;
        }

        const RectF& rotatedTextureRegion() const noexcept {
            return m_rotatedTextureRegion;
        }

        double scale() const noexcept {
            return m_scale;
        }

        double minimumScale() const noexcept {
            return m_minScale;
        }

        double maximumScale() const noexcept {
            return m_maxScale;
        }

        double scaleRate() const noexcept {
            return m_scaleRate;
        }

        /// <returns>degrees</returns>
        double angle() const noexcept {
            return m_angle * 180.0 / Math::Pi;
        }

        size_t texturesCount() const noexcept {
            return m_textures.size();
        }

        RectF visibleTextureRect() const noexcept {
            return RectF(m_rotatedTextureRegion.x < layer().left() ? layer().left() : m_rotatedTextureRegion.x,
                         m_rotatedTextureRegion.y < layer().top() ? layer().top() : m_rotatedTextureRegion.y,
                         m_rotatedTextureRegion.w < layer().width() ? m_rotatedTextureRegion.w : layer().width(),
                         m_rotatedTextureRegion.h < layer().height() ? m_rotatedTextureRegion.h : layer().height());
        }

        void release() override;

        void releaseImages();

        void rotate(double degrees);

        void updateTexture(size_t index, const Image& image) {
            m_textures[index].fill(image);
        }

        void updateTextureIfNotBusy(size_t index, const Image& image) {
            m_textures[index].fillIfNotBusy(image);
        }

        void updateTexture(size_t index, const Image& image, const Rect& rect) {
            const auto x = Clamp(rect.x, 0, image.width());
            const auto y = Clamp(rect.y, 0, image.height());
            const auto w = Clamp(rect.w, 0, image.width() - x);
            const auto h = Clamp(rect.h, 0, image.height() - y);
            m_textures[index].fillRegion(image, Rect{x, y, w, h});
        }

        void updateTextureIfNotBusy(size_t index, const Image& image, const Rect& rect) {
            const auto x = Clamp(rect.x, 0, image.width());
            const auto y = Clamp(rect.y, 0, image.height());
            const auto w = Clamp(rect.w, 0, image.width() - x);
            const auto h = Clamp(rect.h, 0, image.height() - y);
            m_textures[index].fillRegionIfNotBusy(image, Rect{x, y, w, h});
        }

        void setAlphaRate(size_t index, double rate) {
            m_alphas[index] = 255 * rate;
        }

        /// <param name="rate">From 0.0 to 1.0</param>
        void setScale(double rate);

        void resetScale();

        void setViewingCenterPixel(const Point& centerPixel);

        void appendImage(const Image& image, double alphaRate = 1.0);

        void removeImage(size_t index);

    protected:
        void initialize() override;

        void draw() const override;

        void updateLayer(const Rect& scissor) override;

    private:
        double calcMinimumScale();

        double calcMaximumScale() noexcept;

        void restrictImageMovement(bool safeRerecursion = false);

        void updateTextureRegion(bool safeRerecursion = false);

        void setDrawingCenterPos(const Vec2& pos);
    };
}
