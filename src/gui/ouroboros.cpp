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

#include "sfutil/center.hpp"
#include "sfutil/display.hpp"
#include "sfutil/fitting.hpp"
#include "sfutil/primitives.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

namespace heroespath
{
namespace gui
{

    Ouroboros::Ouroboros(const std::string & NAME, const bool WILL_MAKE_BLACK)
        : Entity(std::string(NAME).append("_Ouroboros"), sf::FloatRect())
        , IMAGE_DIMMENSION_INITIAL_(sfutil::ScreenRatioToPixelsHoriz(0.4f))
        , IMAGE_DIMMENSION_MIN_(IMAGE_DIMMENSION_INITIAL_ * 0.99f)
        , IMAGE_DIMMENSION_MAX_(IMAGE_DIMMENSION_INITIAL_ * 1.1f)
        , cachedTexture_(
              "media-image-misc-ouroboros",
              ImageOpt::Default | ((WILL_MAKE_BLACK) ? ImageOpt::None : ImageOpt::Invert))
        , sprite_(cachedTexture_.Get())
        , sizeDrifter_(
              IMAGE_DIMMENSION_MIN_,
              IMAGE_DIMMENSION_MAX_,
              0.1f,
              0.35f,
              IMAGE_DIMMENSION_INITIAL_,
              IMAGE_DIMMENSION_MAX_)
        , alphaDrifter_(5.0f, 25.0f, 0.1f, 0.75f)
        , rotationSpeedDrifter_(1.0f, 10.0f, 0.25f, 0.75f)
    {
        sprite_.setOrigin(sfutil::Size(sprite_.getLocalBounds()) * 0.5f);
        sprite_.setPosition(sfutil::Center());
    }

    Ouroboros::~Ouroboros() = default;

    void Ouroboros::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        target.draw(sprite_, states);
    }

    bool Ouroboros::UpdateTime(const float ELAPSED_TIME_SEC)
    {
        sprite_.setColor(sf::Color(
            255, 255, 255, static_cast<sf::Uint8>(alphaDrifter_.Update(ELAPSED_TIME_SEC))));

        sprite_.rotate(-1.0f * ELAPSED_TIME_SEC * rotationSpeedDrifter_.Update(ELAPSED_TIME_SEC));

        const auto NEW_IMAGE_DIMMENSION { sizeDrifter_.Update(ELAPSED_TIME_SEC) };
        sfutil::Fit(sprite_, sf::Vector2f(NEW_IMAGE_DIMMENSION, NEW_IMAGE_DIMMENSION));

        return true;
    }

    void Ouroboros::MoveEntityPos(const float HORIZ, const float VERT)
    {
        Entity::MoveEntityPos(HORIZ, VERT);
        sprite_.move(HORIZ, VERT);
    }

} // namespace gui
} // namespace heroespath
