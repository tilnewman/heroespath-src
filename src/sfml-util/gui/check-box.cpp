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
// check-box.cpp
//
#include "check-box.hpp"

#include "sfml-util/gui/gui-elements.hpp"
#include "sfml-util/gui/color-set.hpp"
#include "sfml-util/sound-manager.hpp"

#include <sstream>

#include "misc/assertlogandthrow.hpp"


namespace sfml_util
{
namespace gui
{

    CheckBox::CheckBox( const std::string &    NAME,
                        const Brightness::Enum BRIGHTNESS,
                        const float            POS_LEFT,
                        const float            POS_TOP,
                        const sf::Sprite &     SPRITE_UP,
                        const sf::Sprite &     SPRITE_DOWN,
                        const MouseTextInfo &  MOUSE_TEXT_INFO,
                        const bool             IS_CHECKED,
                        const bool             IS_INVALID)//TODO why no call to Setup() in this constructor?  zTn 2016-10-9
    :
        TwoStateEntity( std::string(NAME).append("_CheckBox"),
                        POS_LEFT,
                        POS_TOP,
                        SPRITE_UP,
                        SPRITE_DOWN,
                        MOUSE_TEXT_INFO)
    {
        if (IS_CHECKED)
            SetIsInFirstState(false);

        SetupSprites(BRIGHTNESS);
        Setup(POS_LEFT, POS_TOP, IS_INVALID);
    }


    CheckBox::CheckBox( const std::string &    NAME,
                        const Brightness::Enum BRIGHTNESS,
                        const float            POS_LEFT,
                        const float            POS_TOP,
                        const MouseTextInfo &  MOUSE_TEXT_INFO,
                        const bool             IS_CHECKED,
                        const bool             IS_INVALID)//TODO why no call to Setup() in this constructor?  zTn 2016-10-9
    :
        TwoStateEntity( std::string(NAME).append("_CheckBox"),
                        POS_LEFT,
                        POS_TOP,
                        MOUSE_TEXT_INFO )
    {
        if (IS_CHECKED)
            SetIsInFirstState(false);

        SetupSprites(BRIGHTNESS);
        Setup(POS_LEFT, POS_TOP, IS_INVALID);
    }


    CheckBox::CheckBox(const std::string &    NAME,
                       const Brightness::Enum BRIGHTNESS,
                       const float            POS_LEFT,
                       const float            POS_TOP,
                       const TextInfo &       TEXT_INFO_UP,
                       const bool             IS_CHECKED,
                       const bool             IS_INVALID)//TODO why no call to Setup() in this constructor?  zTn 2016-10-9
    :
        TwoStateEntity( std::string(NAME).append("_CheckBox"),
                        POS_LEFT,
                        POS_TOP,
                        MouseTextInfo(TEXT_INFO_UP) )
    {
        if (IS_CHECKED)
            SetIsInFirstState(false);

        SetupSprites(BRIGHTNESS);
        Setup(POS_LEFT, POS_TOP, IS_INVALID);
    }


    CheckBox::~CheckBox()
    {}


    void CheckBox::Setup(const float POS_LEFT, const float POS_TOP, const bool IS_INVALID)
    {
        guiImage_.Setup(POS_LEFT, POS_TOP);

        if (IS_INVALID)
        {
            const sf::Color INVALID_COLOR(sf::Color(255, 255, 255, 127));
            guiImage_.SpriteColorsSet(INVALID_COLOR);
            guiText_.SpriteColorSet(INVALID_COLOR);
        }

        PostSpriteSetup(POS_LEFT, POS_TOP);
    }


    bool CheckBox::MouseUp(const sf::Vector2f & MOUSE_POS_V)
    {
        return TwoStateEntity::MouseUp(MOUSE_POS_V);
    }


    bool CheckBox::MouseDown(const sf::Vector2f & MOUSE_POS_V)
    {
        return TwoStateEntity::MouseDown(MOUSE_POS_V);
    }


    void CheckBox::SetupSprites(const Brightness::Enum BRIGHTNESS)
    {
        auto const GE_PTR{ GuiElements::Instance() };

        guiImage_.GetUpSprite().setTexture(GE_PTR->GetTexture());
        guiImage_.GetDownSprite().setTexture(GE_PTR->GetTexture());

        switch (BRIGHTNESS)
        {
            case Brightness::Dark:
            {
                guiImage_.GetUpSprite().setTextureRect(GE_PTR->GetRect_CheckBoxDarkOff());
                guiImage_.GetDownSprite().setTextureRect(GE_PTR->GetRect_CheckBoxDarkOn());
                break;
            }
            case Brightness::Medium:
            {
                guiImage_.GetUpSprite().setTextureRect(GE_PTR->GetRect_CheckBoxMedOff());
                guiImage_.GetDownSprite().setTextureRect(GE_PTR->GetRect_CheckBoxMedOn());
                break;
            }
            case Brightness::Bright:
            case Brightness::Count:
            default:
            {
                guiImage_.GetUpSprite().setTextureRect(GE_PTR->GetRect_CheckBoxBrightOff());
                guiImage_.GetDownSprite().setTextureRect(GE_PTR->GetRect_CheckBoxBrightOn());
                break;
            }
        }
    }


    const float CheckBoxSet::OUTER_PAD_DEFAULT_  (6.0f);
    const float CheckBoxSet::BETWEEN_PAD_DEFAULT_(6.0f);


    CheckBoxSet::CheckBoxSet(const std::string &       NAME,
                             const float               POS_LEFT,
                             const float               POS_TOP,
                             const TextInfoVec_t &     TEXT_INFO_VEC,
                             const Brightness::Enum    BRIGHTNESS,
                             const IndexVec_t &        INVALID_SEL_VEC,
                             const gui::box::Info &    BOX_INFO,
                             const std::deque<bool> &  INITIAL_SELECTIONS,
                             const float               OUTER_PAD,
                             const float               BETWEEN_PAD)
    :
        GuiEntity            (std::string(NAME).append("_CheckBoxSet"), POS_LEFT, POS_TOP),
        outerPad_            (OUTER_PAD),
        betweenPad_          (BETWEEN_PAD),
        downInWhichRegion_   (TEXT_INFO_VEC.size()),
        currentSelections_   (INITIAL_SELECTIONS),
        checkBoxSVec_        (),
        box_                 (std::string(GetEntityName()).append("'s")),
        invalidSelectionsVec_(INVALID_SEL_VEC)
    {
        const std::size_t NUM_CHECKBOXES(TEXT_INFO_VEC.size());

        if (currentSelections_.empty())
            currentSelections_.resize(NUM_CHECKBOXES, false);

        M_ASSERT_OR_LOGANDTHROW_SS((NUM_CHECKBOXES > 0), entityName_ << " given an empty vector of TextInfo objects.");
        M_ASSERT_OR_LOGANDTHROW_SS((currentSelections_.size() == NUM_CHECKBOXES), entityName_ << " given an initial selections vector with more or less values (" << currentSelections_.size() << ") than there are check boxes (" << NUM_CHECKBOXES << ").");

        for (std::size_t i(0); i < NUM_CHECKBOXES; ++i)
        {
            std::ostringstream ss;
            ss << NAME << "'s #" << i + 1;
            checkBoxSVec_.push_back( CheckBoxSPtr_t( new CheckBox(ss.str(),
                                                                  BRIGHTNESS,
                                                                  POS_LEFT + OUTER_PAD,
                                                                  POS_TOP + OUTER_PAD + (static_cast<float>(i) * BETWEEN_PAD),
                                                                  TEXT_INFO_VEC[i],
                                                                  currentSelections_[i],
                                                                  IsInvalid(i)) ) );
        }

        SetupInitialSelections();
        PostSetupTasks(POS_LEFT, POS_TOP, BOX_INFO);
    }


    CheckBoxSet::CheckBoxSet(const std::string &        NAME,
                             const float                POS_LEFT,
                             const float                POS_TOP,
                             const MouseTextInfoVec_t & TEXT_INFO_VEC,
                             const Brightness::Enum     BRIGHTNESS,
                             const IndexVec_t &         INVALID_SEL_VEC,
                             const gui::box::Info &     BOX_INFO,
                             const std::deque<bool> &   INITIAL_SELECTIONS,
                             const float                OUTER_PAD,
                             const float                BETWEEN_PAD)
    :
        GuiEntity            (std::string(NAME).append("_CheckBoxSet"), POS_LEFT, POS_TOP),
        outerPad_            (OUTER_PAD),
        betweenPad_          (BETWEEN_PAD),
        downInWhichRegion_   (TEXT_INFO_VEC.size()),
        currentSelections_   (INITIAL_SELECTIONS),
        checkBoxSVec_        (),
        box_                 (std::string(GetEntityName()).append("'s")),
        invalidSelectionsVec_(INVALID_SEL_VEC)
    {
        const std::size_t NUM_CHECKBOXES(TEXT_INFO_VEC.size());

        if (currentSelections_.empty())
            currentSelections_.resize(NUM_CHECKBOXES, false);

        M_ASSERT_OR_LOGANDTHROW_SS((NUM_CHECKBOXES > 0), entityName_ << " given an empty vector of TextInfo objects.");
        M_ASSERT_OR_LOGANDTHROW_SS((currentSelections_.size() == NUM_CHECKBOXES), entityName_ << " given an initial selections vector with more or less values (" << currentSelections_.size() << ") than there are check boxes (" << NUM_CHECKBOXES << ").");

        for (std::size_t i(0); i < NUM_CHECKBOXES; ++i)
        {
            std::ostringstream ss;
            ss << NAME << "'s #" << i + 1;
            checkBoxSVec_.push_back(CheckBoxSPtr_t(new CheckBox(ss.str(),
                                                                BRIGHTNESS,
                                                                POS_LEFT + OUTER_PAD,
                                                                POS_TOP + OUTER_PAD + (static_cast<float>(i) * BETWEEN_PAD),
                                                                MouseTextInfo(TEXT_INFO_VEC[i].up, TEXT_INFO_VEC[i].down, TEXT_INFO_VEC[i].over),
                                                                currentSelections_[i],
                                                                IsInvalid(i)) ) );
        }

        SetupInitialSelections();
        PostSetupTasks(POS_LEFT, POS_TOP, BOX_INFO);
    }


    CheckBoxSet::CheckBoxSet(const std::string &              NAME,
                             const float                      POS_LEFT,
                             const float                      POS_TOP,
                             const TextInfo &                 TEXT_INFO,
                             const std::vector<std::string> & LABEL_VEC,
                             const Brightness::Enum           BRIGHTNESS,
                             const IndexVec_t &               INVALID_SEL_VEC,
                             const gui::box::Info &           BOX_INFO,
                             const std::deque<bool> &         INITIAL_SELECTIONS,
                             const float                      OUTER_PAD,
                             const float                      BETWEEN_PAD)
    :
        GuiEntity            (std::string(NAME).append("_CheckBoxSet"), POS_LEFT, POS_TOP),
        outerPad_            (OUTER_PAD),
        betweenPad_          (BETWEEN_PAD),
        downInWhichRegion_   (LABEL_VEC.size()),
        currentSelections_   (INITIAL_SELECTIONS),
        checkBoxSVec_        (),
        box_                 (std::string(GetEntityName()).append("'s")),
        invalidSelectionsVec_(INVALID_SEL_VEC)
    {
        const std::size_t NUM_CHECKBOXES(LABEL_VEC.size());

        if (currentSelections_.empty())
            currentSelections_.resize(NUM_CHECKBOXES, false);

        M_ASSERT_OR_LOGANDTHROW_SS((NUM_CHECKBOXES > 0), entityName_ << " given an empty vector of string labels.");
        M_ASSERT_OR_LOGANDTHROW_SS((currentSelections_.size() == NUM_CHECKBOXES), entityName_ << " given an initial selections vector with more or less values (" << currentSelections_.size() << ") than there are check boxes (" << NUM_CHECKBOXES << ").");

        for (std::size_t i(0); i < NUM_CHECKBOXES; ++i)
        {
            std::ostringstream ss;
            ss << NAME << "'s #" << i + 1;
            checkBoxSVec_.push_back( CheckBoxSPtr_t( new CheckBox(ss.str(),
                                                                  BRIGHTNESS,
                                                                  POS_LEFT + OUTER_PAD,
                                                                  POS_TOP + OUTER_PAD + (static_cast<float>(i) * BETWEEN_PAD),
                                                                  TEXT_INFO,
                                                                  currentSelections_[i],
                                                                  IsInvalid(i)) ) );
        }

        SetupInitialSelections();
        PostSetupTasks(POS_LEFT, POS_TOP, BOX_INFO);
    }


    CheckBoxSet::~CheckBoxSet()
    {}


    void CheckBoxSet::SetupInitialSelections()
    {
        const std::size_t NUM_SELECTIONS( currentSelections_.size() );
        for (std::size_t i(0); i < NUM_SELECTIONS; ++i)
            if (currentSelections_[i] == true)
                checkBoxSVec_[i]->SetIsInFirstState(false);
    }


    const std::vector<std::size_t> CheckBoxSet::GetCheckedNumbers() const
    {
        std::vector<std::size_t> checkedBoxNumbers;

        const std::size_t NUM_CHECKBOXES(checkBoxSVec_.size());
        for (std::size_t i(0); i < NUM_CHECKBOXES; ++i)
            if (false == checkBoxSVec_[i]->IsInFirstState())
                checkedBoxNumbers.push_back(i);

        return checkedBoxNumbers;
    }


    const CheckBoxSVec_t CheckBoxSet::GetCheckedBoxes()
    {
        CheckBoxSVec_t checkedBoxes;

        const std::size_t NUM_CHECKBOXES(checkBoxSVec_.size());
        for (std::size_t i(0); i < NUM_CHECKBOXES; ++i)
            if (false == checkBoxSVec_[i]->IsInFirstState())
                checkedBoxes.push_back( checkBoxSVec_[i] );

        return checkedBoxes;
    }


    bool CheckBoxSet::MouseUp(const sf::Vector2f & MOUSE_POS_V)
    {
        const std::size_t NUM_CHECKBOXES(checkBoxSVec_.size());
        for (std::size_t i(0); i < NUM_CHECKBOXES; ++i)
        {
            if ((false == IsInvalid(i)) && (checkBoxSVec_[i]->GetEntityRegion().contains(MOUSE_POS_V) && (downInWhichRegion_ == i)))
            {
                SoundManager::Instance()->GetSfxSet(sfml_util::SfxSet::Thock).PlayRandom();
                currentSelections_[i] = ! currentSelections_[i];
                checkBoxSVec_[i]->SetIsInFirstState( ! checkBoxSVec_[i]->IsInFirstState());
                return true;
            }
        }

        return false;
    }


    bool CheckBoxSet::MouseDown(const sf::Vector2f & MOUSE_POS_V)
    {
        const std::size_t NUM_CHECKBOXES(checkBoxSVec_.size());
        for (std::size_t i(0); i < NUM_CHECKBOXES; ++i)
        {
            if ((false == IsInvalid(i)) && (checkBoxSVec_[i]->GetEntityRegion().contains(MOUSE_POS_V)))
            {
                downInWhichRegion_ = i;
                return true;
            }
        }

        downInWhichRegion_ = checkBoxSVec_.size();//reset to an invalid value
        return false;
    }


    bool CheckBoxSet::UpdateMousePos(const sf::Vector2f & MOUSE_POS_V)
    {
        bool wereAnyStatesChanged(false);

        const std::size_t NUM_CHECKBOXES(checkBoxSVec_.size());
        for (std::size_t i(0); i < NUM_CHECKBOXES; ++i)
        {
            const MouseState::Enum MOUSE_STATE(checkBoxSVec_[i]->GetMouseState());
            const bool IS_IN_REGION(checkBoxSVec_[i]->GetEntityRegion().contains(MOUSE_POS_V));

            if (IS_IN_REGION                      &&
                (MOUSE_STATE != MouseState::Down) &&
                (currentSelections_[i] == false))
            {
                if (checkBoxSVec_[i]->GetMouseState() != MouseState::Over)
                    SoundManager::Instance()->GetSfxSet(sfml_util::SfxSet::TickOn).PlayRandom();

                checkBoxSVec_[i]->SetMouseState(MouseState::Over);
                wereAnyStatesChanged = true;
                break;
            }

            if ((false == IS_IN_REGION)           &&
                (MOUSE_STATE != MouseState::Down) &&
                (currentSelections_[i] == false))
            {
                checkBoxSVec_[i]->SetMouseState(MouseState::Up);
                wereAnyStatesChanged = true;
            }
        }

        return wereAnyStatesChanged;
    }


    bool CheckBoxSet::IsInvalid(const std::size_t INDEX)
    {
        const std::size_t NUM_INVALIDS(invalidSelectionsVec_.size());
        for (std::size_t i(0); i < NUM_INVALIDS; ++i)
            if (invalidSelectionsVec_[i] == INDEX)
                return true;

        return false;
    }


    void CheckBoxSet::PostSetupTasks(const float            POS_LEFT,
                                     const float            POS_TOP,
                                     const gui::box::Info & BOX_INFO)
    {
        SetCheckboxPositions(POS_LEFT, POS_TOP);

        //setup box's region
        box::Info boxInfo(BOX_INFO);
        boxInfo.SetBoxAndBackgroundRegion(GetEntityRegion());
        box_.SetupBox(boxInfo);
    }


    void CheckBoxSet::SetCheckboxPositions(const float POS_LEFT, const float POS_TOP)
    {
        //set the vertical positions of the radio buttons and find the maxX position
        float maxX(checkBoxSVec_[0]->GetEntityRegion().width + (outerPad_ * 2.0f));
        float posY(checkBoxSVec_[0]->GetEntityRegion().top);

        checkBoxSVec_[0]->Setup(POS_LEFT + outerPad_, posY, IsInvalid(0));

        const std::size_t NUM_CHECKBOXES(checkBoxSVec_.size());
        for (std::size_t i(1); i < NUM_CHECKBOXES; ++i)
        {
            posY += checkBoxSVec_[i - 1]->GetEntityRegion().height + betweenPad_;
            checkBoxSVec_[i]->Setup(POS_LEFT + outerPad_, posY, IsInvalid(i));

            const float NEXT_FAR_X_POINT(checkBoxSVec_[i]->GetEntityRegion().width + (outerPad_ * 2.0f));
            if (maxX < NEXT_FAR_X_POINT)
                maxX = NEXT_FAR_X_POINT;
        }
        posY += checkBoxSVec_[checkBoxSVec_.size() - 1]->GetEntityRegion().height;

        //set the overall rect encompasing all radio buttons with their text
        SetEntityRegion(sf::FloatRect(POS_LEFT, POS_TOP, maxX, (posY - POS_TOP) + outerPad_));
    }


    void CheckBoxSet::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        target.draw(box_, states);

        const std::size_t NUM_CHECKBOXES(checkBoxSVec_.size());
        for (std::size_t i(0); i < NUM_CHECKBOXES; ++i)
            checkBoxSVec_[i]->draw(target, states);
    }


    void CheckBoxSet::OnColorChange()
    {
        box_.SetEntityColors(entityColorSet_);
    }


    void CheckBoxSet::SetEntityPos(const float POS_LEFT, const float POS_TOP)
    {
        GuiEntity::SetEntityPos(POS_LEFT, POS_TOP);

        SetCheckboxPositions(POS_LEFT, POS_TOP);

        box_.SetEntityPos(POS_LEFT, POS_TOP);
    }


    void CheckBoxSet::MoveEntityPos(const float HORIZ, const float VERT)
    {
        GuiEntity::MoveEntityPos(HORIZ, VERT);

        const std::size_t NUM_CHECKBOXES(checkBoxSVec_.size());
        for (std::size_t i(0); i < NUM_CHECKBOXES; ++i)
            checkBoxSVec_[i]->MoveEntityPos(HORIZ, VERT);

        box_.MoveEntityPos(HORIZ, VERT);
    }

}
}
