//
//  pos-slider.cpp
//
#include "pos-slider.hpp"


namespace sfml_util
{
namespace sliders
{

    PosSlider::PosSlider(const sf::Vector2f & FROM_POS_V,
                         const sf::Vector2f & TO_POS_V,
                         const float          SLIDER_SPEED)
    :
        posV_                 (FROM_POS_V),
        sliderSpeed_          (SLIDER_SPEED),
        homePosV_             (FROM_POS_V),
        fromPosV_             (FROM_POS_V),
        toPosV_               (TO_POS_V),
        isFinishedMovingHoriz_(true),//will not start moving immediately
        isFinishedMovingVert_ (true),
        movingDir_            (sfml_util::Moving::Still),
        currHoriz_            (0.0f),
        currVert_             (0.0f),
        sliderHoriz_          (SLIDER_SPEED),
        sliderVert_           (SLIDER_SPEED)
    {
        Setup(FROM_POS_V, TO_POS_V, SLIDER_SPEED);
    }


    PosSlider::~PosSlider()
    {}


    void PosSlider::Setup(const sf::Vector2f & FROM_POS_V,
                          const sf::Vector2f & TO_POS_V,
                          const float          SLIDER_SPEED)
    {
        posV_ = FROM_POS_V;
        fromPosV_ = FROM_POS_V;
        toPosV_ = TO_POS_V;
        homePosV_ = FROM_POS_V;
        sliderSpeed_ = SLIDER_SPEED;
        ResetSlidersAndStopMoving();
    }


    void PosSlider::Reset(const bool WILL_RESET_POSITION)
    {
        if (WILL_RESET_POSITION)
            posV_ = homePosV_;

        ResetSlidersAndStopMoving();
    }


    void PosSlider::StartMovingToward(const float DEST_POS_LEFT, const float DEST_POS_TOP)
    {
        if (DEST_POS_LEFT > -1.0f)
            toPosV_.x = DEST_POS_LEFT;

        if (DEST_POS_TOP > -1.0f)
            toPosV_.y = DEST_POS_TOP;

        fromPosV_ = posV_;

        ResetSlidersAndStartMoving(Moving::Toward);
    }


    void PosSlider::StartMovingAway(const bool TO_HOME)
    {
        if (TO_HOME)
            toPosV_ = homePosV_;
        else
            toPosV_ = fromPosV_;

        fromPosV_ = posV_;

        ResetSlidersAndStartMoving(Moving::Away);
    }


    bool PosSlider::UpdateTime(const float ELAPSED_TIME_SECONDS)
    {
        if (isFinishedMovingHoriz_ && isFinishedMovingVert_)
        {
            return false;
        }
        else
        {
            if (false == isFinishedMovingHoriz_)
            {
                currHoriz_ = sliderHoriz_.Update(ELAPSED_TIME_SECONDS);
                posV_.x = fromPosV_.x + ((toPosV_.x - fromPosV_.x) * currHoriz_);
                isFinishedMovingHoriz_ = sliderHoriz_.GetIsDone();
            }

            if (false == isFinishedMovingVert_)
            {
                currVert_ = sliderVert_.Update(ELAPSED_TIME_SECONDS);
                posV_.y = fromPosV_.y + ((toPosV_.y - fromPosV_.y) * currVert_);
                isFinishedMovingVert_ = sliderVert_.GetIsDone();
            }

            return true;
        }
    }


    void PosSlider::ResetSlidersAndStartMoving(const sfml_util::Moving::Enum MOVING_DIR)
    {
        currHoriz_ = 0.0f;
        currVert_ = 0.0f;
        sliderHoriz_.Reset(sliderSpeed_);
        sliderVert_.Reset(sliderSpeed_);
        movingDir_ = MOVING_DIR;
        isFinishedMovingHoriz_ = false;
        isFinishedMovingVert_ = false;
    }


    void PosSlider::ResetSlidersAndStopMoving()
    {
        ResetSlidersAndStartMoving(Moving::Still);
        isFinishedMovingHoriz_ = true;
        isFinishedMovingVert_ = true;
    }

}
}
