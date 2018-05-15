// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_AVATAR_IVIEW_HPP_INCLUDED
#define HEROESPATH_AVATAR_IVIEW_HPP_INCLUDED
//
// i-view.hpp
//
#include "avatar/avatar-enum.hpp"
#include "avatar/pose-enum.hpp"
#include "sfml-util/direction-enum.hpp"
#include <SFML/System/Vector2.hpp>
#include <memory>

namespace sf
{
class Sprite;
}

namespace heroespath
{
namespace avatar
{

    // Pure virtual interface for all NPC classes.
    struct IView
    {
        virtual ~IView() = default;

        virtual void Set(const Pose::Enum, const sfml_util::Direction::Enum) = 0;

        // returns true if the current animation finished
        virtual bool Update(const float TIME_ELAPSED) = 0;

        virtual void UpdatePos(const sf::Vector2f &) = 0;

        virtual sfml_util::Direction::Enum Direction() const = 0;

        virtual Pose::Enum WhichPose() const = 0;

        virtual const sf::Sprite & SpriteRef() const = 0;

        virtual const sf::Vector2f SpriteSize() const = 0;

        virtual Avatar::Enum WhichAvatar() const = 0;
    };

    using IViewUPtr_t = std::unique_ptr<IView>;
} // namespace avatar
} // namespace heroespath

#endif // HEROESPATH_AVATAR_IVIEW_HPP_INCLUDED
