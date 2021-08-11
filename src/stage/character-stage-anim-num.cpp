// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// character-stage-anim-num.cpp
//
#include "character-stage-anim-num.hpp"

#include "sfutil/display.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

#include <sstream>
#include <tuple>

namespace heroespath
{
namespace stage
{

    AnimNum::AnimNum(
        const creature::Trait_t VALUE,
        const creature::Traits::Enum WHICH_STAT,
        const float START_LEFT,
        const float START_TOP,
        const float TARGET_LEFT,
        const float TARGET_TOP,
        const gui::TextInfo & TEXT_INFO)
        : value_(VALUE)
        , whichStat_(WHICH_STAT)
        , ignoreMe_(VALUE < 0)
        , colorVal_(100)
        , startLeft_(START_LEFT)
        , startTop_(START_TOP)
        , distanceX_(TARGET_LEFT - START_LEFT)
        , distanceY_(TARGET_TOP - START_TOP)
        , color_(sf::Color(colorVal_, colorVal_, colorVal_, colorVal_))
        , isDoneMoving_(false)
        , isDoneFading_(false)
        , willFade_(true)
        , fadeCounter_(0.0f)
        , isHeldDown_(false)
        , textInfo_(TEXT_INFO)
        , text_()
        , sliderX_(misc::Random(0.5f, 0.7f))
        , // this controls the speed
        timerSec_(0.0f)
        , prevPosX_(-1.0f) // any negative value will work here
    {
        text_.setPosition(START_LEFT, START_TOP);
        CreateNewTextRegion();
    }

    AnimNum::~AnimNum() = default;

    void AnimNum::CreateNewTextRegion()
    {
        std::ostringstream ss;
        ss << value_;
        textInfo_.text = ss.str();

        if (textInfo_.text == "0")
        {
            textInfo_.color = sf::Color(255, 200, 200);
        }
        else
        {
            textInfo_.color = sf::Color::White;
        }

        text_.setup(textInfo_);
    }

    bool AnimNum::UpdateTime(const float ELAPSED_TIME_SEC)
    {
        if (false == isDoneMoving_)
        {
            // position
            const float SLIDERX_RATIO(sliderX_.Update(ELAPSED_TIME_SEC));

            text_.setPosition(
                startLeft_ + (SLIDERX_RATIO * distanceX_),
                startTop_ + (SLIDERX_RATIO * distanceY_));

            // size
            const float SCALE(sfutil::Map(SLIDERX_RATIO, 0.0f, 1.0f, 0.6f, 1.0f));
            text_.setScale(SCALE, SCALE);

            // color
            const auto COLOR_VALF { static_cast<float>(colorVal_) };
            color_.r = static_cast<sf::Uint8>(COLOR_VALF + ((255.0f - COLOR_VALF) * SLIDERX_RATIO));
            color_.g = static_cast<sf::Uint8>(COLOR_VALF + ((255.0f - COLOR_VALF) * SLIDERX_RATIO));
            color_.b = static_cast<sf::Uint8>(COLOR_VALF + ((255.0f - COLOR_VALF) * SLIDERX_RATIO));
            color_.a = static_cast<sf::Uint8>(COLOR_VALF + ((255.0f - COLOR_VALF) * SLIDERX_RATIO));
            text_.setFillColor(color_);

            isDoneMoving_ = (SLIDERX_RATIO >= 0.99f);
            return false;
        }
        else
        {
            if ((false == isDoneFading_) && (willFade_))
            {
                fadeCounter_ += ELAPSED_TIME_SEC * 600.0f;
                if (color_.a > 0)
                {
                    const auto ALPHA { static_cast<sf::Int8>(fadeCounter_) };
                    if (fadeCounter_ < 254.0f)
                    {
                        color_.a = static_cast<sf::Uint8>(255 - static_cast<int>(ALPHA));
                    }
                    else
                    {
                        isDoneFading_ = true;
                        color_.a = 0;
                    }

                    text_.setFillColor(color_);
                    return false;
                }
                else
                {
                    isDoneFading_ = true;
                    return true;
                }
            }
            else
            {
                return true;
            }
        }
    }

    bool AnimNum::MouseDown(const float POS_LEFT, const float POS_TOP)
    {
        if (text_.getGlobalBounds().contains(POS_LEFT, POS_TOP))
        {
            isHeldDown_ = true;
            return true;
        }
        else
        {
            return false;
        }
    }

    bool AnimNum::UpdateTimer(const float ELAPSED_TIME_SEC)
    {
        if (misc::IsRealClose(prevPosX_, text_.getPosition().x))
        {
            timerSec_ += ELAPSED_TIME_SEC;
        }
        else
        {
            timerSec_ = 0.0f;
        }

        prevPosX_ = text_.getPosition().x;

        return (timerSec_ >= 3.0f);
    }

    void AnimNum::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        target.draw(text_, states);
    }

    bool operator==(const AnimNum & L, const AnimNum & R)
    {
        if (((misc::IsRealClose(L.startLeft_, R.startLeft_))
             && (misc::IsRealClose(L.startTop_, R.startTop_))
             && (misc::IsRealClose(L.distanceX_, R.distanceX_))
             && (misc::IsRealClose(L.distanceY_, R.distanceY_))
             && (misc::IsRealClose(L.fadeCounter_, R.fadeCounter_))
             && (misc::IsRealClose(L.timerSec_, R.timerSec_))
             && (misc::IsRealClose(L.prevPosX_, R.prevPosX_))
             && (misc::IsRealClose(L.sliderX_.Value(), R.sliderX_.Value())))
            == false)
        {
            return false;
        }

        return std::tie(
                   L.value_,
                   L.whichStat_,
                   L.ignoreMe_,
                   L.colorVal_,
                   L.color_,
                   L.isDoneMoving_,
                   L.isDoneFading_,
                   L.willFade_,
                   L.isHeldDown_,
                   L.textInfo_)
            == std::tie(
                   R.value_,
                   R.whichStat_,
                   R.ignoreMe_,
                   R.colorVal_,
                   R.color_,
                   R.isDoneMoving_,
                   R.isDoneFading_,
                   R.willFade_,
                   R.isHeldDown_,
                   R.textInfo_);
    }

} // namespace stage
} // namespace heroespath
