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
// party-stage.cpp
//
#include "party-stage.hpp"

#include "sfml-util/sfml-util.hpp"
#include "sfml-util/loaders.hpp"
#include "sfml-util/display.hpp"
#include "sfml-util/tile.hpp"
#include "sfml-util/sound-manager.hpp"
#include "sfml-util/gui/gui-elements.hpp"
#include "sfml-util/gui/text-region.hpp"
#include "sfml-util/gui/list-box-item.hpp"
#include "sfml-util/gui/creature-image-manager.hpp"

#include "popup/popup-manager.hpp"

#include "log/log-macros.hpp"

#include "game/game-data-file.hpp"
#include "game/loop-manager.hpp"
#include "game/game.hpp"

#include "state/game-state-factory.hpp"
#include "creature/name-info.hpp"
#include "player/party.hpp"
#include "player/character.hpp"
#include "player/character-warehouse.hpp"

#include "misc/real.hpp"

#include <sstream>
#include <string>


namespace heroespath
{
namespace stage
{

    const std::string   PartyStage::POPUP_NAME_STR_NOT_ENOUGH_CHARS_{
        "NotEnoughCharactersErrorMsg" };

    const std::string   PartyStage::POPUP_NAME_STR_TOO_MANY_CHARS_{
        "TooManyCharactersErrorMsg" };

    const std::string   PartyStage::POPUP_NAME_STR_DELETE_CONFIRM_{
        "CharacterDeleteConfirmMsg" };

    const float         PartyStage::MOUSE_OVER_POPUP_DELAY_SEC_{ 1.0f };
    const float         PartyStage::MOUSE_OVER_SLIDER_SPEED_   { 4.0f };
    const float         PartyStage::MOUSE_OVER_POPUP_POS_LEFT_ { 20.0f };
    const float         PartyStage::MOUSE_OVER_POPUP_POS_TOP_  { 20.0f };
    const float         PartyStage::MOUSE_OVER_IMAGE_PAD_      { 10.0f };
    const float         PartyStage::MOUSE_OVER_BOX_ALPHA_      { 170.0f };


    PartyStage::PartyStage()
    :
        Stage("Party"),
        SCREEN_WIDTH_           (sfml_util::Display::Instance()->GetWinWidth()),
        SCREEN_HEIGHT_          (sfml_util::Display::Instance()->GetWinHeight()),
        mainMenuTitle_          ("create_party_normal.png"),
        backgroundImage_        ("media-images-backgrounds-tile-darkknot"),
        backButtonUPtr_         ( std::make_unique<sfml_util::gui::FourStateButton>(
            "PartyStage'sBack",
            0.0f,
            0.0f,
            std::string(game::GameDataFile::Instance()->GetMediaPath(
                "media-images-buttons-mainmenu-dir")).append("back_button_normal.png"),
            "",
            std::string(game::GameDataFile::Instance()->GetMediaPath(
                "media-images-buttons-mainmenu-dir")).append("back_button_lit.png")) ),
        startButtonUPtr_        ( std::make_unique<sfml_util::gui::FourStateButton>(
            "PartyStage'sStartGame",
            0.0f,
            0.0f,
            std::string(game::GameDataFile::Instance()->GetMediaPath(
                "media-images-buttons-mainmenu-dir")).append("startgame_button_normal.png"),
            "",
            std::string(game::GameDataFile::Instance()->GetMediaPath(
                "media-images-buttons-mainmenu-dir")).append("startgame_button_lit.png")) ),
        deleteButtonUPtr_       ( std::make_unique<sfml_util::gui::FourStateButton>(
            "PartyStage'sDelete",
            0.0f,
            0.0f,
            std::string(game::GameDataFile::Instance()->GetMediaPath(
                "media-images-buttons-mainmenu-dir")).append("delete_button_normal.png"),
            "",
            std::string(game::GameDataFile::Instance()->GetMediaPath(
                "media-images-buttons-mainmenu-dir")).append("delete_button_lit.png")) ),
        characterListBoxUPtr_   (),
        partyListBoxUPtr_       (),
        insTextRegionUPtr_      (),
        upTextRegionUPtr_       (),
        partyTextRegionUPtr_    (),
        warningTextInfo_        (),
        warningTextRegionUPtr_  (),
        warningTextSlider_      (
            150,
            255,
            4.0f,
            static_cast<sf::Uint8>(misc::random::Int(150, 255))),
        ouroborosUPtr_          (),
        bottomSymbol_           (),
        willDisplayCharacterCountWarningText_(false),
        willShowMouseOverPopup_ (true),
        mouseOverPopupTimerSec_ (0.0f),
        mouseOverQuad_          (sf::Quads, 4),
        mouseOverBoxWidth_      (0.0f),
        mouseOverBoxHeight_     (0.0f),
        mouseOverPosV_          (),
        mouseOverSprite_        (),
        mouseOverCharPtr_       (),
        mouseOverTexture_       (),
        isMouseOverTexture_     (false),
        mouseOverTextRegionUPtr_(),
        mouseOverSlider_        (4.0f),
        charactersPSet_         ()
    {}


    PartyStage::~PartyStage()
    {
        ClearAllEntities();

        for (auto & nextCharacterPtr : charactersPSet_)
        {
            delete nextCharacterPtr;
        }

        charactersPSet_.clear();
    }


    bool PartyStage::HandleCallback(
        const sfml_util::gui::callback::ListBoxEventPackage & PACKAGE)
    {
        ResetMouseOverPopupState();

        //if this event was not triggered by a double-click then exit
        if (PACKAGE.gui_event != sfml_util::GuiEvent::DoubleClick)
        {
            return false;
        }

        //handle double-clicks by moving items from one ListBox to the other

        if (PACKAGE.package.PTR_ == characterListBoxUPtr_.get())
        {
            auto itemSPtr{ characterListBoxUPtr_->AtPos(PACKAGE.mouse_pos) };

            if (itemSPtr.get() != nullptr)
            {
                partyListBoxUPtr_->Add(itemSPtr);
                characterListBoxUPtr_->Remove(itemSPtr);
            }

            return true;
        }
        else if (PACKAGE.package.PTR_ == partyListBoxUPtr_.get())
        {
            auto itemSPtr{ partyListBoxUPtr_->AtPos(PACKAGE.mouse_pos) };

            if (itemSPtr.get() != nullptr)
            {
                characterListBoxUPtr_->Add(itemSPtr);
                partyListBoxUPtr_->Remove(itemSPtr);
            }

            return false;
        }

        return false;
    }


    bool PartyStage::HandleCallback(
        const sfml_util::gui::callback::FourStateButtonCallbackPackage_t & PACKAGE)
    {
        ResetMouseOverPopupState();

        if (PACKAGE.PTR_ == backButtonUPtr_.get())
        {
            return HandleCallback_BackButton();
        }

        if (PACKAGE.PTR_ == startButtonUPtr_.get())
        {
            return HandleCallback_StartButton();
        }

        if (PACKAGE.PTR_ == deleteButtonUPtr_.get())
        {
            return HandleCallback_DeleteButton();
        }

        return false;
    }


    bool PartyStage::HandleCallback(const popup::PopupResponse & PACKAGE)
    {
        ResetMouseOverPopupState();
        willShowMouseOverPopup_ = true;

        if ((PACKAGE.Info().Name() == POPUP_NAME_STR_DELETE_CONFIRM_) &&
            (PACKAGE.Response() == popup::ResponseTypes::Yes))
        {
            auto selectedItemSPtr{ GetSelectedItemSPtr() };

            if ((selectedItemSPtr.get() != nullptr) &&
                (selectedItemSPtr->CHARACTER_CPTR != nullptr))
            {
                auto characterPtr{ selectedItemSPtr->CHARACTER_CPTR };

                if (state::GameStateFactory::Instance()->DeleteCharacter(characterPtr) == false)
                {
                    M_HP_LOG_ERR("stage::PartyStage::HandleCallback(delete confirm popup)"
                        << " unable to delete character \"" << characterPtr->Name() << "\" file.");
                }

                characterListBoxUPtr_->Remove(selectedItemSPtr);
                partyListBoxUPtr_->Remove(selectedItemSPtr);
                //actual Character object will be free'd when the PartyStage object is destroyed
            }
        }

        return false;
    }


    bool PartyStage::HandleCallback_BackButton()
    {
        game::LoopManager::Instance()->TransitionTo_CharacterCreation();
        return true;
    }


    bool PartyStage::HandleCallback_StartButton()
    {
        if (partyListBoxUPtr_->Size() != player::Party::MAX_CHARACTER_COUNT_)
        {
            std::ostringstream ss;
            ss << "There are " << partyListBoxUPtr_->Size()
                << " characters in your party.  You need exactly "
                << player::Party::MAX_CHARACTER_COUNT_ << " characters to start the game.";

            auto const POPUP_INFO{ popup::PopupManager::Instance()->CreatePopupInfo(
                POPUP_NAME_STR_NOT_ENOUGH_CHARS_,
                ss.str(),
                popup::PopupButtons::Okay,
                popup::PopupImage::Banner,
                sfml_util::Justified::Center,
                sfml_util::sound_effect::PromptQuestion) };

            game::LoopManager::Instance()->PopupWaitBegin(this, POPUP_INFO);
            return false;
        }
        else
        {
            //determine if any beasts are in the party
            auto isAnyCharacterBeast{ false };
            auto const NUM_CHARACTERS{ partyListBoxUPtr_->Size() };
            for (std::size_t i(0); i < NUM_CHARACTERS; ++i)
            {
                auto const NEXT_CHAR_RACE{ partyListBoxUPtr_->At(i)->CHARACTER_CPTR->Race() };

                if ((creature::race::Dragon == NEXT_CHAR_RACE) ||
                    (creature::race::Wolfen == NEXT_CHAR_RACE))
                {
                    isAnyCharacterBeast = true;
                    break;
                }
            }

            //determine if any are beastmasters
            auto isAnyCharacterBeastmaster{ false };
            for (std::size_t i(0); i < NUM_CHARACTERS; ++i)
            {
                auto const NEXT_CHAR_ROLE{ partyListBoxUPtr_->At(i)->CHARACTER_CPTR->Role() };

                if (NEXT_CHAR_ROLE == creature::role::Beastmaster)
                {
                    isAnyCharacterBeastmaster = true;
                    break;
                }
            }

            //if any characters are beasts, then one in the party must be a beastmaster
            if (isAnyCharacterBeast && (false == isAnyCharacterBeastmaster))
            {
                std::ostringstream ss;
                ss << "There are beast characters in your party, but no character with the role"
                    << " of Beastmaster.  To have Wolfens or Dragons in your party, you must also"
                    << " have a Beastmaster.";

                auto const POP_INFO{ popup::PopupManager::Instance()->CreatePopupInfo(
                    POPUP_NAME_STR_NOT_ENOUGH_CHARS_,
                    ss.str(),
                    popup::PopupButtons::Okay,
                    popup::PopupImage::RegularSidebar,
                    sfml_util::Justified::Left,
                    sfml_util::sound_effect::PromptWarn) };

                game::LoopManager::Instance()->PopupWaitBegin(this, POP_INFO);
                return false;
            }

            StartNewGame();
            return true;
        }
    }


    bool PartyStage::HandleCallback_DeleteButton()
    {
        auto selectedCharPtr{ GetSelectedCharacter() };

        if (selectedCharPtr == nullptr)
        {
            return false;
        }

        std::ostringstream ss;
        ss << "Delete " << selectedCharPtr->Name() << "?  This cannot be undone.  Are you sure?";

        auto const POPUP_INFO{ popup::PopupManager::Instance()->CreatePopupInfo(
            POPUP_NAME_STR_DELETE_CONFIRM_,
            ss.str(),
            popup::PopupButtons::YesNo,
            popup::PopupImage::Banner,
            sfml_util::Justified::Center,
            sfml_util::sound_effect::PromptWarn) };

        game::LoopManager::Instance()->PopupWaitBegin(this, POPUP_INFO);
        return true;
    }


    void PartyStage::Setup()
    {
        //ouroboros
        ouroborosUPtr_ = std::make_unique<sfml_util::Ouroboros>("PartyStage's");
        EntityAdd(ouroborosUPtr_.get());

        //back button
        backButtonUPtr_->SetScaleToRes();
        backButtonUPtr_->SetVertPositionToBottomOfScreenByRes(sfml_util::MapByRes(75.0f, 800.0f));
        backButtonUPtr_->SetCallbackHandler(this);
        EntityAdd(backButtonUPtr_.get());

        //start game button
        startButtonUPtr_->SetScaleToRes();
        startButtonUPtr_->SetVertPositionToBottomOfScreenByRes((SCREEN_WIDTH_ * 0.5f) + 110.0f);
        startButtonUPtr_->SetCallbackHandler(this);
        EntityAdd(startButtonUPtr_.get());

        //delete game button
        deleteButtonUPtr_->SetScaleToRes();
        deleteButtonUPtr_->SetVertPositionToBottomOfScreenByRes((SCREEN_WIDTH_ * 0.5f) - 325.0f);
        deleteButtonUPtr_->SetCallbackHandler(this);
        EntityAdd(deleteButtonUPtr_.get());

        //top instruction text
        {
            sfml_util::gui::TextInfo insTextInfo(
                "(double-click or use the arrow keys and enter to move characters back and forth)",
                sfml_util::FontManager::Instance()->Font_Typical(),
                sfml_util::FontManager::Instance()->Size_Smallish(),
                sfml_util::FontManager::Color_GrayLight(),
                sf::BlendAlpha,
                sf::Text::Italic,
                sfml_util::Justified::Left);

            insTextRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
                "Instructions",
                insTextInfo,
                sf::FloatRect());

            auto const HALF_INSTR_TEXT_WIDTH{ insTextRegionUPtr_->GetEntityRegion().width * 0.5f };

            insTextRegionUPtr_->SetEntityPos(
                ((SCREEN_WIDTH_ * 0.5f) - HALF_INSTR_TEXT_WIDTH) + 125.0f,
                mainMenuTitle_.Bottom() - 45.0f);

            EntityAdd(insTextRegionUPtr_.get());
        }

        //load all players not yet assigned to a party/started game
        charactersPSet_ = state::GameStateFactory::Instance()->LoadAllCompanions();

        //fill a list with TextRegions for the character names
        sfml_util::gui::ListBoxItemSVec_t itemSVec;
        sfml_util::gui::TextInfo textInfo(
            "",
            sfml_util::FontManager::Instance()->Font_Default2(),
            sfml_util::FontManager::Instance()->Size_Largeish(),
            sf::Color::White,
            sfml_util::Justified::Left);

        std::ostringstream ssCharText;
        std::ostringstream ssTitle;
        for (auto const NEXT_CHAR_PTR : charactersPSet_)
        {
            ssCharText.str("");
            ssCharText << NEXT_CHAR_PTR->Name() << "'s_CharacterListing";

            ssTitle.str("");
            ssTitle << NEXT_CHAR_PTR->Name() << ", " << NEXT_CHAR_PTR->RaceName();

            if (NEXT_CHAR_PTR->Race() != creature::race::Wolfen)
            {
                ssTitle << ", " << NEXT_CHAR_PTR->RoleName();
            }

            textInfo.text = ssTitle.str();

            auto nextCharTextSPtr = std::make_shared<sfml_util::gui::ListBoxItem>(
                ssCharText.str(),
                textInfo,
                NEXT_CHAR_PTR);

            itemSVec.push_back( nextCharTextSPtr );
        }

        //establish the longest line that needs to fit in the listboxes
        std::ostringstream longestSS;
        longestSS << creature::NameInfo::Instance()->LargestName() << ", " << "Human, Beastmaster";

        auto const LONGEST_TEXT_INFO{
            creature::NameInfo::Instance()->MakeTextInfo(longestSS.str()) };

        auto const LIST_BOX_WIDTH{
            creature::NameInfo::Instance()->Length(LONGEST_TEXT_INFO) +
                sfml_util::MapByRes(50.0f, 300.0f) };

        //player list box
        auto const CHAR_LIST_POS_LEFT{
            ((SCREEN_WIDTH_ * 0.1f) - 75.0f) + sfml_util::MapByRes(0.0f, 800.0f) };

        auto const CHAR_LIST_WIDTH{ LIST_BOX_WIDTH };
        auto const HALF_SCREEN_HEIGHT{ SCREEN_HEIGHT_ * 0.5f };
        auto const CHAR_LIST_HEIGHT{
            sfml_util::MapByRes(HALF_SCREEN_HEIGHT + 20.0f, HALF_SCREEN_HEIGHT + 300.0f) };

        auto const CHAR_LIST_POS_TOP{
            ((SCREEN_HEIGHT_ * 0.5f) - (CHAR_LIST_HEIGHT * 0.5f)) +
                sfml_util::MapByRes(70.0f, 350.0f) };

        const sf::FloatRect CHAR_LIST_RECT(
            CHAR_LIST_POS_LEFT,
            CHAR_LIST_POS_TOP,
            CHAR_LIST_WIDTH,
            CHAR_LIST_HEIGHT);

        auto const BG_COLOR{
            sfml_util::FontManager::Color_Orange() - sf::Color(100, 100, 100, 220) };

        sfml_util::gui::BackgroundInfo bgInfo(BG_COLOR);

        sfml_util::gui::box::Info boxInfo(
            1,
            true,
            CHAR_LIST_RECT,
            sfml_util::gui::ColorSet(
                sfml_util::FontManager::Color_Orange(),
                BG_COLOR,
                sfml_util::FontManager::Color_Orange() -
                    sfml_util::gui::ColorSet::DEFAULT_OFFSET_COLOR_,
                BG_COLOR - sf::Color(40,40,40,0)),
            bgInfo);
        //
        characterListBoxUPtr_ = std::make_unique<sfml_util::gui::ListBox>(
            "PartyStage'sCharacter",
            CHAR_LIST_RECT,
            itemSVec,
            this,
            10.0f,
            6.0f,
            boxInfo,
            sfml_util::FontManager::Color_Orange(),
            this);

        EntityAdd(characterListBoxUPtr_.get());

        //party list box
        auto const PARTY_LIST_POS_LEFT{ (SCREEN_WIDTH_ * 0.55f) - 25.0f };
        auto const PARTY_LIST_POS_TOP{ CHAR_LIST_POS_TOP };
        auto const PARTY_LIST_WIDTH{ LIST_BOX_WIDTH };
        auto const PARTY_LIST_HEIGHT{ CHAR_LIST_HEIGHT };

        const sf::FloatRect PARTY_LIST_RECT(
            PARTY_LIST_POS_LEFT,
            PARTY_LIST_POS_TOP,
            PARTY_LIST_WIDTH,
            PARTY_LIST_HEIGHT);

        boxInfo.SetBoxAndBackgroundRegion(PARTY_LIST_RECT);

        partyListBoxUPtr_ = std::make_unique<sfml_util::gui::ListBox>(
            "PartyStage'sParty",
            PARTY_LIST_RECT,
            sfml_util::gui::ListBoxItemSVec_t(),
            this,
            10.0f,
            6.0f,
            boxInfo,
            sfml_util::FontManager::Color_Orange(),
            this);

        EntityAdd(partyListBoxUPtr_.get());

        //unplayed character label text
        {
            sfml_util::gui::TextInfo labelTextInfo(
                "Unplayed Characters",
                sfml_util::FontManager::Instance()->Font_Default2(),
                sfml_util::FontManager::Instance()->Size_Largeish(),
                sfml_util::FontManager::Color_Orange() + sf::Color(0, 30, 30, 0));

            upTextRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
                "CharacterLabel",
                labelTextInfo,
                sf::FloatRect());

            upTextRegionUPtr_->SetEntityPos(
                characterListBoxUPtr_->GetEntityRegion().left + 50.0f,
                (characterListBoxUPtr_->GetEntityRegion().top -
                    upTextRegionUPtr_->GetEntityRegion().height));

            EntityAdd(upTextRegionUPtr_.get());
        }

        //party label text
        {
            sfml_util::gui::TextInfo labelTextInfo(
                "New Party",
                sfml_util::FontManager::Instance()->Font_Default2(),
                sfml_util::FontManager::Instance()->Size_Largeish(),
                sfml_util::FontManager::Color_Orange() + sf::Color(0, 30, 30, 0));

            partyTextRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
                "PartyLabel",
                labelTextInfo,
                sf::FloatRect());

            partyTextRegionUPtr_->SetEntityPos(
                partyListBoxUPtr_->GetEntityRegion().left + 50.0f,
                (partyListBoxUPtr_->GetEntityRegion().top -
                    partyTextRegionUPtr_->GetEntityRegion().height));

            EntityAdd(partyTextRegionUPtr_.get());
        }

        //warning instruction text
        {
            std::ostringstream ss;
            ss << "(There are not enough characters to make a party of "
                << player::Party::MAX_CHARACTER_COUNT_ << ".  Go back and create more.)";

            warningTextInfo_ = sfml_util::gui::TextInfo(
                ss.str(),
                sfml_util::FontManager::Instance()->Font_Typical(),
                sfml_util::FontManager::Instance()->Size_Smallish(),
                sf::Color(255, 200, 200),
                sf::BlendAlpha,
                sf::Text::Italic,
                sfml_util::Justified::Left);

            warningTextRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
                "WarningsText",
                warningTextInfo_,
                sf::FloatRect());

            auto const WARNING_TEXT_LEFT{
                (SCREEN_WIDTH_ * 0.5f) - (warningTextRegionUPtr_->GetEntityRegion().width * 0.5f) +
                    110.0f };

            auto const WARNING_TEXT_TOP{
                (insTextRegionUPtr_->GetEntityRegion().top +
                    insTextRegionUPtr_->GetEntityRegion().height) -
                    sfml_util::MapByRes(0.0f, 20.0f) };

            warningTextRegionUPtr_->SetEntityPos(WARNING_TEXT_LEFT, WARNING_TEXT_TOP);
        }
    }


    void PartyStage::Draw(sf::RenderTarget & target, const sf::RenderStates & STATES)
    {
        target.draw(backgroundImage_, STATES);
        target.draw(mainMenuTitle_, STATES);
        target.draw(bottomSymbol_, STATES);
        Stage::Draw(target, STATES);

        if (willDisplayCharacterCountWarningText_)
        {
            target.draw( * warningTextRegionUPtr_, STATES);
        }

        if (willShowMouseOverPopup_ && isMouseOverTexture_)
        {
            target.draw(mouseOverQuad_, STATES);
            target.draw(mouseOverSprite_, STATES);

            if (mouseOverTextRegionUPtr_.get() != nullptr)
            {
                mouseOverTextRegionUPtr_->draw(target, STATES);
            }
        }
    }


    std::size_t PartyStage::NumCharactersInTheParty() const
    {
        return partyListBoxUPtr_->Size();
    }


    sfml_util::gui::ListBoxItemSPtr_t PartyStage::GetSelectedItemSPtr() const
    {
        if (partyListBoxUPtr_->HasFocus())
        {
            return partyListBoxUPtr_->Selected();
        }
        else
        {
            return characterListBoxUPtr_->Selected();
        }
    }


    player::CharacterPtr_t PartyStage::GetSelectedCharacter() const
    {
        auto itemSPtr{ GetSelectedItemSPtr() };

        if (itemSPtr.get() != nullptr)
        {
            return itemSPtr->CHARACTER_CPTR;
        }

        return nullptr;
    }


    void PartyStage::UpdateTime(const float ELAPSED_TIME_SECONDS)
    {
        Stage::UpdateTime(ELAPSED_TIME_SECONDS);

        willDisplayCharacterCountWarningText_ =
            ((characterListBoxUPtr_->Size() + partyListBoxUPtr_->Size()) <
                player::Party::MAX_CHARACTER_COUNT_);

        //oscillate the warning text color
        if (willDisplayCharacterCountWarningText_)
        {
            auto const NEW_COLOR_VAL{
                static_cast<sf::Uint8>(warningTextSlider_.Update(ELAPSED_TIME_SECONDS)) };

            auto color(warningTextRegionUPtr_->GetEntityColorForeground());

            color.r = 255;
            color.g = NEW_COLOR_VAL;
            color.b = NEW_COLOR_VAL;
            warningTextRegionUPtr_->SetEntityColorFgBoth(color);
        }

        mouseOverPopupTimerSec_ += ELAPSED_TIME_SECONDS;

        if ((mouseOverPopupTimerSec_ > MOUSE_OVER_POPUP_DELAY_SEC_) &&
            (false == isMouseOverTexture_))
        {
            auto itemSPtr{ characterListBoxUPtr_->AtPos(mouseOverPosV_) };

            if (itemSPtr.get() == nullptr)
            {
                itemSPtr = partyListBoxUPtr_->AtPos(mouseOverPosV_);
            }

            if ((itemSPtr.get() != nullptr) &&
                (itemSPtr->CHARACTER_CPTR != nullptr) &&
                (itemSPtr->CHARACTER_CPTR->ImageFilename().empty() == false))
            {
                mouseOverCharPtr_ = itemSPtr->CHARACTER_CPTR;

                mouseOverSlider_.Reset(MOUSE_OVER_SLIDER_SPEED_);

                sfml_util::gui::CreatureImageManager::Instance()->
                    GetImage(mouseOverTexture_, itemSPtr->CHARACTER_CPTR->ImageFilename());

                isMouseOverTexture_ = true;
                mouseOverSprite_.setTexture(mouseOverTexture_ );

                mouseOverSprite_.setTextureRect( sf::IntRect(
                    0,
                    0,
                    static_cast<int>(mouseOverTexture_.getSize().x),
                    static_cast<int>(mouseOverTexture_.getSize().y)) );

                mouseOverSprite_.setScale(0.0f, 0.0f);

                mouseOverBoxWidth_ = MOUSE_OVER_IMAGE_PAD_ +
                    sfml_util::gui::CreatureImageManager::Instance()->DimmensionMax() +
                    creature::NameInfo::Instance()->Length() +
                    sfml_util::MapByRes(50.0f, 150.0f);

                mouseOverBoxHeight_ = mouseOverSprite_.getLocalBounds().height +
                    (MOUSE_OVER_IMAGE_PAD_ * 2.0f) + sfml_util::MapByRes(0.0f, 600.0f);
            }
        }

        if (isMouseOverTexture_)
        {
            auto const RATIO{ mouseOverSlider_.Update(ELAPSED_TIME_SECONDS) };

            auto const CURR_BOX_WIDTH{ mouseOverBoxWidth_ * RATIO };
            auto const CURR_BOX_HEIGHT{ mouseOverBoxHeight_ * RATIO };

            auto const CURR_BOX_POS_LEFT{
                mouseOverPosV_.x - ((mouseOverPosV_.x - MOUSE_OVER_POPUP_POS_LEFT_) * RATIO) };

            auto const CURR_BOX_POS_TOP{
                mouseOverPosV_.y - ((mouseOverPosV_.y - MOUSE_OVER_POPUP_POS_TOP_) * RATIO) };

            mouseOverQuad_[0].position = sf::Vector2f(
                CURR_BOX_POS_LEFT,
                CURR_BOX_POS_TOP);

            mouseOverQuad_[1].position = sf::Vector2f(
                CURR_BOX_POS_LEFT + CURR_BOX_WIDTH,
                CURR_BOX_POS_TOP);

            mouseOverQuad_[2].position = sf::Vector2f(
                CURR_BOX_POS_LEFT + CURR_BOX_WIDTH,
                CURR_BOX_POS_TOP + CURR_BOX_HEIGHT);

            mouseOverQuad_[3].position = sf::Vector2f(
                CURR_BOX_POS_LEFT,
                CURR_BOX_POS_TOP + CURR_BOX_HEIGHT);

            auto const NEW_ALPHA{ static_cast<sf::Uint8>(MOUSE_OVER_BOX_ALPHA_ * RATIO) };
            const sf::Color NEW_COLOR(0, 0, 0, NEW_ALPHA);
            mouseOverQuad_[0].color = NEW_COLOR;
            mouseOverQuad_[1].color = NEW_COLOR;
            mouseOverQuad_[2].color = NEW_COLOR;
            mouseOverQuad_[3].color = NEW_COLOR;

            mouseOverSprite_.setScale(RATIO, RATIO);

            auto const SPRITE_POS_LEFT{
                mouseOverPosV_.x - ((mouseOverPosV_.x -
                    (MOUSE_OVER_POPUP_POS_LEFT_ + MOUSE_OVER_IMAGE_PAD_)) * RATIO) };

            auto const SPRITE_POS_TOP{
                mouseOverPosV_.y - ((mouseOverPosV_.y - (MOUSE_OVER_POPUP_POS_TOP_ +
                    ((mouseOverBoxHeight_ - mouseOverSprite_.getLocalBounds().height) * 0.5f)))
                        * RATIO) };

            mouseOverSprite_.setPosition(SPRITE_POS_LEFT, SPRITE_POS_TOP);

            if (mouseOverSlider_.IsDone())
            {
                std::ostringstream ss;
                ss << "\"" << mouseOverCharPtr_->Name() << "\"\n"
                   << mouseOverCharPtr_->SexName() << ", "
                   << mouseOverCharPtr_->RaceName() << ", "
                   << mouseOverCharPtr_->RoleName() << "\n"
                   << "Created on " << mouseOverCharPtr_->DateTimeCreated().date.ToString()
                   << " at " << mouseOverCharPtr_->DateTimeCreated().time.ToString() << "\n\n"
                   << "Strength:        "      << mouseOverCharPtr_->Strength() << "\n"
                   << "Accuracy:       "       << mouseOverCharPtr_->Accuracy() << "\n"
                   << "Charm:           "      << mouseOverCharPtr_->Charm() << "\n"
                   << "Luck:              "    << mouseOverCharPtr_->Luck() << "\n"
                   << "Speed:            "     << mouseOverCharPtr_->Speed() << "\n"
                   << "Intelligence:    "      << mouseOverCharPtr_->Intelligence();

                const sf::FloatRect TEXT_RECT(
                    MOUSE_OVER_POPUP_POS_LEFT_ +
                        MOUSE_OVER_IMAGE_PAD_ +
                        mouseOverSprite_.getGlobalBounds().width +
                        MOUSE_OVER_IMAGE_PAD_,
                    MOUSE_OVER_POPUP_POS_TOP_ + (MOUSE_OVER_IMAGE_PAD_ * 2.0f),
                    mouseOverBoxWidth_,
                    mouseOverBoxHeight_);

                const sfml_util::gui::TextInfo TEXT_INFO(
                    ss.str(),
                    sfml_util::FontManager::Instance()->Font_Typical(),
                    sfml_util::FontManager::Instance()->Size_Smallish(),
                    sfml_util::FontManager::Color_Light(),
                    sfml_util::Justified::Left);

                mouseOverTextRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
                    "PartyStage'sMouseOverPopup",
                    TEXT_INFO,
                    TEXT_RECT);
            }
        }
    }


    void PartyStage::UpdateMousePos(const sf::Vector2i & NEW_MOUSE_POS_V)
    {
        auto const NEW_MOUSE_POS_VF{ sfml_util::ConvertVector<int, float>(NEW_MOUSE_POS_V) };
        mouseOverPosV_ = NEW_MOUSE_POS_VF;
        ResetMouseOverPopupState();
        Stage::UpdateMousePos(NEW_MOUSE_POS_V);
    }


    void PartyStage::StartNewGame()
    {
        //create a new party structure
        player::CharacterPVec_t charPVec;
        {
            for (std::size_t i(0); i < partyListBoxUPtr_->Size(); ++i)
            {
                auto characterPtr{ partyListBoxUPtr_->At(i)->CHARACTER_CPTR };
                charPVec.push_back(characterPtr);

                charactersPSet_.erase(characterPtr);

                if (state::GameStateFactory::Instance()->
                    DeleteCharacter(characterPtr) == false)
                {
                    M_HP_LOG_ERR(
                        "PartyStage::HandleCallback_StartButton() while trying to state::"
                        << "GameStateFactory::DeleteCharacter(\"" << characterPtr->Name()
                        << "\") failed.");
                }
            }
        }

        //create a new GameState with the given party and then save it
        state::GameStateFactory::Instance()->NewGame( new player::Party(charPVec) );

        //Don't bother clearing the party ListBox because it flashes the
        //"not engouh characters" text, and since we are immediately transitioning
        //to the Camp Stage anyway.
        //partyListBoxUPtr_->Clear();

        game::LoopManager::Instance()->TransitionTo_Camp();
    }


    bool PartyStage::KeyRelease(const sf::Event::KeyEvent & KEY_EVENT)
    {
        ResetMouseOverPopupState();

        if (Stage::KeyRelease(KEY_EVENT))
        {
            return true;
        }
        else if (KEY_EVENT.code == sf::Keyboard::B)
        {
            backButtonUPtr_->SetMouseState(sfml_util::MouseState::Over);
            sfml_util::SoundManager::Instance()->PlaySfx_Keypress();
            willShowMouseOverPopup_ = false;
            HandleCallback_BackButton();
            return true;
        }
        else if (KEY_EVENT.code == sf::Keyboard::D)
        {
            deleteButtonUPtr_->SetMouseState(sfml_util::MouseState::Over);
            sfml_util::SoundManager::Instance()->PlaySfx_Keypress();
            willShowMouseOverPopup_ = false;
            HandleCallback_DeleteButton();
            return true;
        }
        else if (KEY_EVENT.code == sf::Keyboard::S)
        {
            startButtonUPtr_->SetMouseState(sfml_util::MouseState::Over);
            sfml_util::SoundManager::Instance()->PlaySfx_Keypress();
            willShowMouseOverPopup_ = false;
            HandleCallback_StartButton();
            return true;
        }
        else
        {
            return false;
        }
    }


    void PartyStage::ResetMouseOverPopupState()
    {
        mouseOverPopupTimerSec_ = 0.0f;
        isMouseOverTexture_ = false;
        mouseOverTextRegionUPtr_.reset();
    }

}
}
