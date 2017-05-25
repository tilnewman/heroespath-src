// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
//
// radio-button.cpp
//
#include "radio-button.hpp"

#include "sfml-util/gui/gui-elements.hpp"
#include "sfml-util/gui/color-set.hpp"
#include "sfml-util/sound-manager.hpp"

#include "misc/assertlogandthrow.hpp"
#include "game/log-macros.hpp"

#include <sstream>


namespace sfml_util
{
namespace gui
{


    const sf::Color RadioButton::COLOR_VALID_  (255, 255, 255, 255);
    const sf::Color RadioButton::COLOR_INVALID_(255, 255, 255, 127);


    RadioButton::RadioButton(const std::string &    NAME,
                             const Brightness::Enum BRIGHTNESS,
                             const float            POS_LEFT,
                             const float            POS_TOP,
                             const sf::Sprite &     SPRITE_UP,
                             const sf::Sprite &     SPRITE_DOWN,
                             const MouseTextInfo &  MOUSE_TEXT_INFO,
                             const bool             IS_SELECTED,
                             const bool             IS_INVALID)
    :
        TwoStateEntity( std::string(NAME).append("_RadioButton"),
                        POS_LEFT,
                        POS_TOP,
                        SPRITE_UP,
                        SPRITE_DOWN,
                        MOUSE_TEXT_INFO )
    {
        if (IS_SELECTED)
            SetIsInFirstState(false);

        SetupSprites(BRIGHTNESS);
        Setup(POS_LEFT, POS_TOP, IS_INVALID);
    }


    RadioButton::RadioButton(const std::string &    NAME,
                             const Brightness::Enum BRIGHTNESS,
                             const float            POS_LEFT,
                             const float            POS_TOP,
                             const MouseTextInfo &  MOUSE_TEXT_INFO,
                             const bool             IS_SELECTED,
                             const bool             IS_INVALID)
    :
        TwoStateEntity( std::string(NAME).append("_RadioButton"),
                        POS_LEFT,
                        POS_TOP,
                        MOUSE_TEXT_INFO )
    {
        if (IS_SELECTED)
            SetIsInFirstState(false);

        SetupSprites(BRIGHTNESS);
        Setup(POS_LEFT, POS_TOP, IS_INVALID);
    }


    RadioButton::RadioButton(const std::string &    NAME,
                             const Brightness::Enum BRIGHTNESS,
                             const float            POS_LEFT,
                             const float            POS_TOP,
                             const TextInfo &       TEXT_INFO_UP,
                             const bool             IS_SELECTED,
                             const bool             IS_INVALID)
    :
        TwoStateEntity( std::string(NAME).append("_RadioButton"),
                        POS_LEFT,
                        POS_TOP,
                        MouseTextInfo(TEXT_INFO_UP) )
    {
        if (IS_SELECTED)
            SetIsInFirstState(false);

        SetupSprites(BRIGHTNESS);
        Setup(POS_LEFT, POS_TOP, IS_INVALID);
    }


    RadioButton::~RadioButton()
    {}


    void RadioButton::Setup(const float POS_LEFT, const float POS_TOP, const bool IS_INVALID)
    {
        guiImage_.Setup(POS_LEFT, POS_TOP);
        SetValidity(! IS_INVALID);
        PostSpriteSetup(POS_LEFT, POS_TOP);
    }


    void RadioButton::SetValidity(const bool IS_VALID)
    {
        if (IS_VALID)
        {
            guiImage_.SpriteColorsSet(COLOR_VALID_);
            guiText_.SpriteColorSet(COLOR_VALID_);
        }
        else
        {
            guiImage_.SpriteColorsSet(COLOR_INVALID_);
            guiText_.SpriteColorSet(COLOR_INVALID_);
        }
    }


    void RadioButton::SetupSprites(const Brightness::Enum BRIGHTNESS)
    {
        auto const GE_PTR{ GuiElements::Instance() };

        guiImage_.GetUpSprite().setTexture( * GE_PTR->GetTexture());
        guiImage_.GetDownSprite().setTexture( * GE_PTR->GetTexture());

        switch (BRIGHTNESS)
        {
            case Brightness::Dark:
            {
                guiImage_.GetUpSprite().setTextureRect(GE_PTR->GetRect_RadioButtonDarkOff());
                guiImage_.GetDownSprite().setTextureRect(GE_PTR->GetRect_RadioButtonDarkOn());
                break;
            }
            case Brightness::Medium:
            {
                guiImage_.GetUpSprite().setTextureRect(GE_PTR->GetRect_RadioButtonMedOff());
                guiImage_.GetDownSprite().setTextureRect(GE_PTR->GetRect_RadioButtonMedOn());
                break;
            }
            case Brightness::Bright:
            case Brightness::Count:
            default:
            {
                guiImage_.GetUpSprite().setTextureRect(GE_PTR->GetRect_RadioButtonBrightOff());
                guiImage_.GetDownSprite().setTextureRect(GE_PTR->GetRect_RadioButtonBrightOn());
                break;
            }
        }
    }


    const float RadioButtonSet::OUTER_PAD_DEFAULT_  (6.0f);
    const float RadioButtonSet::BETWEEN_PAD_DEFAULT_(6.0f);


    RadioButtonSet::RadioButtonSet(const std::string & NAME)
    :
        GuiEntity           (std::string(NAME).append("_RadioButtonSet"), 0.0f, 0.0f),
        outerPad_           (OUTER_PAD_DEFAULT_),
        betweenPad_         (BETWEEN_PAD_DEFAULT_),
        currentSelection_   (0),
        buttonSVec_         (),
        box_                (std::string(GetEntityName()).append("'s")),
        downInWhichRegion_  (0),
        invalidSelectionVec_(),
        prevSelection_      (0),
        callbackHandlerPtrSet_()
    {}


    RadioButtonSet::RadioButtonSet( const std::string &    NAME,
                                    const float            POS_LEFT,
                                    const float            POS_TOP,
                                    const TextInfoVec_t &  TEXT_INFO_VEC,
                                    const std::size_t      INITIAL_SELECTION,
                                    const Brightness::Enum BRIGHTNESS,
                                    const IndexVec_t &     INVALID_SEL_VEC,
                                    const box::Info &      BOX_INFO,
                                    const float            OUTER_PAD,
                                    const float            BETWEEN_PAD)
    :
        GuiEntity           (std::string(NAME).append("_RadioButtonSet"), POS_LEFT, POS_TOP),
        outerPad_           (OUTER_PAD),
        betweenPad_         (BETWEEN_PAD),
        currentSelection_   (0),
        buttonSVec_         (),
        box_                (std::string(GetEntityName()).append("'s")),
        downInWhichRegion_  (TEXT_INFO_VEC.size()),//set to an out-of-range value to mean 'not down in any region'
        invalidSelectionVec_(INVALID_SEL_VEC),
        prevSelection_      (TEXT_INFO_VEC.size()),//set original value to invalid out-of-range
        callbackHandlerPtrSet_()
    {
        Setup(POS_LEFT,
              POS_TOP,
              TEXT_INFO_VEC,
              INITIAL_SELECTION,
              BRIGHTNESS,
              INVALID_SEL_VEC,
              BOX_INFO,
              outerPad_,
              betweenPad_);
    }


    RadioButtonSet::RadioButtonSet( const std::string &        NAME,
                                    const float                POS_LEFT,
                                    const float                POS_TOP,
                                    const MouseTextInfoVec_t & MOUSE_TEXT_INFO_VEC,
                                    const std::size_t          INITIAL_SELECTION,
                                    const Brightness::Enum     BRIGHTNESS,
                                    const IndexVec_t &         INVALID_SEL_VEC,
                                    const box::Info &          BOX_INFO,
                                    const float                OUTER_PAD,
                                    const float                BETWEEN_PAD )
    :
        GuiEntity           (std::string(NAME).append("_RadioButtonSet"), POS_LEFT, POS_TOP),
        outerPad_           (OUTER_PAD),
        betweenPad_         (BETWEEN_PAD),
        currentSelection_   (INITIAL_SELECTION),
        buttonSVec_         (),
        box_                (std::string(GetEntityName()).append("'s")),
        downInWhichRegion_  (MOUSE_TEXT_INFO_VEC.size()),//set to an out-of-range value to mean 'not down in any region'
        invalidSelectionVec_(INVALID_SEL_VEC),
        prevSelection_      (MOUSE_TEXT_INFO_VEC.size()),//set original value to invalid out-of-range
        callbackHandlerPtrSet_()
    {
        Setup(POS_LEFT,
              POS_TOP,
              MOUSE_TEXT_INFO_VEC,
              INITIAL_SELECTION,
              BRIGHTNESS,
              INVALID_SEL_VEC,
              BOX_INFO,
              outerPad_,
              betweenPad_);
    }


    RadioButtonSet::RadioButtonSet( const std::string &              NAME,
                                    const float                      POS_LEFT,
                                    const float                      POS_TOP,
                                    const TextInfo &                 TEXT_INFO,
                                    const std::vector<std::string> & LABEL_VEC,
                                    const std::size_t                INITIAL_SELECTION,
                                    const Brightness::Enum           BRIGHTNESS,
                                    const IndexVec_t &               INVALID_SEL_VEC,
                                    const box::Info &                BOX_INFO,
                                    const float                      OUTER_PAD,
                                    const float                      BETWEEN_PAD)
    :
        GuiEntity           (std::string(NAME).append("_RadioButtonSet"), POS_LEFT, POS_TOP),
        outerPad_           (OUTER_PAD),
        betweenPad_         (BETWEEN_PAD),
        currentSelection_   (INITIAL_SELECTION),
        buttonSVec_         (),
        box_                (std::string(GetEntityName()).append("'s")),
        downInWhichRegion_  (LABEL_VEC.size()),//set to an out-of-range value to mean 'not down in any region'
        invalidSelectionVec_(INVALID_SEL_VEC),
        prevSelection_      (LABEL_VEC.size()),//set original value to invalid out-of-range
        callbackHandlerPtrSet_()
    {
        Setup(POS_LEFT,
              POS_TOP,
              TEXT_INFO,
              LABEL_VEC,
              INITIAL_SELECTION,
              BRIGHTNESS,
              INVALID_SEL_VEC,
              BOX_INFO,
              outerPad_,
              betweenPad_);
    }


    RadioButtonSet::~RadioButtonSet()
    {}


    void RadioButtonSet::Setup(const float            POS_LEFT,
                               const float            POS_TOP,
                               const TextInfoVec_t &  TEXT_INFO_VEC,
                               const std::size_t      INITIAL_SELECTION,
                               const Brightness::Enum BRIGHTNESS,
                               const IndexVec_t &     INVALID_SEL_VEC,
                               const box::Info &      BOX_INFO,
                               const float            OUTER_PAD,
                               const float            BETWEEN_PAD)
    {
        outerPad_   = OUTER_PAD;
        betweenPad_ = BETWEEN_PAD;

        currentSelection_ = INITIAL_SELECTION;
        invalidSelectionVec_ = INVALID_SEL_VEC;

        //set to an out-of-range value to mean 'not down in any region'
        downInWhichRegion_ = TEXT_INFO_VEC.size();

        const std::size_t NUM_BUTTONS(TEXT_INFO_VEC.size());

        M_ASSERT_OR_LOGANDTHROW_SS((NUM_BUTTONS > 0), entityName_ << " given an empty vector of TextInfo objects.");
        M_ASSERT_OR_LOGANDTHROW_SS((INITIAL_SELECTION < NUM_BUTTONS), entityName_ << " given an initial selection value larger than the number of buttons=" << NUM_BUTTONS << ".");

        for (std::size_t i(0); i < NUM_BUTTONS; ++i)
        {
            std::ostringstream ss;
            ss << GetEntityName() << "'s #" << i + 1;
            buttonSVec_.push_back( RadioButtonSPtr_t( new RadioButton(ss.str(),
                                                                      BRIGHTNESS,
                                                                      POS_LEFT + OUTER_PAD,
                                                                      POS_TOP + OUTER_PAD + (static_cast<float>(i) * BETWEEN_PAD),
                                                                      TEXT_INFO_VEC[i],
                                                                      ((INITIAL_SELECTION == i) ? true : false),
                                                                      IsInvalid(i)) ) );

        }

        buttonSVec_[INITIAL_SELECTION]->SetIsInFirstState(false);
        PostSetupTasks(POS_LEFT, POS_TOP, BOX_INFO);
    }


    void RadioButtonSet::Setup( const float                POS_LEFT,
                                const float                POS_TOP,
                                const MouseTextInfoVec_t & MOUSE_TEXT_INFO_VEC,
                                const std::size_t          INITIAL_SELECTION,
                                const Brightness::Enum     BRIGHTNESS,
                                const IndexVec_t &         INVALID_SEL_VEC,
                                const box::Info &          BOX_INFO,
                                const float                OUTER_PAD,
                                const float                BETWEEN_PAD )
    {
        outerPad_ = OUTER_PAD;
        betweenPad_ = BETWEEN_PAD;

        currentSelection_ = INITIAL_SELECTION;
        invalidSelectionVec_ = INVALID_SEL_VEC;

        //set to an out-of-range value to mean 'not down in any region'
        downInWhichRegion_ = MOUSE_TEXT_INFO_VEC.size();

        const std::size_t NUM_BUTTONS(MOUSE_TEXT_INFO_VEC.size());

        M_ASSERT_OR_LOGANDTHROW_SS((NUM_BUTTONS > 0), entityName_ << " given an empty vector of string labels.");
        M_ASSERT_OR_LOGANDTHROW_SS((INITIAL_SELECTION < NUM_BUTTONS), entityName_ << " given an initial selection value larger than the number of buttons=" << NUM_BUTTONS << ".");

        for (std::size_t i(0); i < NUM_BUTTONS; ++i)
        {
            std::ostringstream ss;
            ss << GetEntityName() << "'s #" << i + 1;
            buttonSVec_.push_back(RadioButtonSPtr_t(new RadioButton(ss.str(),
                                                                    BRIGHTNESS,
                                                                    POS_LEFT + OUTER_PAD,
                                                                    POS_TOP + OUTER_PAD + (static_cast<float>(i) * BETWEEN_PAD),
                                                                    MouseTextInfo(MOUSE_TEXT_INFO_VEC[i].up, MOUSE_TEXT_INFO_VEC[i].down, MOUSE_TEXT_INFO_VEC[i].over),
                                                                    ((INITIAL_SELECTION == i) ? true : false),
                                                                    IsInvalid(i)) ) );
        }

        buttonSVec_[INITIAL_SELECTION]->SetIsInFirstState(false);
        PostSetupTasks(POS_LEFT, POS_TOP, BOX_INFO);
    }


    void RadioButtonSet::Setup( const float                      POS_LEFT,
                                const float                      POS_TOP,
                                const TextInfo &                 TEXT_INFO,
                                const std::vector<std::string> & LABEL_VEC,
                                const std::size_t                INITIAL_SELECTION,
                                const Brightness::Enum           BRIGHTNESS,
                                const IndexVec_t &               INVALID_SEL_VEC,
                                const box::Info &                BOX_INFO,
                                const float                      OUTER_PAD,
                                const float                      BETWEEN_PAD )
    {
        outerPad_ = OUTER_PAD;
        betweenPad_ = BETWEEN_PAD;

        currentSelection_ = INITIAL_SELECTION;
        invalidSelectionVec_ = INVALID_SEL_VEC;

        //set to an out-of-range value to mean 'not down in any region'
        downInWhichRegion_ = LABEL_VEC.size();

        const std::size_t NUM_BUTTONS(LABEL_VEC.size());

        M_ASSERT_OR_LOGANDTHROW_SS((NUM_BUTTONS > 0), entityName_ << " given an empty vector of string labels.");
        M_ASSERT_OR_LOGANDTHROW_SS((INITIAL_SELECTION < NUM_BUTTONS), entityName_ << " given an initial selection value larger than the number of buttons=" << NUM_BUTTONS << ".");

        for (std::size_t i(0); i < NUM_BUTTONS; ++i)
        {
            std::ostringstream ss;
            ss << GetEntityName() << "'s #" << i + 1;

            sfml_util::gui::TextInfo nextTextInfo(TEXT_INFO);
            nextTextInfo.text = LABEL_VEC[i];

            buttonSVec_.push_back( RadioButtonSPtr_t( new RadioButton(ss.str(),
                                                                      BRIGHTNESS,
                                                                      POS_LEFT + OUTER_PAD,
                                                                      POS_TOP + OUTER_PAD + (static_cast<float>(i) * BETWEEN_PAD),
                                                                      nextTextInfo,
                                                                      ((INITIAL_SELECTION == i) ? true : false),
                                                                      IsInvalid(i)) ) );
        }

        buttonSVec_[INITIAL_SELECTION]->SetIsInFirstState(false);
        PostSetupTasks(POS_LEFT, POS_TOP, BOX_INFO);
    }


    bool RadioButtonSet::MouseUp(const sf::Vector2f & MOUSE_POS_V)
    {
        bool wasButtonClicked(false);

        const std::size_t ORIGINAL_SELECTION(currentSelection_);

        const std::size_t NUM_BUTTONS(buttonSVec_.size());
        for (std::size_t i(0); i < NUM_BUTTONS; ++i)
        {
            if (buttonSVec_[i]->GetEntityRegion().contains(MOUSE_POS_V) && (downInWhichRegion_ == i))
            {
                if ((ORIGINAL_SELECTION != i) && (false == IsInvalid(i)))
                {
                    SoundManager::Instance()->SoundEffectsSet_Thock()->PlayRandom();
                    currentSelection_ = i;
                    buttonSVec_[i]->SetIsInFirstState(false);
                    wasButtonClicked = true;
                    OnClick(MOUSE_POS_V);
                    break;
                }
            }
        }

        if (true == wasButtonClicked)
        {
            prevSelection_ = ORIGINAL_SELECTION;
            buttonSVec_[ORIGINAL_SELECTION]->SetIsInFirstState(true);
        }

        downInWhichRegion_ = buttonSVec_.size();//reset to an invalid value
        return wasButtonClicked;
    }


    bool RadioButtonSet::MouseDown(const sf::Vector2f & MOUSE_POS_V)
    {
        const std::size_t NUM_BUTTONS(buttonSVec_.size());
        for (std::size_t i(0); i < NUM_BUTTONS; ++i)
        {
            if (buttonSVec_[i]->GetEntityRegion().contains(MOUSE_POS_V))
            {
                downInWhichRegion_ = i;
                return true;
            }
        }

        downInWhichRegion_ = buttonSVec_.size();//reset to an invalid value
        return false;
    }


    bool RadioButtonSet::IsInvalid(const std::size_t INDEX) const
    {
        const std::size_t NUM_INVALIDS(invalidSelectionVec_.size());
        for (std::size_t i(0); i < NUM_INVALIDS; ++i)
            if (invalidSelectionVec_[i] == INDEX)
                return true;

        return false;
    }


    void RadioButtonSet::PostSetupTasks(const float       POS_LEFT,
                                        const float       POS_TOP,
                                        const box::Info & BOX_INFO)
    {
        //The RadioButtonSet has little need for these colors,
        //but when they change they are used to set the backgrond/box
        //(see OnColorChange() below), so this is important.
        SetEntityColors(BOX_INFO.color_set);

        PositionRadioButtons(POS_LEFT, POS_TOP);

        //setup box
        box::Info boxInfo(BOX_INFO);
        boxInfo.SetBoxAndBackgroundRegion(GetEntityRegion());
        box_.SetupBox(boxInfo);
    }


    void RadioButtonSet::PositionRadioButtons(const float POS_LEFT, const float POS_TOP)
    {
        //set the vertical positions of the radio buttons and find the maxX position
        float maxX(buttonSVec_[0]->GetEntityRegion().width + (outerPad_ * 2.0f));
        float posY(POS_TOP + outerPad_);

        buttonSVec_[0]->Setup(POS_LEFT + outerPad_, posY, IsInvalid(0));

        const std::size_t NUM_BUTTONS(buttonSVec_.size());
        for (std::size_t i(1); i < NUM_BUTTONS; ++i)
        {
            posY += buttonSVec_[i - 1]->GetEntityRegion().height + betweenPad_;
            buttonSVec_[i]->Setup(POS_LEFT + outerPad_, posY, IsInvalid(i));

            const float NEXT_HORIZ_LIMIT(buttonSVec_[i]->GetEntityRegion().width + (outerPad_ * 2.0f));
            if (maxX < NEXT_HORIZ_LIMIT)
                maxX = NEXT_HORIZ_LIMIT;
        }
        posY += buttonSVec_[buttonSVec_.size() - 1]->GetEntityRegion().height;

        //set the overall rect encompasing all radio buttons with their text
        SetEntityRegion(sf::FloatRect(POS_LEFT, POS_TOP, maxX, (posY - POS_TOP) + outerPad_));
    }


    void RadioButtonSet::RevertToPreviousSelection()
    {
        buttonSVec_[prevSelection_]->SetIsInFirstState(false);
        buttonSVec_[currentSelection_]->SetIsInFirstState(true);
        currentSelection_ = prevSelection_;
    }


    void RadioButtonSet::ChangeCurrentSelection(const std::size_t NEW_SELECTION)
    {
        prevSelection_ = currentSelection_;
        buttonSVec_[currentSelection_]->SetIsInFirstState(true);
        buttonSVec_[NEW_SELECTION]->SetIsInFirstState(false);
        currentSelection_ = NEW_SELECTION;
    }


    void RadioButtonSet::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        target.draw(box_, states);

        const std::size_t NUM_BUTTONS(buttonSVec_.size());
        for (std::size_t i(0); i < NUM_BUTTONS; ++i)
            buttonSVec_[i]->draw(target, states);
    }


    void RadioButtonSet::SetSelectNumber(const std::size_t NEW_VAL)
    {
        if (NEW_VAL >= buttonSVec_.size())
        {
            std::ostringstream ss;
            ss << "RadioButtonSet::SetSelectNumber(" << NEW_VAL << ") is out of range from the max of " << buttonSVec_.size() << ".";
            throw std::out_of_range(ss.str());
        }

        buttonSVec_[currentSelection_]->SetIsInFirstState(true);
        currentSelection_ = NEW_VAL;
        buttonSVec_[currentSelection_]->SetIsInFirstState(false);
        TriggerCallback();
    }


    void RadioButtonSet::SetInvalidSelections(const IndexVec_t & INVALID_SELECTIONS_VEC)
    {
        invalidSelectionVec_ = INVALID_SELECTIONS_VEC;

        const std::size_t ORIG_CURR_SEL(currentSelection_);

        bool isCurrentSelectionValid(false);
        bool hasSetFirstValid(false);
        const std::size_t NUM_BUTTONS(buttonSVec_.size());
        for (std::size_t i(0); i < NUM_BUTTONS; ++i)
        {
            const bool IS_VALID(!IsInvalid(i));

            if ((IS_VALID) && (ORIG_CURR_SEL == i))
                isCurrentSelectionValid = true;

            if ((IS_VALID) && (false == hasSetFirstValid))
            {
                buttonSVec_[i]->SetIsInFirstState(false);
                currentSelection_ = i;
                hasSetFirstValid = true;
            }
            else
                buttonSVec_[i]->SetIsInFirstState(true);

            buttonSVec_[i]->SetValidity(IS_VALID);
        }

        //restore original selection if it is still valid
        if (isCurrentSelectionValid)
        {
            for (std::size_t i(0); i < NUM_BUTTONS; ++i)
                buttonSVec_[i]->SetIsInFirstState(true);

            buttonSVec_[ORIG_CURR_SEL]->SetIsInFirstState(false);
            currentSelection_ = ORIG_CURR_SEL;
        }
    }


    bool RadioButtonSet::UpdateMousePos(const sf::Vector2f & MOUSE_POS_V)
    {
        bool didStatesChange(false);

        const std::size_t NUM_BUTTONS(buttonSVec_.size());
        for (std::size_t i(0); i < NUM_BUTTONS; ++i)
        {
            const bool IS_IN_REGION(buttonSVec_[i]->GetEntityRegion().contains(MOUSE_POS_V));
            const MouseState::Enum MOUSE_STATE(buttonSVec_[i]->GetMouseState());

            if (IS_IN_REGION                      &&
                (MOUSE_STATE != MouseState::Down) &&
                (currentSelection_ != i))
            {
                if (buttonSVec_[i]->GetMouseState() != MouseState::Over)
                    SoundManager::Instance()->SoundEffectsSet_TickOn()->PlayRandom();

                buttonSVec_[i]->SetMouseState(MouseState::Over);
                didStatesChange = true;
                break;
            }

            if ((false == IS_IN_REGION)           &&
                (MOUSE_STATE != MouseState::Down) &&
                (currentSelection_ != i))
            {
                buttonSVec_[i]->SetMouseState(MouseState::Up);
                didStatesChange = true;
            }
        }

        return didStatesChange;
    }


    void RadioButtonSet::SetEntityPos(const float POS_LEFT, const float POS_TOP)
    {
        GuiEntity::SetEntityPos(POS_LEFT, POS_TOP);

        PositionRadioButtons(POS_LEFT, POS_TOP);

        box_.SetEntityPos(POS_LEFT, POS_TOP);
    }


    void RadioButtonSet::MoveEntityPos(const float HORIZ, const float VERT)
    {
        GuiEntity::MoveEntityPos(HORIZ, VERT);

        const std::size_t NUM_BUTTONS(buttonSVec_.size());
        for (std::size_t i(0); i < NUM_BUTTONS; ++i)
            buttonSVec_[i]->MoveEntityPos(HORIZ, VERT);

        box_.MoveEntityPos(HORIZ, VERT);
    }


    void RadioButtonSet::OnClick(const sf::Vector2f &)
    {
        TriggerCallback();
    }


    void RadioButtonSet::TriggerCallback()
    {
        for (auto handlerPtr : callbackHandlerPtrSet_)
        {
            M_ASSERT_OR_LOGANDTHROW_SS((handlerPtr != nullptr), "sfml_util::gui::RadioButtonSet::TriggerCallback() had a nullptr in the callbackHandlerPtrSet_.");
            handlerPtr->HandleCallback(this);
        }
    }


    void RadioButtonSet::OnColorChange()
    {
        box_.SetEntityColors(entityColorSet_);
    }


    void RadioButtonSet::CallbackHandlerAdd(sfml_util::callback::IRadioButtonSetCallbackHandler_t * const handlerPtr)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((handlerPtr != nullptr), "sfml_util::gui::RadioButtonSet::HandlerAdd() was given a nullptr.");
        callbackHandlerPtrSet_.insert(handlerPtr);
    }


    bool RadioButtonSet::CallbackHandlerRemove(sfml_util::callback::IRadioButtonSetCallbackHandler_t * const handlerPtr)
    {
        return (1 == callbackHandlerPtrSet_.erase(handlerPtr));
    }


    bool RadioButtonSet::SetHasFocus(const bool HAS_FOCUS)
    {
        const bool DID_FOCUS_CHANGE(GuiEntity::SetHasFocus(HAS_FOCUS));

        box_.SetHasFocus(HAS_FOCUS);

        return DID_FOCUS_CHANGE;
    }

}
}
