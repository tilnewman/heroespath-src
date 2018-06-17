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
#include "creature/player-party-factory.hpp"
#include "creature/player-party.hpp"
#include "game/game-data-file.hpp"
#include "game/game-state-factory.hpp"
#include "game/game.hpp"
#include "game/loop-manager.hpp"
#include "log/log-macros.hpp"
#include "misc/real.hpp"
#include "popup/popup-manager.hpp"
#include "popup/popup-stage-image-select.hpp"
#include "sfml-util/display.hpp"
#include "sfml-util/gui/creature-image-loader.hpp"
#include "sfml-util/gui/gui-elements.hpp"
#include "sfml-util/gui/list-box-item-factory.hpp"
#include "sfml-util/gui/list-box-item.hpp"
#include "sfml-util/gui/text-region.hpp"
#include "sfml-util/loaders.hpp"
#include "sfml-util/sfml-util.hpp"
#include "sfml-util/sound-manager.hpp"
#include "sfml-util/tile.hpp"

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

    const float PartyStage::MOUSEOVER_POPUP_DELAY_SEC_{ 1.0f };
    const float PartyStage::MOUSEOVER_SLIDER_SPEED_{ 4.0f };
    const float PartyStage::MOUSEOVER_BACKGROUND_FINAL_ALPHA_{ 120.0f };

    PartyStage::PartyStage()
        : Stage(
              "Party",
              sfml_util::Display::Instance()->FullscreenRect(),
              { sfml_util::Font::Default,
                sfml_util::Font::System,
                sfml_util::Font::SystemCondensed,
                sfml_util::Font::Number,
                sfml_util::Font::Handwriting },
              true)
        , LISTBOX_FONT_ENUM_(sfml_util::Font::System)
        , LISTBOX_FONT_SIZE_(sfml_util::FontManager::Instance()->Size_Largeish())
        , LISTBOX_WIDTH_(sfml_util::ScreenRatioToPixelsHoriz(0.33f))
        , LISTBOX_HEIGHT_(sfml_util::ScreenRatioToPixelsVert(0.5f))
        , BETWEEN_LISTBOXES_SPACER_(sfml_util::ScreenRatioToPixelsHoriz(0.077f))
        , CHAR_LISTBOX_POS_LEFT_(
              ((Stage::StageRegionWidth() * 0.5f) - (BETWEEN_LISTBOXES_SPACER_ * 0.5f))
              - LISTBOX_WIDTH_)
        , PARTY_LISTBOX_POS_LEFT_(
              (Stage::StageRegionWidth() * 0.5f) + (BETWEEN_LISTBOXES_SPACER_ * 0.5f))
        , MOUSEOVER_FINAL_INNER_EDGE_PAD_(sfml_util::ScreenRatioToPixelsHoriz(0.008f))
        , MOUSEOVER_CREATURE_IMAGE_WIDTH_MAX_(sfml_util::ScreenRatioToPixelsHoriz(0.115f))
        , MOUSEOVER_COLORCYCLE_START_(sf::Color::Transparent)
        , HEROESPATH_ORANGE_(sfml_util::FontManager::Instance()->Color_Orange())
        , MOUSEOVER_COLORCYCLE_ALT_(
              HEROESPATH_ORANGE_.r, HEROESPATH_ORANGE_.g, HEROESPATH_ORANGE_.b, 32)
        , MOUSEOVER_COLORCYCLE_SPEED_(28.0f)
        , MOUSEOVER_COLORCYCLE_COUNT_(4)
        , listBoxInfo_()
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
        , ouroborosUPtr_(std::make_unique<sfml_util::Ouroboros>("PartyStage's"))
        , bottomSymbol_()
        , willDisplayCharacterCountWarningText_(false)
        , unplayedCharactersPVec_()
        , colorShakerRect_()
        , willShowMouseOverPopup_(false)
        , mouseOverPopupTimerSec_(0.0f)
        , mouseOverBackground_()
        , mousePosV_()
        , mouseOverBackgroundRectFinal_()
        , mouseOverCreatureSprite_()
        , mouseOverCreatureTexture_()
        , mouseOverTextRegionUPtr_()
        , mouseOverSlider_(MOUSEOVER_SLIDER_SPEED_)
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
            auto const SELECTED_ITEM_PTR_OPT{ characterListBoxUPtr_->Selected() };

            if (SELECTED_ITEM_PTR_OPT)
            {
                if (partyListBoxUPtr_->Size() < creature::PlayerParty::MAX_CHARACTER_COUNT_)
                {
                    characterListBoxUPtr_->MoveItemToOtherListBox(
                        characterListBoxUPtr_->SelectedIndex(), *partyListBoxUPtr_);

                    sfml_util::SoundManager::Instance()->PlaySfx_AckMajor();
                }
                else
                {
                    sfml_util::SoundManager::Instance()->PlaySfx_Reject();
                }

                return true;
            }
        }
        else if (PACKAGE.package.PTR_ == partyListBoxUPtr_.get())
        {
            auto const SELECTED_ITEM_PTR_OPT{ partyListBoxUPtr_->Selected() };

            if (SELECTED_ITEM_PTR_OPT)
            {
                partyListBoxUPtr_->MoveItemToOtherListBox(
                    partyListBoxUPtr_->SelectedIndex(), *characterListBoxUPtr_);

                sfml_util::SoundManager::Instance()->PlaySfx_AckMajor();

                return true;
            }
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

        if ((PACKAGE.Info().Name() == POPUP_NAME_STR_DELETE_CONFIRM_)
            && (PACKAGE.Response() == popup::ResponseTypes::Yes))
        {
            auto const SELECTED_ITEM_PTR_OPT{ GetSelectedItemPtrOpt() };

            if (SELECTED_ITEM_PTR_OPT && SELECTED_ITEM_PTR_OPT.value()->CharacterPtrOpt())
            {
                auto const CHARACTER_PTR{
                    SELECTED_ITEM_PTR_OPT.value()->CharacterPtrOpt().value()
                };

                game::GameStateFactory::Instance()->DeleteUnplayedCharacterFile(CHARACTER_PTR);

                // NOTE after this block SELECTED_ITEM_PTR_OPT will be left dangling
                if (partyListBoxUPtr_->HasFocus())
                {
                    partyListBoxUPtr_->Remove(SELECTED_ITEM_PTR_OPT.value());
                }
                else
                {
                    characterListBoxUPtr_->Remove(SELECTED_ITEM_PTR_OPT.value());
                }

                unplayedCharactersPVec_.erase(
                    std::remove(
                        std::begin(unplayedCharactersPVec_),
                        std::end(unplayedCharactersPVec_),
                        CHARACTER_PTR),
                    std::end(unplayedCharactersPVec_));

                creature::CreatureWarehouse::Access().Free(CHARACTER_PTR);

                UpdateWillDisplayCharacterCountWarning();
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
        ResetMouseOverPopupState();
        game::LoopManager::Instance()->TransitionTo_CharacterCreation();
        return true;
    }

    bool PartyStage::HandleCallback_StartButton()
    {
        ResetMouseOverPopupState();

        if (partyListBoxUPtr_->Size() != creature::PlayerParty::MAX_CHARACTER_COUNT_)
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
        ResetMouseOverPopupState();

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
        Setup_Ouroboros();
        Setup_Buttons();
        Setup_TopInstructionText();
        Setup_TopWarningText();
        Setup_CharactersListBoxLabel();
        Setup_PartyListBoxLabel();
        Setup_CharactersListBox();
        Setup_PartyListBox();

        UpdateWillDisplayCharacterCountWarning();
        ResetMouseOverPopupState();
    }

    void PartyStage::Setup_Ouroboros() { EntityAdd(ouroborosUPtr_.get()); }

    void PartyStage::Setup_Buttons()
    {
        backButtonUPtr_->SetScaleToRes();
        backButtonUPtr_->SetVertPositionToBottomOfScreenByRes(sfml_util::MapByRes(75.0f, 800.0f));
        backButtonUPtr_->SetCallbackHandler(this);
        EntityAdd(backButtonUPtr_.get());

        startButtonUPtr_->SetScaleToRes();

        startButtonUPtr_->SetVertPositionToBottomOfScreenByRes(
            (Stage::StageRegionWidth() * 0.5f) + 110.0f);

        startButtonUPtr_->SetCallbackHandler(this);
        EntityAdd(startButtonUPtr_.get());

        deleteButtonUPtr_->SetScaleToRes();

        deleteButtonUPtr_->SetVertPositionToBottomOfScreenByRes(
            (Stage::StageRegionWidth() * 0.5f) - 325.0f);

        deleteButtonUPtr_->SetCallbackHandler(this);
        EntityAdd(deleteButtonUPtr_.get());
    }

    void PartyStage::Setup_TopInstructionText()
    {
        sfml_util::gui::TextInfo insTextInfo(
            "(double-click or use the arrow keys and enter to move characters back and forth)",
            sfml_util::FontManager::Instance()->GetFont(sfml_util::Font::SystemCondensed),
            sfml_util::FontManager::Instance()->Size_Smallish(),
            sfml_util::FontManager::Color_GrayLight(),
            sf::BlendAlpha,
            sf::Text::Italic,
            sfml_util::Justified::Left);

        insTextRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
            "Instructions", insTextInfo, sf::FloatRect());

        auto const HALF_INSTR_TEXT_WIDTH{ insTextRegionUPtr_->GetEntityRegion().width * 0.5f };

        insTextRegionUPtr_->SetEntityPos(
            ((Stage::StageRegionWidth() * 0.5f) - HALF_INSTR_TEXT_WIDTH) + 125.0f,
            mainMenuTitle_.Bottom() - 45.0f);

        EntityAdd(insTextRegionUPtr_.get());
    }

    void PartyStage::Setup_TopWarningText()
    {
        std::ostringstream warningSS;
        warningSS << "(There are not enough characters to make a party of six.  Go back and "
                     "create more.)";

        warningTextInfo_ = sfml_util::gui::TextInfo(
            warningSS.str(),
            sfml_util::FontManager::Instance()->GetFont(sfml_util::Font::SystemCondensed),
            sfml_util::FontManager::Instance()->Size_Smallish(),
            sf::Color(255, 200, 200),
            sf::BlendAlpha,
            sf::Text::Italic,
            sfml_util::Justified::Left);

        warningTextRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
            "WarningsText", warningTextInfo_, sf::FloatRect());

        auto const WARNING_TEXT_LEFT{ (Stage::StageRegionWidth() * 0.5f)
                                      - (warningTextRegionUPtr_->GetEntityRegion().width * 0.5f)
                                      + 110.0f };

        auto const WARNING_TEXT_TOP{ (insTextRegionUPtr_->GetEntityRegion().top
                                      + insTextRegionUPtr_->GetEntityRegion().height)
                                     - sfml_util::ScreenRatioToPixelsVert(0.0025f) };

        warningTextRegionUPtr_->SetEntityPos(WARNING_TEXT_LEFT, WARNING_TEXT_TOP);
    }

    void PartyStage::Setup_CharactersListBoxLabel()
    {
        sfml_util::gui::TextInfo labelTextInfo(
            "Unplayed Characters",
            sfml_util::FontManager::Instance()->GetFont(sfml_util::Font::System),
            sfml_util::FontManager::Instance()->Size_Largeish(),
            sfml_util::FontManager::Color_Orange() + sf::Color(0, 30, 30, 0));

        upTextRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
            "CharacterLabel", labelTextInfo, sf::FloatRect());

        upTextRegionUPtr_->SetEntityPos(
            CHAR_LISTBOX_POS_LEFT_ + sfml_util::ScreenRatioToPixelsHoriz(0.025f),
            (warningTextRegionUPtr_->GetEntityRegion().top
             + warningTextRegionUPtr_->GetEntityRegion().height)
                + sfml_util::ScreenRatioToPixelsVert(0.0025f));

        EntityAdd(upTextRegionUPtr_.get());
    }

    void PartyStage::Setup_PartyListBoxLabel()
    {
        sfml_util::gui::TextInfo labelTextInfo(
            "New Party",
            sfml_util::FontManager::Instance()->GetFont(sfml_util::Font::System),
            sfml_util::FontManager::Instance()->Size_Largeish(),
            sfml_util::FontManager::Color_Orange() + sf::Color(0, 30, 30, 0));

        partyTextRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
            "PartyLabel", labelTextInfo, sf::FloatRect());

        partyTextRegionUPtr_->SetEntityPos(
            PARTY_LISTBOX_POS_LEFT_ + sfml_util::ScreenRatioToPixelsHoriz(0.025f),
            (warningTextRegionUPtr_->GetEntityRegion().top
             + warningTextRegionUPtr_->GetEntityRegion().height)
                + sfml_util::ScreenRatioToPixelsVert(0.0025f));

        EntityAdd(partyTextRegionUPtr_.get());
    }

    void PartyStage::Setup_CharactersListBox()
    {
        auto const CHAR_LIST_POS_LEFT{ CHAR_LISTBOX_POS_LEFT_ };
        auto const CHAR_LIST_WIDTH{ LISTBOX_WIDTH_ };
        auto const CHAR_LIST_HEIGHT{ LISTBOX_HEIGHT_ };

        auto const CHAR_LIST_POS_TOP{
            (upTextRegionUPtr_->GetEntityRegion().top + upTextRegionUPtr_->GetEntityRegion().height)
        };

        const sf::FloatRect CHAR_LIST_RECT(
            CHAR_LIST_POS_LEFT, CHAR_LIST_POS_TOP, CHAR_LIST_WIDTH, CHAR_LIST_HEIGHT);

        auto const BG_COLOR{ sfml_util::FontManager::Color_Orange()
                             - sf::Color(100, 100, 100, 220) };

        sfml_util::gui::BackgroundInfo bgInfo(BG_COLOR);

        listBoxInfo_ = sfml_util::gui::box::Info(
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

        characterListBoxUPtr_ = std::make_unique<sfml_util::gui::ListBox>(
            "PartyStage'sCharacter",
            CHAR_LIST_RECT,
            sfml_util::IStagePtr_t(this),
            listBoxInfo_,
            sfml_util::FontManager::Color_Orange(),
            sfml_util::gui::callback::IListBoxCallbackHandlerPtr_t(this));

        // load all players not yet assigned to a party/started game
        unplayedCharactersPVec_ = game::GameStateFactory::Instance()->LoadAllUnplayedCharacters();

        sfml_util::gui::TextInfo textInfo(
            "",
            sfml_util::FontManager::Instance()->GetFont(LISTBOX_FONT_ENUM_),
            LISTBOX_FONT_SIZE_);

        sfml_util::gui::ListBoxItemFactory listBoxItemFactory;

        for (auto const & CHARACTER_PTR : unplayedCharactersPVec_)
        {
            std::ostringstream ssTitle;
            ssTitle << CHARACTER_PTR->Name() << ", " << CHARACTER_PTR->RaceName();

            if (CHARACTER_PTR->Race() != creature::race::Wolfen)
            {
                ssTitle << ", " << CHARACTER_PTR->RoleName();
            }

            textInfo.text = ssTitle.str();

            characterListBoxUPtr_->Add(listBoxItemFactory.Make(
                characterListBoxUPtr_->GetEntityName(), textInfo, CHARACTER_PTR));
        }

        std::sort(
            std::begin(*characterListBoxUPtr_),
            std::end(*characterListBoxUPtr_),
            [](auto const & A, auto const & B) {
                return (
                    A->CharacterPtrOpt().value()->Name() < B->CharacterPtrOpt().value()->Name());
            });

        EntityAdd(characterListBoxUPtr_.get());
    }

    void PartyStage::Setup_PartyListBox()
    {
        auto const PARTY_LIST_POS_LEFT{ PARTY_LISTBOX_POS_LEFT_ };

        auto const PARTY_LIST_POS_TOP{
            (upTextRegionUPtr_->GetEntityRegion().top + upTextRegionUPtr_->GetEntityRegion().height)
        };

        auto const PARTY_LIST_WIDTH{ LISTBOX_WIDTH_ };
        auto const PARTY_LIST_HEIGHT{ LISTBOX_HEIGHT_ };

        const sf::FloatRect PARTY_LIST_RECT(
            PARTY_LIST_POS_LEFT, PARTY_LIST_POS_TOP, PARTY_LIST_WIDTH, PARTY_LIST_HEIGHT);

        listBoxInfo_.SetBoxAndBackgroundRegion(PARTY_LIST_RECT);

        partyListBoxUPtr_ = std::make_unique<sfml_util::gui::ListBox>(
            "PartyStage'sParty",
            PARTY_LIST_RECT,
            sfml_util::IStagePtr_t(this),
            listBoxInfo_,
            sfml_util::FontManager::Color_Orange(),
            sfml_util::gui::callback::IListBoxCallbackHandlerPtr_t(this));

        partyListBoxUPtr_->ImageColor(sf::Color(255, 255, 255, 190));
        EntityAdd(partyListBoxUPtr_.get());
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

        if (willShowMouseOverPopup_)
        {
            target.draw(colorShakerRect_, STATES);
            target.draw(mouseOverBackground_, STATES);
            target.draw(mouseOverCreatureSprite_, STATES);

            if (mouseOverTextRegionUPtr_ && mouseOverSlider_.IsDone())
            {
                mouseOverTextRegionUPtr_->draw(target, STATES);
            }
        }
    }

    sfml_util::gui::ListBoxItemPtrOpt_t PartyStage::GetSelectedItemPtrOpt() const
    {
        if (partyListBoxUPtr_->HasFocus())
        {
            return partyListBoxUPtr_->Selected();
        }
        else if (characterListBoxUPtr_->HasFocus())
        {
            return characterListBoxUPtr_->Selected();
        }
        else
        {
            return boost::none;
        }
    }

    const creature::CreaturePtrOpt_t PartyStage::GetSelectedCharacterPtrOpt() const
    {
        auto const SELECTED_ITEM_PTR_OPT{ GetSelectedItemPtrOpt() };

        if (SELECTED_ITEM_PTR_OPT)
        {
            return SELECTED_ITEM_PTR_OPT.value()->CharacterPtrOpt();
        }
        else
        {
            return boost::none;
        }
    }

    void PartyStage::UpdateTime(const float ELAPSED_TIME_SECONDS)
    {
        Stage::UpdateTime(ELAPSED_TIME_SECONDS);
        UpdateTime_WarningTextColorCycling(ELAPSED_TIME_SECONDS);
        UpdateTime_MouseOver_Detection(ELAPSED_TIME_SECONDS);
        UpdateTime_MouseOver_Animation(ELAPSED_TIME_SECONDS);
    }

    void PartyStage::UpdateTime_WarningTextColorCycling(const float ELAPSED_TIME_SECONDS)
    {
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
    }

    void PartyStage::UpdateTime_MouseOver_Detection(const float ELAPSED_TIME_SECONDS)
    {
        if (willShowMouseOverPopup_)
        {
            return;
        }

        mouseOverPopupTimerSec_ += ELAPSED_TIME_SECONDS;

        if ((mouseOverPopupTimerSec_ > MOUSEOVER_POPUP_DELAY_SEC_)
            && (false == willShowMouseOverPopup_))
        {
            auto mouseOverListItemPtrOpt{ characterListBoxUPtr_->AtPos(mousePosV_) };

            auto isItemFromCharacterListBox{ true };
            if (!mouseOverListItemPtrOpt)
            {
                isItemFromCharacterListBox = false;
                mouseOverListItemPtrOpt = partyListBoxUPtr_->AtPos(mousePosV_);
            }

            if (mouseOverListItemPtrOpt && mouseOverListItemPtrOpt.value()->CharacterPtrOpt())
            {
                auto const CHARACTER_PTR{
                    mouseOverListItemPtrOpt.value()->CharacterPtrOpt().value()
                };

                willShowMouseOverPopup_ = true;
                mouseOverSlider_.Reset(MOUSEOVER_SLIDER_SPEED_);
                sfml_util::gui::CreatureImageLoader creatureImageLoader;
                creatureImageLoader.Load(mouseOverCreatureTexture_, CHARACTER_PTR);
                mouseOverCreatureSprite_.setTexture(mouseOverCreatureTexture_, true);
                SetupMouseOverPositionsAndDimmensions(CHARACTER_PTR);

                auto const LISTBOX_ITEM_RECT{ (
                    (isItemFromCharacterListBox)
                        ? characterListBoxUPtr_->FullItemRect(mouseOverListItemPtrOpt.value())
                        : partyListBoxUPtr_->FullItemRect(mouseOverListItemPtrOpt.value())) };

                colorShakerRect_.Reset(
                    LISTBOX_ITEM_RECT,
                    MOUSEOVER_COLORCYCLE_START_,
                    MOUSEOVER_COLORCYCLE_ALT_,
                    MOUSEOVER_COLORCYCLE_SPEED_,
                    MOUSEOVER_COLORCYCLE_COUNT_);

                colorShakerRect_.Start();
                mouseOverBackground_.Color(sf::Color::Transparent);
            }
        }
    }

    void PartyStage::UpdateTime_MouseOver_Animation(const float ELAPSED_TIME_SECONDS)
    {
        if (false == willShowMouseOverPopup_)
        {
            return;
        }

        if (colorShakerRect_.IsShaking())
        {
            colorShakerRect_.Update(ELAPSED_TIME_SECONDS);
        }
        else
        {
            if (mouseOverSlider_.IsDone())
            {
                return;
            }

            auto const RATIO{ mouseOverSlider_.Update(ELAPSED_TIME_SECONDS) };

            auto const BG_WIDTH{ mouseOverBackgroundRectFinal_.width * RATIO };
            auto const BG_HEIGHT{ mouseOverBackgroundRectFinal_.height * RATIO };

            auto const BG_LEFT{ mousePosV_.x
                                - ((mousePosV_.x - mouseOverBackgroundRectFinal_.left) * RATIO) };

            auto const BG_TOP{ mousePosV_.y
                               - ((mousePosV_.y - mouseOverBackgroundRectFinal_.top) * RATIO) };

            mouseOverBackground_.Rect(sf::FloatRect(BG_LEFT, BG_TOP, BG_WIDTH, BG_HEIGHT));

            auto const NEW_ALPHA{ static_cast<sf::Uint8>(
                MOUSEOVER_BACKGROUND_FINAL_ALPHA_ * RATIO) };

            mouseOverBackground_.Color(sf::Color(0, 0, 0, NEW_ALPHA));

            auto const IMAGE_LEFT{
                mousePosV_.x
                - ((mousePosV_.x
                    - (mouseOverBackgroundRectFinal_.left + MOUSEOVER_FINAL_INNER_EDGE_PAD_))
                   * RATIO)
            };

            auto const IMAGE_TOP{
                mousePosV_.y
                - ((mousePosV_.y
                    - (mouseOverBackgroundRectFinal_.top + MOUSEOVER_FINAL_INNER_EDGE_PAD_))
                   * RATIO)
            };

            mouseOverCreatureSprite_.setPosition(IMAGE_LEFT, IMAGE_TOP);

            auto const IMAGE_WIDTH_AVAIL{ BG_WIDTH - (MOUSEOVER_FINAL_INNER_EDGE_PAD_ * 3.0f) };
            auto const IMAGE_HEIGHT_AVAIL{ BG_HEIGHT - (MOUSEOVER_FINAL_INNER_EDGE_PAD_ * 2.0f) };
            if ((IMAGE_WIDTH_AVAIL > 1.0f) && (IMAGE_HEIGHT_AVAIL > 1.0f))
            {
                sfml_util::ScaleSpriteToFit(
                    mouseOverCreatureSprite_,
                    std::min(IMAGE_WIDTH_AVAIL, MOUSEOVER_CREATURE_IMAGE_WIDTH_MAX_),
                    IMAGE_HEIGHT_AVAIL);
            }
        }
    }

    void PartyStage::UpdateMousePos(const sf::Vector2i & NEW_MOUSE_POS_V)
    {
        auto const NEW_MOUSE_POS_VF{ sfml_util::ConvertVector<int, float>(NEW_MOUSE_POS_V) };
        mousePosV_ = NEW_MOUSE_POS_VF;
        ResetMouseOverPopupState();
        Stage::UpdateMousePos(NEW_MOUSE_POS_V);
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
            HandleCallback_BackButton();
            return true;
        }
        else if (KEY_EVENT.code == sf::Keyboard::D)
        {
            deleteButtonUPtr_->SetMouseState(sfml_util::MouseState::Over);
            sfml_util::SoundManager::Instance()->PlaySfx_Keypress();
            HandleCallback_DeleteButton();
            return true;
        }
        else if (KEY_EVENT.code == sf::Keyboard::S)
        {
            startButtonUPtr_->SetMouseState(sfml_util::MouseState::Over);
            sfml_util::SoundManager::Instance()->PlaySfx_Keypress();
            HandleCallback_StartButton();
            return true;
        }
        else
        {
            return false;
        }
    }

    void PartyStage::StartNewGame(const avatar::Avatar::Enum PARTY_AVATAR)
    {
        creature::CreaturePVec_t characters;
        for (auto const & ITEM_UPTR : *partyListBoxUPtr_)
        {
            if (ITEM_UPTR && ITEM_UPTR->CharacterPtrOpt())
            {
                auto const CHARACTER_PTR{ ITEM_UPTR->CharacterPtrOpt().value() };

                characters.emplace_back(CHARACTER_PTR);

                unplayedCharactersPVec_.erase(
                    std::remove(
                        std::begin(unplayedCharactersPVec_),
                        std::end(unplayedCharactersPVec_),
                        CHARACTER_PTR),
                    std::end(unplayedCharactersPVec_));

                game::GameStateFactory::Instance()->DeleteUnplayedCharacterFile(CHARACTER_PTR);
            }
        }

        // create a new GameState with the given party and then save it
        creature::PlayerPartyFactory partyFactory;
        game::GameStateFactory::Instance()->NewGame(partyFactory.Make(PARTY_AVATAR, characters));

        // Don't bother clearing the party ListBox because it flashes the
        //"not engouh characters" text, and since we are immediately transitioning
        // to the Camp Stage anyway.
        // partyListBoxUPtr_->Clear();

        game::LoopManager::Instance()->TransitionTo_Camp();
    }

    void PartyStage::ResetMouseOverPopupState()
    {
        mouseOverPopupTimerSec_ = 0.0f;
        willShowMouseOverPopup_ = false;
        mouseOverTextRegionUPtr_.reset();
    }

    bool PartyStage::AreAnyCharactersBeasts() const
    {
        for (auto const & ITEM_UPTR : *partyListBoxUPtr_)
        {
            if (ITEM_UPTR->CharacterPtrOpt() && (ITEM_UPTR->CharacterPtrOpt().value()->IsBeast()))
            {
                return true;
            }
        }

        return false;
    }

    bool PartyStage::AreAnyCharactersBeastmasters() const
    {
        for (auto const & ITEM_UPTR : *partyListBoxUPtr_)
        {
            if (ITEM_UPTR->CharacterPtrOpt()
                && (ITEM_UPTR->CharacterPtrOpt().value()->Role() == creature::role::Beastmaster))
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
           << " characters in your party.  You need exactly "
           << creature::PlayerParty::MAX_CHARACTER_COUNT_ << " characters to start the game.";

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

    void PartyStage::UpdateWillDisplayCharacterCountWarning()
    {
        willDisplayCharacterCountWarningText_
            = (characterListBoxUPtr_->Size() < creature::PlayerParty::MAX_CHARACTER_COUNT_);
    }

    void PartyStage::SetupMouseOverPositionsAndDimmensions(
        const creature::CreaturePtr_t CREATURE_PTR)
    {
        std::ostringstream ss;
        ss << "\"" << CREATURE_PTR->Name() << "\"\n"
           << CREATURE_PTR->SexName() << ", " << CREATURE_PTR->RaceName() << ", "
           << CREATURE_PTR->RoleName() << "\n"
           << "Created on " << CREATURE_PTR->DateTimeCreated().date.ToString() << " at "
           << CREATURE_PTR->DateTimeCreated().time.ToString() << "\n\n"
           << "Strength:        " << CREATURE_PTR->Strength() << "\n"
           << "Accuracy:       " << CREATURE_PTR->Accuracy() << "\n"
           << "Charm:           " << CREATURE_PTR->Charm() << "\n"
           << "Luck:              " << CREATURE_PTR->Luck() << "\n"
           << "Speed:            " << CREATURE_PTR->Speed() << "\n"
           << "Intelligence:    " << CREATURE_PTR->Intelligence();

        const sfml_util::gui::TextInfo TEXT_INFO(
            ss.str(),
            sfml_util::FontManager::Instance()->GetFont(sfml_util::Font::SystemCondensed),
            sfml_util::FontManager::Instance()->Size_Smallish(),
            sfml_util::FontManager::Color_Light(),
            sfml_util::Justified::Left);

        // initially the TextRegion is not positioned, see below
        mouseOverTextRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
            "PartyStage'sMouseOverPopup", TEXT_INFO, sf::FloatRect());

        auto const TEXT_WIDTH{ mouseOverTextRegionUPtr_->GetEntityRegion().width };

        auto const TEXT_HEIGHT{ mouseOverTextRegionUPtr_->GetEntityRegion().height };

        mouseOverBackgroundRectFinal_.width
            = (MOUSEOVER_CREATURE_IMAGE_WIDTH_MAX_ + TEXT_WIDTH
               + (MOUSEOVER_FINAL_INNER_EDGE_PAD_ * 3.0f));

        mouseOverBackgroundRectFinal_.height
            = (TEXT_HEIGHT + (MOUSEOVER_FINAL_INNER_EDGE_PAD_ * 2.0f));

        mouseOverBackgroundRectFinal_.left
            = ((Stage::StageRegionWidth() * 0.5f) - (mouseOverBackgroundRectFinal_.width * 0.5f));

        mouseOverBackgroundRectFinal_.top
            = ((Stage::StageRegionHeight() * 0.5f) - (mouseOverBackgroundRectFinal_.height * 0.5f));

        // temp scale the sprite to see what the final dimmensions will be so that the text can be
        // positioned
        sfml_util::ScaleSpriteToFit(
            mouseOverCreatureSprite_, MOUSEOVER_CREATURE_IMAGE_WIDTH_MAX_, TEXT_HEIGHT);

        mouseOverTextRegionUPtr_->SetEntityPos(
            (mouseOverBackgroundRectFinal_.left + mouseOverCreatureSprite_.getGlobalBounds().width
             + (MOUSEOVER_FINAL_INNER_EDGE_PAD_ * 2.0f)),
            (mouseOverBackgroundRectFinal_.top + MOUSEOVER_FINAL_INNER_EDGE_PAD_));

        // set the sprite's actual start scale at zero
        mouseOverCreatureSprite_.setScale(0.0f, 0.0f);
    }

} // namespace stage
} // namespace heroespath
