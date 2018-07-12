// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// lpc-view.cpp
//
#include "lpc-view.hpp"
#include "log/log-macros.hpp"
#include "misc/assertlogandthrow.hpp"
#include "misc/random.hpp"
#include "sfml-util/loaders.hpp"
#include "sfml-util/sfml-util.hpp"
#include "sfml-util/texture-cache.hpp"

#include <boost/filesystem/path.hpp>

namespace heroespath
{
namespace avatar
{

    const int LPCView::CELL_SIZE_ { 64 };
    const int LPCView::CELL_COUNT_ { 8 };
    const float LPCView::FRAME_DURATION_SEC_WALK_ { 0.05f };
    const float LPCView::FRAME_DURATION_SEC_GIVETAKE_ { 0.1f };
    const float LPCView::FRAME_DURATION_SEC_SINGLEFRAME_ { 1.5f };
    const float LPCView::FRAME_DURATION_SEC_BLINK_MIN_ { 0.025f };
    const float LPCView::FRAME_DURATION_SEC_BLINK_MAX_ { 0.20f };

    LPCView::LPCView(const Avatar::Enum WHICH_AVATAR, const sf::Vector2f & CENTERED_MAP_POS_V)
        : whichAvatar_(WHICH_AVATAR)
        , cachedTexture_(boost::filesystem::path(Avatar::ImagePath(whichAvatar_)))
        , sprite_(cachedTexture_.Get())
        , animation_(CreateAnimation(Pose::Standing, sfml_util::Direction::Right))
        , frameTimerSec_(0.0f)
        , frameIndex_(0)
    {
        auto const REQUIRED_DIMMENSION { static_cast<unsigned>(CELL_SIZE_ * CELL_COUNT_) };
        const sf::Vector2u REQUIRED_SIZE_V(REQUIRED_DIMMENSION, REQUIRED_DIMMENSION);

        auto const ACTUAL_SIZE_V { cachedTexture_.Get().getSize() };

        M_ASSERT_OR_LOGANDTHROW_SS(
            (ACTUAL_SIZE_V == REQUIRED_SIZE_V),
            "avatar::LPCView::LPCView(avatar_enum="
                << WHICH_AVATAR << ", centered_map_pos=" << CENTERED_MAP_POS_V
                << ") but the image loaded was not the required size of " << REQUIRED_SIZE_V
                << ", instead it was " << ACTUAL_SIZE_V << ".");

        SetupSprite();
        sprite_.setPosition(CENTERED_MAP_POS_V);
    }

    void LPCView::Set(const Pose::Enum POSE, const sfml_util::Direction::Enum DIRECTION)
    {
        if ((POSE != animation_.pose) || (DIRECTION != animation_.direction))
        {
            animation_ = CreateAnimation(POSE, DIRECTION);
            frameTimerSec_ = 0.0f;
            frameIndex_ = 0;
            SetupSprite();
        }
    }

    bool LPCView::Update(const float TIME_ELAPSED)
    {
        if (animation_.frame_duration > 0.0f)
        {
            frameTimerSec_ += TIME_ELAPSED;
            if (frameTimerSec_ > animation_.frame_duration)
            {
                frameTimerSec_ = 0.0f;

                if (++frameIndex_ >= animation_.frame_num_vec.size())
                {
                    frameIndex_ = 0;

                    if (false == animation_.will_loop)
                    {
                        return true;
                    }
                }
                else
                {
                    SetupSprite();
                }
            }
        }

        return false;
    }

    void LPCView::MoveIfWalking(const float AMOUNT)
    {
        if (Pose::Walking == animation_.pose)
        {
            switch (Direction())
            {
                case sfml_util::Direction::Left:
                {
                    sprite_.move(AMOUNT * -1.0f, 0.0f);
                    break;
                }
                case sfml_util::Direction::Right:
                {
                    sprite_.move(AMOUNT, 0.0f);
                    break;
                }
                case sfml_util::Direction::Up:
                {
                    sprite_.move(0.0f, AMOUNT * -1.0f);
                    break;
                }
                case sfml_util::Direction::Down:
                case sfml_util::Direction::Count:
                default:
                {
                    sprite_.move(0.0f, AMOUNT);
                    break;
                }
            }
        }
    }

    const sf::Sprite LPCView::DefaultPoseSprite() const
    {
        return sf::Sprite(
            cachedTexture_.Get(),
            FrameRect(FrameNumbers(Pose::Standing, sfml_util::Direction::Right).at(0)));
    }

    const FrameNumVec_t
        LPCView::FrameNumbers(const Pose::Enum POSE, const sfml_util::Direction::Enum DIRECTION)
    {
        namespace su = sfml_util;

        switch (POSE)
        {
            case Pose::Standing:
            {
                switch (DIRECTION)
                {
                    case su::Direction::Left:
                    {
                        return { 9 };
                    }
                    case su::Direction::Right:
                    {
                        return { 25 };
                    }
                    case su::Direction::Up:
                    {
                        return { 1 };
                    }
                    case su::Direction::Down:
                    case su::Direction::Count:
                    default:
                    {
                        return { 17 };
                    }
                }
            }
            case Pose::Walking:
            {
                switch (DIRECTION)
                {
                    case su::Direction::Left:
                    {
                        return { 10, 11, 12, 13, 14, 15, 16, 41 };
                    }
                    case su::Direction::Right:
                    {
                        return { 26, 27, 28, 29, 30, 31, 32, 57 };
                    }
                    case su::Direction::Up:
                    {
                        return { 2, 3, 4, 1, 6, 7, 8 };
                    }
                    case su::Direction::Down:
                    case su::Direction::Count:
                    default:
                    {
                        return { 18, 19, 20, 17, 22, 23, 24 };
                    }
                }
            }
            case Pose::GiveTake:
            {
                switch (DIRECTION)
                {
                    case su::Direction::Left:
                    {
                        return { 42, 43, 44, 45, 46 };
                    }
                    case su::Direction::Right:
                    {
                        return { 58, 59, 60, 61, 62 };
                    }
                    case su::Direction::Up:
                    {
                        return { 34, 35, 36, 37, 38 };
                    }
                    case su::Direction::Down:
                    case su::Direction::Count:
                    default:
                    {
                        return { 50, 51, 52, 53, 54 };
                    }
                }
            }
            case Pose::Fight:
            {
                switch (DIRECTION)
                {
                    case su::Direction::Left:
                    {
                        return { 47 };
                    }
                    case su::Direction::Right:
                    {
                        return { 63 };
                    }
                    case su::Direction::Up:
                    {
                        return { 39 };
                    }
                    case su::Direction::Down:
                    case su::Direction::Count:
                    default:
                    {
                        return { 55 };
                    }
                }
            }
            case Pose::Cast:
            {
                switch (DIRECTION)
                {
                    case su::Direction::Left:
                    {
                        return { 48 };
                    }
                    case su::Direction::Right:
                    {
                        return { 64 };
                    }
                    case su::Direction::Up:
                    {
                        return { 40 };
                    }
                    case su::Direction::Down:
                    case su::Direction::Count:
                    default:
                    {
                        return { 56 };
                    }
                }
            }
            case Pose::Blink:
            {
                switch (DIRECTION)
                {
                    case su::Direction::Left:
                    {
                        return { 5 };
                    }
                    case su::Direction::Right:
                    {
                        return { 21 };
                    }
                    case su::Direction::Up:
                    {
                        return { 1 };
                    }
                    case su::Direction::Down:
                    case su::Direction::Count:
                    default:
                    {
                        return { 33 };
                    }
                }
            }
            case Pose::Dead:
            case Pose::Count:
            default:
            {
                return { 49 };
            }
        }
    }

    const sf::IntRect LPCView::FrameRect(const FrameNum_t FRAME_NUM)
    {
        auto const FRAME_INDEX_X { [&]() {
            auto const INDEX { (FRAME_NUM % CELL_COUNT_) - 1 };
            if (INDEX == -1)
            {
                return CELL_COUNT_ - 1;
            }
            else
            {
                return INDEX;
            }
        }() };

        auto const FRAME_INDEX_Y { [&]() {
            auto const INDEX { FRAME_NUM / CELL_COUNT_ };
            if ((FRAME_NUM % CELL_COUNT_) == 0)
            {
                return INDEX - 1;
            }
            else
            {
                return INDEX;
            }
        }() };

        return sf::IntRect(
            FRAME_INDEX_X * CELL_SIZE_, FRAME_INDEX_Y * CELL_SIZE_, CELL_SIZE_, CELL_SIZE_);
    }

    const Animation LPCView::CreateAnimation(
        const Pose::Enum POSE, const sfml_util::Direction::Enum DIRECTION) const
    {
        return Animation(
            POSE,
            DIRECTION,
            FrameNumbers(POSE, DIRECTION),
            FrameDuration(POSE),
            (POSE == Pose::Walking));
    }

    float LPCView::FrameDuration(const Pose::Enum POSE) const
    {
        switch (POSE)
        {
            case Pose::Walking:
            {
                return FRAME_DURATION_SEC_WALK_;
            }
            case Pose::GiveTake:
            {
                return FRAME_DURATION_SEC_GIVETAKE_;
            }
            case Pose::Cast:
            {
                return FRAME_DURATION_SEC_SINGLEFRAME_;
            }
            case Pose::Blink:
            {
                return misc::random::Float(
                    FRAME_DURATION_SEC_BLINK_MIN_, FRAME_DURATION_SEC_BLINK_MAX_);
            }
            case Pose::Fight:
            case Pose::Standing:
            case Pose::Dead:
            case Pose::Count:
            default:
            {
                return 0.0f;
            }
        }
    }

    void LPCView::SetupSprite()
    {
        sprite_.setTextureRect(FrameRect(animation_.frame_num_vec[frameIndex_]));

        auto const SCALE { Avatar::Scale(Avatar::Name(whichAvatar_)) };
        sprite_.setScale(SCALE, SCALE);
    }

} // namespace avatar
} // namespace heroespath
