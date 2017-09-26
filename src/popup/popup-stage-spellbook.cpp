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
// popup-stage-spellbook.cpp
//
#include "popup-stage-spellbook.hpp"

#include "game/spell/spell-base.hpp"
#include "game/creature/creature.hpp"
#include "game/loop-manager.hpp"

#include "popup/popup-manager.hpp"
#include "sfml-util/gui/creature-image-manager.hpp"
#include "sfml-util/gui/spell-image-manager.hpp"
#include "sfml-util/sound-manager.hpp"

#include "misc/random.hpp"

#include <sstream>
#include <random>
#include <algorithm>


namespace popup
{

    const float     PopupStageSpellbook::BACKGROUND_WIDTH_RATIO_    { 0.85f };
    const float     PopupStageSpellbook::COLOR_FADE_SPEED_          { 6.0f };
    const sf::Uint8 PopupStageSpellbook::SPELL_IMAGE_ALPHA_         { 192 };
    const sf::Color PopupStageSpellbook::UNABLE_TEXT_COLOR_         { sf::Color(127, 32, 32) };
    const float     PopupStageSpellbook::WARNING_DURATION_SEC_      { 2.0f };


    PopupStageSpellbook::PopupStageSpellbook(const popup::PopupInfo & POPUP_INFO)
        :
        PopupStageBase(POPUP_INFO),
        playerTexture_(),
        playerSprite_(),
        pageRectLeft_(),
        pageRectRight_(),
        charDetailsTextRegionUPtr_(),
        listBoxLabelTextRegionUPtr_(),
        listBoxSPtr_(),
        LISTBOX_IMAGE_COLOR_(sf::Color(255, 255, 255, 190)),
        LISTBOX_LINE_COLOR_(sfml_util::FontManager::Color_GrayDark()),
        LISTBOX_COLOR_FG_(LISTBOX_LINE_COLOR_),
        LISTBOX_COLOR_BG_(sfml_util::FontManager::Color_Orange() - sf::Color(100, 100, 100, 220)),
        LISTBOX_COLORSET_(LISTBOX_COLOR_FG_, LISTBOX_COLOR_BG_),
        LISTBOX_BG_INFO_(LISTBOX_COLOR_BG_),
        listBoxItemTextInfo_(
            " ",
            sfml_util::FontManager::Instance()->Font_Default2(),
            sfml_util::FontManager::Instance()->Size_Smallish(),
            sfml_util::FontManager::Color_GrayDarker(),
            sfml_util::Justified::Left),
        spellTexture_(),
        spellSprite_(),
        spellTitleTextRegionUPtr_(),
        spellDetailsTextUPtr_(),
        unableTextUPtr_(),
        spellDescTextUPtr_(),
        currentSpellPtr_(nullptr),
        warnColorShaker_(UNABLE_TEXT_COLOR_, sf::Color::Transparent, 20.0f),
        imageColorSlider_(sf::Color::Transparent, sf::Color::White, COLOR_FADE_SPEED_),
        textColorSlider_(sf::Color::Transparent, sf::Color::Black, COLOR_FADE_SPEED_)
    {}


    PopupStageSpellbook::~PopupStageSpellbook()
    {}


    bool PopupStageSpellbook::HandleCallback(
        const sfml_util::gui::callback::ListBoxEventPackage & PACKAGE)
    {
        if (PACKAGE.package.PTR_ == nullptr)
        {
            return false;
        }

        if ((PACKAGE.gui_event == sfml_util::GuiEvent::Click) ||
            (PACKAGE.gui_event == sfml_util::GuiEvent::SelectionChange) ||
            (PACKAGE.keypress_event.code == sf::Keyboard::Up) ||
            (PACKAGE.keypress_event.code == sf::Keyboard::Down))
        {
            if ((PACKAGE.package.PTR_->GetSelected() != nullptr) &&
                (currentSpellPtr_ != PACKAGE.package.PTR_->GetSelected()->SPELL_CPTRC))
            {
                if (currentSpellPtr_ != PACKAGE.package.PTR_->GetSelected()->SPELL_CPTRC)
                {
                    currentSpellPtr_ = PACKAGE.package.PTR_->GetSelected()->SPELL_CPTRC;
                }

                if (imageColorSlider_.Direction() != sfml_util::Moving::Away)
                {
                    imageColorSlider_.ChangeDirection();
                    imageColorSlider_.Start();
                    textColorSlider_.ChangeDirection();
                    textColorSlider_.Start();
                }

                return true;
            }
        }
        else if ((PACKAGE.gui_event == sfml_util::GuiEvent::DoubleClick) ||
                 (PACKAGE.keypress_event.code == sf::Keyboard::Return))
        {
            if (PACKAGE.package.PTR_->GetSelected()->SPELL_CPTRC != nullptr)
            {
                return HandleSpellCast();
            }
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

        //Force spell listbox to take focus so that user up/down
        //keystrokes work without having to click on the listbox.
        SetFocus(listBoxSPtr_.get());

        //Force spell listbox selection up and down to force
        //colors to correct.
        listBoxSPtr_->WillPlaySoundEffects(false);
        sf::Event::KeyEvent keyEvent;
        keyEvent.code = sf::Keyboard::Down;
        listBoxSPtr_->KeyRelease(keyEvent);
        keyEvent.code = sf::Keyboard::Up;
        listBoxSPtr_->KeyRelease(keyEvent);
        listBoxSPtr_->WillPlaySoundEffects(true);

        currentSpellPtr_ = listBoxSPtr_->At(0)->SPELL_CPTRC;
        SetupPageRightText(currentSpellPtr_);
    }


    void PopupStageSpellbook::Draw(
        sf::RenderTarget & target, const sf::RenderStates & STATES)
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

        Stage::Draw(target, STATES);

        PopupStageBase::DrawRedX(target, STATES);
    }


    void PopupStageSpellbook::UpdateTime(const float ELAPSED_TIME_SECONDS)
    {
        Stage::UpdateTime(ELAPSED_TIME_SECONDS);

        textColorSlider_.UpdateTime(ELAPSED_TIME_SECONDS);
        imageColorSlider_.UpdateTime(ELAPSED_TIME_SECONDS);

        SetPageRightColors(imageColorSlider_.Current(), textColorSlider_.Current());

        if ((imageColorSlider_.IsMoving() == false) &&
            (imageColorSlider_.Direction() == sfml_util::Moving::Away))
        {
            sfml_util::SoundManager::Instance()->SoundEffectPlay(
                sfml_util::sound_effect::Magic1);

            SetupPageRightText(currentSpellPtr_);
            imageColorSlider_.ChangeDirection();
            imageColorSlider_.Start();
            textColorSlider_.ChangeDirection();
            textColorSlider_.Start();
        }

        if (willShowXImage_)
        {
            unableTextUPtr_->SetEntityColorFgBoth(warnColorShaker_.Update(ELAPSED_TIME_SECONDS));
        }
    }


    bool PopupStageSpellbook::KeyRelease(const sf::Event::KeyEvent & KEY_EVENT)
    {
        if ((KEY_EVENT.code == sf::Keyboard::Escape) ||
            (KEY_EVENT.code == sf::Keyboard::Space))
        {
            sfml_util::SoundManager::Instance()->Getsound_effect_set(
                sfml_util::sound_effect_set::Thock).PlayRandom();

            game::LoopManager::Instance()->PopupWaitEnd(ResponseTypes::Cancel, 0);
            return true;
        }
        else if ((KEY_EVENT.code == sf::Keyboard::Return) ||
                 (KEY_EVENT.code == sf::Keyboard::C))
        {
            return HandleSpellCast();
        }

        return PopupStageBase::KeyRelease(KEY_EVENT);
    }


    void PopupStageSpellbook::SetupOuterAndInnerRegion()
    {
        auto const SPELLBOOK_WIDTH{ sfml_util::Display::Instance()->GetWinWidth() *
            BACKGROUND_WIDTH_RATIO_ };

        auto const SPELLBOOK_HEIGHT{ (static_cast<float>(backgroundTexture_.getSize().y) *
            SPELLBOOK_WIDTH) / static_cast<float>(backgroundTexture_.getSize().x) };

        sf::FloatRect region;
        region.left =
            ((sfml_util::Display::Instance()->GetWinWidth() - SPELLBOOK_WIDTH) * 0.5f);

        region.top =
            ((sfml_util::Display::Instance()->GetWinHeight() - SPELLBOOK_HEIGHT) * 0.5f);

        region.width = SPELLBOOK_WIDTH;
        region.height = SPELLBOOK_HEIGHT;

        SetupFullscreenRegionsAndBackgroundImage(region);
    }


    void PopupStageSpellbook::SetupRegions()
    {
        auto const LEFT_PAGE_RECT_RAW{
            sfml_util::ConvertRect<int, float>(
                popup::PopupManager::Rect_Spellbook_PageLeft()) };

        auto const SCALE{
            innerRegion_.width / static_cast<float>(backgroundTexture_.getSize().x) };

        pageRectLeft_.left   = innerRegion_.left + (LEFT_PAGE_RECT_RAW.left * SCALE);
        pageRectLeft_.top    = innerRegion_.top + (LEFT_PAGE_RECT_RAW.top * SCALE);
        pageRectLeft_.width  = LEFT_PAGE_RECT_RAW.width * SCALE;
        pageRectLeft_.height = LEFT_PAGE_RECT_RAW.height * SCALE;

        auto const RIGHT_PAGE_RECT_RAW{ sfml_util::ConvertRect<int, float>(
            popup::PopupManager::Rect_Spellbook_PageRight()) };

        pageRectRight_.left = innerRegion_.left + (RIGHT_PAGE_RECT_RAW.left * SCALE);
        pageRectRight_.top = innerRegion_.top + (RIGHT_PAGE_RECT_RAW.top * SCALE);
        pageRectRight_.width = RIGHT_PAGE_RECT_RAW.width * SCALE;
        pageRectRight_.height = RIGHT_PAGE_RECT_RAW.height * SCALE;
    }


    void PopupStageSpellbook::SetupLeftAccentImage()
    {
        popup::PopupManager::Instance()->LoadRandomAccentImage(accentTexture1_);
        accentSprite1_.setTexture(accentTexture1_);

        sfml_util::CenterAndScaleSpriteToFit(
            accentSprite1_,
            pageRectLeft_,
            ACCENT_IMAGE_SCALEDOWN_RATIO_);

        accentSprite1_.setColor(sf::Color(255, 255, 255, ACCENT_IMAGE_ALPHA_));
    }


    void PopupStageSpellbook::SetupRightAccentImage()
    {
        popup::PopupManager::Instance()->LoadRandomAccentImage(accentTexture2_);
        accentSprite2_.setTexture(accentTexture2_);

        sfml_util::CenterAndScaleSpriteToFit(
            accentSprite2_,
            pageRectRight_,
            ACCENT_IMAGE_SCALEDOWN_RATIO_);

        accentSprite2_.setColor(sf::Color(255, 255, 255, ACCENT_IMAGE_ALPHA_));
    }


    void PopupStageSpellbook::SetupPlayerImage()
    {
        sfml_util::gui::CreatureImageManager::Instance()->GetImage(
            playerTexture_, popupInfo_.CreaturePtr()->ImageFilename(), true);

        sfml_util::Invert(playerTexture_);
        sfml_util::Mask(playerTexture_, sf::Color::White);

        playerSprite_.setTexture(playerTexture_ );

        auto const PLAYER_IMAGE_SCALE{ sfml_util::MapByRes(0.55f, 3.5f) };

        playerSprite_.setScale(PLAYER_IMAGE_SCALE, PLAYER_IMAGE_SCALE);
        playerSprite_.setColor(sf::Color(255, 255, 255, 127));
        playerSprite_.setPosition(pageRectLeft_.left, pageRectLeft_.top);
    }


    void PopupStageSpellbook::SetupPlayerDetailsText()
    {
        auto creaturePtr{ popupInfo_.CreaturePtr() };

        std::ostringstream ss;
        ss << creaturePtr->Name() << "\n";

        if (creaturePtr->IsBeast())
        {
            ss << creaturePtr->RaceName();

            if (creaturePtr->Race() != game::creature::race::Wolfen)
            {
                ss << ", " << creaturePtr->RoleName();
            }

            ss << " " << creaturePtr->RankClassName() << "\n";
        }
        else
        {
            ss << creaturePtr->RankClassName()
                << " " << creaturePtr->RoleName()
                << "\n" << creaturePtr->RaceName()
                << "\n";
        }

        ss << "Rank:  " << creaturePtr->Rank() << "\n"
            << "Health:  " << creaturePtr->HealthCurrent() << "/" << creaturePtr->HealthNormal()
            << " " << creaturePtr->HealthPercentStr() << "\n"
            << "Mana:  " << creaturePtr->TraitWorking(game::stats::Traits::Mana) << "/"
            << creaturePtr->TraitNormal(game::stats::Traits::Mana) << "\n"
            << "\n";

        const sfml_util::gui::TextInfo DETAILS_TEXTINFO(
            ss.str(),
            sfml_util::FontManager::Instance()->Font_Default1(),
            sfml_util::FontManager::Instance()->Size_Small(),
            sfml_util::FontManager::Color_GrayDarker(),
            sfml_util::Justified::Left);

        const sf::FloatRect DETAILS_TEXT_RECT{
            pageRectLeft_.left + playerSprite_.getGlobalBounds().width +
                sfml_util::MapByRes(10.0f, 40.0f),
            pageRectLeft_.top + sfml_util::MapByRes(20.0f, 80.0f),
            0.0f,
            0.0f };

        charDetailsTextRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
            "SpellnbookPopupWindowDetails",
            DETAILS_TEXTINFO,
            DETAILS_TEXT_RECT);
    }


    void PopupStageSpellbook::SetupSpellListboxLabel()
    {
        const sfml_util::gui::TextInfo LISTBOX_LABEL_TEXTINFO(
            "Spells",
            sfml_util::FontManager::Instance()->Font_Default1(),
            sfml_util::FontManager::Instance()->Size_Largeish(),
            sfml_util::FontManager::Color_GrayDarker(),
            sfml_util::Justified::Left);

        const sf::FloatRect LISTBOX_LABEL_TEXTRECT{
            pageRectLeft_.left + sfml_util::MapByRes(10.0f, 40.0f),
            playerSprite_.getGlobalBounds().top + playerSprite_.getGlobalBounds().height +
                sfml_util::MapByRes(20.0f, 80.0f),
            0.0f,
            0.0f };

        listBoxLabelTextRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
            "SpellnbookPopupWindowSpellListLabel",
            LISTBOX_LABEL_TEXTINFO,
            LISTBOX_LABEL_TEXTRECT);
    }


    void PopupStageSpellbook::SetPageRightColors(
        const sf::Color & IMAGE_COLOR,
        const sf::Color & TEXT_COLOR)
    {
        spellSprite_.setColor( IMAGE_COLOR );

        const sfml_util::gui::ColorSet TEXT_COLOR_SET(
            TEXT_COLOR,
            TEXT_COLOR,
            TEXT_COLOR,
            TEXT_COLOR);

        spellTitleTextRegionUPtr_->SetEntityColors(TEXT_COLOR_SET);
        spellDetailsTextUPtr_->SetEntityColors(TEXT_COLOR_SET);
        spellDescTextUPtr_->SetEntityColors(TEXT_COLOR_SET);

        auto unableTextColor{ UNABLE_TEXT_COLOR_ };
        unableTextColor.a = TEXT_COLOR.a;

        const sfml_util::gui::ColorSet UNABLE_EXT_COLOR_SET(
            unableTextColor,
            unableTextColor,
            unableTextColor,
            unableTextColor);

        unableTextUPtr_->SetEntityColors(UNABLE_EXT_COLOR_SET);

        auto redXColor{ sf::Color::Red };
        redXColor.a = IMAGE_COLOR.a;
        xSymbolSprite_.setColor(redXColor);
    }


    void PopupStageSpellbook::SetupSpellListbox()
    {
        auto const LISTBOX_MARGIN     { sfml_util::MapByRes(15.0f, 45.0f) };
        auto const LISTBOX_RECT_LEFT  { pageRectLeft_.left + LISTBOX_MARGIN };
        auto const LISTBOX_RECT_TOP   { listBoxLabelTextRegionUPtr_->GetEntityRegion().top + listBoxLabelTextRegionUPtr_->GetEntityRegion().height + LISTBOX_MARGIN };
        auto const LISTBOX_RECT_WIDTH { pageRectLeft_.width - (LISTBOX_MARGIN * 2.0f) };
        auto const LISTBOX_RECT_HEIGHT{ ((pageRectLeft_.top + pageRectLeft_.height) - LISTBOX_RECT_TOP) - (LISTBOX_MARGIN * 2.0f) };

        const sf::FloatRect LISTBOX_RECT(
            LISTBOX_RECT_LEFT,
            LISTBOX_RECT_TOP,
            LISTBOX_RECT_WIDTH,
            LISTBOX_RECT_HEIGHT);

        const sfml_util::gui::box::Info LISTBOX_BOX_INFO(
            1,
            true,
            LISTBOX_RECT,
            LISTBOX_COLORSET_,
            LISTBOX_BG_INFO_);

        sfml_util::gui::ListBoxItemSLst_t listBoxItemsSList;
        auto const SPELL_PVEC{ popupInfo_.CreaturePtr()->SpellsPVec() };
        for (auto const NEXT_SPELL_PTR : SPELL_PVEC)
        {
            listBoxItemTextInfo_.text = NEXT_SPELL_PTR->Name();
            auto const LISTBOXITEM_SPTR( std::make_shared<sfml_util::gui::ListBoxItem>(
                NEXT_SPELL_PTR->Name() + "_SpellsListBoxEntry",
                listBoxItemTextInfo_,
                NEXT_SPELL_PTR,
                CanCastSpell(NEXT_SPELL_PTR)) );

            listBoxItemsSList.push_back(LISTBOXITEM_SPTR);
        }

        listBoxSPtr_ = std::make_shared<sfml_util::gui::ListBox>(
            "PopupStage'sSpellListBox",
            LISTBOX_RECT,
            listBoxItemsSList,
            this,
            10.0f,
            6.0f,
            LISTBOX_BOX_INFO,
            LISTBOX_LINE_COLOR_,
            sfml_util::gui::ListBox::NO_LIMIT_,
            this);

        EntityAdd(listBoxSPtr_.get());
        listBoxSPtr_->SetSelectedIndex(0);
        listBoxSPtr_->SetImageColor(LISTBOX_IMAGE_COLOR_);
    }


    void PopupStageSpellbook::SetupPageRightText(
        const game::spell::SpellPtrC_t SPELL_CPTRC)
    {
        if (SPELL_CPTRC == nullptr)
        {
            return;
        }

        //setup spell title text
        const sfml_util::gui::TextInfo SPELL_TITLE_TEXTINFO(
            SPELL_CPTRC->Name(),
            sfml_util::FontManager::Instance()->Font_Default1(),
            sfml_util::FontManager::Instance()->Size_Large(),
            sfml_util::FontManager::Color_GrayDarker(),
            sfml_util::Justified::Center);

        const sf::FloatRect SPELL_TITLE_TEXTRECT{
            pageRectRight_.left,
            pageRectRight_.top,
            pageRectRight_.width,
            0.0f };

        if (spellTitleTextRegionUPtr_.get() == nullptr)
        {
            spellTitleTextRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
                "SpellnbookPopupWindowSpellTitle",
                SPELL_TITLE_TEXTINFO,
                SPELL_TITLE_TEXTRECT);
        }
        else
        {
            spellTitleTextRegionUPtr_->SetText(SPELL_CPTRC->Name());
        }

        //setup spell image
        sfml_util::gui::SpellImageManager::Instance()->Get(
            spellTexture_, SPELL_CPTRC->Which());

        spellSprite_.setTexture(spellTexture_);
        auto const SPELL_IMAGE_SCALE{ sfml_util::MapByRes(0.75f, 4.0f) };
        spellSprite_.setScale(SPELL_IMAGE_SCALE, SPELL_IMAGE_SCALE);
        spellSprite_.setColor(imageColorSlider_.Current());

        spellSprite_.setPosition((pageRectRight_.left + (pageRectRight_.width * 0.5f)) -
            (spellSprite_.getGlobalBounds().width * 0.5f),
                spellTitleTextRegionUPtr_->GetEntityRegion().top +
                    spellTitleTextRegionUPtr_->GetEntityRegion().height +
                        sfml_util::MapByRes(5.0f, 60.0f));

        auto const SYM_SCALE{ sfml_util::MapByRes(0.75f, 3.75f) };
        xSymbolSprite_.setScale(SYM_SCALE, SYM_SCALE);

        auto const X_SYM_POS_LEFT{ (spellSprite_.getGlobalBounds().left +
            (spellSprite_.getGlobalBounds().width * 0.5f)) -
                (xSymbolSprite_.getGlobalBounds().width * 0.5f) };

        auto const X_SYM_POS_TOP{ (spellSprite_.getGlobalBounds().top +
            (spellSprite_.getGlobalBounds().height * 0.5f)) -
                (xSymbolSprite_.getGlobalBounds().height * 0.5f) };

        xSymbolSprite_.setPosition(X_SYM_POS_LEFT, X_SYM_POS_TOP);

        //setup spell details text
        std::ostringstream ss;
        ss << "Mana Cost: " << SPELL_CPTRC->ManaCost() << "\n"
           << "Rank: " << SPELL_CPTRC->Rank() << "\n"
           << "Targets " << game::TargetType::Name(SPELL_CPTRC->Target()) << "\n"
           << "Cast during " << game::Phase::ToString(SPELL_CPTRC->ValidPhases(), false) << "\n";

        const sfml_util::gui::TextInfo SPELL_DETAILS_TEXTINFO(
            ss.str(),
            sfml_util::FontManager::Instance()->Font_Default1(),
            sfml_util::FontManager::Instance()->Size_Small(),
            sfml_util::FontManager::Color_GrayDarker(),
            sfml_util::Justified::Center);

        auto const SPELLDETAILS_TEXTRECT_LEFT   { pageRectRight_.left };

        auto const SPELLDETAILS_TEXTRECT_TOP    { spellSprite_.getGlobalBounds().top +
            spellSprite_.getGlobalBounds().height + sfml_util::MapByRes(10.0f, 90.0f) };

        auto const SPELLDETAILS_TEXTRECT_WIDTH  { pageRectRight_.width };
        auto const SPELLDETAILS_TEXTRECT_HEIGHT { 0.0f };

        const sf::FloatRect SPELL_DETAILS_TEXTRECT{
            SPELLDETAILS_TEXTRECT_LEFT,
            SPELLDETAILS_TEXTRECT_TOP,
            SPELLDETAILS_TEXTRECT_WIDTH,
            SPELLDETAILS_TEXTRECT_HEIGHT };

        if (spellDetailsTextUPtr_.get() == nullptr)
        {
            spellDetailsTextUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
                "SpellnbookPopupWindowSpellDetails",
                SPELL_DETAILS_TEXTINFO,
                SPELL_DETAILS_TEXTRECT);
        }
        else
        {
            spellDetailsTextUPtr_->SetText(ss.str());
        }

        //setup spell 'unable to cast' text
        willShowXImage_ = false;
        ss.str(" ");
        if (DoesCharacterHaveEnoughManaToCastSpell(SPELL_CPTRC) == false)
        {
            willShowXImage_ = true;
            ss << "Insufficient Mana";
        }
        else if(CanCastSpellInPhase(SPELL_CPTRC) == false)
        {
            willShowXImage_ = true;

            auto const CURRENT_PHASE{ game::LoopManager::Instance()->GetPhase() };

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
                ss << "Only during "
                    << game::Phase::ToString(SPELL_CPTRC->ValidPhases(), false) << ".";
            }
        }

        const sfml_util::gui::TextInfo SPELL_UNABLE_TEXTINFO(
            ss.str(),
            sfml_util::FontManager::Instance()->Font_Default2(),
            sfml_util::FontManager::Instance()->Size_Normal(),
            UNABLE_TEXT_COLOR_,
            sf::BlendAlpha,
            sf::Text::Bold,
            sfml_util::Justified::Center);

        auto const VERT_SPACER{ sfml_util::MapByRes(15.0f, 60.0f) };

        auto const SPELL_UNABLE_TEXTRECT_LEFT   { pageRectRight_.left };

        auto const SPELL_UNABLE_TEXTRECT_TOP    { spellDetailsTextUPtr_->GetEntityRegion().top +
            spellDetailsTextUPtr_->GetEntityRegion().height + VERT_SPACER };

        auto const SPELL_UNABLE_TEXTRECT_WIDTH  { pageRectRight_.width };
        auto const SPELL_UNABLE_TEXTRECT_HEIGHT { 0.0f };

        const sf::FloatRect SPELL_UNABLE_TEXTRECT{
            SPELL_UNABLE_TEXTRECT_LEFT,
            SPELL_UNABLE_TEXTRECT_TOP,
            SPELL_UNABLE_TEXTRECT_WIDTH,
            SPELL_UNABLE_TEXTRECT_HEIGHT };

        unableTextUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
            "SpellnbookPopupWindowSpellUnableToCast",
            SPELL_UNABLE_TEXTINFO,
            SPELL_UNABLE_TEXTRECT);

        //setup spell description text
        ss.str("");
        ss << SPELL_CPTRC->Desc() << "  " << SPELL_CPTRC->DescExtra();

        const sfml_util::gui::TextInfo SPELL_DESC_TEXTINFO(
            ss.str(),
            sfml_util::FontManager::Instance()->Font_Default1(),
            sfml_util::FontManager::Instance()->Size_Small(),
            sfml_util::FontManager::Color_GrayDarker(),
            sfml_util::Justified::Center);

        auto const SPELL_DESC_HORIZ_MARGIN{ sfml_util::MapByRes(15.0f, 30.0f) };
        auto const SPELL_DESC_TEXTRECT_LEFT{ pageRectRight_.left + SPELL_DESC_HORIZ_MARGIN };
        auto spellDescTextRectTop{ 0.0f };
        if (willShowXImage_)
        {
            spellDescTextRectTop = unableTextUPtr_->GetEntityRegion().top +
                unableTextUPtr_->GetEntityRegion().height + VERT_SPACER;
        }
        else
        {
            spellDescTextRectTop = spellDetailsTextUPtr_->GetEntityRegion().top +
                spellDetailsTextUPtr_->GetEntityRegion().height + VERT_SPACER;
        }

        auto const SPELL_DESC_TEXTRECT_WIDTH{ pageRectRight_.width -
            (SPELL_DESC_HORIZ_MARGIN * 2.0f) };

        auto const SPELL_DESC_TEXTRECT_HEIGHT{ ((pageRectRight_.top + pageRectRight_.height) -
            spellDescTextRectTop) - VERT_SPACER };

        const sf::FloatRect SPELL_DESC_TEXTRECT{
            SPELL_DESC_TEXTRECT_LEFT,
            spellDescTextRectTop,
            SPELL_DESC_TEXTRECT_WIDTH,
            SPELL_DESC_TEXTRECT_HEIGHT };

        if (spellDescTextUPtr_.get() == nullptr)
        {
            spellDescTextUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
                "SpellnbookPopupWindowSpellDescription",
                SPELL_DESC_TEXTINFO,
                SPELL_DESC_TEXTRECT);
        }
        else
        {
            spellDescTextUPtr_->Setup(SPELL_DESC_TEXTINFO, SPELL_DESC_TEXTRECT, this);
        }
    }


    bool PopupStageSpellbook::DoesCharacterHaveEnoughManaToCastSpell(
        const game::spell::SpellPtrC_t SPELL_CPTRC) const
    {
        return (popupInfo_.CreaturePtr()->TraitWorking(game::stats::Traits::Mana) >=
            SPELL_CPTRC->ManaCost());
    }


    bool PopupStageSpellbook::CanCastSpellInPhase(const game::spell::SpellPtrC_t SPELL_CPTRC) const
    {
        return (SPELL_CPTRC->ValidPhases() & game::LoopManager::Instance()->GetPhase());
    }


    bool PopupStageSpellbook::CanCastSpell(const game::spell::SpellPtrC_t SPELL_CPTRC) const
    {
        return (DoesCharacterHaveEnoughManaToCastSpell(SPELL_CPTRC) &&
            CanCastSpellInPhase(SPELL_CPTRC));
    }


    bool PopupStageSpellbook::HandleSpellCast()
    {
        if (CanCastSpell(listBoxSPtr_->GetSelected()->SPELL_CPTRC))
        {
            sfml_util::SoundManager::Instance()->Getsound_effect_set(
                sfml_util::sound_effect_set::SpellSelect).PlayRandom();

            game::LoopManager::Instance()->PopupWaitEnd(
                ResponseTypes::Select, listBoxSPtr_->GetSelectedIndex());

            willShowXImage_ = false;

            return true;
        }
        else
        {
            sfml_util::SoundManager::Instance()->Getsound_effect_set(
                sfml_util::sound_effect_set::Prompt).Play(sfml_util::sound_effect::PromptWarn);

            willShowXImage_ = true;

            return false;
        }
    }

}
