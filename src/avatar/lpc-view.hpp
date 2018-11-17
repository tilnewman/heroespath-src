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
#include "gui/cached-texture.hpp"
#include "gui/direction-enum.hpp"

#include <SFML/Graphics/Sprite.hpp>

namespace heroespath
{
namespace avatar
{

    // Responsible for drawing an LPC-style NPC.
    class LPCView
    {
    public:
        explicit LPCView(const Avatar::Enum);

        LPCView(const LPCView &) = default;
        LPCView(LPCView &&) = default;
        LPCView & operator=(const LPCView &) = default;
        LPCView & operator=(LPCView &&) = default;

        gui::Direction::Enum Direction() const { return animation_.direction; }
        Pose::Enum WhichPose() const { return animation_.pose; }
        const sf::Sprite & CurrentSprite() const { return sprite_; }
        Avatar::Enum WhichAvatar() const { return whichAvatar_; }
        const sf::Sprite & DefaultPoseSprite() const { return defaultPoseSprite_; }

        void Set(const Pose::Enum, const gui::Direction::Enum);
        bool Update(const float TIME_ELAPSED);
        void SetCenteredPos(const sf::Vector2f & NEW_POS_V);
        static const sf::IntRect GetStandingRightFrameRect();

    private:
        static const FrameNumVec_t FrameNumbers(const Pose::Enum, const gui::Direction::Enum);
        static const sf::IntRect FrameRect(const FrameNum_t FRAME_NUM);
        const Animation CreateAnimation(const Pose::Enum, const gui::Direction::Enum) const;
        float FrameDuration(const Pose::Enum) const;
        void SetupSprite();
        void SetupDefaultPoseSprite();

    private:
        static const int CELL_SIZE_;
        static const int CELL_COUNT_;
        static const float FRAME_DURATION_SEC_WALK_;
        static const float FRAME_DURATION_SEC_GIVETAKE_;
        static const float FRAME_DURATION_SEC_SINGLEFRAME_;
        static const float FRAME_DURATION_SEC_BLINK_MIN_;
        static const float FRAME_DURATION_SEC_BLINK_MAX_;

        Avatar::Enum whichAvatar_;
        gui::CachedTexture cachedTexture_;

        sf::Sprite sprite_;
        sf::Sprite defaultPoseSprite_;

        Animation animation_;
        float frameTimerSec_;
        std::size_t frameIndex_;
    };

} // namespace avatar
} // namespace heroespath

#endif // HEROESPATH_AVATAR_LPCVIEW_HPP_INCLUDED
