// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// ouroboros.cpp
//
#include "ouroboros.hpp"

#include "misc/log-macros.hpp"
#include "sfml-util/display.hpp"
#include "sfutil/center.hpp"
#include "sfutil/display.hpp"
#include "sfutil/fitting.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace heroespath
{
namespace sfml_util
{

    Ouroboros::Ouroboros(const std::string & NAME, const bool WILL_MAKE_BLACK)
        : Entity(std::string(NAME).append("_Ouroboros"), sf::FloatRect())
        , IMAGE_INITIAL_WIDTH_(sfutil::ScreenRatioToPixelsHoriz(0.34f))
        , IMAGE_DRIFT_WIDTH_(sfutil::ScreenRatioToPixelsHoriz(0.12f))
        , IMAGE_MIN_DRIFT_WIDTH_(IMAGE_INITIAL_WIDTH_ - (IMAGE_DRIFT_WIDTH_ * 0.5f))
        , IMAGE_MAX_DRIFT_WIDTH_(IMAGE_INITIAL_WIDTH_ + (IMAGE_DRIFT_WIDTH_ * 0.5f))
        , rotation_(0.0f)
        , cachedTexture_(
              "media-images-gui-accents-ouroboros",
              ImageOpt::Default | ((WILL_MAKE_BLACK) ? ImageOpt::None : ImageOpt::Invert))
        , sprite_(cachedTexture_.Get())
        , sizeDrifter_(
              IMAGE_MIN_DRIFT_WIDTH_,
              IMAGE_MAX_DRIFT_WIDTH_,
              0.1,
              0.35,
              IMAGE_INITIAL_WIDTH_,
              IMAGE_MAX_DRIFT_WIDTH_)
        , shadeDrifter_(5.0f, 25.0f, 0.1, 0.75)
        , rotSpeedDrifter_(1.0f, 10.0f, 0.25, 0.75)
    {
        const sf::Vector2f IMAGE_INITIAL_CONSTRAINING_SIZE_V(IMAGE_INITIAL_WIDTH_, 0.0f);
        sfutil::Fit(sprite_, IMAGE_INITIAL_CONSTRAINING_SIZE_V);
        sfutil::Center(sprite_);
        sprite_.setColor(sf::Color(255, 255, 255, 20));
    }

    Ouroboros::~Ouroboros() = default;

    void Ouroboros::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        target.draw(sprite_, states);
    }

    bool Ouroboros::UpdateTime(const float ELAPSED_TIME_SEC)
    {
        // of all the other transforms set the rotation first while still positioned at the origin
        // and orig scale
        sprite_.setPosition(0.0f, 0.0f);
        sprite_.setScale(1.0f, 1.0f);

        sprite_.setOrigin(
            sprite_.getLocalBounds().width * 0.5f, sprite_.getLocalBounds().height * 0.5f);

        rotation_ += -1.0f * ELAPSED_TIME_SEC * rotSpeedDrifter_.Update(ELAPSED_TIME_SEC);

        sprite_.setRotation(rotation_);

        sfutil::Fit(sprite_, sf::Vector2f(sizeDrifter_.Update(ELAPSED_TIME_SEC), 0.0f));
        sfutil::Center(sprite_);

        sprite_.setColor(sf::Color(
            255, 255, 255, static_cast<sf::Uint8>(shadeDrifter_.Update(ELAPSED_TIME_SEC))));

        return true;
    }

} // namespace sfml_util
} // namespace heroespath
