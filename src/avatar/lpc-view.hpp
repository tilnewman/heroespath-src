///////////////////////////////////////////////////////////////////////////////
//
// Heroes' Path - Open-source, non-commercial, simple, game in the RPG style.
// Copyright (C) 2017 Ziesche Til Newman (tilnewman@gmail.com)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from
// the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
//  1. The origin of this software must not be misrepresented; you must not
//     claim that you wrote the original software.  If you use this software
//     in a product, an acknowledgment in the product documentation would be
//     appreciated but is not required.
//
//  2. Altered source versions must be plainly marked as such, and must not
//     be misrepresented as being the original software.
//
//  3. This notice may not be removed or altered from any source distribution.
//
///////////////////////////////////////////////////////////////////////////////
#ifndef HEROESPATH_AVATAR_LPCVIEW_HPP_INCLUDED
#define HEROESPATH_AVATAR_LPCVIEW_HPP_INCLUDED
//
// lpc-npc-view.hpp
//
#include "sfml-util/direction-enum.hpp"
#include "sfml-util/sfml-graphics.hpp"

#include "avatar/animation.hpp"
#include "avatar/i-view.hpp"
#include "avatar/pose-enum.hpp"

namespace heroespath
{
namespace avatar
{

    // Responsible for drawing an LPC-style NPC.
    class LPCView : public IView
    {
    public:
        explicit LPCView(const Avatar::Enum);
        virtual ~LPCView() = default;

        virtual void Set(const Pose::Enum, const sfml_util::Direction::Enum) override;

        virtual bool Update(const float TIME_ELAPSED) override;

        inline virtual void UpdatePos(const sf::Vector2f & V) override { sprite_.setPosition(V); }

        inline virtual sfml_util::Direction::Enum Direction() const override
        {
            return animation_.direction;
        }

        inline virtual Pose::Enum WhichPose() const override { return animation_.pose; }

        inline virtual const sf::Sprite & SpriteRef() const override { return sprite_; }

        inline virtual const sf::Vector2f SpriteSize() const override
        {
            return sf::Vector2f(sprite_.getGlobalBounds().width, sprite_.getGlobalBounds().height);
        }

        inline virtual Avatar::Enum WhichAvatar() const override { return whichAvatar_; }

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
        sf::Texture texture_;
        sf::Sprite sprite_;
        Animation animation_;
        float frameTimerSec_;
        std::size_t frameIndex_;
    };
} // namespace avatar
} // namespace heroespath

#endif // HEROESPATH_AVATAR_LPCVIEW_HPP_INCLUDED
