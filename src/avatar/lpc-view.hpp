// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_AVATAR_LPCVIEW_HPP_INCLUDED
#define HEROESPATH_AVATAR_LPCVIEW_HPP_INCLUDED
//
// lpc-npc-view.hpp
//
#include "avatar/animation.hpp"
#include "avatar/avatar-enum.hpp"
#include "avatar/pose-enum.hpp"
#include "sfml-util/direction-enum.hpp"
#include "sfml-util/sfml-graphics.hpp"

#include <SFML/System/Vector2.hpp>

namespace heroespath
{
namespace avatar
{

    // Responsible for drawing an LPC-style NPC.
    class LPCView
    {
    public:
        explicit LPCView(const Avatar::Enum);

        void Set(const Pose::Enum, const sfml_util::Direction::Enum);

        bool Update(const float TIME_ELAPSED);

        void UpdatePos(const sf::Vector2f & V) { sprite_.setPosition(V); }

        sfml_util::Direction::Enum Direction() const { return animation_.direction; }

        Pose::Enum WhichPose() const { return animation_.pose; }

        const sf::Sprite & SpriteRef() const { return sprite_; }

        const sf::Vector2f SpriteSize() const
        {
            return sf::Vector2f(sprite_.getGlobalBounds().width, sprite_.getGlobalBounds().height);
        }

        Avatar::Enum WhichAvatar() const { return whichAvatar_; }

        std::size_t TextureIndex() const { return textureIndex_; }

    private:
        const FrameNumVec_t FrameNumbers(const Pose::Enum, const sfml_util::Direction::Enum) const;

        const sf::IntRect FrameRect(const FrameNum_t FRAME_NUM) const;

        const Animation CreateAnimation(const Pose::Enum, const sfml_util::Direction::Enum) const;

        float FrameDuration(const Pose::Enum) const;

        void SetupSprite();

    private:
        static const int CELL_SIZE_;
        static const float FRAME_DURATION_SEC_WALK_;
        static const float FRAME_DURATION_SEC_GIVETAKE_;
        static const float FRAME_DURATION_SEC_SINGLEFRAME_;
        static const float FRAME_DURATION_SEC_BLINK_MIN_;
        static const float FRAME_DURATION_SEC_BLINK_MAX_;

        Avatar::Enum whichAvatar_;
        std::size_t textureIndex_;
        int textureSize_;
        sf::Sprite sprite_;
        Animation animation_;
        float frameTimerSec_;
        std::size_t frameIndex_;
    };

} // namespace avatar
} // namespace heroespath

#endif // HEROESPATH_AVATAR_LPCVIEW_HPP_INCLUDED
