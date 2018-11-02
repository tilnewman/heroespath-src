// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// popup-stage-spellbook.cpp
//
#include "popup-stage-spellbook.hpp"

#include "creature/creature.hpp"
#include "gui/cached-texture.hpp"
#include "gui/font-manager.hpp"
#include "gui/sound-manager.hpp"
#include "misc/random.hpp"
#include "popup/popup-manager.hpp"
#include "spell/spell.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

#include <algorithm>
#include <sstream>

namespace heroespath
{
namespace popup
{

    const float PopupStageSpellbook::BORDER_SCREEN_RATIO_ { 0.15f };
    const float PopupStageSpellbook::COLOR_FADE_SPEED_ { 6.0f };
    const sf::Uint8 PopupStageSpellbook::SPELL_IMAGE_ALPHA_ { 192 };
    const sf::Color PopupStageSpellbook::UNABLE_TEXT_COLOR_ { sf::Color(127, 32, 32) };
    const float PopupStageSpellbook::WARNING_DURATION_SEC_ { 2.0f };

    PopupStageSpellbook::PopupStageSpellbook(const popup::PopupInfo & POPUP_INFO)
        : PopupStageBase(POPUP_INFO)
        , playerCachedTextureOpt_()
        , playerSprite_()
        , pageRectLeft_()
        , pageRectRight_()
        , charDetailsTextRegionUPtr_()
        , listBoxLabelTextRegionUPtr_()
        , listBoxUPtr_()
        , LISTBOX_IMAGE_COLOR_(sf::Color(255, 255, 255, 190))
        , LISTBOX_LINE_COLOR_(sfutil::color::GrayDark)
        , LISTBOX_COLOR_FG_(LISTBOX_LINE_COLOR_)
        , LISTBOX_COLOR_BG_(sfutil::color::Orange - sf::Color(100, 100, 100, 220))
        , LISTBOX_COLORSET_(LISTBOX_COLOR_FG_, LISTBOX_COLOR_BG_)
        , listBoxBackgroundInfo_()
        , listElementTextInfo_(
              " ",
              gui::GuiFont::System,
              gui::FontManager::Instance()->Size_Smallish(),
              sfutil::color::GrayDarker,
              gui::Justified::Left)
        , spellCachedTextureOpt_()
        , spellSprite_()
        , spellTitleTextRegionUPtr_()
        , spellDetailsTextUPtr_()
        , unableTextUPtr_()
        , spellDescTextUPtr_()
        , imageColorSlider_(
              sf::Color::Transparent,
              sf::Color::White,
              COLOR_FADE_SPEED_,
              gui::WillOscillate::No,
              gui::WillAutoStart::No)
        , textColorSlider_(
              sf::Color::Transparent,
              sf::Color::Black,
              COLOR_FADE_SPEED_,
              gui::WillOscillate::No,
              gui::WillAutoStart::No)
        , warnTextColorSlider_(
              UNABLE_TEXT_COLOR_,
              sf::Color::Transparent,
              20.0f,
              gui::WillOscillate::Yes,
              gui::WillAutoStart::Yes)
        , currentListboxIndex_(0)
    {}

    PopupStageSpellbook::~PopupStageSpellbook() = default;

    bool PopupStageSpellbook::HandleCallback(
        const gui::ListBox<PopupStageSpellbook, spell::SpellPtr_t>::Callback_t::PacketPtr_t &
            PACKET_PTR)
    {
        if ((PACKET_PTR->gui_event == gui::GuiEvent::Click)
            || (PACKET_PTR->gui_event == gui::GuiEvent::SelectionChange)
            || (PACKET_PTR->keypress_event.code == sf::Keyboard::Up)
            || (PACKET_PTR->keypress_event.code == sf::Keyboard::Down))
        {
            if (currentListboxIndex_ != PACKET_PTR->listbox_ptr->SelectionIndex())
            {
                currentListboxIndex_ = PACKET_PTR->listbox_ptr->SelectionIndex();

                if (imageColorSlider_.Direction() != gui::Moving::Away)
                {
                    imageColorSlider_.ReverseDirection();
                    imageColorSlider_.Start();
                }

                if (textColorSlider_.Direction() != gui::Moving::Away)
                {
                    textColorSlider_.ReverseDirection();
                    textColorSlider_.Start();
                }

                return true;
            }
        }
        else if (
            (PACKET_PTR->gui_event == gui::GuiEvent::DoubleClick)
            || (PACKET_PTR->keypress_event.code == sf::Keyboard::Return))
        {
            currentListboxIndex_ = PACKET_PTR->listbox_ptr->SelectionIndex();
            return HandleSpellCast();
        }

        return false;
    }

    void PopupStageSpellbook::Setup()
    {
        PopupStageBase::Setup();

        SetupRegions();
        SetupLeftAccentImage();
        SetupRightAccentImage();
        SetupPlayerImage();
        SetupPlayerDetailsText();
        SetupSpellListboxLabel();
        SetupSpellListbox();

        currentListboxIndex_ = listBoxUPtr_->SelectionIndex();
        SetupPageRightText(CurrentSelectedSpell());

        SetFocus(listBoxUPtr_.get());
    }

    void PopupStageSpellbook::Draw(sf::RenderTarget & target, const sf::RenderStates & STATES)
    {
        PopupStageBase::Draw(target, STATES);

        target.draw(accentSprite1_, STATES);
        target.draw(accentSprite2_, STATES);
        target.draw(playerSprite_, STATES);
        target.draw(spellSprite_, STATES);
        charDetailsTextRegionUPtr_->draw(target, STATES);
        listBoxLabelTextRegionUPtr_->draw(target, STATES);
        spellTitleTextRegionUPtr_->draw(target, STATES);
        spellDetailsTextUPtr_->draw(target, STATES);

        if (willShowXImage_)
        {
            unableTextUPtr_->draw(target, STATES);
        }

        spellDescTextUPtr_->draw(target, STATES);

        StageBase::Draw(target, STATES);

        PopupStageBase::DrawRedX(target, STATES);
    }

    void PopupStageSpellbook::UpdateTime(const float ELAPSED_TIME_SECONDS)
    {
        StageBase::UpdateTime(ELAPSED_TIME_SECONDS);

        const auto HAS_TEXT_COLOR_SLIDER_STOPPED { textColorSlider_.UpdateAndReturnIsStopped(
            ELAPSED_TIME_SECONDS) };

        const auto HAS_IMAGE_COLOR_SLIDER_STOPPED { imageColorSlider_.UpdateAndReturnIsStopped(
            ELAPSED_TIME_SECONDS) };

        SetPageRightColors(imageColorSlider_.Value(), textColorSlider_.Value());

        if (HAS_TEXT_COLOR_SLIDER_STOPPED || HAS_IMAGE_COLOR_SLIDER_STOPPED)
        {
            gui::SoundManager::Instance()->SoundEffectPlay(gui::sound_effect::Magic1);

            SetupPageRightText(CurrentSelectedSpell());

            imageColorSlider_.Reset();
            textColorSlider_.Reset();
        }

        if (willShowXImage_)
        {
            unableTextUPtr_->SetEntityColorFgBoth(
                warnTextColorSlider_.UpdateAndReturnValue(ELAPSED_TIME_SECONDS));
        }
    }

    bool PopupStageSpellbook::KeyRelease(const sf::Event::KeyEvent & KEY_EVENT)
    {
        if ((KEY_EVENT.code == sf::Keyboard::Escape) || (KEY_EVENT.code == sf::Keyboard::Space))
        {
            PlayValidKeypressSoundEffect();

            RemovePopup(ResponseTypes::Cancel, 0);
            return true;
        }
        else if ((KEY_EVENT.code == sf::Keyboard::Return) || (KEY_EVENT.code == sf::Keyboard::C))
        {
            return HandleSpellCast();
        }

        return PopupStageBase::KeyRelease(KEY_EVENT);
    }

    void PopupStageSpellbook::SetupOuterAndInnerRegion()
    {
        SetupForFullScreenWithBorderRatio(BORDER_SCREEN_RATIO_);
    }

    void PopupStageSpellbook::SetupRegions()
    {
        const sf::FloatRect LEFT_PAGE_RECT_RAW { popup::PopupManager::Rect_Spellbook_PageLeft() };

        const auto SCALE { innerRegion_.width
                           / static_cast<float>(backgroundTexture_.Get().getSize().x) };

        pageRectLeft_.left = innerRegion_.left + (LEFT_PAGE_RECT_RAW.left * SCALE);
        pageRectLeft_.top = innerRegion_.top + (LEFT_PAGE_RECT_RAW.top * SCALE);
        pageRectLeft_.width = LEFT_PAGE_RECT_RAW.width * SCALE;
        pageRectLeft_.height = LEFT_PAGE_RECT_RAW.height * SCALE;

        const sf::FloatRect RIGHT_PAGE_RECT_RAW { popup::PopupManager::Rect_Spellbook_PageRight() };

        pageRectRight_.left = innerRegion_.left + (RIGHT_PAGE_RECT_RAW.left * SCALE);
        pageRectRight_.top = innerRegion_.top + (RIGHT_PAGE_RECT_RAW.top * SCALE);
        pageRectRight_.width = RIGHT_PAGE_RECT_RAW.width * SCALE;
        pageRectRight_.height = RIGHT_PAGE_RECT_RAW.height * SCALE;
    }

    void PopupStageSpellbook::SetupLeftAccentImage()
    {
        accent1CachedTextureOpt_ = popup::PopupManager::Instance()->LoadRandomAccentImage();
        accentSprite1_.setTexture(accent1CachedTextureOpt_->Get(), true);

        sfutil::FitAndReCenter(
            accentSprite1_,
            sfutil::ScaleAndReCenterCopy(pageRectLeft_, ACCENT_IMAGE_SCALEDOWN_RATIO_));

        accentSprite1_.setColor(sf::Color(255, 255, 255, ACCENT_IMAGE_ALPHA_));
    }

    void PopupStageSpellbook::SetupRightAccentImage()
    {
        accent2CachedTextureOpt_ = popup::PopupManager::Instance()->LoadRandomAccentImage();
        accentSprite2_.setTexture(accent2CachedTextureOpt_->Get(), true);

        sfutil::FitAndReCenter(
            accentSprite2_,
            sfutil::ScaleAndReCenterCopy(pageRectRight_, ACCENT_IMAGE_SCALEDOWN_RATIO_));

        accentSprite2_.setColor(sf::Color(255, 255, 255, ACCENT_IMAGE_ALPHA_));
    }

    void PopupStageSpellbook::SetupPlayerImage()
    {
        playerCachedTextureOpt_ = gui::LoadAndCacheImage(
            popupInfo_.CreaturePtrOpt().value(), gui::ImageOptions::InvertedCharacterOptions());

        playerSprite_.setTexture(playerCachedTextureOpt_->Get(), true);

        const auto PLAYER_IMAGE_SCALE { sfutil::MapByRes(0.55f, 3.5f) };

        playerSprite_.setScale(PLAYER_IMAGE_SCALE, PLAYER_IMAGE_SCALE);
        playerSprite_.setColor(sf::Color(255, 255, 255, 127));
        playerSprite_.setPosition(pageRectLeft_.left, pageRectLeft_.top);
    }

    void PopupStageSpellbook::SetupPlayerDetailsText()
    {
        const auto CREATURE_PTR { popupInfo_.CreaturePtrOpt().value() };

        std::ostringstream ss;
        ss << CREATURE_PTR->Name() << "\n";

        if (CREATURE_PTR->IsBeast())
        {
            ss << CREATURE_PTR->RaceName();

            if (CREATURE_PTR->Race() != creature::race::Wolfen)
            {
                ss << ", " << CREATURE_PTR->RoleName();
            }

            ss << " " << CREATURE_PTR->RankClassName() << "\n";
        }
        else
        {
            ss << CREATURE_PTR->RankClassName() << " " << CREATURE_PTR->RoleName() << "\n"
               << CREATURE_PTR->RaceName() << "\n";
        }

        ss << "Rank:  " << CREATURE_PTR->Rank() << "\n"
           << "Health:  " << CREATURE_PTR->HealthCurrent() << "/" << CREATURE_PTR->HealthNormal()
           << " " << CREATURE_PTR->HealthPercentStr() << "\n"
           << "Mana:  " << CREATURE_PTR->Mana() << "/" << CREATURE_PTR->ManaNormal() << "\n"
           << "\n";

        const gui::TextInfo DETAILS_TEXTINFO(
            ss.str(),
            gui::GuiFont::Default,
            gui::FontManager::Instance()->Size_Small(),
            sfutil::color::GrayDarker,
            gui::Justified::Left);

        const sf::FloatRect DETAILS_TEXT_RECT { pageRectLeft_.left
                                                    + playerSprite_.getGlobalBounds().width
                                                    + sfutil::MapByRes(10.0f, 40.0f),
                                                pageRectLeft_.top + sfutil::MapByRes(20.0f, 80.0f),
                                                0.0f,
                                                0.0f };

        charDetailsTextRegionUPtr_ = std::make_unique<gui::TextRegion>(
            "SpellnbookPopupWindowDetails", DETAILS_TEXTINFO, DETAILS_TEXT_RECT);
    }

    void PopupStageSpellbook::SetupSpellListboxLabel()
    {
        const gui::TextInfo LISTBOX_LABEL_TEXTINFO(
            "Spells",
            gui::GuiFont::Default,
            gui::FontManager::Instance()->Size_Largeish(),
            sfutil::color::GrayDarker,
            gui::Justified::Left);

        const sf::FloatRect LISTBOX_LABEL_TEXTRECT {
            pageRectLeft_.left + sfutil::MapByRes(10.0f, 40.0f),
            playerSprite_.getGlobalBounds().top + playerSprite_.getGlobalBounds().height
                + sfutil::MapByRes(20.0f, 80.0f),
            0.0f,
            0.0f
        };

        listBoxLabelTextRegionUPtr_ = std::make_unique<gui::TextRegion>(
            "SpellnbookPopupWindowSpellListLabel", LISTBOX_LABEL_TEXTINFO, LISTBOX_LABEL_TEXTRECT);
    }

    void PopupStageSpellbook::SetPageRightColors(
        const sf::Color & IMAGE_COLOR, const sf::Color & TEXT_COLOR)
    {
        spellSprite_.setColor(IMAGE_COLOR);

        const gui::FocusColors TEXT_COLOR_SET(TEXT_COLOR, TEXT_COLOR, TEXT_COLOR, TEXT_COLOR);

        spellTitleTextRegionUPtr_->SetEntityColors(TEXT_COLOR_SET);
        spellDetailsTextUPtr_->SetEntityColors(TEXT_COLOR_SET);
        spellDescTextUPtr_->SetEntityColors(TEXT_COLOR_SET);

        auto unableTextColor { UNABLE_TEXT_COLOR_ };
        unableTextColor.a = TEXT_COLOR.a;

        const gui::FocusColors UNABLE_EXT_COLOR_SET(
            unableTextColor, unableTextColor, unableTextColor, unableTextColor);

        unableTextUPtr_->SetEntityColors(UNABLE_EXT_COLOR_SET);

        auto redXColor { sf::Color::Red };
        redXColor.a = IMAGE_COLOR.a;
        xSymbolSprite_.setColor(redXColor);
    }

    void PopupStageSpellbook::SetupSpellListbox()
    {
        const auto LISTBOX_MARGIN { sfutil::MapByRes(15.0f, 45.0f) };
        const auto LISTBOX_RECT_LEFT { pageRectLeft_.left + LISTBOX_MARGIN };
        const auto LISTBOX_RECT_TOP { listBoxLabelTextRegionUPtr_->GetEntityRegion().top
                                      + listBoxLabelTextRegionUPtr_->GetEntityRegion().height
                                      + LISTBOX_MARGIN };
        const auto LISTBOX_RECT_WIDTH { pageRectLeft_.width - (LISTBOX_MARGIN * 2.0f) };
        const auto LISTBOX_RECT_HEIGHT { ((pageRectLeft_.top + pageRectLeft_.height)
                                          - LISTBOX_RECT_TOP)
                                         - (LISTBOX_MARGIN * 2.0f) };

        const sf::FloatRect LISTBOX_RECT(
            LISTBOX_RECT_LEFT, LISTBOX_RECT_TOP, LISTBOX_RECT_WIDTH, LISTBOX_RECT_HEIGHT);

        listBoxBackgroundInfo_.SetupColor(LISTBOX_COLOR_BG_);
        listBoxBackgroundInfo_.SetupBorder(true, 1.0f);
        listBoxBackgroundInfo_.focus_colors = LISTBOX_COLORSET_;

        listBoxUPtr_ = std::make_unique<gui::ListBox<PopupStageSpellbook, spell::SpellPtr_t>>(
            "PopupStage'sSpellListBox",
            this,
            this,
            gui::ListBoxPacket(
                LISTBOX_RECT, listBoxBackgroundInfo_, LISTBOX_LINE_COLOR_, LISTBOX_IMAGE_COLOR_));

        for (const auto & SPELL_PTR : popupInfo_.CreaturePtrOpt().value()->SpellsPVec())
        {
            listBoxUPtr_->Append(std::make_unique<gui::ListElement<spell::SpellPtr_t>>(
                SPELL_PTR,
                gui::TextInfo(listElementTextInfo_, SPELL_PTR->Name()),
                boost::none,
                CanCastSpell(SPELL_PTR)));
        }

        EntityAdd(listBoxUPtr_.get());
    }

    void PopupStageSpellbook::SetupPageRightText(const spell::SpellPtr_t SPELL_PTR)
    {
        if (!spellTitleTextRegionUPtr_)
        {
            const sf::FloatRect SPELL_TITLE_TEXTRECT {
                pageRectRight_.left, pageRectRight_.top, pageRectRight_.width, 0.0f
            };

            const gui::TextInfo SPELL_TITLE_TEXTINFO(
                SPELL_PTR->Name(),
                gui::GuiFont::Default,
                gui::FontManager::Instance()->Size_Large(),
                sfutil::color::GrayDarker,
                gui::Justified::Center);

            spellTitleTextRegionUPtr_ = std::make_unique<gui::TextRegion>(
                "SpellnbookPopupWindowSpellTitle", SPELL_TITLE_TEXTINFO, SPELL_TITLE_TEXTRECT);
        }
        else
        {
            spellTitleTextRegionUPtr_->SetText(SPELL_PTR->Name());
        }

        // setup spell image
        spellCachedTextureOpt_ = gui::LoadAndCacheImage(SPELL_PTR->Which());
        spellSprite_.setTexture(spellCachedTextureOpt_->Get(), true);

        const auto SPELL_IMAGE_SCALE { sfutil::MapByRes(0.75f, 4.0f) };
        spellSprite_.setScale(SPELL_IMAGE_SCALE, SPELL_IMAGE_SCALE);
        spellSprite_.setColor(imageColorSlider_.Value());

        spellSprite_.setPosition(
            (pageRectRight_.left + (pageRectRight_.width * 0.5f))
                - (spellSprite_.getGlobalBounds().width * 0.5f),
            spellTitleTextRegionUPtr_->GetEntityRegion().top
                + spellTitleTextRegionUPtr_->GetEntityRegion().height
                + sfutil::MapByRes(5.0f, 60.0f));

        const auto SYM_SCALE { sfutil::MapByRes(0.75f, 3.75f) };
        xSymbolSprite_.setScale(SYM_SCALE, SYM_SCALE);

        const auto X_SYM_POS_LEFT { (spellSprite_.getGlobalBounds().left
                                     + (spellSprite_.getGlobalBounds().width * 0.5f))
                                    - (xSymbolSprite_.getGlobalBounds().width * 0.5f) };

        const auto X_SYM_POS_TOP { (spellSprite_.getGlobalBounds().top
                                    + (spellSprite_.getGlobalBounds().height * 0.5f))
                                   - (xSymbolSprite_.getGlobalBounds().height * 0.5f) };

        xSymbolSprite_.setPosition(X_SYM_POS_LEFT, X_SYM_POS_TOP);

        // setup spell details text
        std::ostringstream ss;
        ss << "Mana Cost: " << SPELL_PTR->ManaCost() << "\n"
           << "Rank: " << SPELL_PTR->Rank() << "\n"
           << "Targets " << combat::TargetType::Name(SPELL_PTR->Target()) << "\n"
           << "Cast during " << game::Phase::ToString(SPELL_PTR->ValidPhases()) << "\n";

        if (!spellDetailsTextUPtr_)
        {
            const auto SPELLDETAILS_TEXTRECT_LEFT { pageRectRight_.left };

            const auto SPELLDETAILS_TEXTRECT_TOP { spellSprite_.getGlobalBounds().top
                                                   + spellSprite_.getGlobalBounds().height
                                                   + sfutil::MapByRes(10.0f, 90.0f) };

            const auto SPELLDETAILS_TEXTRECT_WIDTH { pageRectRight_.width };
            const auto SPELLDETAILS_TEXTRECT_HEIGHT { 0.0f };

            const sf::FloatRect SPELL_DETAILS_TEXTRECT { SPELLDETAILS_TEXTRECT_LEFT,
                                                         SPELLDETAILS_TEXTRECT_TOP,
                                                         SPELLDETAILS_TEXTRECT_WIDTH,
                                                         SPELLDETAILS_TEXTRECT_HEIGHT };

            const gui::TextInfo SPELL_DETAILS_TEXTINFO(
                ss.str(),
                gui::GuiFont::Default,
                gui::FontManager::Instance()->Size_Small(),
                sfutil::color::GrayDarker,
                gui::Justified::Center);

            spellDetailsTextUPtr_ = std::make_unique<gui::TextRegion>(
                "SpellnbookPopupWindowSpellDetails",
                SPELL_DETAILS_TEXTINFO,
                SPELL_DETAILS_TEXTRECT);
        }
        else
        {
            spellDetailsTextUPtr_->SetText(ss.str());
        }

        // setup spell 'unable to cast' text
        willShowXImage_ = false;
        ss.str(" ");
        if (DoesCharacterHaveEnoughManaToCastSpell(SPELL_PTR) == false)
        {
            willShowXImage_ = true;
            ss << "Insufficient Mana";
        }
        else if (CanCastSpellInPhase(SPELL_PTR) == false)
        {
            willShowXImage_ = true;

            const auto CURRENT_PHASE { GetPhase() };

            if (CURRENT_PHASE & game::Phase::Combat)
            {
                ss << "Cannot cast during combat.";
            }
            else if (CURRENT_PHASE & game::Phase::Conversation)
            {
                ss << "Cannot cast while talking.";
            }
            else if (CURRENT_PHASE & game::Phase::Exploring)
            {
                ss << "Cannot cast while exploring.";
            }
            else if (CURRENT_PHASE & game::Phase::Inventory)
            {
                ss << "Cannot cast from inventory.";
            }
            else
            {
                ss << "Only during " << game::Phase::ToString(SPELL_PTR->ValidPhases()) << ".";
            }
        }

        const gui::TextInfo SPELL_UNABLE_TEXTINFO(
            ss.str(),
            gui::GuiFont::System,
            gui::FontManager::Instance()->Size_Normal(),
            UNABLE_TEXT_COLOR_,
            gui::Justified::Center,
            sf::Text::Bold);

        const auto VERT_SPACER { sfutil::MapByRes(15.0f, 60.0f) };

        const auto SPELL_UNABLE_TEXTRECT_LEFT { pageRectRight_.left };

        const auto SPELL_UNABLE_TEXTRECT_TOP { spellDetailsTextUPtr_->GetEntityRegion().top
                                               + spellDetailsTextUPtr_->GetEntityRegion().height
                                               + VERT_SPACER };

        const auto SPELL_UNABLE_TEXTRECT_WIDTH { pageRectRight_.width };
        const auto SPELL_UNABLE_TEXTRECT_HEIGHT { 0.0f };

        const sf::FloatRect SPELL_UNABLE_TEXTRECT { SPELL_UNABLE_TEXTRECT_LEFT,
                                                    SPELL_UNABLE_TEXTRECT_TOP,
                                                    SPELL_UNABLE_TEXTRECT_WIDTH,
                                                    SPELL_UNABLE_TEXTRECT_HEIGHT };

        unableTextUPtr_ = std::make_unique<gui::TextRegion>(
            "SpellnbookPopupWindowSpellUnableToCast", SPELL_UNABLE_TEXTINFO, SPELL_UNABLE_TEXTRECT);

        // setup spell description text
        ss.str("");
        ss << SPELL_PTR->Desc() << "  " << SPELL_PTR->DescExtra();

        const gui::TextInfo SPELL_DESC_TEXTINFO(
            ss.str(),
            gui::GuiFont::Default,
            gui::FontManager::Instance()->Size_Small(),
            sfutil::color::GrayDarker,
            gui::Justified::Center);

        const auto SPELL_DESC_HORIZ_MARGIN { sfutil::MapByRes(15.0f, 30.0f) };
        const auto SPELL_DESC_TEXTRECT_LEFT { pageRectRight_.left + SPELL_DESC_HORIZ_MARGIN };
        auto spellDescTextRectTop { 0.0f };
        if (willShowXImage_)
        {
            spellDescTextRectTop = unableTextUPtr_->GetEntityRegion().top
                + unableTextUPtr_->GetEntityRegion().height + VERT_SPACER;
        }
        else
        {
            spellDescTextRectTop = spellDetailsTextUPtr_->GetEntityRegion().top
                + spellDetailsTextUPtr_->GetEntityRegion().height + VERT_SPACER;
        }

        const auto SPELL_DESC_TEXTRECT_WIDTH { pageRectRight_.width
                                               - (SPELL_DESC_HORIZ_MARGIN * 2.0f) };

        const auto SPELL_DESC_TEXTRECT_HEIGHT {
            ((pageRectRight_.top + pageRectRight_.height) - spellDescTextRectTop) - VERT_SPACER
        };

        const sf::FloatRect SPELL_DESC_TEXTRECT { SPELL_DESC_TEXTRECT_LEFT,
                                                  spellDescTextRectTop,
                                                  SPELL_DESC_TEXTRECT_WIDTH,
                                                  SPELL_DESC_TEXTRECT_HEIGHT };

        if (!spellDescTextUPtr_)
        {
            spellDescTextUPtr_ = std::make_unique<gui::TextRegion>(
                "SpellnbookPopupWindowSpellDescription", SPELL_DESC_TEXTINFO, SPELL_DESC_TEXTRECT);
        }
        else
        {
            spellDescTextUPtr_->Setup(
                SPELL_DESC_TEXTINFO, SPELL_DESC_TEXTRECT, stage::IStagePtr_t(this));
        }
    }

    bool PopupStageSpellbook::DoesCharacterHaveEnoughManaToCastSpell(
        const spell::SpellPtr_t SPELL_PTR) const
    {
        return (popupInfo_.CreaturePtrOpt().value()->Mana() >= SPELL_PTR->ManaCost());
    }

    bool PopupStageSpellbook::CanCastSpellInPhase(const spell::SpellPtr_t SPELL_PTR) const
    {
        return (SPELL_PTR->ValidPhases() & GetPhase());
    }

    bool PopupStageSpellbook::CanCastSpell(const spell::SpellPtr_t SPELL_PTR) const
    {
        return (
            DoesCharacterHaveEnoughManaToCastSpell(SPELL_PTR) && CanCastSpellInPhase(SPELL_PTR));
    }

    bool PopupStageSpellbook::HandleSpellCast()
    {
        if (CanCastSpell(CurrentSelectedSpell()))
        {
            gui::SoundManager::Instance()
                ->GetSoundEffectSet(gui::sound_effect_set::SpellSelect)
                .PlayRandom();

            RemovePopup(ResponseTypes::Select, listBoxUPtr_->SelectionIndex());

            willShowXImage_ = false;

            return true;
        }
        else
        {
            gui::SoundManager::Instance()->PlaySfx_Reject();
            willShowXImage_ = true;
            return false;
        }
    }

    const spell::SpellPtr_t PopupStageSpellbook::CurrentSelectedSpell() const
    {
        return listBoxUPtr_->Selection()->Element();
    }

} // namespace popup
} // namespace heroespath
