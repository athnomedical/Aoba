#include "Aoba/UIImageView.hpp"

#include "Aoba/Imaging.hpp"

namespace s3d::aoba {
    void UIImageView::initialize() {
        UIRect::initialize();

        addEventListener<Event::Mouse::RightDragging>(
            [this] {
                const auto movement = Cursor::Pos() - Cursor::PreviousPos();
                setDrawingCenterPos(m_drawingCenterPos.movedBy(movement));
            },
            true);

        addEventListener<Event::Mouse::Hovering>(
            [this](const Event::Mouse::Hovering& e) {
                if (m_textures) {
                    m_cursoredPixel   = Imaging::ScenePosToPixel(e.pos, m_textureRegion, m_scale, angle());
                    m_cursoredPixel.x = Clamp(m_cursoredPixel.x, 0, m_textures[0].width() - 1);
                    m_cursoredPixel.y = Clamp(m_cursoredPixel.y, 0, m_textures[0].height() - 1);

                    m_preCursoredPixel   = Imaging::ScenePosToPixel(e.previousPos, m_textureRegion, m_scale, angle());
                    m_preCursoredPixel.x = Clamp(m_preCursoredPixel.x, 0, m_textures[0].width() - 1);
                    m_preCursoredPixel.y = Clamp(m_preCursoredPixel.y, 0, m_textures[0].height() - 1);
                }
            },
            true);

        addEventListener<Event::Mouse::Wheel>(
            [this](const Event::Mouse::Wheel& e) {
                if (manualScalingEnabled) {
                    setScale(m_scaleRate + (Sign(e.wheel) < 0 ? 0.05 : -0.05));
                }
            },
            true);
    }

    void UIImageView::appendImage(const Image& image, double alphaRate) {
        m_textures.emplace_back(image, TextureDesc::Unmipped);
        m_alphas.emplace_back(255 * alphaRate);

        setDrawingCenterPos(layer().center());

        requestToUpdateLayer();

        m_maxPixel = Max(image.width(), image.height());
        m_maxPixel = Clamp(m_maxPixel, m_maxPixel, m_maxPixel);
    }

    void UIImageView::removeImage(size_t index) {
        m_textures.remove_at(index);
        m_alphas.remove_at(index);
    }

    void UIImageView::release() {
        releaseImages();
        UIRect::release();
    }

    void UIImageView::releaseImages() {
        m_textures.release();
        m_alphas.release();
        setDrawingCenterPos(layer().center());
    }

    void UIImageView::draw() const {
        UIRect::draw();

        const auto scissor = Graphics2D::GetScissorRect();
        Graphics2D::SetScissorRect(layer().asRect());
        for (size_t i : step(m_textures.size())) {
            m_textures[i].scaled(m_scale).rotated(m_angle).drawAt(
                m_drawingCenterPos, Color(255, 255, 255, static_cast<uint8>(m_alphas[i])));
        }
        Graphics2D::SetScissorRect(scissor);
    }

    void UIImageView::updateLayer(const Rect& scissor) {
        UIRect::updateLayer(scissor);

        const double preMinScale = m_minScale;
        const double preScale    = m_scale;

        if (m_textures) {
            m_minScale = calcMinimumScale();
            m_maxScale = calcMaximumScale();

            m_scale = preScale * m_minScale / preMinScale;

            m_scale = Clamp(m_scale, m_minScale, m_maxScale);

            // Need to update twice
            updateTextureRegion();
            updateTextureRegion();
        }
    }

    double UIImageView::calcMinimumScale() {
        m_baseRotatedTextureSize = Imaging::GetSizeFitsTexture(m_textures[0].size(), angle());

        double scale = layer().width() / static_cast<double>(m_baseRotatedTextureSize.x);
        if (const double h = scale * m_baseRotatedTextureSize.y; h > layer().height()) {
            scale *= layer().height() / h;
        }

        return scale;
    }

    double UIImageView::calcMaximumScale() noexcept {
        return m_minScale * m_maxPixel / m_minPixel;
    }

    void UIImageView::setScale(double rate) {
        m_scaleRate = Clamp(rate, 0.0, 1.0);

        const double preScale = m_scale;

        m_scale = m_minScale * m_maxPixel / (m_maxPixel - (m_maxPixel - m_minPixel) * m_scaleRate);

        if (m_scale != preScale) {
            const auto diff = (layer().center() - m_drawingCenterPos) * (1.0 - m_scale / preScale);
            setDrawingCenterPos(m_drawingCenterPos.movedBy(diff));
        }
    }

    void UIImageView::resetScale() {
        m_minScale = calcMinimumScale();
        m_maxScale = calcMaximumScale();

        setScale(0.0);
    }

    void UIImageView::rotate(double degrees) {
        m_angle = degrees * Math::Pi / 180.0;

        m_minScale = calcMinimumScale();
        m_maxScale = calcMaximumScale();

        m_scale = Clamp(m_scale, m_minScale, m_maxScale);

        updateTextureRegion();
    }

    void UIImageView::restrictImageMovement(bool safeRerecursion) {
        const auto center = layer().center();

        bool updated = false;

        // Correct the overhang
        if (m_rotatedTextureRegion.x > layer().left()) {
            m_drawingCenterPos.x = layer().left() + m_rotatedTextureRegion.w * 0.5;
            updated              = true;
        } else if (m_rotatedTextureRegion.x + m_rotatedTextureRegion.w < layer().right()) {
            m_drawingCenterPos.x = layer().right() - m_rotatedTextureRegion.w * 0.5;
            updated              = true;
        }

        // Centering
        if (m_rotatedTextureRegion.w <= layer().width()) {
            m_drawingCenterPos.x = center.x;
        }

        // Correct the overhang
        if (m_rotatedTextureRegion.y > layer().top()) {
            m_drawingCenterPos.y = layer().top() + m_rotatedTextureRegion.h * 0.5;
            updated              = true;
        } else if (m_rotatedTextureRegion.y + m_rotatedTextureRegion.h < layer().bottom()) {
            m_drawingCenterPos.y = layer().bottom() - m_rotatedTextureRegion.h * 0.5;
            updated              = true;
        }

        // Centering
        if (m_rotatedTextureRegion.h <= layer().height()) {
            m_drawingCenterPos.y = center.y;
        }

        if (!safeRerecursion && updated) {
            updateTextureRegion(true);
        }
    }

    void UIImageView::setViewingCenterPixel(const Point& centerPixel) {
        // Current scene position of the pixel that will be centered
        const auto pos      = Imaging::PixelToScenePos(centerPixel, m_textureRegion, m_scale, angle());
        const auto movement = layer().center() - pos;
        setDrawingCenterPos(m_drawingCenterPos.movedBy(movement));
    }

    void UIImageView::updateTextureRegion(bool safeRerecursion) {
        if (m_textures) {
            m_textureRegion        = m_textures[0].scaled(m_scale).regionAt(m_drawingCenterPos);
            m_rotatedTextureRegion = m_textureRegion;
            m_rotatedTextureSize   = m_baseRotatedTextureSize * m_scale;

            const auto center = m_rotatedTextureRegion.center();
            m_rotatedTextureRegion.setSize(m_rotatedTextureSize);
            m_rotatedTextureRegion.setCenter(center);

            restrictImageMovement(safeRerecursion);
        }
    }

    void UIImageView::setDrawingCenterPos(const Vec2& pos) {
        m_drawingCenterPos = pos;
        updateTextureRegion();
    }
}
