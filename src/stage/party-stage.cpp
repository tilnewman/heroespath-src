// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// party-stage.cpp
//
#include "party-stage.hpp"

#include "avatar/portrait-factory.hpp"
#include "creature/creature-warehouse.hpp"
#include "creature/creature.hpp"
#include "creature/name-info.hpp"
#include "game/game-data-file.hpp"
#include "game/game.hpp"
#include "game/loop-manager.hpp"
#include "log/log-macros.hpp"
#include "misc/real.hpp"
#include "player/party-factory.hpp"
#include "player/party.hpp"
#include "popup/popup-manager.hpp"
#include "popup/popup-stage-image-select.hpp"
#include "sfml-util/display.hpp"
#include "sfml-util/gui/creature-image-loader.hpp"
#include "sfml-util/gui/gui-elements.hpp"
#include "sfml-util/gui/list-box-item.hpp"
#include "sfml-util/gui/text-region.hpp"
#include "sfml-util/loaders.hpp"
#include "sfml-util/sfml-util.hpp"
#include "sfml-util/sound-manager.hpp"
#include "sfml-util/tile.hpp"
#include "state/game-state-factory.hpp"

#include <algorithm>
#include <sstream>

namespace heroespath
{
namespace stage
{

    const std::string PartyStage::POPUP_NAME_STR_NOT_ENOUGH_CHARS_{ "NotEnoughCharactersErrorMsg" };

    const std::string PartyStage::POPUP_NAME_STR_TOO_MANY_CHARS_{ "TooManyCharactersErrorMsg" };

    const std::string PartyStage::POPUP_NAME_STR_DELETE_CONFIRM_{ "CharacterDeleteConfirmMsg" };

    const std::string PartyStage::POPUP_NAME_STR_PARTY_IMAGE_SELECT_{ "PartyImageSelection" };

    const float PartyStage::MOUSE_OVER_POPUP_DELAY_SEC_{ 1.0f };
    const float PartyStage::MOUSE_OVER_SLIDER_SPEED_{ 4.0f };
    const float PartyStage::MOUSE_OVER_POPUP_POS_LEFT_{ 20.0f };
    const float PartyStage::MOUSE_OVER_POPUP_POS_TOP_{ 20.0f };
    const float PartyStage::MOUSE_OVER_IMAGE_PAD_{ 10.0f };
    const float PartyStage::MOUSE_OVER_BOX_ALPHA_{ 170.0f };

    PartyStage::PartyStage()
        : Stage("Party")
        , SCREEN_WIDTH_(sfml_util::Display::Instance()->GetWinWidth())
        , SCREEN_HEIGHT_(sfml_util::Display::Instance()->GetWinHeight())
        , mainMenuTitle_("create_party_normal.png")
        , backgroundImage_("media-images-backgrounds-tile-darkknot")
        , backButtonUPtr_(std::make_unique<sfml_util::gui::FourStateButton>(
              "PartyStage'sBack",
              0.0f,
              0.0f,
              std::string(
                  game::GameDataFile::Instance()->GetMediaPath("media-images-buttons-mainmenu-dir"))
                  .append("back_button_normal.png"),
              "",
              std::string(
                  game::GameDataFile::Instance()->GetMediaPath("media-images-buttons-mainmenu-dir"))
                  .append("back_button_lit.png")))
        , startButtonUPtr_(std::make_unique<sfml_util::gui::FourStateButton>(
              "PartyStage'sStartGame",
              0.0f,
              0.0f,
              std::string(
                  game::GameDataFile::Instance()->GetMediaPath("media-images-buttons-mainmenu-dir"))
                  .append("startgame_button_normal.png"),
              "",
              std::string(
                  game::GameDataFile::Instance()->GetMediaPath("media-images-buttons-mainmenu-dir"))
                  .append("startgame_button_lit.png")))
        , deleteButtonUPtr_(std::make_unique<sfml_util::gui::FourStateButton>(
              "PartyStage'sDelete",
              0.0f,
              0.0f,
              std::string(
                  game::GameDataFile::Instance()->GetMediaPath("media-images-buttons-mainmenu-dir"))
                  .append("delete_button_normal.png"),
              "",
              std::string(
                  game::GameDataFile::Instance()->GetMediaPath("media-images-buttons-mainmenu-dir"))
                  .append("delete_button_lit.png")))
        , characterListBoxUPtr_()
        , partyListBoxUPtr_()
        , insTextRegionUPtr_()
        , upTextRegionUPtr_()
        , partyTextRegionUPtr_()
        , warningTextInfo_()
        , warningTextRegionUPtr_()
        , warningTextSlider_(150, 255, 4.0f, static_cast<sf::Uint8>(misc::random::Int(150, 255)))
        , ouroborosUPtr_()
        , bottomSymbol_()
        , willDisplayCharacterCountWarningText_(false)
        , willShowMouseOverPopup_(true)
        , mouseOverPopupTimerSec_(0.0f)
        , mouseOverQuad_(sf::Quads, 4)
        , mouseOverBoxWidth_(0.0f)
        , mouseOverBoxHeight_(0.0f)
        , mouseOverPosV_()
        , mouseOverSprite_()
        , mouseOverTexture_()
        , isMouseOverTexture_(false)
        , mouseOverTextRegionUPtr_()
        , mouseOverSlider_(4.0f)
        , unplayedCharactersPVec_()
    {}

    PartyStage::~PartyStage()
    {
        Stage::ClearAllEntities();
        creature::CreatureWarehouse::Access().Free(unplayedCharactersPVec_);
    }

    bool PartyStage::HandleCallback(const sfml_util::gui::callback::ListBoxEventPackage & PACKAGE)
    {
        ResetMouseOverPopupState();

        // if this event was not triggered by a double-click then exit
        if (PACKAGE.gui_event != sfml_util::GuiEvent::DoubleClick)
        {
            return false;
        }

        // handle double-clicks by moving items from one ListBox to the other

        if (PACKAGE.package.PTR_ == characterListBoxUPtr_.get())
        {
            auto itemSPtr{ characterListBoxUPtr_->AtPos(PACKAGE.mouse_pos) };

            if (itemSPtr)
            {
                if (partyListBoxUPtr_->Size() < partyListBoxUPtr_->CountLimit())
                {
                    partyListBoxUPtr_->Add(itemSPtr);
                    characterListBoxUPtr_->Remove(itemSPtr);
                    sfml_util::SoundManager::Instance()->PlaySfx_AckMajor();
                }
                else
                {
                    sfml_util::SoundManager::Instance()->PlaySfx_Reject();
                }
            }

            return true;
        }
        else if (PACKAGE.package.PTR_ == partyListBoxUPtr_.get())
        {
            auto itemSPtr{ partyListBoxUPtr_->AtPos(PACKAGE.mouse_pos) };

            if (itemSPtr)
            {
                characterListBoxUPtr_->Add(itemSPtr);
                partyListBoxUPtr_->Remove(itemSPtr);
                sfml_util::SoundManager::Instance()->PlaySfx_AckMajor();
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

        if ((PACKAGE.Info().Name() == POPUP_NAME_STR_DELETE_CONFIRM_)
            && (PACKAGE.Response() == popup::ResponseTypes::Yes))
        {
            auto selectedItemSPtr{ GetSelectedItemSPtr() };

            if ((selectedItemSPtr) && selectedItemSPtr->CHARACTER_PTR_OPT)
            {
                auto const CHARACTER_PTR{ selectedItemSPtr->CHARACTER_PTR_OPT.value() };
                state::GameStateFactory::Instance()->DeleteUnplayedCharacterFile(CHARACTER_PTR);
                characterListBoxUPtr_->Remove(selectedItemSPtr);
                partyListBoxUPtr_->Remove(selectedItemSPtr);

                unplayedCharactersPVec_.erase(
                    std::remove(
                        std::begin(unplayedCharactersPVec_),
                        std::end(unplayedCharactersPVec_),
                        CHARACTER_PTR),
                    std::end(unplayedCharactersPVec_));

                creature::CreatureWarehouse::Access().Free(CHARACTER_PTR);
            }
        }
        else if (
            (PACKAGE.Info().Name() == POPUP_NAME_STR_PARTY_IMAGE_SELECT_)
            && (PACKAGE.Response() != popup::ResponseTypes::Cancel))
        {
            auto const SELECTED_NUM{ static_cast<misc::EnumUnderlying_t>(PACKAGE.Selection()) };
            auto const ANIM_NUM{ avatar::Avatar::Player_First + SELECTED_NUM };
            auto const ANIM_ENUM{ static_cast<avatar::Avatar::Enum>(ANIM_NUM) };
            if (avatar::Avatar::IsPlayer(ANIM_ENUM))
            {
                StartNewGame(ANIM_ENUM);
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
            NotEnoughCharactersPopup();
            return false;
        }

        if (AreAnyCharactersBeasts() && (AreAnyCharactersBeastmasters() == false))
        {
            MissingBeastmasterPopup();
            return false;
        }

        PartyAvatarSelectionPopup();
        return true;
    }

    bool PartyStage::HandleCallback_DeleteButton()
    {
        auto const SELECTED_CHARACTER_PTR_OPT{ GetSelectedCharacterPtrOpt() };

        if (!SELECTED_CHARACTER_PTR_OPT)
        {
            return false;
        }

        std::ostringstream ss;
        ss << "\n\nDelete " << SELECTED_CHARACTER_PTR_OPT.value()->Name()
           << "?  This cannot be undone.  Are you sure?";

        auto const POPUP_INFO{ popup::PopupManager::Instance()->CreatePopupInfo(
            POPUP_NAME_STR_DELETE_CONFIRM_,
            ss.str(),
            popup::PopupButtons::YesNo,
            popup::PopupImage::Regular,
            sfml_util::Justified::Center,
            sfml_util::sound_effect::PromptWarn) };

        game::LoopManager::Instance()->PopupWaitBegin(this, POPUP_INFO);
        return true;
    }

    void PartyStage::Setup()
    {
        // ouroboros
        ouroborosUPtr_ = std::make_unique<sfml_util::Ouroboros>("PartyStage's");
        EntityAdd(ouroborosUPtr_.get());

        // back button
        backButtonUPtr_->SetScaleToRes();
        backButtonUPtr_->SetVertPositionToBottomOfScreenByRes(sfml_util::MapByRes(75.0f, 800.0f));
        backButtonUPtr_->SetCallbackHandler(this);
        EntityAdd(backButtonUPtr_.get());

        // start game button
        startButtonUPtr_->SetScaleToRes();
        startButtonUPtr_->SetVertPositionToBottomOfScreenByRes((SCREEN_WIDTH_ * 0.5f) + 110.0f);
        startButtonUPtr_->SetCallbackHandler(this);
        EntityAdd(startButtonUPtr_.get());

        // delete game button
        deleteButtonUPtr_->SetScaleToRes();
        deleteButtonUPtr_->SetVertPositionToBottomOfScreenByRes((SCREEN_WIDTH_ * 0.5f) - 325.0f);
        deleteButtonUPtr_->SetCallbackHandler(this);
        EntityAdd(deleteButtonUPtr_.get());

        // top instruction text
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
                "Instructions", insTextInfo, sf::FloatRect());

            auto const HALF_INSTR_TEXT_WIDTH{ insTextRegionUPtr_->GetEntityRegion().width * 0.5f };

            insTextRegionUPtr_->SetEntityPos(
                ((SCREEN_WIDTH_ * 0.5f) - HALF_INSTR_TEXT_WIDTH) + 125.0f,
                mainMenuTitle_.Bottom() - 45.0f);

            EntityAdd(insTextRegionUPtr_.get());
        }

        // load all players not yet assigned to a party/started game
        unplayedCharactersPVec_ = state::GameStateFactory::Instance()->LoadAllUnplayedCharacters();

        // fill a list with TextRegions for the character names
        sfml_util::gui::ListBoxItemSVec_t itemSVec;
        sfml_util::gui::TextInfo textInfo(
            "",
            sfml_util::FontManager::Instance()->Font_Default2(),
            sfml_util::FontManager::Instance()->Size_Largeish(),
            sf::Color::White,
            sfml_util::Justified::Left);

        std::ostringstream ssCharText;
        std::ostringstream ssTitle;
        for (auto const & NEXT_CHAR_PTR : unplayedCharactersPVec_)
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
                ssCharText.str(), textInfo, NEXT_CHAR_PTR);

            itemSVec.emplace_back(nextCharTextSPtr);
        }

        std::sort(std::begin(itemSVec), std::end(itemSVec), [](auto const & A, auto const & B) {
            return (A->CHARACTER_PTR_OPT->Obj().Name() < B->CHARACTER_PTR_OPT->Obj().Name());
        });

        // establish the longest line that needs to fit in the listboxes
        std::ostringstream longestSS;
        creature::NameInfo creatureNameInfo;
        longestSS << creatureNameInfo.LargestName() << ", "
                  << "Human, Beastmaster";

        auto const LONGEST_TEXT_INFO{ creatureNameInfo.MakeTextInfo(longestSS.str()) };

        auto const LIST_BOX_WIDTH{ creatureNameInfo.Length(LONGEST_TEXT_INFO)
                                   + sfml_util::MapByRes(50.0f, 300.0f) };

        // player list box
        auto const CHAR_LIST_POS_LEFT{ ((SCREEN_WIDTH_ * 0.1f) - 75.0f)
                                       + sfml_util::MapByRes(0.0f, 800.0f) };

        auto const CHAR_LIST_WIDTH{ LIST_BOX_WIDTH };
        auto const HALF_SCREEN_HEIGHT{ SCREEN_HEIGHT_ * 0.5f };
        auto const CHAR_LIST_HEIGHT{ sfml_util::MapByRes(
            HALF_SCREEN_HEIGHT + 20.0f, HALF_SCREEN_HEIGHT + 300.0f) };

        auto const CHAR_LIST_POS_TOP{ ((SCREEN_HEIGHT_ * 0.5f) - (CHAR_LIST_HEIGHT * 0.5f))
                                      + sfml_util::MapByRes(70.0f, 350.0f) };

        const sf::FloatRect CHAR_LIST_RECT(
            CHAR_LIST_POS_LEFT, CHAR_LIST_POS_TOP, CHAR_LIST_WIDTH, CHAR_LIST_HEIGHT);

        auto const BG_COLOR{ sfml_util::FontManager::Color_Orange()
                             - sf::Color(100, 100, 100, 220) };

        sfml_util::gui::BackgroundInfo bgInfo(BG_COLOR);

        sfml_util::gui::box::Info boxInfo(
            1,
            true,
            CHAR_LIST_RECT,
            sfml_util::gui::ColorSet(
                sfml_util::FontManager::Color_Orange(),
                BG_COLOR,
                sfml_util::FontManager::Color_Orange()
                    - sfml_util::gui::ColorSet::DEFAULT_OFFSET_COLOR_,
                BG_COLOR - sf::Color(40, 40, 40, 0)),
            bgInfo);
        //
        characterListBoxUPtr_ = std::make_unique<sfml_util::gui::ListBox>(
            "PartyStage'sCharacter",
            CHAR_LIST_RECT,
            itemSVec,
            sfml_util::IStagePtr_t(this),
            10.0f,
            6.0f,
            boxInfo,
            sfml_util::FontManager::Color_Orange(),
            sfml_util::gui::callback::IListBoxCallbackHandlerPtr_t(this));

        characterListBoxUPtr_->ImageColor(sf::Color(255, 255, 255, 190));

        EntityAdd(characterListBoxUPtr_.get());

        // party list box
        auto const PARTY_LIST_POS_LEFT{ (SCREEN_WIDTH_ * 0.55f) - 25.0f };
        auto const PARTY_LIST_POS_TOP{ CHAR_LIST_POS_TOP };
        auto const PARTY_LIST_WIDTH{ LIST_BOX_WIDTH };
        auto const PARTY_LIST_HEIGHT{ CHAR_LIST_HEIGHT };

        const sf::FloatRect PARTY_LIST_RECT(
            PARTY_LIST_POS_LEFT, PARTY_LIST_POS_TOP, PARTY_LIST_WIDTH, PARTY_LIST_HEIGHT);

        boxInfo.SetBoxAndBackgroundRegion(PARTY_LIST_RECT);

        partyListBoxUPtr_ = std::make_unique<sfml_util::gui::ListBox>(
            "PartyStage'sParty",
            PARTY_LIST_RECT,
            sfml_util::gui::ListBoxItemSVec_t(),
            sfml_util::IStagePtr_t(this),
            10.0f,
            6.0f,
            boxInfo,
            sfml_util::FontManager::Color_Orange(),
            sfml_util::gui::callback::IListBoxCallbackHandlerPtr_t(this));

        partyListBoxUPtr_->ImageColor(sf::Color(255, 255, 255, 190));
        partyListBoxUPtr_->CountLimit(player::Party::MAX_CHARACTER_COUNT_);

        EntityAdd(partyListBoxUPtr_.get());

        // unplayed character label text
        {
            sfml_util::gui::TextInfo labelTextInfo(
                "Unplayed Characters",
                sfml_util::FontManager::Instance()->Font_Default2(),
                sfml_util::FontManager::Instance()->Size_Largeish(),
                sfml_util::FontManager::Color_Orange() + sf::Color(0, 30, 30, 0));

            upTextRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
                "CharacterLabel", labelTextInfo, sf::FloatRect());

            upTextRegionUPtr_->SetEntityPos(
                characterListBoxUPtr_->GetEntityRegion().left + 50.0f,
                (characterListBoxUPtr_->GetEntityRegion().top
                 - upTextRegionUPtr_->GetEntityRegion().height));

            EntityAdd(upTextRegionUPtr_.get());
        }

        // party label text
        {
            sfml_util::gui::TextInfo labelTextInfo(
                "New Party",
                sfml_util::FontManager::Instance()->Font_Default2(),
                sfml_util::FontManager::Instance()->Size_Largeish(),
                sfml_util::FontManager::Color_Orange() + sf::Color(0, 30, 30, 0));

            partyTextRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
                "PartyLabel", labelTextInfo, sf::FloatRect());

            partyTextRegionUPtr_->SetEntityPos(
                partyListBoxUPtr_->GetEntityRegion().left + 50.0f,
                (partyListBoxUPtr_->GetEntityRegion().top
                 - partyTextRegionUPtr_->GetEntityRegion().height));

            EntityAdd(partyTextRegionUPtr_.get());
        }

        // warning instruction text
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
                "WarningsText", warningTextInfo_, sf::FloatRect());

            auto const WARNING_TEXT_LEFT{ (SCREEN_WIDTH_ * 0.5f)
                                          - (warningTextRegionUPtr_->GetEntityRegion().width * 0.5f)
                                          + 110.0f };

            auto const WARNING_TEXT_TOP{ (insTextRegionUPtr_->GetEntityRegion().top
                                          + insTextRegionUPtr_->GetEntityRegion().height)
                                         - sfml_util::MapByRes(0.0f, 20.0f) };

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
            target.draw(*warningTextRegionUPtr_, STATES);
        }

        if (willShowMouseOverPopup_ && isMouseOverTexture_)
        {
            target.draw(mouseOverQuad_, STATES);
            target.draw(mouseOverSprite_, STATES);

            if (mouseOverTextRegionUPtr_)
            {
                mouseOverTextRegionUPtr_->draw(target, STATES);
            }
        }
    }

    std::size_t PartyStage::NumCharactersInTheParty() const { return partyListBoxUPtr_->Size(); }

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

    const creature::CreaturePtrOpt_t PartyStage::GetSelectedCharacterPtrOpt() const
    {
        auto itemSPtr{ GetSelectedItemSPtr() };

        if (itemSPtr)
        {
            return itemSPtr->CHARACTER_PTR_OPT;
        }

        return boost::none;
    }

    void PartyStage::UpdateTime(const float ELAPSED_TIME_SECONDS)
    {
        Stage::UpdateTime(ELAPSED_TIME_SECONDS);

        willDisplayCharacterCountWarningText_
            = ((characterListBoxUPtr_->Size() + partyListBoxUPtr_->Size())
               < player::Party::MAX_CHARACTER_COUNT_);

        // oscillate the warning text color
        if (willDisplayCharacterCountWarningText_)
        {
            auto const NEW_COLOR_VAL{ static_cast<sf::Uint8>(
                warningTextSlider_.Update(ELAPSED_TIME_SECONDS)) };

            auto color(warningTextRegionUPtr_->GetEntityColorForeground());

            color.r = 255;
            color.g = NEW_COLOR_VAL;
            color.b = NEW_COLOR_VAL;
            warningTextRegionUPtr_->SetEntityColorFgBoth(color);
        }

        mouseOverPopupTimerSec_ += ELAPSED_TIME_SECONDS;

        creature::CreaturePtrOpt_t mouseOverCharacterPtrOpt{ boost::none };

        if ((mouseOverPopupTimerSec_ > MOUSE_OVER_POPUP_DELAY_SEC_)
            && (false == isMouseOverTexture_))
        {
            auto itemSPtr{ characterListBoxUPtr_->AtPos(mouseOverPosV_) };

            if (!itemSPtr)
            {
                itemSPtr = partyListBoxUPtr_->AtPos(mouseOverPosV_);
            }

            if ((itemSPtr) && itemSPtr->CHARACTER_PTR_OPT
                && (itemSPtr->CHARACTER_PTR_OPT->Obj().ImageFilename().empty() == false))
            {
                mouseOverCharacterPtrOpt = itemSPtr->CHARACTER_PTR_OPT;

                mouseOverSlider_.Reset(MOUSE_OVER_SLIDER_SPEED_);

                sfml_util::gui::CreatureImageLoader creatureImageLoader;

                creatureImageLoader.GetImage(
                    mouseOverTexture_, itemSPtr->CHARACTER_PTR_OPT.value());

                isMouseOverTexture_ = true;
                mouseOverSprite_.setTexture(mouseOverTexture_);

                mouseOverSprite_.setTextureRect(sf::IntRect(
                    0,
                    0,
                    static_cast<int>(mouseOverTexture_.getSize().x),
                    static_cast<int>(mouseOverTexture_.getSize().y)));

                mouseOverSprite_.setScale(0.0f, 0.0f);

                creature::NameInfo creatureNameInfo;

                mouseOverBoxWidth_ = MOUSE_OVER_IMAGE_PAD_
                    + sfml_util::gui::CreatureImageLoader::MaxDimmension()
                    + creatureNameInfo.Length() + sfml_util::MapByRes(50.0f, 150.0f);

                mouseOverBoxHeight_ = mouseOverSprite_.getLocalBounds().height
                    + (MOUSE_OVER_IMAGE_PAD_ * 2.0f) + sfml_util::MapByRes(0.0f, 600.0f);
            }
        }

        if (isMouseOverTexture_)
        {
            auto const RATIO{ mouseOverSlider_.Update(ELAPSED_TIME_SECONDS) };

            auto const CURR_BOX_WIDTH{ mouseOverBoxWidth_ * RATIO };
            auto const CURR_BOX_HEIGHT{ mouseOverBoxHeight_ * RATIO };

            auto const CURR_BOX_POS_LEFT{
                mouseOverPosV_.x - ((mouseOverPosV_.x - MOUSE_OVER_POPUP_POS_LEFT_) * RATIO)
            };

            auto const CURR_BOX_POS_TOP{
                mouseOverPosV_.y - ((mouseOverPosV_.y - MOUSE_OVER_POPUP_POS_TOP_) * RATIO)
            };

            mouseOverQuad_[0].position = sf::Vector2f(CURR_BOX_POS_LEFT, CURR_BOX_POS_TOP);

            mouseOverQuad_[1].position
                = sf::Vector2f(CURR_BOX_POS_LEFT + CURR_BOX_WIDTH, CURR_BOX_POS_TOP);

            mouseOverQuad_[2].position = sf::Vector2f(
                CURR_BOX_POS_LEFT + CURR_BOX_WIDTH, CURR_BOX_POS_TOP + CURR_BOX_HEIGHT);

            mouseOverQuad_[3].position
                = sf::Vector2f(CURR_BOX_POS_LEFT, CURR_BOX_POS_TOP + CURR_BOX_HEIGHT);

            auto const NEW_ALPHA{ static_cast<sf::Uint8>(MOUSE_OVER_BOX_ALPHA_ * RATIO) };
            const sf::Color NEW_COLOR(0, 0, 0, NEW_ALPHA);
            mouseOverQuad_[0].color = NEW_COLOR;
            mouseOverQuad_[1].color = NEW_COLOR;
            mouseOverQuad_[2].color = NEW_COLOR;
            mouseOverQuad_[3].color = NEW_COLOR;

            mouseOverSprite_.setScale(RATIO, RATIO);

            auto const SPRITE_POS_LEFT{ mouseOverPosV_.x
                                        - ((mouseOverPosV_.x
                                            - (MOUSE_OVER_POPUP_POS_LEFT_ + MOUSE_OVER_IMAGE_PAD_))
                                           * RATIO) };

            auto const SPRITE_POS_TOP{
                mouseOverPosV_.y
                - ((mouseOverPosV_.y
                    - (MOUSE_OVER_POPUP_POS_TOP_
                       + ((mouseOverBoxHeight_ - mouseOverSprite_.getLocalBounds().height) * 0.5f)))
                   * RATIO)
            };

            mouseOverSprite_.setPosition(SPRITE_POS_LEFT, SPRITE_POS_TOP);

            if (mouseOverCharacterPtrOpt && (mouseOverSlider_.IsDone()))
            {
                auto const MOUSE_OVER_CHARACTER_PTR{ mouseOverCharacterPtrOpt.value() };

                std::ostringstream ss;
                ss << "\"" << MOUSE_OVER_CHARACTER_PTR->Name() << "\"\n"
                   << MOUSE_OVER_CHARACTER_PTR->SexName() << ", "
                   << MOUSE_OVER_CHARACTER_PTR->RaceName() << ", "
                   << MOUSE_OVER_CHARACTER_PTR->RoleName() << "\n"
                   << "Created on " << MOUSE_OVER_CHARACTER_PTR->DateTimeCreated().date.ToString()
                   << " at " << MOUSE_OVER_CHARACTER_PTR->DateTimeCreated().time.ToString()
                   << "\n\n"
                   << "Strength:        " << MOUSE_OVER_CHARACTER_PTR->Strength() << "\n"
                   << "Accuracy:       " << MOUSE_OVER_CHARACTER_PTR->Accuracy() << "\n"
                   << "Charm:           " << MOUSE_OVER_CHARACTER_PTR->Charm() << "\n"
                   << "Luck:              " << MOUSE_OVER_CHARACTER_PTR->Luck() << "\n"
                   << "Speed:            " << MOUSE_OVER_CHARACTER_PTR->Speed() << "\n"
                   << "Intelligence:    " << MOUSE_OVER_CHARACTER_PTR->Intelligence();

                const sf::FloatRect TEXT_RECT(
                    MOUSE_OVER_POPUP_POS_LEFT_ + MOUSE_OVER_IMAGE_PAD_
                        + mouseOverSprite_.getGlobalBounds().width + MOUSE_OVER_IMAGE_PAD_,
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
                    "PartyStage'sMouseOverPopup", TEXT_INFO, TEXT_RECT);
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

    void PartyStage::StartNewGame(const avatar::Avatar::Enum PARTY_AVATAR)
    {
        creature::CreaturePVec_t charPVec;
        for (auto const & ITEM_SPTR : partyListBoxUPtr_->Items())
        {
            M_ASSERT_OR_LOGANDTHROW_SS(
                (!!ITEM_SPTR->CHARACTER_PTR_OPT),
                "stage::PartyStage::StartNewGame() found a partyListBoxUPtr_ ItemSPtr_t with an "
                "uninitialized CHARACTER_PTR_OPT.");

            auto const CHARACTER_PTR{ ITEM_SPTR->CHARACTER_PTR_OPT.value() };

            charPVec.emplace_back(CHARACTER_PTR);

            unplayedCharactersPVec_.erase(
                std::remove(
                    std::begin(unplayedCharactersPVec_),
                    std::end(unplayedCharactersPVec_),
                    CHARACTER_PTR),
                std::end(unplayedCharactersPVec_));

            state::GameStateFactory::Instance()->DeleteUnplayedCharacterFile(CHARACTER_PTR);
        }

        // create a new GameState with the given party and then save it
        player::PartyFactory partyFactory;
        state::GameStateFactory::Instance()->NewGame(partyFactory.Make(PARTY_AVATAR, charPVec));

        // Don't bother clearing the party ListBox because it flashes the
        //"not engouh characters" text, and since we are immediately transitioning
        // to the Camp Stage anyway.
        // partyListBoxUPtr_->Clear();

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

    bool PartyStage::AreAnyCharactersBeasts() const
    {
        for (auto const & ITEM_SPTR : partyListBoxUPtr_->Items())
        {
            if (ITEM_SPTR->CHARACTER_PTR_OPT && (ITEM_SPTR->CHARACTER_PTR_OPT->Obj().IsBeast()))
            {
                return true;
            }
        }

        return false;
    }

    bool PartyStage::AreAnyCharactersBeastmasters() const
    {
        for (auto const & ITEM_SPTR : partyListBoxUPtr_->Items())
        {
            if (ITEM_SPTR->CHARACTER_PTR_OPT
                && (ITEM_SPTR->CHARACTER_PTR_OPT->Obj().Role() == creature::role::Beastmaster))
            {
                return true;
            }
        }

        return false;
    }

    void PartyStage::MissingBeastmasterPopup()
    {
        std::ostringstream ss;
        ss << "\nThere are beast characters in your party, but no character with the role"
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
    }

    void PartyStage::NotEnoughCharactersPopup()
    {
        std::ostringstream ss;
        ss << "\n\nThere are " << partyListBoxUPtr_->Size()
           << " characters in your party.  You need exactly " << player::Party::MAX_CHARACTER_COUNT_
           << " characters to start the game.";

        auto const POPUP_INFO{ popup::PopupManager::Instance()->CreatePopupInfo(
            POPUP_NAME_STR_NOT_ENOUGH_CHARS_,
            ss.str(),
            popup::PopupButtons::Okay,
            popup::PopupImage::Regular,
            sfml_util::Justified::Center,
            sfml_util::sound_effect::PromptQuestion) };

        game::LoopManager::Instance()->PopupWaitBegin(this, POPUP_INFO);
    }

    void PartyStage::PartyAvatarSelectionPopup()
    {
        sfml_util::TextureVec_t partyTextureVec;

        for (misc::EnumUnderlying_t i(avatar::Avatar::Player_First);
             i <= avatar::Avatar::Player_Last;
             ++i)
        {
            partyTextureVec.emplace_back(sf::Texture());
            auto const WHICH_AVATAR{ static_cast<avatar::Avatar::Enum>(i) };

            avatar::PortraitFactory::Make(
                WHICH_AVATAR, partyTextureVec[static_cast<std::size_t>(i)]);
        }

        std::ostringstream ss;
        ss << "Choose an avatar for your party...";

        auto const POPUP_INFO{ popup::PopupManager::Instance()->CreateImageSelectionPopupInfo(
            POPUP_NAME_STR_PARTY_IMAGE_SELECT_, ss.str(), partyTextureVec, false) };

        game::LoopManager::Instance()->PopupWaitBeginSpecific<popup::PopupStageImageSelect>(
            this, POPUP_INFO);
    }

} // namespace stage
} // namespace heroespath
