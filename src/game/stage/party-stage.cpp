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
#include "sfml-util/gui/popup-manager.hpp"
#include "sfml-util/gui/list-box-item.hpp"
#include "sfml-util/gui/creature-image-manager.hpp"

#include "game/log-macros.hpp"
#include "game/game-data-file.hpp"
#include "game/state/game-state-factory.hpp"
#include "game/loop-manager.hpp"
#include "game/game.hpp"
#include "game/creature/name-info.hpp"
#include "game/player/party.hpp"
#include "game/player/character.hpp"
#include "game/player/character-warehouse.hpp"

#include "misc/real.hpp"

#include <sstream>
#include <string>


namespace game
{
namespace stage
{

    const std::string   PartyStage::POPUP_NAME_STR_NOT_ENOUGH_CHARS_("NotEnoughCharactersErrorMsg");
    const std::string   PartyStage::POPUP_NAME_STR_TOO_MANY_CHARS_  ("TooManyCharactersErrorMsg");
    const std::string   PartyStage::POPUP_NAME_STR_DELETE_CONFIRM_  ("CharacterDeleteConfirmMsg");
    const float         PartyStage::MOUSE_OVER_POPUP_DELAY_SEC_     (1.0f);
    const float         PartyStage::MOUSE_OVER_SLIDER_SPEED_        (4.0f);
    const float         PartyStage::MOUSE_OVER_POPUP_POS_LEFT_      (20.0f);
    const float         PartyStage::MOUSE_OVER_POPUP_POS_TOP_       (20.0f);
    const float         PartyStage::MOUSE_OVER_IMAGE_PAD_           (10.0f);
    const float         PartyStage::MOUSE_OVER_BOX_ALPHA_           (170.0f);


    PartyStage::PartyStage()
    :
        Stage("Party"),
        SCREEN_WIDTH_           (sfml_util::Display::Instance()->GetWinWidth()),
        SCREEN_HEIGHT_          (sfml_util::Display::Instance()->GetWinHeight()),
        mainMenuTitle_          ("create_party_normal.png"),
        backgroundImage_        ("media-images-backgrounds-tile-darkknot"),
        backButtonSPtr_         ( new sfml_util::gui::FourStateButton("PartyStage'sBack",      0.0f, 0.0f, std::string(GameDataFile::Instance()->GetMediaPath("media-images-buttons-mainmenu-dir")).append("back_button_normal.png"),      "", std::string(GameDataFile::Instance()->GetMediaPath("media-images-buttons-mainmenu-dir")).append("back_button_lit.png")) ),
        startButtonSPtr_        ( new sfml_util::gui::FourStateButton("PartyStage'sStartGame", 0.0f, 0.0f, std::string(GameDataFile::Instance()->GetMediaPath("media-images-buttons-mainmenu-dir")).append("startgame_button_normal.png"), "", std::string(GameDataFile::Instance()->GetMediaPath("media-images-buttons-mainmenu-dir")).append("startgame_button_lit.png")) ),
        deleteButtonSPtr_       ( new sfml_util::gui::FourStateButton("PartyStage'sDelete",    0.0f, 0.0f, std::string(GameDataFile::Instance()->GetMediaPath("media-images-buttons-mainmenu-dir")).append("delete_button_normal.png"),    "", std::string(GameDataFile::Instance()->GetMediaPath("media-images-buttons-mainmenu-dir")).append("delete_button_lit.png")) ),
        characterListBoxSPtr_   (),
        partyListBoxSPtr_       (),
        insTextRegionUPtr_      (),
        upTextRegionUPtr_       (),
        partyTextRegionUPtr_    (),
        warningTextInfo_        (),
        warningTextRegionUPtr_  (),
        warningTextSlider_      (150, 255, 4.0f, static_cast<sf::Uint8>(misc::random::Int(150, 255))),
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
            if (nextCharacterPtr != nullptr)
            {
                delete nextCharacterPtr;
            }
        }

        charactersPSet_.clear();
    }


    bool PartyStage::HandleCallback(const sfml_util::gui::callback::ListBoxEventPackage & PACKAGE)
    {
        ResetMouseOverPopupState();

        //if this event was not triggered by a double-click then exit
        if (PACKAGE.gui_event != sfml_util::GuiEvent::DoubleClick)
        {
            return false;
        }

        //handle double-clicks by moving items from one ListBox to the other

        if (PACKAGE.package.PTR_ == characterListBoxSPtr_.get())
        {
            sfml_util::gui::ListBoxItemSPtr_t itemSPtr(characterListBoxSPtr_->GetAtPosition(PACKAGE.mouse_pos));

            if ((partyListBoxSPtr_->GetCount() < partyListBoxSPtr_->GetLimit()) && (itemSPtr.get() != nullptr))
            {
                partyListBoxSPtr_->Add(itemSPtr);
                characterListBoxSPtr_->Remove(itemSPtr);
            }

            return true;
        }
        else if (PACKAGE.package.PTR_ == partyListBoxSPtr_.get())
        {
            sfml_util::gui::ListBoxItemSPtr_t itemSPtr(partyListBoxSPtr_->GetAtPosition(PACKAGE.mouse_pos));

            if (itemSPtr.get() != nullptr)
            {
                characterListBoxSPtr_->Add(itemSPtr);
                partyListBoxSPtr_->Remove(itemSPtr);
            }

            return false;
        }

        return false;
    }


    bool PartyStage::HandleCallback(const sfml_util::gui::callback::FourStateButtonCallbackPackage_t & PACKAGE)
    {
        ResetMouseOverPopupState();

        if (PACKAGE.PTR_ == backButtonSPtr_.get())
            return HandleCallback_BackButton();

        if (PACKAGE.PTR_ == startButtonSPtr_.get())
            return HandleCallback_StartButton();

        if (PACKAGE.PTR_ == deleteButtonSPtr_.get())
            return HandleCallback_DeleteButton();

        return false;
    }


    bool PartyStage::HandleCallback(const game::callback::PopupResponse & PACKAGE)
    {
        ResetMouseOverPopupState();
        willShowMouseOverPopup_ = true;

        if ((PACKAGE.Info().Name() == POPUP_NAME_STR_DELETE_CONFIRM_) &&
            (PACKAGE.Response() == sfml_util::Response::Yes))
        {
            auto selectedItemSPtr{ GetSelectedItemSPtr() };

            if ((selectedItemSPtr.get() != nullptr) &&
                (selectedItemSPtr->CHARACTER_CPTR != nullptr))
            {
                auto characterPtr{ selectedItemSPtr->CHARACTER_CPTR };

                if (state::GameStateFactory::Instance()->DeleteCharacter(characterPtr) == false)
                {
                    M_HP_LOG_ERR("game::stage::PartyStage::HandleCallback(delete confirm popup)"
                        << " unable to delete character \"" << characterPtr->Name() << "\" file.");
                }
                
                characterListBoxSPtr_->Remove(selectedItemSPtr);
                partyListBoxSPtr_->Remove(selectedItemSPtr);
                //actual Character object will be free'd when the PartyStage object is destroyed
            }
        }

        return false;
    }


    bool PartyStage::HandleCallback_BackButton()
    {
        game::LoopManager::Instance()->Goto_CharacterCreation();
        return true;
    }


    bool PartyStage::HandleCallback_StartButton()
    {
        if (partyListBoxSPtr_->GetCount() != player::Party::MAX_CHARACTER_COUNT_)
        {
            std::ostringstream ss;
            ss << "There are " << partyListBoxSPtr_->GetCount() << " characters in your party.  You need exactly " << player::Party::MAX_CHARACTER_COUNT_ << " characters to start the game.";
            const PopupInfo POPUP_INFO(sfml_util::gui::PopupManager::Instance()->CreatePopupInfo(POPUP_NAME_STR_NOT_ENOUGH_CHARS_,
                                                                                                 ss.str(),
                                                                                                 sfml_util::PopupButtons::Okay,
                                                                                                 sfml_util::PopupImage::Banner,
                                                                                                 sfml_util::Justified::Center,
                                                                                                 sfml_util::sound_effect::PromptQuestion));

            LoopManager::Instance()->PopupWaitBegin(this, POPUP_INFO);
            return false;
        }
        else
        {
            //determine if any beasts are in the party
            bool isAnyCharacterBeast(false);
            const std::size_t NUM_CHARACTERS( partyListBoxSPtr_->GetCount() );
            for (std::size_t i(0); i < NUM_CHARACTERS; ++i)
            {
                const creature::race::Enum NEXT_CHAR_RACE(
                    partyListBoxSPtr_->At(i)->CHARACTER_CPTR->Race().Which());

                if ((creature::race::Dragon == NEXT_CHAR_RACE) ||
                    (creature::race::Wolfen == NEXT_CHAR_RACE))
                {
                    isAnyCharacterBeast = true;
                    break;
                }
            }

            //determine if any are beastmasters
            bool isAnyCharacterBeastmaster(false);
            for (std::size_t i(0); i < NUM_CHARACTERS; ++i)
            {
                const creature::role::Enum NEXT_CHAR_ROLE(
                    partyListBoxSPtr_->At(i)->CHARACTER_CPTR->Role().Which());

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

                const PopupInfo POP_INFO(sfml_util::gui::PopupManager::Instance()->CreatePopupInfo(
                    POPUP_NAME_STR_NOT_ENOUGH_CHARS_,
                    ss.str(),
                    sfml_util::PopupButtons::Okay,
                    sfml_util::PopupImage::RegularSidebar,
                    sfml_util::Justified::Left,
                    sfml_util::sound_effect::PromptWarn));

                LoopManager::Instance()->PopupWaitBegin(this, POP_INFO);
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
        const PopupInfo POPUP_INFO(sfml_util::gui::PopupManager::Instance()->CreatePopupInfo(
            POPUP_NAME_STR_DELETE_CONFIRM_,
            ss.str(),
            sfml_util::PopupButtons::YesNo,
            sfml_util::PopupImage::Banner,
            sfml_util::Justified::Center,
            sfml_util::sound_effect::PromptWarn));

        LoopManager::Instance()->PopupWaitBegin(this, POPUP_INFO);
        return true;
    }


    void PartyStage::Setup()
    {
        //ouroboros
        ouroborosUPtr_.reset( new Ouroboros("PartyStage's") );
        EntityAdd(ouroborosUPtr_.get());

        //back button
        backButtonSPtr_->SetScaleToRes();
        backButtonSPtr_->SetVertPositionToBottomOfScreenByRes(sfml_util::MapByRes(75.0f, 800.0f));
        backButtonSPtr_->SetCallbackHandler(this);
        EntityAdd(backButtonSPtr_.get());

        //start game button
        startButtonSPtr_->SetScaleToRes();
        startButtonSPtr_->SetVertPositionToBottomOfScreenByRes((SCREEN_WIDTH_ * 0.5f) + 110.0f);
        startButtonSPtr_->SetCallbackHandler(this);
        EntityAdd(startButtonSPtr_.get());

        //delete game button
        deleteButtonSPtr_->SetScaleToRes();
        deleteButtonSPtr_->SetVertPositionToBottomOfScreenByRes((SCREEN_WIDTH_ * 0.5f) - 325.0f);
        deleteButtonSPtr_->SetCallbackHandler(this);
        EntityAdd(deleteButtonSPtr_.get());

        //top instruction text
        {
            sfml_util::gui::TextInfo insTextInfo("(double-click or use the arrow keys and enter to move characters back and forth)",
                                                 sfml_util::FontManager::Instance()->Font_Typical(),
                                                 sfml_util::FontManager::Instance()->Size_Smallish(),
                                                 sfml_util::FontManager::Color_GrayLight(),
                                                 sf::BlendAlpha,
                                                 sf::Text::Italic,
                                                 sfml_util::Justified::Left);

            insTextRegionUPtr_.reset( new sfml_util::gui::TextRegion("Instructions", insTextInfo, sf::FloatRect()) );
            insTextRegionUPtr_->SetEntityPos((SCREEN_WIDTH_ * 0.5f) - (insTextRegionUPtr_->GetEntityRegion().width * 0.5f) + 125.0f, mainMenuTitle_.LowerPosition() - 45.0f);
            EntityAdd(insTextRegionUPtr_.get());
        }

        //load all players not yet assigned to a party/started game
        charactersPSet_ = game::state::GameStateFactory::Instance()->LoadAllCompanions();

        //fill a list with TextRegions for the character names
        sfml_util::gui::ListBoxItemSLst_t itemSList;
        sfml_util::gui::TextInfo textInfo("",
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
            ssTitle << NEXT_CHAR_PTR->Name() << ", " << NEXT_CHAR_PTR->Race().Name();

            if (NEXT_CHAR_PTR->Race().Which() != creature::race::Wolfen)
            {
                ssTitle << ", " << NEXT_CHAR_PTR->Role().Name();
            }

            textInfo.text = ssTitle.str();

            auto nextCharTextSPtr = std::make_shared<sfml_util::gui::ListBoxItem>(
                ssCharText.str(),
                textInfo,
                NEXT_CHAR_PTR);

            itemSList.push_back( nextCharTextSPtr );
        }

        //establish the longest line that needs to fit in the listboxes
        std::ostringstream longestSS;
        longestSS << creature::NameInfo::Instance()->LargestName() << ", " << "Human, Beastmaster";
        const sfml_util::gui::TextInfo LONGEST_TEXT_INFO( creature::NameInfo::Instance()->MakeTextInfo(longestSS.str()) );
        const float LIST_BOX_WIDTH(game::creature::NameInfo::Instance()->Length(LONGEST_TEXT_INFO) + sfml_util::MapByRes(50.0f, 300.0f));

        //player list box
        const float CHAR_LIST_POS_LEFT  (((SCREEN_WIDTH_ * 0.1f) - 75.0f) + sfml_util::MapByRes(0.0f, 800.0f));
        const float CHAR_LIST_WIDTH     (LIST_BOX_WIDTH);
        const float HALF_SCREEN_HEIGHT  (SCREEN_HEIGHT_ * 0.5f);
        const float CHAR_LIST_HEIGHT    (sfml_util::MapByRes(HALF_SCREEN_HEIGHT + 20.0f, HALF_SCREEN_HEIGHT + 300.0f));
        const float CHAR_LIST_POS_TOP   (((SCREEN_HEIGHT_ * 0.5f) - (CHAR_LIST_HEIGHT * 0.5f)) + sfml_util::MapByRes(70.0f, 350.0f));
        //
        const sf::FloatRect CHAR_LIST_RECT(CHAR_LIST_POS_LEFT,
                                           CHAR_LIST_POS_TOP,
                                           CHAR_LIST_WIDTH,
                                           CHAR_LIST_HEIGHT);
        //
        const sf::Color BG_COLOR(sfml_util::FontManager::Color_Orange() - sf::Color(100, 100, 100, 220));
        sfml_util::gui::BackgroundInfo bgInfo(BG_COLOR);
        sfml_util::gui::box::Info boxInfo(1,
                                          true,
                                          CHAR_LIST_RECT,
                                          sfml_util::gui::ColorSet(sfml_util::FontManager::Color_Orange(),
                                                                   BG_COLOR,
                                                                   sfml_util::FontManager::Color_Orange() - sfml_util::gui::ColorSet::DEFAULT_OFFSET_COLOR_,
                                                                   BG_COLOR - sf::Color(40,40,40,0)),
                                          bgInfo);
        //
        characterListBoxSPtr_.reset( new sfml_util::gui::ListBox("PartyStage'sCharacter",
                                                                 CHAR_LIST_RECT,
                                                                 itemSList,
                                                                 this,
                                                                 10.0f,
                                                                 6.0f,
                                                                 boxInfo,
                                                                 sfml_util::FontManager::Color_Orange(),
                                                                 sfml_util::gui::ListBox::NO_LIMIT_,
                                                                 this) );
        EntityAdd(characterListBoxSPtr_.get());

        //party list box
        const float PARTY_LIST_POS_LEFT ((SCREEN_WIDTH_ * 0.55f) - 25.0f);
        const float PARTY_LIST_POS_TOP  (CHAR_LIST_POS_TOP);
        const float PARTY_LIST_WIDTH    (LIST_BOX_WIDTH);
        const float PARTY_LIST_HEIGHT   (CHAR_LIST_HEIGHT);
        //
        const sf::FloatRect PARTY_LIST_RECT(PARTY_LIST_POS_LEFT,
                                            PARTY_LIST_POS_TOP,
                                            PARTY_LIST_WIDTH,
                                            PARTY_LIST_HEIGHT);
        //
        boxInfo.SetBoxAndBackgroundRegion(PARTY_LIST_RECT);
        partyListBoxSPtr_.reset( new sfml_util::gui::ListBox("PartyStage'sParty",
                                                             PARTY_LIST_RECT,
                                                             sfml_util::gui::ListBoxItemSLst_t(),
                                                             this,
                                                             10.0f,
                                                             6.0f,
                                                             boxInfo,
                                                             sfml_util::FontManager::Color_Orange(),
                                                             player::Party::MAX_CHARACTER_COUNT_,
                                                             this) );
        EntityAdd(partyListBoxSPtr_.get());

        //unplayed character label text
        {
            sfml_util::gui::TextInfo labelTextInfo("Unplayed Characters",
                                                   sfml_util::FontManager::Instance()->Font_Default2(),
                                                   sfml_util::FontManager::Instance()->Size_Largeish(),
                                                   sfml_util::FontManager::Color_Orange() + sf::Color(0, 30, 30, 0));

            upTextRegionUPtr_.reset( new sfml_util::gui::TextRegion("CharacterLabel", labelTextInfo, sf::FloatRect()) );
            upTextRegionUPtr_->SetEntityPos(characterListBoxSPtr_->GetEntityRegion().left + 50.0f, (characterListBoxSPtr_->GetEntityRegion().top - upTextRegionUPtr_->GetEntityRegion().height));
            EntityAdd(upTextRegionUPtr_.get());
        }

        //party label text
        {
            sfml_util::gui::TextInfo labelTextInfo("New Party",
                                                   sfml_util::FontManager::Instance()->Font_Default2(),
                                                   sfml_util::FontManager::Instance()->Size_Largeish(),
                                                   sfml_util::FontManager::Color_Orange() + sf::Color(0, 30, 30, 0));

            partyTextRegionUPtr_.reset( new sfml_util::gui::TextRegion("PartyLabel", labelTextInfo, sf::FloatRect()) );
            partyTextRegionUPtr_->SetEntityPos(partyListBoxSPtr_->GetEntityRegion().left + 50.0f, (partyListBoxSPtr_->GetEntityRegion().top - partyTextRegionUPtr_->GetEntityRegion().height));
            EntityAdd(partyTextRegionUPtr_.get());
        }

        //warning instruction text
        {
            std::ostringstream ss;
            ss << "(There are not enough characters to make a party of " << player::Party::MAX_CHARACTER_COUNT_ << ".  Go back and create more.)";
            warningTextInfo_ = sfml_util::gui::TextInfo(ss.str(),
                                                        sfml_util::FontManager::Instance()->Font_Typical(),
                                                        sfml_util::FontManager::Instance()->Size_Smallish(),
                                                        sf::Color(255, 200, 200),
                                                        sf::BlendAlpha,
                                                        sf::Text::Italic,
                                                        sfml_util::Justified::Left);

            warningTextRegionUPtr_.reset( new sfml_util::gui::TextRegion("WarningsText", warningTextInfo_, sf::FloatRect()) );
            warningTextRegionUPtr_->SetEntityPos((SCREEN_WIDTH_ * 0.5f) - (warningTextRegionUPtr_->GetEntityRegion().width * 0.5f) + 110.0f,
                                                 (insTextRegionUPtr_->GetEntityRegion().top + insTextRegionUPtr_->GetEntityRegion().height) - sfml_util::MapByRes(0.0f, 20.0f));
        }
    }


    void PartyStage::Draw(sf::RenderTarget & target, const sf::RenderStates & STATES)
    {
        target.draw(backgroundImage_, STATES);
        target.draw(mainMenuTitle_, STATES);
        bottomSymbol_.Draw(target, STATES);
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
        return partyListBoxSPtr_->GetCount();
    }


    sfml_util::gui::ListBoxItemSPtr_t PartyStage::GetSelectedItemSPtr() const
    {
        if (partyListBoxSPtr_->HasFocus())
        {
            return partyListBoxSPtr_->GetSelected();
        }
        else
        {
            return characterListBoxSPtr_->GetSelected();
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

        willDisplayCharacterCountWarningText_ = ((characterListBoxSPtr_->GetCount() + partyListBoxSPtr_->GetCount()) < player::Party::MAX_CHARACTER_COUNT_);

        //oscillate the warning text color
        if (willDisplayCharacterCountWarningText_)
        {
            const sf::Uint8 NEW_COLOR_VAL(static_cast<sf::Uint8>(warningTextSlider_.Update(ELAPSED_TIME_SECONDS)));
            sf::Color color(warningTextRegionUPtr_->GetEntityColorForeground());
            color.r = 255;
            color.g = NEW_COLOR_VAL;
            color.b = NEW_COLOR_VAL;
            warningTextRegionUPtr_->SetEntityColorFgBoth(color);
        }

        mouseOverPopupTimerSec_ += ELAPSED_TIME_SECONDS;
        if ((mouseOverPopupTimerSec_ > MOUSE_OVER_POPUP_DELAY_SEC_) &&
            (false == isMouseOverTexture_))
        {
            auto itemSPtr(characterListBoxSPtr_->GetItemAtLocation(mouseOverPosV_));

            if (itemSPtr.get() == nullptr)
            {
                itemSPtr = partyListBoxSPtr_->GetItemAtLocation(mouseOverPosV_);
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
                mouseOverTexture_.setSmooth(true);
                mouseOverSprite_.setTexture(mouseOverTexture_ );
                mouseOverSprite_.setTextureRect( sf::IntRect(0, 0, static_cast<int>(mouseOverTexture_.getSize().x), static_cast<int>(mouseOverTexture_.getSize().y)) );
                mouseOverSprite_.setScale(0.0f, 0.0f);

                mouseOverBoxWidth_ = MOUSE_OVER_IMAGE_PAD_ + sfml_util::gui::CreatureImageManager::Instance()->DimmensionMax() + game::creature::NameInfo::Instance()->Length() + sfml_util::MapByRes(50.0f, 150.0f);
                mouseOverBoxHeight_ = mouseOverSprite_.getLocalBounds().height + (MOUSE_OVER_IMAGE_PAD_ * 2.0f) + sfml_util::MapByRes(0.0f, 600.0f);
            }
        }

        if (isMouseOverTexture_)
        {
            const float RATIO(mouseOverSlider_.Update(ELAPSED_TIME_SECONDS));

            const float CURR_BOX_WIDTH(mouseOverBoxWidth_ * RATIO);
            const float CURR_BOX_HEIGHT(mouseOverBoxHeight_ * RATIO);
            const float CURR_BOX_POS_LEFT(mouseOverPosV_.x - ((mouseOverPosV_.x - MOUSE_OVER_POPUP_POS_LEFT_) * RATIO));
            const float CURR_BOX_POS_TOP(mouseOverPosV_.y - ((mouseOverPosV_.y - MOUSE_OVER_POPUP_POS_TOP_) * RATIO));

            mouseOverQuad_[0].position = sf::Vector2f(CURR_BOX_POS_LEFT, CURR_BOX_POS_TOP);
            mouseOverQuad_[1].position = sf::Vector2f(CURR_BOX_POS_LEFT + CURR_BOX_WIDTH, CURR_BOX_POS_TOP);
            mouseOverQuad_[2].position = sf::Vector2f(CURR_BOX_POS_LEFT + CURR_BOX_WIDTH, CURR_BOX_POS_TOP + CURR_BOX_HEIGHT);
            mouseOverQuad_[3].position = sf::Vector2f(CURR_BOX_POS_LEFT, CURR_BOX_POS_TOP + CURR_BOX_HEIGHT);

            const sf::Uint8 NEW_ALPHA(static_cast<sf::Uint8>(MOUSE_OVER_BOX_ALPHA_ * RATIO));
            const sf::Color NEW_COLOR(0, 0, 0, NEW_ALPHA);
            mouseOverQuad_[0].color = NEW_COLOR;
            mouseOverQuad_[1].color = NEW_COLOR;
            mouseOverQuad_[2].color = NEW_COLOR;
            mouseOverQuad_[3].color = NEW_COLOR;

            mouseOverSprite_.setScale(RATIO, RATIO);
            const float SPRITE_POS_LEFT(mouseOverPosV_.x - ((mouseOverPosV_.x - (MOUSE_OVER_POPUP_POS_LEFT_ + MOUSE_OVER_IMAGE_PAD_)) * RATIO));
            const float SPRITE_POS_TOP (mouseOverPosV_.y - ((mouseOverPosV_.y - (MOUSE_OVER_POPUP_POS_TOP_  + ((mouseOverBoxHeight_ - mouseOverSprite_.getLocalBounds().height) * 0.5f))) * RATIO));
            mouseOverSprite_.setPosition(SPRITE_POS_LEFT, SPRITE_POS_TOP);

            if (mouseOverSlider_.GetIsDone())
            {
                std::ostringstream ss;
                ss << "\"" << mouseOverCharPtr_->Name() << "\"\n"
                   << mouseOverCharPtr_->SexName() << ", "
                   << mouseOverCharPtr_->Race().Name() << ", "
                   << mouseOverCharPtr_->Role().Name() << "\n"
                   << "Created on " << mouseOverCharPtr_->DateTimeCreated().date.ToString() << " at " << mouseOverCharPtr_->DateTimeCreated().time.ToString() << "\n\n"
                   << "Strength:        "      << mouseOverCharPtr_->Stats().Str().Normal() << "\n"
                   << "Accuracy:       "       << mouseOverCharPtr_->Stats().Acc().Normal() << "\n"
                   << "Charm:           "      << mouseOverCharPtr_->Stats().Cha().Normal() << "\n"
                   << "Luck:              "    << mouseOverCharPtr_->Stats().Lck().Normal() << "\n"
                   << "Speed:            "     << mouseOverCharPtr_->Stats().Spd().Normal() << "\n"
                   << "Intelligence:    "      << mouseOverCharPtr_->Stats().Int().Normal();

                const sf::FloatRect TEXT_RECT(MOUSE_OVER_POPUP_POS_LEFT_ + MOUSE_OVER_IMAGE_PAD_ + mouseOverSprite_.getGlobalBounds().width + MOUSE_OVER_IMAGE_PAD_,
                                              MOUSE_OVER_POPUP_POS_TOP_ + (MOUSE_OVER_IMAGE_PAD_ * 2.0f),
                                              mouseOverBoxWidth_,
                                              mouseOverBoxHeight_);

                const sfml_util::gui::TextInfo TEXT_INFO(ss.str(),
                                                         sfml_util::FontManager::Instance()->Font_Typical(),
                                                         sfml_util::FontManager::Instance()->Size_Smallish(),
                                                         sfml_util::FontManager::Color_Light(),
                                                         sfml_util::Justified::Left);

                mouseOverTextRegionUPtr_.reset( new sfml_util::gui::TextRegion("PartyStage'sMouseOverPopup", TEXT_INFO, TEXT_RECT) );
            }
        }
    }


    void PartyStage::UpdateMousePos(const sf::Vector2f & MOUSE_POS_V)
    {
        mouseOverPosV_ = MOUSE_POS_V;
        ResetMouseOverPopupState();
        Stage::UpdateMousePos(MOUSE_POS_V);
    }


    void PartyStage::StartNewGame()
    {
        //create a new party structure
        player::CharacterPVec_t charPVec;
        {
            for (std::size_t i(0); i < partyListBoxSPtr_->GetCount(); ++i)
            {
                auto characterPtr{ partyListBoxSPtr_->At(i)->CHARACTER_CPTR };
                charPVec.push_back(characterPtr);
                
                charactersPSet_.erase(characterPtr);

                if (state::GameStateFactory::Instance()->
                    DeleteCharacter(characterPtr) == false)
                {
                    M_HP_LOG_ERR("PartyStage::HandleCallback_StartButton() while trying to game::state::"
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
        //partyListBoxSPtr_->Clear();

        LoopManager::Instance()->Goto_Camp();
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
            backButtonSPtr_->SetMouseState(sfml_util::MouseState::Over);
            sfml_util::SoundManager::Instance()->SoundEffectsSet_Switch().PlayRandom();
            willShowMouseOverPopup_ = false;
            HandleCallback_BackButton();
            return true;
        }
        else if (KEY_EVENT.code == sf::Keyboard::D)
        {
            deleteButtonSPtr_->SetMouseState(sfml_util::MouseState::Over);
            sfml_util::SoundManager::Instance()->SoundEffectsSet_Switch().PlayRandom();
            willShowMouseOverPopup_ = false;
            HandleCallback_DeleteButton();
            return true;
        }
        else if (KEY_EVENT.code == sf::Keyboard::S)
        {
            startButtonSPtr_->SetMouseState(sfml_util::MouseState::Over);
            sfml_util::SoundManager::Instance()->SoundEffectsSet_Switch().PlayRandom();
            willShowMouseOverPopup_ = false;
            HandleCallback_StartButton();
            return true;
        }
        else
            return false;
    }


    void PartyStage::ResetMouseOverPopupState()
    {
        mouseOverPopupTimerSec_ = 0.0f;
        isMouseOverTexture_ = false;
        mouseOverTextRegionUPtr_.reset();
    }

}
}
