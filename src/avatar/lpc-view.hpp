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
        explicit LPCView(const Avatar::Enum, const sf::Vector2f & CENTERED_MAP_POS_V);

        void Set(const Pose::Enum, const sfml_util::Direction::Enum);

        bool Update(const float TIME_ELAPSED);

        sfml_util::Direction::Enum Direction() const { return animation_.direction; }

        void MoveIfWalking(const float AMOUNT);

        Pose::Enum WhichPose() const { return animation_.pose; }

        const sf::Sprite & SpriteRef() const { return sprite_; }

        Avatar::Enum WhichAvatar() const { return whichAvatar_; }

        std::size_t TextureIndex() const { return textureIndex_; }

        const sf::Sprite DefaultPoseSprite() const;

        // see comment below on sf::Sprite member variable about these coordinates
        const sf::Vector2f GetCenteredMapPos() const { return sprite_.getPosition(); }

        void SetCenteredMapPos(const sf::Vector2f & NEW_POS_V) { sprite_.setPosition(NEW_POS_V); }

        static const sf::IntRect GetStandingRightFrameRect()
        {
            return FrameRect(FrameNumbers(Pose::Standing, sfml_util::Direction::Right).at(0));
        }

    private:
        static const FrameNumVec_t FrameNumbers(const Pose::Enum, const sfml_util::Direction::Enum);

        static const sf::IntRect FrameRect(const FrameNum_t FRAME_NUM);

        const Animation CreateAnimation(const Pose::Enum, const sfml_util::Direction::Enum) const;

        float FrameDuration(const Pose::Enum) const;

        void SetupSprite();

    private:
        static const int CELL_SIZE_;
        static const int CELL_COUNT_;
        static const float FRAME_DURATION_SEC_WALK_;
        static const float FRAME_DURATION_SEC_GIVETAKE_;
        static const float FRAME_DURATION_SEC_SINGLEFRAME_;
        static const float FRAME_DURATION_SEC_BLINK_MIN_;
        static const float FRAME_DURATION_SEC_BLINK_MAX_;

        Avatar::Enum whichAvatar_;
        std::size_t textureIndex_;

        // note that sprite_.getPosition() does not hold the actual top-left corner position of the
        // sprite, instead this holds the centered map coordinates position, see
        // MapDisplay::DrawCharacterImages() where the translation from centered map coordinates to
        // top-left offscreen coordinates occurs
        sf::Sprite sprite_;

        Animation animation_;
        float frameTimerSec_;
        std::size_t frameIndex_;
    };

} // namespace avatar
} // namespace heroespath

#endif // HEROESPATH_AVATAR_LPCVIEW_HPP_INCLUDED
