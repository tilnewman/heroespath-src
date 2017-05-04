//
// character-stage-anim-num.cpp
//
#include "character-stage-anim-num.hpp"

#include "sfml-util/sfml-util.hpp"

#include <sstream>


namespace game
{
namespace stage
{

    AnimNum::AnimNum(const stats::Stat_t            VALUE,
                     const stats::stat::Enum        WHICH_STAT,
                     const float                    START_LEFT,
                     const float                    START_TOP,
                     const float                    TARGET_LEFT,
                     const float                    TARGET_TOP,
                     const sfml_util::gui::TextInfo TEXT_INFO)
    :
        value_       (VALUE),
        whichStat_   (WHICH_STAT),
        ignoreMe_    (VALUE < stats::Stat::VAL_MIN_),
        colorVal_    (100),
        startLeft_   (START_LEFT),
        startTop_    (START_TOP),
        distanceX_   (TARGET_LEFT - START_LEFT),
        distanceY_   (TARGET_TOP - START_TOP),
        color_       (sf::Color(colorVal_, colorVal_, colorVal_, colorVal_)),
        isDoneMoving_(false),
        isDoneFading_(false),
        willFade_    (true),
        fadeCounter_ (0.0f),
        isHeldDown_  (false),
        textInfo_    (TEXT_INFO),
        textObj_     (),
        sliderX_     (utilz::random::Float(0.35f, 0.75f)),
        timerSec_    (0.0f),
        prevPosX_    (-1.0f) //any negative value will work here
    {
        textObj_.setPosition(START_LEFT, START_TOP);
        CreateNewTextRegion();
    }


    AnimNum::~AnimNum()
    {}


    void AnimNum::CreateNewTextRegion()
    {
        std::ostringstream ss;
        ss << value_;
        textInfo_.text = ss.str();

        if (textInfo_.text == "0")
            textInfo_.color = sf::Color(255, 200, 200);
        else
            textInfo_.color = sf::Color::White;

        sfml_util::gui::SetupText(textObj_, textInfo_);
    }


    bool AnimNum::UpdateTime(const float ELAPSED_TIME_SEC)
    {
        if (false == isDoneMoving_)
        {
            //position
            const float SLIDERX_RATIO(sliderX_.Update(ELAPSED_TIME_SEC));
            textObj_.setPosition(startLeft_ + (SLIDERX_RATIO * distanceX_), startTop_ + (SLIDERX_RATIO * distanceY_));

            //size
            const float SCALE( sfml_util::Map(SLIDERX_RATIO, 0.0f, 1.0f, 0.6f, 1.0f) );
            textObj_.setScale(SCALE, SCALE);

            //color
            const float COLOR_VALF(static_cast<float>(colorVal_));
            color_.r = static_cast<sf::Int8>(COLOR_VALF + ((255 - COLOR_VALF) * SLIDERX_RATIO));
            color_.g = static_cast<sf::Int8>(COLOR_VALF + ((255 - COLOR_VALF) * SLIDERX_RATIO));
            color_.b = static_cast<sf::Int8>(COLOR_VALF + ((255 - COLOR_VALF) * SLIDERX_RATIO));
            color_.a = static_cast<sf::Int8>(COLOR_VALF + ((255 - COLOR_VALF) * SLIDERX_RATIO));
            sfml_util::SetTextColor(textObj_, color_);

            isDoneMoving_ = (SLIDERX_RATIO >= 0.99);
            return false;
        }
        else
        {
            if ((false == isDoneFading_) && (willFade_))
            {
                fadeCounter_ += ELAPSED_TIME_SEC * 600.0f;
                if (color_.a > 0)
                {
                    const sf::Int8 ALPHA(static_cast<sf::Int8>(fadeCounter_));
                    if (fadeCounter_ < 254.0f)
                        color_.a = 255 - ALPHA;
                    else
                    {
                        isDoneFading_ = true;
                        color_.a = 0;
                    }

                    sfml_util::SetTextColor(textObj_, color_);
                    return false;
                }
                else
                {
                    isDoneFading_ = true;
                    return true;
                }
            }
            else
                return true;
        }
    }


    bool AnimNum::MouseDown(const float POS_LEFT, const float POS_TOP)
    {
        if (textObj_.getGlobalBounds().contains(POS_LEFT, POS_TOP))
        {
            isHeldDown_ = true;
            return true;
        }
        else
            return false;
    }


    bool AnimNum::UpdateTimer(const float ELAPSED_TIME_SEC)
    {
        if (sfml_util::IsRealClose(prevPosX_, textObj_.getPosition().x))
            timerSec_ += ELAPSED_TIME_SEC;
        else
            timerSec_ = 0.0f;

        prevPosX_ = textObj_.getPosition().x;

        return (timerSec_ >= 3.0f);
    }


    bool operator==(const AnimNum & L, const AnimNum & R)
    {
        return ((L.colorVal_ == R.colorVal_) &&
                (L.color_ == R.color_) &&
                (sfml_util::IsRealClose(L.distanceX_, R.distanceX_)) &&
                (sfml_util::IsRealClose(L.distanceY_, R.distanceY_)) &&
                (sfml_util::IsRealClose(L.fadeCounter_, R.fadeCounter_)) &&
                (L.ignoreMe_ == R.ignoreMe_) &&
                (L.isDoneFading_ == R.isDoneFading_) &&
                (L.isDoneMoving_ == R.isDoneMoving_) &&
                (sfml_util::IsRealClose(L.startLeft_, R.startLeft_)) &&
                (sfml_util::IsRealClose(L.startTop_, R.startTop_)) &&
                (L.textInfo_ == R.textInfo_) &&
                (L.textObj_.getPosition() == R.textObj_.getPosition()) &&
                (L.value_ == R.value_) &&
                (L.whichStat_ == R.whichStat_) &&
                (L.willFade_ == R.willFade_));
    }


    bool operator!=(const AnimNum & L, const AnimNum & R)
    {
        return ! (L == R);
    }

}
}
