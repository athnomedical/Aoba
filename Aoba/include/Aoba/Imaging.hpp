#pragma once

#include <Siv3D.hpp>

namespace s3d::aoba::Imaging {
    Point ScenePosToPixel(const Vec2& pos, const RectF& textureRegion, double scale, double degrees);

    Vec2 PixelToScenePos(const Point& pixel, const RectF& textureRegion, double scale, double degrees);

    Size GetSizeFitsTexture(const Size& textureSize, double degrees);
}
