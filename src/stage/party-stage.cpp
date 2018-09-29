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
#include "sfml-util/font-manager.hpp"
#include "sfml-util/gui-images.hpp"
#include "sfml-util/main-menu-buttons.hpp"
#include "sfml-util/sound-manager.hpp"

#include <algorithm>
#include <sstream>

namespace heroespath
{
namespace stage
{

    const std::string PartyStage::POPUP_NAME_STR_NOT_ENOUGH_CHARS_ {
        "NotEnoughCharactersErrorMsg"
    };

    const std::string PartyStage::POPUP_NAME_STR_TOO_MANY_CHARS_ { "TooManyCharactersErrorMsg" };
    const std::string PartyStage::POPUP_NAME_STR_DELETE_CONFIRM_ { "CharacterDeleteConfirmMsg" };
    const std::string PartyStage::POPUP_NAME_STR_PARTY_IMAGE_SELECT_ { "PartyImageSelection" };
    const float PartyStage::MOUSEOVER_POPUP_DELAY_SEC_ { 1.0f };
    const float PartyStage::MOUSEOVER_SLIDER_SPEED_ { 4.0f };
    const float PartyStage::MOUSEOVER_BACKGROUND_FINAL_ALPHA_ { 120.0f };

    PartyStage::PartyStage()
        : Stage(
              "Party",
              sfml_util::Display::Instance()->FullscreenRect(),
              { sfml_util::GuiFont::Default,
                sfml_util::GuiFont::System,
                sfml_util::GuiFont::SystemCondensed,
                sfml_util::GuiFont::Number,
                sfml_util::GuiFont::Handwriting },
              true)
        , LISTBOX_FONT_ENUM_(sfml_util::GuiFont::System)
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
        , HEROESPATH_ORANGE_(sfml_util::color::Orange)
        , MOUSEOVER_COLORCYCLE_ALT_(
              HEROESPATH_ORANGE_.r, HEROESPATH_ORANGE_.g, HEROESPATH_ORANGE_.b, 32)
        , MOUSEOVER_COLORCYCLE_SPEED_(28.0f)
        , MOUSEOVER_COLORCYCLE_COUNT_(4)
        , bottomSymbol_()
        , listBoxInfo_()
        , stageTitle_("media-images-buttons-mainmenu-party-normal")
        , backgroundBox_(
              "PartyStage'sBackground",
              StageRegion(),
              sfml_util::BoxEntityInfo(sfml_util::CachedTexture(
                  "media-images-backgrounds-tile-darkknot",
                  sfml_util::ImageOpt::Default | sfml_util::ImageOpt::Repeated)))
        , backButtonUPtr_(std::make_unique<sfml_util::MainMenuButton>(
              sfml_util::LoopState::Previous,
              sfml_util::ImageTextEntity::Callback_t::IHandlerPtr_t(this),
              -1.0f))
        , startButtonUPtr_(std::make_unique<sfml_util::ImageTextEntity>(
              "PartyStage'sStartGame",
              MakeMouseImageInfoForMenuButton(
                  "media-images-buttons-mainmenu-start-normal",
                  "media-images-buttons-mainmenu-start-lit"),
              sfml_util::MouseTextInfo(),
              sfml_util::ImageTextEntity::Callback_t::IHandlerPtr_t(this),
              sfml_util::ImageTextEntity::MouseStateSync::Image))
        , deleteButtonUPtr_(std::make_unique<sfml_util::ImageTextEntity>(
              "PartyStage'sDelete",
              MakeMouseImageInfoForMenuButton(
                  "media-images-buttons-mainmenu-delete-normal",
                  "media-images-buttons-mainmenu-delete-lit"),
              sfml_util::MouseTextInfo(),
              sfml_util::ImageTextEntity::Callback_t::IHandlerPtr_t(this),
              sfml_util::ImageTextEntity::MouseStateSync::Image))
        , characterListBoxUPtr_()
        , partyListBoxUPtr_()
        , insTextRegionUPtr_()
        , upTextRegionUPtr_()
        , partyTextRegionUPtr_()
        , warningTextInfo_()
        , warningTextRegionUPtr_()
        , warningTextSlider_(150, 255, 4.0f, static_cast<sf::Uint8>(misc::random::Int(150, 255)))
        , ouroborosUPtr_(std::make_unique<sfml_util::Ouroboros>("PartyStage's"))

        , willDisplayCharacterCountWarningText_(false)
        , unplayedCharactersPVec_()
        , colorShakerRect_()
        , willShowMouseOverPopup_(false)
        , mouseOverPopupTimerSec_(0.0f)
        , mouseOverBackground_()
        , mousePosV_()
        , mouseOverBackgroundRectFinal_()
        , mouseOverCreatureSprite_()
        , mouseOverCreatureTextureOpt_()
        , mouseOverTextRegionUPtr_()
        , mouseOverSlider_(MOUSEOVER_SLIDER_SPEED_)
    {}

    PartyStage::~PartyStage()
    {
        Stage::ClearAllEntities();
        creature::CreatureWarehouse::Access().Free(unplayedCharactersPVec_);
    }

    bool PartyStage::HandleCallback(const PartyListBox_t::Callback_t::PacketPtr_t & PACKET_PTR)
    {
        ResetMouseOverPopupState();

        if (((PACKET_PTR->gui_event == sfml_util::GuiEvent::Keypress)
             && (PACKET_PTR->keypress_event.code == sf::Keyboard::Return))
            || (PACKET_PTR->gui_event == sfml_util::GuiEvent::DoubleClick))
        {
            if (PACKET_PTR->listbox_ptr == characterListBoxUPtr_.get())
            {
                if (partyListBoxUPtr_->Size() < creature::PlayerParty::MAX_CHARACTER_COUNT_)
                {
                    characterListBoxUPtr_->MoveSelection(*partyListBoxUPtr_);
                    sfml_util::SoundManager::Instance()->PlaySfx_AckMajor();
                    return true;
                }
                else
                {
                    sfml_util::SoundManager::Instance()->PlaySfx_Reject();
                }
            }
            else if (PACKET_PTR->listbox_ptr == partyListBoxUPtr_.get())
            {
                partyListBoxUPtr_->MoveSelection(*characterListBoxUPtr_);
                sfml_util::SoundManager::Instance()->PlaySfx_AckMajor();
                return true;
            }
        }

        return false;
    }

    bool PartyStage::HandleCallback(
        const sfml_util::ImageTextEntity::Callback_t::PacketPtr_t & PACKET_PTR)
    {
        ResetMouseOverPopupState();

        // the back button is a MainMenuButton that will handle everything itself

        if (PACKET_PTR->entity_ptr.Ptr() == startButtonUPtr_.get())
        {
            return HandleCallback_StartButton();
        }

        if (PACKET_PTR->entity_ptr.Ptr() == deleteButtonUPtr_.get())
        {
            return HandleCallback_DeleteButton();
        }

        return false;
    }

    bool PartyStage::HandleCallback(const sfml_util::PopupCallback_t::PacketPtr_t & PACKET_PTR)
    {
        ResetMouseOverPopupState();

        if ((PACKET_PTR->Name() == POPUP_NAME_STR_DELETE_CONFIRM_)
            && (PACKET_PTR->Response() == popup::ResponseTypes::Yes))
        {
            if (partyListBoxUPtr_->HasFocus())
            {
                return DeleteCharacterIfSelected(*partyListBoxUPtr_);
            }
            else if (characterListBoxUPtr_->HasFocus())
            {
                return DeleteCharacterIfSelected(*characterListBoxUPtr_);
            }
        }
        else if (
            (PACKET_PTR->Name() == POPUP_NAME_STR_PARTY_IMAGE_SELECT_)
            && (PACKET_PTR->Response() != popup::ResponseTypes::Cancel)
            && PACKET_PTR->SelectionOpt())
        {
            auto const SELECTED_NUM { static_cast<misc::EnumUnderlying_t>(
                PACKET_PTR->SelectionOpt().value()) };
            auto const ANIM_NUM { avatar::Avatar::Player_First + SELECTED_NUM };
            auto const ANIM_ENUM { static_cast<avatar::Avatar::Enum>(ANIM_NUM) };

            if (avatar::Avatar::IsPlayer(ANIM_ENUM))
            {
                StartNewGame(ANIM_ENUM);
                return true;
            }
        }

        return false;
    }

    bool PartyStage::HandleCallback_StartButton()
    {
        ResetMouseOverPopupState();

        if (partyListBoxUPtr_->Size() != creature::PlayerParty::MAX_CHARACTER_COUNT_)
        {
            NotEnoughCharactersPopup();
            return false;
        }

        if (AreAnyInPartyBeasts() && (AreAnyInPartyBeastmasters() == false))
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

        auto const SELECTED_CHARACTER_PTR_OPT { GetSelectedCharacterPtrOpt() };

        if (!SELECTED_CHARACTER_PTR_OPT)
        {
            return false;
        }

        std::ostringstream ss;
        ss << "\n\nDelete " << SELECTED_CHARACTER_PTR_OPT.value()->Name()
           << "?  This cannot be undone.  Are you sure?";

        auto const POPUP_INFO { popup::PopupManager::Instance()->CreatePopupInfo(
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

        Stage::SetFocus(characterListBoxUPtr_.get());
    }

    void PartyStage::Setup_Ouroboros() { EntityAdd(ouroborosUPtr_.get()); }

    void PartyStage::Setup_Buttons()
    {
        backButtonUPtr_->SetEntityPos(
            sfml_util::ScreenRatioToPixelsHoriz(0.0586f),
            sfml_util::CenterOfVert(bottomSymbol_.Region())
                - (backButtonUPtr_->GetEntityRegion().height * 0.5f));

        EntityAdd(backButtonUPtr_.get());

        startButtonUPtr_->SetEntityPos(
            sfml_util::ScreenRatioToPixelsHoriz(0.586f),
            sfml_util::CenterOfVert(bottomSymbol_.Region())
                - (startButtonUPtr_->GetEntityRegion().height * 0.5f));

        EntityAdd(startButtonUPtr_.get());

        deleteButtonUPtr_->SetEntityPos(
            sfml_util::ScreenRatioToPixelsHoriz(0.246f),
            sfml_util::CenterOfVert(bottomSymbol_.Region())
                - (deleteButtonUPtr_->GetEntityRegion().height * 0.5f));

        EntityAdd(deleteButtonUPtr_.get());
    }

    void PartyStage::Setup_TopInstructionText()
    {
        sfml_util::TextInfo insTextInfo(
            "(double-click or use the arrow keys and enter to move characters back and forth)",
            sfml_util::GuiFont::SystemCondensed,
            sfml_util::FontManager::Instance()->Size_Smallish(),
            sfml_util::color::GrayLight,
            sfml_util::Justified::Left,
            sf::Text::Italic);

        insTextRegionUPtr_
            = std::make_unique<sfml_util::TextRegion>("Instructions", insTextInfo, sf::FloatRect());

        auto const HALF_INSTR_TEXT_WIDTH { insTextRegionUPtr_->GetEntityRegion().width * 0.5f };

        insTextRegionUPtr_->SetEntityPos(
            ((Stage::StageRegionWidth() * 0.5f) - HALF_INSTR_TEXT_WIDTH) + 125.0f,
            sfml_util::Bottom(stageTitle_.Region()) - 45.0f);

        EntityAdd(insTextRegionUPtr_.get());
    }

    void PartyStage::Setup_TopWarningText()
    {
        std::ostringstream warningSS;
        warningSS << "(There are not enough characters to make a party of six.  Go back and "
                     "create more.)";

        warningTextInfo_ = sfml_util::TextInfo(
            warningSS.str(),
            sfml_util::GuiFont::SystemCondensed,
            sfml_util::FontManager::Instance()->Size_Smallish(),
            sf::Color(255, 200, 200),
            sfml_util::Justified::Left,
            sf::Text::Italic);

        warningTextRegionUPtr_ = std::make_unique<sfml_util::TextRegion>(
            "WarningsText", warningTextInfo_, sf::FloatRect());

        auto const WARNING_TEXT_LEFT { (Stage::StageRegionWidth() * 0.5f)
                                       - (warningTextRegionUPtr_->GetEntityRegion().width * 0.5f)
                                       + 110.0f };

        auto const WARNING_TEXT_TOP { (insTextRegionUPtr_->GetEntityRegion().top
                                       + insTextRegionUPtr_->GetEntityRegion().height)
                                      - sfml_util::ScreenRatioToPixelsVert(0.0025f) };

        warningTextRegionUPtr_->SetEntityPos(WARNING_TEXT_LEFT, WARNING_TEXT_TOP);
    }

    void PartyStage::Setup_CharactersListBoxLabel()
    {
        sfml_util::TextInfo labelTextInfo(
            "Unplayed Characters",
            sfml_util::GuiFont::System,
            sfml_util::FontManager::Instance()->Size_Largeish(),
            sfml_util::color::Orange + sf::Color(0, 30, 30, 0));

        upTextRegionUPtr_ = std::make_unique<sfml_util::TextRegion>(
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
        sfml_util::TextInfo labelTextInfo(
            "New Party",
            sfml_util::GuiFont::System,
            sfml_util::FontManager::Instance()->Size_Largeish(),
            sfml_util::color::Orange + sf::Color(0, 30, 30, 0));

        partyTextRegionUPtr_
            = std::make_unique<sfml_util::TextRegion>("PartyLabel", labelTextInfo, sf::FloatRect());

        partyTextRegionUPtr_->SetEntityPos(
            PARTY_LISTBOX_POS_LEFT_ + sfml_util::ScreenRatioToPixelsHoriz(0.025f),
            (warningTextRegionUPtr_->GetEntityRegion().top
             + warningTextRegionUPtr_->GetEntityRegion().height)
                + sfml_util::ScreenRatioToPixelsVert(0.0025f));

        EntityAdd(partyTextRegionUPtr_.get());
    }

    void PartyStage::Setup_CharactersListBox()
    {
        auto const CHAR_LIST_POS_LEFT { CHAR_LISTBOX_POS_LEFT_ };
        auto const CHAR_LIST_WIDTH { LISTBOX_WIDTH_ };
        auto const CHAR_LIST_HEIGHT { LISTBOX_HEIGHT_ };

        auto const CHAR_LIST_POS_TOP {
            (upTextRegionUPtr_->GetEntityRegion().top + upTextRegionUPtr_->GetEntityRegion().height)
        };

        const sf::FloatRect CHAR_LIST_RECT(
            CHAR_LIST_POS_LEFT, CHAR_LIST_POS_TOP, CHAR_LIST_WIDTH, CHAR_LIST_HEIGHT);

        auto const BG_COLOR { sfml_util::color::Orange - sf::Color(100, 100, 100, 220) };

        listBoxInfo_.SetupBorder(true, 1.0f);
        listBoxInfo_.SetupColor(BG_COLOR);

        listBoxInfo_.focus_colors = sfml_util::FocusColors(
            sfml_util::color::Orange,
            BG_COLOR,
            sfml_util::color::Orange - sfml_util::FocusColors::DEFAULT_OFFSET_COLOR_,
            BG_COLOR - sf::Color(40, 40, 40, 0));

        sfml_util::ListBoxPacket charListBoxPacket(
            CHAR_LIST_RECT, listBoxInfo_, sfml_util::color::Orange, sf::Color(255, 255, 255, 190));

        characterListBoxUPtr_
            = std::make_unique<sfml_util::ListBox<PartyStage, creature::CreaturePtr_t>>(
                "PartyStage'sCharacter", this, this, charListBoxPacket);

        // load all players not yet assigned to a party/started game
        unplayedCharactersPVec_ = game::GameStateFactory::Instance()->LoadAllUnplayedCharacters();

        sfml_util::TextInfo textInfo("", LISTBOX_FONT_ENUM_, LISTBOX_FONT_SIZE_);

        for (auto const & CHARACTER_PTR : unplayedCharactersPVec_)
        {
            std::ostringstream ssTitle;
            ssTitle << CHARACTER_PTR->Name() << ", " << CHARACTER_PTR->RaceName();

            if (CHARACTER_PTR->Race() != creature::race::Wolfen)
            {
                ssTitle << ", " << CHARACTER_PTR->RoleName();
            }

            characterListBoxUPtr_->Append(
                std::make_unique<sfml_util::ListElement<creature::CreaturePtr_t>>(
                    CHARACTER_PTR, sfml_util::TextInfo(textInfo, ssTitle.str())));
        }

        characterListBoxUPtr_->Sort([](auto const & A, auto const & B) {
            return (A->Element()->Name() < B->Element()->Name());
        });

        EntityAdd(characterListBoxUPtr_.get());
    }

    void PartyStage::Setup_PartyListBox()
    {
        auto const PARTY_LIST_POS_LEFT { PARTY_LISTBOX_POS_LEFT_ };

        auto const PARTY_LIST_POS_TOP {
            (upTextRegionUPtr_->GetEntityRegion().top + upTextRegionUPtr_->GetEntityRegion().height)
        };

        auto const PARTY_LIST_WIDTH { LISTBOX_WIDTH_ };
        auto const PARTY_LIST_HEIGHT { LISTBOX_HEIGHT_ };

        const sf::FloatRect PARTY_LIST_RECT(
            PARTY_LIST_POS_LEFT, PARTY_LIST_POS_TOP, PARTY_LIST_WIDTH, PARTY_LIST_HEIGHT);

        sfml_util::ListBoxPacket partyListBoxPacket(
            PARTY_LIST_RECT, listBoxInfo_, sfml_util::color::Orange, sf::Color(255, 255, 255, 190));

        partyListBoxUPtr_
            = std::make_unique<sfml_util::ListBox<PartyStage, creature::CreaturePtr_t>>(
                "PartyStage'sParty", this, this, partyListBoxPacket);

        EntityAdd(partyListBoxUPtr_.get());
    }

    void PartyStage::Draw(sf::RenderTarget & target, const sf::RenderStates & STATES)
    {
        target.draw(backgroundBox_, STATES);
        target.draw(stageTitle_, STATES);
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

    const creature::CreaturePtrOpt_t PartyStage::GetSelectedCharacterPtrOpt() const
    {
        if (partyListBoxUPtr_->HasFocus() && (partyListBoxUPtr_->Empty() == false))
        {
            return partyListBoxUPtr_->Selection()->Element();
        }
        else if (characterListBoxUPtr_->HasFocus() && (characterListBoxUPtr_->Empty() == false))
        {
            return characterListBoxUPtr_->Selection()->Element();
        }

        return boost::none;
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
            auto const NEW_COLOR_VAL { static_cast<sf::Uint8>(
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
            auto isMouseOverCharacterListBox { false };

            const auto MOUSEOVER_ELEMENT_PTR_OPT
                = [&]() -> sfml_util::ListElementPtrOpt_t<creature::CreaturePtr_t> {
                if (characterListBoxUPtr_->GetEntityRegion().contains(mousePosV_))
                {
                    isMouseOverCharacterListBox = true;
                    return characterListBoxUPtr_->AtPos(mousePosV_);
                }
                else if (partyListBoxUPtr_->GetEntityRegion().contains(mousePosV_))
                {
                    isMouseOverCharacterListBox = false;
                    return partyListBoxUPtr_->AtPos(mousePosV_);
                }
                else
                {
                    return boost::none;
                }
            }();

            if (MOUSEOVER_ELEMENT_PTR_OPT)
            {
                auto const CHARACTER_PTR { MOUSEOVER_ELEMENT_PTR_OPT.value()->Element() };

                willShowMouseOverPopup_ = true;
                mouseOverSlider_.Reset(MOUSEOVER_SLIDER_SPEED_);

                mouseOverCreatureTextureOpt_ = sfml_util::LoadAndCacheImage(CHARACTER_PTR);
                mouseOverCreatureSprite_.setTexture(mouseOverCreatureTextureOpt_->Get(), true);
                SetupMouseOverPositionsAndDimmensions(CHARACTER_PTR);

                auto const ELEMENT_RECT { (
                    (isMouseOverCharacterListBox) ? characterListBoxUPtr_->ElementRegion(
                                                        MOUSEOVER_ELEMENT_PTR_OPT.value(), true)
                                                  : partyListBoxUPtr_->ElementRegion(
                                                        MOUSEOVER_ELEMENT_PTR_OPT.value(), true)) };

                colorShakerRect_.Reset(
                    ELEMENT_RECT,
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

            auto const RATIO { mouseOverSlider_.Update(ELAPSED_TIME_SECONDS) };

            auto const BG_WIDTH { mouseOverBackgroundRectFinal_.width * RATIO };
            auto const BG_HEIGHT { mouseOverBackgroundRectFinal_.height * RATIO };

            auto const BG_LEFT { mousePosV_.x
                                 - ((mousePosV_.x - mouseOverBackgroundRectFinal_.left) * RATIO) };

            auto const BG_TOP { mousePosV_.y
                                - ((mousePosV_.y - mouseOverBackgroundRectFinal_.top) * RATIO) };

            mouseOverBackground_.Rect(sf::FloatRect(BG_LEFT, BG_TOP, BG_WIDTH, BG_HEIGHT));

            auto const NEW_ALPHA { static_cast<sf::Uint8>(
                MOUSEOVER_BACKGROUND_FINAL_ALPHA_ * RATIO) };

            mouseOverBackground_.Color(sf::Color(0, 0, 0, NEW_ALPHA));

            auto const IMAGE_LEFT {
                mousePosV_.x
                - ((mousePosV_.x
                    - (mouseOverBackgroundRectFinal_.left + MOUSEOVER_FINAL_INNER_EDGE_PAD_))
                   * RATIO)
            };

            auto const IMAGE_TOP {
                mousePosV_.y
                - ((mousePosV_.y
                    - (mouseOverBackgroundRectFinal_.top + MOUSEOVER_FINAL_INNER_EDGE_PAD_))
                   * RATIO)
            };

            mouseOverCreatureSprite_.setPosition(IMAGE_LEFT, IMAGE_TOP);

            auto const IMAGE_WIDTH_AVAIL { BG_WIDTH - (MOUSEOVER_FINAL_INNER_EDGE_PAD_ * 3.0f) };
            auto const IMAGE_HEIGHT_AVAIL { BG_HEIGHT - (MOUSEOVER_FINAL_INNER_EDGE_PAD_ * 2.0f) };
            if ((IMAGE_WIDTH_AVAIL > 1.0f) && (IMAGE_HEIGHT_AVAIL > 1.0f))
            {
                sfml_util::Fit(
                    mouseOverCreatureSprite_,
                    sf::Vector2f(
                        std::min(IMAGE_WIDTH_AVAIL, MOUSEOVER_CREATURE_IMAGE_WIDTH_MAX_),
                        IMAGE_HEIGHT_AVAIL));
            }
        }
    }

    void PartyStage::UpdateMousePos(const sf::Vector2i & NEW_MOUSE_POS_V)
    {
        const sf::Vector2f NEW_MOUSE_POS_VF { NEW_MOUSE_POS_V };
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
            backButtonUPtr_->PretendClicked();
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
        for (auto const & ELEMENT_UPTR : *partyListBoxUPtr_)
        {
            auto const CHARACTER_PTR { ELEMENT_UPTR->Element() };

            characters.emplace_back(CHARACTER_PTR);

            unplayedCharactersPVec_.erase(
                std::remove(
                    std::begin(unplayedCharactersPVec_),
                    std::end(unplayedCharactersPVec_),
                    CHARACTER_PTR),
                std::end(unplayedCharactersPVec_));

            game::GameStateFactory::Instance()->DeleteUnplayedCharacterFile(CHARACTER_PTR);
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

    bool PartyStage::AreAnyInPartyBeasts() const
    {
        return std::any_of(
            std::begin(*partyListBoxUPtr_),
            std::end(*partyListBoxUPtr_),
            [](const auto & ELEMENT_UPTR) { return ELEMENT_UPTR->Element()->IsBeast(); });
    }

    bool PartyStage::AreAnyInPartyBeastmasters() const
    {
        return std::any_of(
            std::begin(*partyListBoxUPtr_),
            std::end(*partyListBoxUPtr_),
            [](const auto & ELEMENT_UPTR) {
                return (ELEMENT_UPTR->Element()->Role() == creature::role::Beastmaster);
            });
    }

    void PartyStage::MissingBeastmasterPopup()
    {
        std::ostringstream ss;
        ss << "\nThere are beast characters in your party, but no character with the role"
           << " of Beastmaster.  To have Wolfens or Dragons in your party, you must also"
           << " have a Beastmaster.";

        auto const POP_INFO { popup::PopupManager::Instance()->CreatePopupInfo(
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

        auto const POPUP_INFO { popup::PopupManager::Instance()->CreatePopupInfo(
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
        sfml_util::CachedTextureVec_t partyCachedTextures;

        for (misc::EnumUnderlying_t i(avatar::Avatar::Player_First);
             i <= avatar::Avatar::Player_Last;
             ++i)
        {
            auto const WHICH_AVATAR { static_cast<avatar::Avatar::Enum>(i) };

            partyCachedTextures.emplace_back(avatar::PortraitFactory::Make(
                "PartyStageAvatar_" + avatar::Avatar::ToString(WHICH_AVATAR), WHICH_AVATAR));
        }

        std::ostringstream ss;
        ss << "Choose an avatar for your party...";

        auto const POPUP_INFO { popup::PopupManager::Instance()->CreateImageSelectionPopupInfo(
            POPUP_NAME_STR_PARTY_IMAGE_SELECT_, ss.str(), partyCachedTextures, false) };

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

        const sfml_util::TextInfo TEXT_INFO(
            ss.str(),
            sfml_util::GuiFont::SystemCondensed,
            sfml_util::FontManager::Instance()->Size_Smallish(),
            sfml_util::color::Light,
            sfml_util::Justified::Left);

        // initially the TextRegion is not positioned, see below
        mouseOverTextRegionUPtr_ = std::make_unique<sfml_util::TextRegion>(
            "PartyStage'sMouseOverPopup", TEXT_INFO, sf::FloatRect());

        auto const TEXT_WIDTH { mouseOverTextRegionUPtr_->GetEntityRegion().width };
        auto const TEXT_HEIGHT { mouseOverTextRegionUPtr_->GetEntityRegion().height };

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
        // positioned, see actual sprite scale set below
        sfml_util::Fit(mouseOverCreatureSprite_, MOUSEOVER_CREATURE_IMAGE_WIDTH_MAX_, TEXT_HEIGHT);

        mouseOverTextRegionUPtr_->SetEntityPos(
            (mouseOverBackgroundRectFinal_.left + mouseOverCreatureSprite_.getGlobalBounds().width
             + (MOUSEOVER_FINAL_INNER_EDGE_PAD_ * 2.0f)),
            (mouseOverBackgroundRectFinal_.top + MOUSEOVER_FINAL_INNER_EDGE_PAD_));

        // set the sprite's actual start scale at zero
        mouseOverCreatureSprite_.setScale(0.0f, 0.0f);
    }

    bool PartyStage::DeleteCharacterIfSelected(PartyListBox_t & listbox)
    {
        if (listbox.Empty())
        {
            return false;
        }

        auto const CHARACTER_PTR { listbox.Selection()->Element() };

        game::GameStateFactory::Instance()->DeleteUnplayedCharacterFile(CHARACTER_PTR);

        listbox.RemoveFreeSelection();

        unplayedCharactersPVec_.erase(
            std::remove(
                std::begin(unplayedCharactersPVec_),
                std::end(unplayedCharactersPVec_),
                CHARACTER_PTR),
            std::end(unplayedCharactersPVec_));

        creature::CreatureWarehouse::Access().Free(CHARACTER_PTR);

        UpdateWillDisplayCharacterCountWarning();
        return true;
    }

    const sfml_util::MouseImageInfo PartyStage::MakeMouseImageInfoForMenuButton(
        const std::string & IMAGE_PATH_KEY_UP, const std::string & IMAGE_PATH_KEY_OVER)
    {
        const sf::FloatRect INITIAL_REGION_TO_SET_WIDTH_(
            0.0f, 0.0f, sfml_util::MainMenuButton::DefaultWidth(), 0.0f);

        const sfml_util::EntityImageInfo ENTITY_IMAGE_INFO_UP(
            (sfml_util::CachedTexture(IMAGE_PATH_KEY_UP)),
            (sf::FloatRect(INITIAL_REGION_TO_SET_WIDTH_)));

        const sfml_util::EntityImageInfo ENTITY_IMAGE_INFO_OVER(
            (sfml_util::CachedTexture(IMAGE_PATH_KEY_OVER)),
            (sf::FloatRect(INITIAL_REGION_TO_SET_WIDTH_)));

        return sfml_util::MouseImageInfo(
            true, ENTITY_IMAGE_INFO_UP, sfml_util::EntityImageInfo(), ENTITY_IMAGE_INFO_OVER);
    }

} // namespace stage
} // namespace heroespath
