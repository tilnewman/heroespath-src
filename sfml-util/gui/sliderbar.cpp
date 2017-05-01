//
// sliderbar.cpp
//
#include "sliderbar.hpp"

#include "sfml-util/gui/gui-elements.hpp"
#include "sfml-util/sound-manager.hpp"
#include "sfml-util/static-sounds.hpp"

#include "utilz/assertlogandthrow.hpp"


namespace sfml_util
{
namespace gui
{

    SliderBar::SliderBar(const std::string & NAME,
                         const float         POS_LEFT,
                         const float         POS_TOP,
                         const float         LENGTH,
                         const SliderStyle & STYLE,
                         callback::ISliderBarCallbackHandler_t * const CHANGE_HANDLER_PTR,
                         const float         INITIAL_VALUE)
    :
        GuiEntity        (std::string(NAME).append("_SliderBar"), POS_LEFT, POS_TOP),
        currentVal_      (INITIAL_VALUE),
        LENGTH_          (LENGTH),
        STYLE_           (STYLE),
        botOrLeftImage_  (std::string(NAME).append((STYLE.orientation == Orientation::Horiz) ? "SliderBar's_Left Arrow"  : "SliderBar's_Bottom Arrow"), 0.0f, 0.0f),
        topOrRightImage_ (std::string(NAME).append((STYLE.orientation == Orientation::Horiz) ? "SliderBar's_Right Arrow" : "SliderBar's_Top Arrow"),    0.0f, 0.0f),
        barImage_        (std::string(NAME).append("SliderBar's"), 0.0f, 0.0f),
        padImage_        (std::string(NAME).append("SliderBar's"), 0.0f, 0.0f),
        changeHandlerPtr_(CHANGE_HANDLER_PTR)
    {
        Setup();
    }


    void SliderBar::SetupEntityRegion()
    {
        //setup bounding rect for SliderBar as a Clickable object itself
        const sf::Vector2f POS_V(GetEntityPos());

        sf::FloatRect r;
        r.left = POS_V.x;//these values were already set in the constructor
        r.top = POS_V.y;
        if (STYLE_.orientation == Orientation::Horiz)
        {
            r.width  = LENGTH_;
            r.height = botOrLeftImage_.GetUpSprite().getLocalBounds().height;
        }
        else
        {
            r.width  = topOrRightImage_.GetUpSprite().getLocalBounds().width;
            r.height = LENGTH_;
        }

        SetEntityRegion(r);
    }


    void SliderBar::SetCurrentValue(const float NEW_VAL)
    {
        currentVal_ = NEW_VAL;
        SetPadPosition();
        OnChange(currentVal_);
    }


    void SliderBar::SetCurrentValueFromScreenCoords(const sf::Vector2f & NEW_COORDS)
    {
        const sf::Vector2f POS_V(GetEntityPos());

        //change from screen to slider coords
        if (STYLE_.orientation == Orientation::Horiz)
        {
            const float MIN_SCREENX(POS_V.x + botOrLeftImage_.GetUpSprite().getLocalBounds().width);
            const float MAX_SCREENX((POS_V.x + (LENGTH_ - topOrRightImage_.GetUpSprite().getLocalBounds().width) - barImage_.GetUpSprite().getLocalBounds().height));
            if (NEW_COORDS.x < MIN_SCREENX)
                currentVal_ = 0.0f;
            else
            {
                if (NEW_COORDS.x > MAX_SCREENX)
                    currentVal_ = 1.0f;
                else
                    currentVal_ = (NEW_COORDS.x - MIN_SCREENX) / (MAX_SCREENX - MIN_SCREENX);
            }
        }
        else
        {
            const float MIN_SCREENY(POS_V.y + topOrRightImage_.GetUpSprite().getLocalBounds().height);
            const float MAX_SCREENY((POS_V.y + (LENGTH_ - botOrLeftImage_.GetUpSprite().getLocalBounds().height) - barImage_.GetUpSprite().getLocalBounds().height));
            if (NEW_COORDS.y < MIN_SCREENY)
                currentVal_ = 0.0f;
            else
            {
                if (NEW_COORDS.y > MAX_SCREENY)
                    currentVal_ = 1.0f;
                else
                    currentVal_ = (NEW_COORDS.y - MIN_SCREENY) / (MAX_SCREENY - MIN_SCREENY);
            }
        }

        SetupAllPositions();
    }


    void SliderBar::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        target.draw(barImage_, states);
        target.draw(padImage_, states);
        target.draw(botOrLeftImage_, states);
        target.draw(topOrRightImage_, states);
    }


    void SliderBar::OnChange(const float)
    {
        if (changeHandlerPtr_ != nullptr)
            changeHandlerPtr_->HandleCallback(this);
    }


    void SliderBar::Setup()
    {
        const GuiElementsSPtr_t GE_SPTR{ GuiElements::Instance() };

        if (STYLE_.isLineLarge)
            barImage_.GetUpSprite() = sf::Sprite( * GE_SPTR->GetTexture(), GE_SPTR->GetRect_HorizontalLineLarge());
        else
            barImage_.GetUpSprite() = sf::Sprite( * GE_SPTR->GetTexture(), GE_SPTR->GetRect_HorizontalLineSmall());


        if (STYLE_.orientation == Orientation::Horiz)
        {
            //pick the pad sprite
            padImage_.GetUpSprite() = sf::Sprite( * GE_SPTR->GetTexture(), GE_SPTR->GetRect_PadHorizontalLarge());

            switch (STYLE_.brightness)
            {
            case Brightness::Medium:
            {
                if (STYLE_.willLabelArrows)
                {
                    botOrLeftImage_.GetUpSprite() = sf::Sprite( * GE_SPTR->GetTexture(), GE_SPTR->GetRect_ArrowMinusLeftMed());
                    topOrRightImage_.GetUpSprite() = sf::Sprite( * GE_SPTR->GetTexture(), GE_SPTR->GetRect_ArrowPlusRightMed());
                }
                else
                {
                    botOrLeftImage_.GetUpSprite() = sf::Sprite( * GE_SPTR->GetTexture(), GE_SPTR->GetRect_ArrowLeftMed());
                    topOrRightImage_.GetUpSprite() = sf::Sprite( * GE_SPTR->GetTexture(), GE_SPTR->GetRect_ArrowRightMed());
                }
                break;
            }
            case Brightness::Dark:
            {
                if (STYLE_.willLabelArrows)
                {
                    botOrLeftImage_.GetUpSprite() = sf::Sprite( * GE_SPTR->GetTexture(), GE_SPTR->GetRect_ArrowMinusLeftDark());
                    topOrRightImage_.GetUpSprite() = sf::Sprite( * GE_SPTR->GetTexture(), GE_SPTR->GetRect_ArrowPlusRightDark());
                }
                else
                {
                    botOrLeftImage_.GetUpSprite() = sf::Sprite( * GE_SPTR->GetTexture(), GE_SPTR->GetRect_ArrowLeftDark());
                    topOrRightImage_.GetUpSprite() = sf::Sprite( * GE_SPTR->GetTexture(), GE_SPTR->GetRect_ArrowRightDark());
                }
                break;
            }

            case Brightness::Bright://default fallthrough
            case Brightness::Count:
            default:
            {
                if (STYLE_.willLabelArrows)
                {
                    botOrLeftImage_.GetUpSprite() = sf::Sprite( * GE_SPTR->GetTexture(), GE_SPTR->GetRect_ArrowMinusLeftBright());
                    topOrRightImage_.GetUpSprite() = sf::Sprite( * GE_SPTR->GetTexture(), GE_SPTR->GetRect_ArrowPlusRightBright());
                }
                else
                {
                    botOrLeftImage_.GetUpSprite() = sf::Sprite( * GE_SPTR->GetTexture(), GE_SPTR->GetRect_ArrowLeftBright());
                    topOrRightImage_.GetUpSprite() = sf::Sprite( * GE_SPTR->GetTexture(), GE_SPTR->GetRect_ArrowRightBright());
                }
                break;
            }//end default
            }//end switch

            SetupAllPositions();

            //set the bar's length
            const float TARGET_LENGTH(LENGTH_ - botOrLeftImage_.GetUpSprite().getLocalBounds().width + (LENGTH_ / 80.0f));
            const float CURRENT_LENGTH(barImage_.GetUpSprite().getLocalBounds().width);
            const float NEW_SCALE(TARGET_LENGTH / CURRENT_LENGTH);
            barImage_.GetUpSprite().setScale(NEW_SCALE, 1.0f);

            //ensure the given length is long enough
            //const float MIN_LENGTH(botOrLeftImage_.GetUpSprite().getLocalBounds().width + topOrRightImage_.GetUpSprite().getLocalBounds().width + (padImage_.GetUpSprite().getLocalBounds().width * 2.0f));
            //M_ASSERT_OR_LOGANDTHROW_SS((LENGTH_ >= MIN_LENGTH), "SliderBar(horizontal) with length=" << LENGTH_ << " is too short!  (min=" << MIN_LENGTH << ")");
        }
        else
        {
            //pick the pad sprite to use
            padImage_.GetUpSprite() = sf::Sprite( * GE_SPTR->GetTexture(), GE_SPTR->GetRect_PadVerticalLarge());

            //bar sprites are only horizontal so rotate if orientation is vertical
            barImage_.GetUpSprite().rotate(270.0f);

            switch (STYLE_.brightness)
            {
            case Brightness::Medium:
            {
                if (STYLE_.willLabelArrows)
                {
                    botOrLeftImage_.GetUpSprite() = sf::Sprite( * GE_SPTR->GetTexture(), GE_SPTR->GetRect_ArrowMinusDownMed());
                    topOrRightImage_.GetUpSprite() = sf::Sprite( * GE_SPTR->GetTexture(), GE_SPTR->GetRect_ArrowPlusUpMed());
                }
                else
                {
                    botOrLeftImage_.GetUpSprite() = sf::Sprite( * GE_SPTR->GetTexture(), GE_SPTR->GetRect_ArrowDownMed());
                    topOrRightImage_.GetUpSprite() = sf::Sprite( * GE_SPTR->GetTexture(), GE_SPTR->GetRect_ArrowUpMed());
                }
                break;
            }
            case Brightness::Dark:
            {
                if (STYLE_.willLabelArrows)
                {
                    botOrLeftImage_.GetUpSprite() = sf::Sprite( * GE_SPTR->GetTexture(), GE_SPTR->GetRect_ArrowMinusDownDark());
                    topOrRightImage_.GetUpSprite() = sf::Sprite( * GE_SPTR->GetTexture(), GE_SPTR->GetRect_ArrowPlusUpDark());
                }
                else
                {
                    botOrLeftImage_.GetUpSprite() = sf::Sprite( * GE_SPTR->GetTexture(), GE_SPTR->GetRect_ArrowDownDark());
                    topOrRightImage_.GetUpSprite() = sf::Sprite( * GE_SPTR->GetTexture(), GE_SPTR->GetRect_ArrowUpDark());
                }
                break;
            }

            case Brightness::Bright://default fallthrough
            case Brightness::Count:
            default:
            {
                if (STYLE_.willLabelArrows)
                {
                    botOrLeftImage_.GetUpSprite() = sf::Sprite( * GE_SPTR->GetTexture(), GE_SPTR->GetRect_ArrowMinusDownBright());
                    topOrRightImage_.GetUpSprite() = sf::Sprite( * GE_SPTR->GetTexture(), GE_SPTR->GetRect_ArrowPlusUpBright());
                }
                else
                {
                    botOrLeftImage_.GetUpSprite() = sf::Sprite( * GE_SPTR->GetTexture(), GE_SPTR->GetRect_ArrowDownBright());
                    topOrRightImage_.GetUpSprite() = sf::Sprite( * GE_SPTR->GetTexture(), GE_SPTR->GetRect_ArrowUpBright());
                }
                break;
            }//end default
            }//end switch

            SetupAllPositions();

            //set the length of the bar sprite
            const float TARGET_LENGTH(LENGTH_ - botOrLeftImage_.GetUpSprite().getLocalBounds().height + (LENGTH_ / 80.0f));
            const float CURRENT_LENGTH(barImage_.GetUpSprite().getLocalBounds().width);
            const float NEW_SCALE(TARGET_LENGTH / CURRENT_LENGTH);
            barImage_.GetUpSprite().setScale(NEW_SCALE, 1.0f);

            //ensure given length is long enough
            //const float MIN_LENGTH((topOrRightImage_.GetUpSprite().getLocalBounds().height + botOrLeftImage_.GetUpSprite().getLocalBounds().height + (padImage_.GetUpSprite().getLocalBounds().height * 2.0f)));
            //M_ASSERT_OR_LOGANDTHROW_SS((LENGTH_ >= MIN_LENGTH), "SliderBar(vertical) with length=" << LENGTH_ << " is too short!  (min = " << MIN_LENGTH << ")");
        }//end if (orientation)
    }


    void SliderBar::SetEntityRegions()
    {
        //set the GuiEntity regions, which should match the sprite_ global bounds
        topOrRightImage_.SetEntityRegion(topOrRightImage_.GetUpSprite().getGlobalBounds());
        botOrLeftImage_.SetEntityRegion(botOrLeftImage_.GetUpSprite().getGlobalBounds());
        barImage_.SetEntityRegion(barImage_.GetUpSprite().getGlobalBounds());
        padImage_.SetEntityRegion(padImage_.GetUpSprite().getGlobalBounds());
    }


    bool SliderBar::MouseWheel(const sf::Vector2f & MOUSE_POS_V, const float WHEEL_MOTION)
    {
        if (IsInEntityRegion(MOUSE_POS_V))
        {
            if (WHEEL_MOTION < 0.0f)
                currentVal_ -= 0.02f;
            else
                currentVal_ += 0.02f;

            SetupAllPositions();
            return true;
        }
        else
            return false;
    }


    //position sprites at either end of the bar
    void SliderBar::SetupAllPositions()
    {
        const sf::Vector2f POS_V(GetEntityPos());

        if (STYLE_.orientation == Orientation::Horiz)
        {
            botOrLeftImage_.GetUpSprite().setPosition(POS_V);
            topOrRightImage_.GetUpSprite().setPosition(POS_V.x + LENGTH_ - topOrRightImage_.GetUpSprite().getLocalBounds().width, POS_V.y);
            barImage_.GetUpSprite().setPosition(POS_V.x + (botOrLeftImage_.GetUpSprite().getLocalBounds().width * 0.5f), POS_V.y + (botOrLeftImage_.GetUpSprite().getLocalBounds().height * 0.5f) - (barImage_.GetUpSprite().getLocalBounds().height * 0.5f));
        }
        else
        {
            botOrLeftImage_.GetUpSprite().setPosition(POS_V.x, POS_V.y + LENGTH_ - botOrLeftImage_.GetUpSprite().getLocalBounds().height);
            topOrRightImage_.GetUpSprite().setPosition(POS_V.x, POS_V.y);
            barImage_.GetUpSprite().setPosition(POS_V.x + (topOrRightImage_.GetUpSprite().getLocalBounds().width * 0.5f) + (barImage_.GetUpSprite().getLocalBounds().height / 3.0f), POS_V.y + (topOrRightImage_.GetUpSprite().getLocalBounds().height * 0.5f));
            barImage_.GetUpSprite().setPosition(POS_V.x + (barImage_.GetUpSprite().getGlobalBounds().width * 0.5f), POS_V.y + LENGTH_ - 10.0f);
        }

        SetupEntityRegion();
        SetPadPosition();
        SetEntityRegions();
        OnChange(currentVal_);
    }


    void SliderBar::SetPadPosition()
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
            const float SLIDER_POSX(POS_V.x + botOrLeftImage_.GetUpSprite().getLocalBounds().width + (SLIDER_RANGE * currentVal_) - 3);//magic number 3 moves passed the shadow
            padImage_.SetEntityPos(SLIDER_POSX, POS_V.y + (botOrLeftImage_.GetUpSprite().getLocalBounds().height * 0.5f) - (padImage_.GetUpSprite().getLocalBounds().height * 0.5f));
        }
        else
        {
            const float SLIDER_RANGE(LENGTH_ - (topOrRightImage_.GetUpSprite().getLocalBounds().height + botOrLeftImage_.GetUpSprite().getLocalBounds().height + padImage_.GetUpSprite().getLocalBounds().height - 5));
            const float SLIDER_POSY(POS_V.y + topOrRightImage_.GetUpSprite().getLocalBounds().height + (SLIDER_RANGE * currentVal_) - 1);//magic number 1 moves passed the shadow
            padImage_.SetEntityPos(POS_V.x + (topOrRightImage_.GetUpSprite().getLocalBounds().width * 0.5f) - (padImage_.GetUpSprite().getLocalBounds().width * 0.5f), SLIDER_POSY);
        }
    }


    bool SliderBar::MouseDown(const sf::Vector2f & MOUSE_POS_V)
    {
        if (botOrLeftImage_.MouseDown(MOUSE_POS_V))
        {
            if (STYLE_.orientation == Orientation::Horiz)
            {
                SoundManager::Instance()->StaticSounds_TickOff()->PlayRandom();
                currentVal_ -= 0.1f;
            }
            else
            {
                SoundManager::Instance()->StaticSounds_TickOn()->PlayRandom();
                currentVal_ += 0.1f;
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
                    SoundManager::Instance()->StaticSounds_TickOn()->PlayRandom();
                    currentVal_ += 0.1f;
                }
                else
                {
                    SoundManager::Instance()->StaticSounds_TickOff()->PlayRandom();
                    currentVal_ -= 0.1f;
                }

                SetupAllPositions();
                return true;
            }
            else
            {
                if (padImage_.MouseDown(MOUSE_POS_V))
                {
                    SoundManager::Instance()->StaticSounds_TickOn()->PlayRandom();
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
                                SoundManager::Instance()->StaticSounds_TickOff()->PlayRandom();
                                currentVal_ -= 0.1f;
                                SetupAllPositions();
                                return true;
                            }
                            else
                            {
                                if (MOUSE_POS_V.x > (padImage_.GetEntityRegion().left + padImage_.GetEntityRegion().width))
                                {
                                    SoundManager::Instance()->StaticSounds_TickOn()->PlayRandom();
                                    currentVal_ += 0.1f;
                                    SetupAllPositions();
                                    return true;
                                }
                            }
                        }
                        else
                        {
                            if (MOUSE_POS_V.y < padImage_.GetEntityRegion().top)
                            {
                                SoundManager::Instance()->StaticSounds_TickOff()->PlayRandom();
                                currentVal_ -= 0.1f;
                                SetupAllPositions();
                                return true;
                            }
                            else
                            {
                                if (MOUSE_POS_V.y > (padImage_.GetEntityRegion().top + padImage_.GetEntityRegion().height))
                                {
                                    SoundManager::Instance()->StaticSounds_TickOn()->PlayRandom();
                                    currentVal_ += 0.1f;
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


    bool SliderBar::MouseUp(const sf::Vector2f & MOUSE_POS_V)
    {
        if (padImage_.MouseUp(MOUSE_POS_V))
        {
            SoundManager::Instance()->StaticSounds_TickOff()->PlayRandom();
            entityMouseState_ = MouseState::Up;
        }

        return false;
    }


    bool SliderBar::UpdateMousePos(const sf::Vector2f & MOUSE_POS_V)
    {
        if (padImage_.IsHeldDown()) //represents the pad
        {
            SetCurrentValueFromScreenCoords(MOUSE_POS_V);
        }

        return false;
    }


    void SliderBar::SetEntityPos(const float POS_LEFT, const float POS_TOP)
    {
        GuiEntity::SetEntityPos(POS_LEFT, POS_TOP);
        SetupAllPositions();
    }


    void SliderBar::MoveEntityPos(const float HORIZ, const float VERT)
    {
        GuiEntity::MoveEntityPos(HORIZ, VERT);
        SetupAllPositions();
    }

}
}
