//
// sliderbar-effects.cpp
//
#include "sliderbar-effects.hpp"

#include "sfml-util/sound-manager.hpp"

#include "game/log-macros.hpp"

#include <sstream>


namespace sfml_util
{

    bool SliderBarLabeled_Effects::willPreventPlayingSound_(false);


    SliderBarLabeled_Effects::SliderBarLabeled_Effects( const std::string &        NAME,
                                                        const float                POS_LEFT,
                                                        const float                POS_TOP,
                                                        const float                LENGTH,
                                                        const gui::SliderStyle &   STYLE,
                                                        const gui::MouseTextInfo & THREE_TEXT_INFOS,
                                                        const float                INITIAL_VALUE,
                                                        const float                RELATIVE_LABEL_POS_LEFT,
                                                        const float                RELATIVE_LABEL_POS_TOP )
    :
        SliderBarLabeled(std::string(NAME).append("_SliderBarLabeled_Music"),
                         POS_LEFT,
                         POS_TOP,
                         LENGTH,
                         STYLE,
                         THREE_TEXT_INFOS,
                         INITIAL_VALUE,
                         RELATIVE_LABEL_POS_LEFT,
                         RELATIVE_LABEL_POS_TOP),
        willPlaySound_(false),
        timeSinceLastPlaySec_(0.0f)
    {
        SetCurrentValue(sfml_util::SoundManager::Instance()->SoundEffectVolume() / 100.0f);
    }


    SliderBarLabeled_Effects::~SliderBarLabeled_Effects()
    {}


    void SliderBarLabeled_Effects::OnChange(const float NEW_VALUE_PARAM)
    {
        const float NEW_VALUE_RANGE_CORRECTED(NEW_VALUE_PARAM * 100.0f);
        const int NEW_VALUE_INT(static_cast<int>(NEW_VALUE_RANGE_CORRECTED));

        SoundManager::Instance()->SoundEffectVolumeSet( static_cast<float>(NEW_VALUE_INT) );

        willPlaySound_ = ! willPreventPlayingSound_;

        gui::TextInfo textInfo(threeTextInfos_.up);

        if (0 == NEW_VALUE_INT)
            textInfo = threeTextInfos_.down;
        else
            if (100 == NEW_VALUE_INT)
                textInfo = threeTextInfos_.over;

        std::ostringstream ss;

        if (NEW_VALUE_INT == 0)
        {
            ss << "MUTE";
            textInfo.text = ss.str();
            textInfo.fontSPtr = FontManager::Instance()->Font_Typical();
        }
        else
        {
            ss << NEW_VALUE_INT;
            textInfo.text = ss.str();
            textInfo.fontSPtr = FontManager::Instance()->Font_NumbersDefault1();
        }

        sf::FloatRect r(textRegion_.GetEntityRegion());
        r.width = 0.0f;
        r.height = 0.0f;
        textRegion_.Setup(textInfo, r);
    }


    void SliderBarLabeled_Effects::SetPadPosition()
    {
        if (currentVal_ > 1.0f)
            currentVal_ = 1.0f;
        else
            if (currentVal_ < 0.0f)
                currentVal_ = 0.0f;

        const sf::Vector2f POS_V(GetEntityPos());

        if (STYLE_.orientation == Orientation::Horiz)
        {
            const float SLIDER_RANGE(LENGTH_ - (botOrLeftImage_.GetUpSprite().getLocalBounds().width + topOrRightImage_.GetUpSprite().getLocalBounds().width + padImage_.GetUpSprite().getLocalBounds().width - 8));
            const float SLIDER_POSX(POS_V.x + botOrLeftImage_.GetUpSprite().getLocalBounds().width + (SLIDER_RANGE * (1.0f - currentVal_)) - 3);//magic number 3 moves passed the shadow
            padImage_.GetUpSprite().setPosition(SLIDER_POSX, POS_V.y + (botOrLeftImage_.GetUpSprite().getLocalBounds().height * 0.5f) - (padImage_.GetUpSprite().getLocalBounds().height * 0.5f));
        }
        else
        {
            const float SLIDER_RANGE(LENGTH_ - (topOrRightImage_.GetUpSprite().getLocalBounds().height + botOrLeftImage_.GetUpSprite().getLocalBounds().height + padImage_.GetUpSprite().getLocalBounds().height - 5));
            const float SLIDER_POSY(POS_V.y + topOrRightImage_.GetUpSprite().getLocalBounds().height + (SLIDER_RANGE * (1.0f - currentVal_)) - 1);//magic number 1 moves passed the shadow
            padImage_.GetUpSprite().setPosition(POS_V.x + (topOrRightImage_.GetUpSprite().getLocalBounds().width * 0.5f) - (padImage_.GetUpSprite().getLocalBounds().width * 0.5f), SLIDER_POSY);
        }
    }


    bool SliderBarLabeled_Effects::MouseDown(const sf::Vector2f & MOUSE_POS_V)
    {
        if (botOrLeftImage_.MouseDown(MOUSE_POS_V))
        {
            if (STYLE_.orientation == Orientation::Horiz)
            {
                SoundManager::Instance()->SoundEffectsSet_TickOn()->PlayRandom();
                currentVal_ += 0.1f;
            }
            else
            {
                SoundManager::Instance()->SoundEffectsSet_TickOff()->PlayRandom();
                currentVal_ -= 0.1f;
            }

            SetupAllPositions();
            return true;
        }
        else
        {
            if (topOrRightImage_.MouseDown(MOUSE_POS_V))
            {
                if (STYLE_.orientation == Orientation::Horiz)
                {
                    SoundManager::Instance()->SoundEffectsSet_TickOff()->PlayRandom();
                    currentVal_ -= 0.1f;
                }
                else
                {
                    SoundManager::Instance()->SoundEffectsSet_TickOn()->PlayRandom();
                    currentVal_ += 0.1f;
                }

                SetupAllPositions();
                return true;
            }
            else
            {
                if (padImage_.MouseDown(MOUSE_POS_V))
                {
                    SoundManager::Instance()->SoundEffectsSet_TickOn()->PlayRandom();
                    entityMouseState_ = MouseState::Down;
                }
                else
                {
                    if (barImage_.MouseDown(MOUSE_POS_V))
                    {
                        if (STYLE_.orientation == Orientation::Horiz)
                        {
                            if (MOUSE_POS_V.x < padImage_.GetEntityRegion().left)
                            {
                                SoundManager::Instance()->SoundEffectsSet_TickOn()->PlayRandom();
                                currentVal_ += 0.1f;
                                SetupAllPositions();
                                return true;
                            }
                            else
                            {
                                if (MOUSE_POS_V.x >(padImage_.GetEntityRegion().left + padImage_.GetEntityRegion().width))
                                {
                                    SoundManager::Instance()->SoundEffectsSet_TickOff()->PlayRandom();
                                    currentVal_ -= 0.1f;
                                    SetupAllPositions();
                                    return true;
                                }
                            }
                        }
                        else
                        {
                            if (MOUSE_POS_V.y < padImage_.GetEntityRegion().top)
                            {
                                SoundManager::Instance()->SoundEffectsSet_TickOn()->PlayRandom();
                                currentVal_ += 0.1f;
                                SetupAllPositions();
                                return true;
                            }
                            else
                            {
                                if (MOUSE_POS_V.y >(padImage_.GetEntityRegion().top + padImage_.GetEntityRegion().height))
                                {
                                    SoundManager::Instance()->SoundEffectsSet_TickOff()->PlayRandom();
                                    currentVal_ -= 0.1f;
                                    SetupAllPositions();
                                    return true;
                                }
                            }
                        }//end else orientation
                    }//end if barEntity click
                }//end if pad click
            }//end else topOrRightEntity click
        }//end else botOrLeftEntity click

        return false;
    }


    void SliderBarLabeled_Effects::SetCurrentValueFromScreenCoords(const sf::Vector2f & NEW_COORDS)
    {
        const sf::Vector2f POS_V(GetEntityPos());

        //change from screen to slider coords
        if (STYLE_.orientation == Orientation::Horiz)
        {
            const float MIN_SCREENX(POS_V.x + botOrLeftImage_.GetUpSprite().getLocalBounds().width);
            const float MAX_SCREENX((POS_V.x + (LENGTH_ - topOrRightImage_.GetUpSprite().getLocalBounds().width) - barImage_.GetUpSprite().getLocalBounds().height));
            if (NEW_COORDS.x < MIN_SCREENX)
                currentVal_ = 1.0f;
            else
                if (NEW_COORDS.x > MAX_SCREENX)
                    currentVal_ = 0.0f;
                else
                {
                    currentVal_ = (NEW_COORDS.x - MIN_SCREENX) / (MAX_SCREENX - MIN_SCREENX);
                    currentVal_ = 1.0f - currentVal_;
                }
        }
        else
        {
            const float MIN_SCREENY(POS_V.y + topOrRightImage_.GetUpSprite().getLocalBounds().height);
            const float MAX_SCREENY((POS_V.y + (LENGTH_ - botOrLeftImage_.GetUpSprite().getLocalBounds().height) - barImage_.GetUpSprite().getLocalBounds().height));
            if (NEW_COORDS.y < MIN_SCREENY)
                currentVal_ = 1.0f;
            else
                if (NEW_COORDS.y > MAX_SCREENY)
                    currentVal_ = 0.0f;
                else
                {
                    currentVal_ = (NEW_COORDS.y - MIN_SCREENY) / (MAX_SCREENY - MIN_SCREENY);
                    currentVal_ = 1.0f - currentVal_;
                }
        }

        SetupAllPositions();
    }


    bool SliderBarLabeled_Effects::UpdateTime(const float ELAPSED_TIME_SEC)
    {
        if (willPlaySound_)
        {
            timeSinceLastPlaySec_ += ELAPSED_TIME_SEC;

            if (timeSinceLastPlaySec_ > 0.5f)
            {
                willPlaySound_ = false;
                timeSinceLastPlaySec_ = 0.0f;
                SoundManager::Instance()->SoundEffectsSet_Prompt()->Play(sound_effect::PromptWarn);
                return true;
            }
        }

        return false;
    }

}
