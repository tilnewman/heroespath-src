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
// settings-stage.cpp
//
#include "settings-stage.hpp"

#include "sfml-util/display.hpp"
#include "sfml-util/font-manager.hpp"
#include "sfml-util/gui/gui-elements.hpp"
#include "sfml-util/gui/text-info.hpp"
#include "sfml-util/loaders.hpp"
#include "sfml-util/radio-button-display.hpp"
#include "sfml-util/sfml-util.hpp"
#include "sfml-util/sound-manager.hpp"
#include "sfml-util/tile.hpp"

#include "misc/real.hpp"

#include "log/log-macros.hpp"

#include "game/game-data-file.hpp"
#include "game/loop-manager.hpp"

#include <sstream>
#include <string>
#include <vector>

namespace heroespath
{
namespace stage
{

    SettingsStage::SettingsStage()
        : Stage("Settings")
        , SLIDER_LENGTH_VERT_(160.0f)
        , hasStageAlreadyBeenSetup_(false)
        , prevAALevel_(sfml_util::Display::Instance()->AntialiasLevel())
        , bgTileImage_("media-images-backgrounds-tile-darkknot")
        , mainMenuTitle_("settings_button_normal.png")
        , bottomSymbol_()
        , backButtonUPtr_(
              std::make_unique<sfml_util::main_menu_buttons::BackButton>(0.0f, 0.0f, 1.0f, false))
        , bgBox_("SettingsStageWoodBackground")
        , resLabelTextRegionUPtr_()
        , resRadioButtonSetUPtr_()
        , aaLabelTextRegionUPtr_()
        , aaRadioButtonSetUPtr_()
        , musicVolLabelTextRegionUPtr_()
        , musicVolSliderBarUPtr_()
        , effectsVolLabelTextRegionUPtr_()
        , effectsVolSliderBarUPtr_()
        , musicInfoLabelTextRegionUPtr_()
        , musicInfoDetailsTextRegionUPtr_()
        , revLabelTextRegionUPtr_()
    {}

    SettingsStage::~SettingsStage() { Stage::ClearAllEntities(); }

    bool SettingsStage::HandleCallback(
        const sfml_util::callback::RadioButtonCallbackPackage_t & PACKAGE)
    {
        if (PACKAGE.PTR_ == aaRadioButtonSetUPtr_.get())
        {
            prevAALevel_ = sfml_util::Display::Instance()->AntialiasLevel();

            auto const SELECT_SELECT{ static_cast<unsigned>(
                std::pow(2.0f, static_cast<double>(PACKAGE.PTR_->GetSelectedNumber() - 1))) };

            auto const NEW_ANTIALIAS_LEVEL{ (
                (PACKAGE.PTR_->GetSelectedNumber() == 0) ? 0 : SELECT_SELECT) };

            game::LoopManager::Instance()->ChangeResolution(
                this, this, sfml_util::Display::GetCurrentResolution(), NEW_ANTIALIAS_LEVEL);

            return true;
        }
        else
        {
            return false;
        }
    }

    bool SettingsStage::HandleCallback(const popup::PopupResponse & POPUP)
    {
        M_HP_LOG(
            GetStageName() << " HandlePopupCallback(response=\""
                           << popup::ResponseTypes::ToString(POPUP.Response()) << "\")");

        if (POPUP.Response() == popup::ResponseTypes::No)
        {
            M_HP_LOG(
                GetStageName() << " Stage.  User rejected the new antialias level.  "
                               << "Changing back to the previous.");

            game::LoopManager::Instance()->ChangeResolution(
                this, this, sfml_util::Display::Instance()->GetCurrentResolution(), prevAALevel_);

            HandleResolutionChange();
            return false;
        }
        else if (POPUP.Response() == popup::ResponseTypes::Okay)
        {
            // case where the antialiasing mode was not supported and
            // need to revert back to original value
            switch (prevAALevel_)
            {
                case 1:
                {
                    aaRadioButtonSetUPtr_->SetSelectNumber(1);
                    break;
                }
                case 2:
                {
                    aaRadioButtonSetUPtr_->SetSelectNumber(2);
                    break;
                }
                case 4:
                {
                    aaRadioButtonSetUPtr_->SetSelectNumber(3);
                    break;
                }
                case 8:
                {
                    aaRadioButtonSetUPtr_->SetSelectNumber(4);
                    break;
                }
                case 16:
                {
                    aaRadioButtonSetUPtr_->SetSelectNumber(5);
                    break;
                }
                default:
                {
                    aaRadioButtonSetUPtr_->SetSelectNumber(0);
                    break;
                }
            }

            return true;
        }
        else
        {
            return true;
        }
    }

    void SettingsStage::Setup()
    {
        Setup_StageRegionToFullscreen();
        Setup_BackgroundImage();
        Setup_MainMenuTitle();
        Setup_BottomSymbol();
        Setup_BackButton();

        auto const BG_BOX_INNER_RECT{ Setup_WoodBackgroundBoxAndReturnInnerRect() };

        Setup_ResolutionChangeLabel(BG_BOX_INNER_RECT);
        Setup_ResolutionChangeRadioButtonSet(BG_BOX_INNER_RECT);

        Setup_MusicVolumeLabel(BG_BOX_INNER_RECT);
        Setup_MusicVolumeSlider(BG_BOX_INNER_RECT);

        Setup_SfxVolumeLabel(BG_BOX_INNER_RECT);
        Setup_SfxVolumeSlider(BG_BOX_INNER_RECT);

        Setup_AntiAliasLabel(BG_BOX_INNER_RECT);
        Setup_AntiAliasRadioButtonSet(BG_BOX_INNER_RECT);

        Setup_MusicInfoLabel(BG_BOX_INNER_RECT);
        Setup_MusicInfoBox(BG_BOX_INNER_RECT);

        Setup_RevisionNumber(BG_BOX_INNER_RECT);

        hasStageAlreadyBeenSetup_ = true;
    }

    void SettingsStage::Draw(sf::RenderTarget & target, const sf::RenderStates & STATES)
    {
        target.draw(bgTileImage_, STATES);
        target.draw(mainMenuTitle_, STATES);
        target.draw(bottomSymbol_, STATES);
        target.draw(bgBox_, STATES);
        Stage::Draw(target, STATES);
    }

    void SettingsStage::HandleResolutionChange() { Setup(); }

    bool SettingsStage::KeyRelease(const sf::Event::KeyEvent & KEY_EVENT)
    {
        if (Stage::KeyRelease(KEY_EVENT))
        {
            return true;
        }
        else if (KEY_EVENT.code == sf::Keyboard::B)
        {
            backButtonUPtr_->SetMouseState(sfml_util::MouseState::Over);
            sfml_util::SoundManager::Instance()->PlaySfx_Keypress();
            game::LoopManager::Instance()->TransitionTo_Previous();
            return true;
        }
        else
        {
            return false;
        }
    }

    void SettingsStage::Setup_StageRegionToFullscreen()
    {
        const sf::FloatRect STAGE_REGION(
            0.0f,
            0.0f,
            sfml_util::Display::Instance()->GetWinWidth(),
            sfml_util::Display::Instance()->GetWinHeight());

        StageRegionSet(STAGE_REGION);
    }

    void SettingsStage::Setup_BackgroundImage() { bgTileImage_.Reset(); }

    void SettingsStage::Setup_MainMenuTitle() { mainMenuTitle_.SetPositionAndSize(); }

    void SettingsStage::Setup_BottomSymbol()
    {
        bottomSymbol_.Setup();

        if (false == hasStageAlreadyBeenSetup_)
        {
            EntityAdd(backButtonUPtr_.get());
        }
    }

    void SettingsStage::Setup_BackButton()
    {
        backButtonUPtr_->SetScaleToRes();
        backButtonUPtr_->SetVertPositionToBottomOfScreenByRes(sfml_util::MapByRes(50.0f, 300.0f));
    }

    const sf::FloatRect SettingsStage::Setup_WoodBackgroundBoxAndReturnInnerRect()
    {
        auto const BG_BOX_WIDTH{ std::max(StageRegionWidth() * 0.45f, 1000.0f) };
        auto const BG_BOX_HEIGHT{ std::max(StageRegionHeight() * 0.5f, 500.0f) };

        auto const BG_BOX_LEFT{ (StageRegionWidth() * 0.5f) - (BG_BOX_WIDTH * 0.5f) };

        auto const BG_BOX_TOP{ mainMenuTitle_.Bottom() };

        const sf::FloatRect BG_BOX_RECT(BG_BOX_LEFT, BG_BOX_TOP, BG_BOX_WIDTH, BG_BOX_HEIGHT);

        auto const BG_BOX_INNER_PAD{ sfml_util::MapByRes(20.0f, 60.0f) };

        const sf::FloatRect BG_BOX_RECT_INNER(
            BG_BOX_LEFT + BG_BOX_INNER_PAD,
            BG_BOX_TOP + BG_BOX_INNER_PAD,
            BG_BOX_WIDTH - (BG_BOX_INNER_PAD * 2.0f),
            BG_BOX_HEIGHT - (BG_BOX_INNER_PAD * 2.0f));

        sfml_util::gui::BackgroundInfo bgBoxbgInfo{
            sfml_util::gui::GuiElements::Instance()->GetTextureWood(), BG_BOX_RECT
        };

        bgBox_.SetupBox(sfml_util::gui::box::Info(
            true, BG_BOX_RECT, sfml_util::gui::ColorSet(sf::Color::White), bgBoxbgInfo));

        return BG_BOX_RECT_INNER;
    }

    void SettingsStage::Setup_ResolutionChangeLabel(const sf::FloatRect & BG_BOX_INNER_RECT)
    {
        if (false == hasStageAlreadyBeenSetup_)
        {
            resLabelTextRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
                "ResolutionChangeLabel", CreateLabelTextInfo("Change Resolution"), sf::FloatRect());

            EntityAdd(resLabelTextRegionUPtr_.get());
        }

        auto const LEFT{ BG_BOX_INNER_RECT.left + sfml_util::MapByRes(60.0f, 125.0f) };
        auto const TOP{ BG_BOX_INNER_RECT.top };

        resLabelTextRegionUPtr_->Setup(
            CreateLabelTextInfo("Change Resolution"), sf::FloatRect(LEFT, TOP, 0.0f, 0.0f));
    }

    void
        SettingsStage::Setup_ResolutionChangeRadioButtonSet(const sf::FloatRect & BG_BOX_INNER_RECT)
    {
        if (false == hasStageAlreadyBeenSetup_)
        {
            resRadioButtonSetUPtr_ = std::make_unique<sfml_util::RadioButtonSet_DisplayChange>(
                BG_BOX_INNER_RECT.left,
                BG_BOX_INNER_RECT.top + sfml_util::MapByRes(20.0f, 50.0f),
                this);

            resRadioButtonSetUPtr_->SetEntityColors(sfml_util::gui::ColorSet(
                sf::Color(180, 180, 180),
                sf::Color(0, 0, 0, 60),
                sf::Color(180, 180, 180),
                sf::Color(0, 0, 0, 60)));

            EntityAdd(resRadioButtonSetUPtr_.get());
        }

        auto const LEFT{ BG_BOX_INNER_RECT.left };

        auto const TOP{ resLabelTextRegionUPtr_->GetEntityPos().y
                        + resLabelTextRegionUPtr_->GetEntityRegion().height };

        resRadioButtonSetUPtr_->SetEntityPos(LEFT, TOP);
    }

    void SettingsStage::Setup_AntiAliasLabel(const sf::FloatRect & BG_BOX_INNER_RECT)
    {
        if (false == hasStageAlreadyBeenSetup_)
        {
            aaLabelTextRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
                "AntiAliasSettingLabel", CreateLabelTextInfo("Anti-\nAliasing"), sf::FloatRect());

            EntityAdd(aaLabelTextRegionUPtr_.get());
        }

        auto const LEFT{ HorizPositionOfColumn(1, BG_BOX_INNER_RECT)
                         - (aaLabelTextRegionUPtr_->GetEntityRegion().width * 0.5f) };

        auto const TOP{ BG_BOX_INNER_RECT.top };

        aaLabelTextRegionUPtr_->Setup(
            CreateLabelTextInfo("Anti-\nAliasing"), sf::FloatRect(LEFT, TOP, 0.0f, 0.0f));
    }

    void SettingsStage::Setup_AntiAliasRadioButtonSet(const sf::FloatRect & BG_BOX_INNER_RECT)
    {
        if (false == hasStageAlreadyBeenSetup_)
        {
            const sfml_util::gui::TextInfo TEXT_INFO(
                " ",
                sfml_util::FontManager::Instance()->Font_Default1(),
                sfml_util::FontManager::Instance()->Size_Small());

            const std::vector<std::string> LABELS_VEC
                = { "0   ", "1   ", "2   ", "4   ", "8   ", "16  " };

            const sfml_util::GradientInfo GRADIENT_INFO(
                sf::Color(0, 0, 0, 150),
                sfml_util::Corner::TopLeft | sfml_util::Corner::BottomRight);

            const sfml_util::gui::BackgroundInfo BG_INFO(sf::Color(0, 0, 0, 60), GRADIENT_INFO);

            const sfml_util::gui::box::Info BOX_INFO(
                1.0f,
                true,
                sf::FloatRect(),
                sfml_util::gui::ColorSet(sf::Color(180, 180, 180)),
                BG_INFO);

            // establish which antialiasing level is current
            std::size_t aaInitialSelection(0);
            const unsigned ANTIALIAS_LEVEL(sfml_util::Display::Instance()->AntialiasLevel());
            switch (ANTIALIAS_LEVEL)
            {
                case 0:
                {
                    aaInitialSelection = 0;
                    break;
                }
                case 1:
                {
                    aaInitialSelection = 1;
                    break;
                }
                case 2:
                {
                    aaInitialSelection = 2;
                    break;
                }
                case 4:
                {
                    aaInitialSelection = 3;
                    break;
                }
                case 8:
                {
                    aaInitialSelection = 4;
                    break;
                }
                case 16:
                {
                    aaInitialSelection = 5;
                    break;
                }
                default:
                {
                    std::ostringstream ss;
                    ss << "Unsupported Antialiasing level " << ANTIALIAS_LEVEL
                       << " detected.  Will report and use 0 until another "
                       << "valid setting is made.";

                    M_HP_LOG("SettingsStage::Setup()  " << ss.str());
                    aaInitialSelection = 0;
                    break;
                }
            }

            aaRadioButtonSetUPtr_ = std::make_unique<sfml_util::gui::RadioButtonSet>(
                "Antialias",
                0.0f,
                0.0f,
                TEXT_INFO,
                LABELS_VEC,
                aaInitialSelection,
                sfml_util::Brightness::Bright,
                misc::SizetVec_t(),
                BOX_INFO);

            aaRadioButtonSetUPtr_->SetEntityColors(sfml_util::gui::ColorSet(
                sf::Color(180, 180, 180),
                sf::Color(0, 0, 0, 60),
                sf::Color(180, 180, 180),
                sf::Color(0, 0, 0, 60)));

            aaRadioButtonSetUPtr_->CallbackHandlerAdd(this);

            EntityAdd(aaRadioButtonSetUPtr_.get());
        }

        auto const LEFT{ HorizPositionOfColumn(1, BG_BOX_INNER_RECT)
                         - (aaRadioButtonSetUPtr_->GetEntityRegion().width * 0.5f) };

        auto const TOP{ aaLabelTextRegionUPtr_->GetEntityPos().y
                        + aaLabelTextRegionUPtr_->GetEntityRegion().height };

        aaRadioButtonSetUPtr_->SetEntityPos(LEFT, TOP);
    }

    void SettingsStage::Setup_MusicVolumeLabel(const sf::FloatRect & BG_BOX_INNER_RECT)
    {
        if (false == hasStageAlreadyBeenSetup_)
        {
            musicVolLabelTextRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
                "MusicVolumeSettingLabel", CreateLabelTextInfo("Music\nVolume"), sf::FloatRect());

            EntityAdd(musicVolLabelTextRegionUPtr_.get());
        }

        auto const LEFT{ HorizPositionOfColumn(2, BG_BOX_INNER_RECT)
                         - (musicVolLabelTextRegionUPtr_->GetEntityRegion().width * 0.5f) };

        auto const TOP{ BG_BOX_INNER_RECT.top };

        musicVolLabelTextRegionUPtr_->Setup(
            CreateLabelTextInfo("Music\nVolume"), sf::FloatRect(LEFT, TOP, 0.0f, 0.0f));
    }

    void SettingsStage::Setup_MusicVolumeSlider(const sf::FloatRect & BG_BOX_INNER_RECT)
    {
        if (false == hasStageAlreadyBeenSetup_)
        {
            musicVolSliderBarUPtr_ = std::make_unique<sfml_util::SliderBarLabeled_Music>(
                "MusicVolume",
                0.0f,
                0.0f,
                SLIDER_LENGTH_VERT_,
                sfml_util::gui::SliderStyle(),
                CreateSliderbarValueTextInfoSet(),
                0.5f,
                0.0f,
                SLIDER_LENGTH_VERT_ + SliderLabelVertPad());

            EntityAdd(musicVolSliderBarUPtr_.get());
        }

        auto const LEFT{ HorizPositionOfColumn(2, BG_BOX_INNER_RECT)
                         - (musicVolSliderBarUPtr_->GetEntityRegion().width * 0.5f) };

        auto const TOP{ musicVolLabelTextRegionUPtr_->GetEntityPos().y
                        + musicVolLabelTextRegionUPtr_->GetEntityRegion().height };

        musicVolSliderBarUPtr_->SetEntityPos(LEFT, TOP);
        musicVolSliderBarUPtr_->ChangeTextInfo(CreateSliderbarValueTextInfoSet());
    }

    void SettingsStage::Setup_SfxVolumeLabel(const sf::FloatRect & BG_BOX_INNER_RECT)
    {
        sfml_util::SliderBarLabeled_Effects::SetPreventSoundEffect(true);

        if (false == hasStageAlreadyBeenSetup_)
        {
            effectsVolLabelTextRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
                "EffectsVolumeSettingLabel",
                CreateLabelTextInfo("Sound Effects\nVolume"),
                sf::FloatRect(),
                sfml_util::gui::TextRegion::DEFAULT_NO_RESIZE_,
                sfml_util::gui::box::Info());

            EntityAdd(effectsVolLabelTextRegionUPtr_.get());
        }

        auto const LEFT{ HorizPositionOfColumn(3, BG_BOX_INNER_RECT)
                         - (effectsVolLabelTextRegionUPtr_->GetEntityRegion().width * 0.5f) };

        auto const TOP{ BG_BOX_INNER_RECT.top };

        effectsVolLabelTextRegionUPtr_->Setup(
            CreateLabelTextInfo("Sound Effects\nVolume"), sf::FloatRect(LEFT, TOP, 0.0f, 0.0f));

        sfml_util::SliderBarLabeled_Effects::SetPreventSoundEffect(false);
    }

    void SettingsStage::Setup_SfxVolumeSlider(const sf::FloatRect & BG_BOX_INNER_RECT)
    {
        sfml_util::SliderBarLabeled_Effects::SetPreventSoundEffect(true);

        if (false == hasStageAlreadyBeenSetup_)
        {
            effectsVolSliderBarUPtr_ = std::make_unique<sfml_util::SliderBarLabeled_Effects>(
                "EffectsVolume",
                0.0f,
                0.0f,
                SLIDER_LENGTH_VERT_,
                sfml_util::gui::SliderStyle(),
                CreateSliderbarValueTextInfoSet(),
                0.5f,
                0.0f,
                SLIDER_LENGTH_VERT_ + SliderLabelVertPad());

            EntityAdd(effectsVolSliderBarUPtr_.get());
        }

        auto const LEFT{ HorizPositionOfColumn(3, BG_BOX_INNER_RECT)
                         - (effectsVolSliderBarUPtr_->GetEntityRegion().width * 0.5f) };

        auto const TOP{ effectsVolLabelTextRegionUPtr_->GetEntityPos().y
                        + effectsVolLabelTextRegionUPtr_->GetEntityRegion().height };

        effectsVolSliderBarUPtr_->SetEntityPos(LEFT, TOP);
        effectsVolSliderBarUPtr_->ChangeTextInfo(CreateSliderbarValueTextInfoSet());

        sfml_util::SliderBarLabeled_Effects::SetPreventSoundEffect(false);
    }

    void SettingsStage::Setup_MusicInfoLabel(const sf::FloatRect & BG_BOX_INNER_RECT)
    {
        if (false == hasStageAlreadyBeenSetup_)
        {
            musicInfoLabelTextRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
                "MusicInfoLabel", CreateLabelTextInfo("Music Currently Playing"), sf::FloatRect());

            EntityAdd(musicInfoLabelTextRegionUPtr_.get());
        }

        auto const PAD{ 30.0f };

        auto const LEFT{ (aaRadioButtonSetUPtr_->GetEntityRegion().left - PAD)
                         - sfml_util::MapByRes(10.0f, 30.0f) };

        auto const TOP{ aaRadioButtonSetUPtr_->GetEntityRegion().top
                        + aaRadioButtonSetUPtr_->GetEntityRegion().height + PAD };

        auto const WIDTH{ (BG_BOX_INNER_RECT.left + BG_BOX_INNER_RECT.width) - LEFT };

        musicInfoLabelTextRegionUPtr_->Setup(
            CreateLabelTextInfo("Music Currently Playing"), sf::FloatRect(LEFT, TOP, WIDTH, 0.0f));
    }

    void SettingsStage::Setup_MusicInfoBox(const sf::FloatRect & BG_BOX_INNER_RECT)
    {
        auto const PAD{ sfml_util::MapByRes(20.0f, 60.0f) };

        auto const LEFT{ aaRadioButtonSetUPtr_->GetEntityPos().x };

        auto const TOP{ musicInfoLabelTextRegionUPtr_->GetEntityRegion().top
                        + musicInfoLabelTextRegionUPtr_->GetEntityRegion().height + PAD };

        auto const WIDTH{ ((BG_BOX_INNER_RECT.left + BG_BOX_INNER_RECT.width) - LEFT) - PAD };

        auto const HEIGHT{ (BG_BOX_INNER_RECT.top + BG_BOX_INNER_RECT.height) - TOP };

        const sf::FloatRect RECT(LEFT, TOP, WIDTH, HEIGHT);

        auto const MUSIC_INFO_VEC{ sfml_util::SoundManager::Instance()->MusicInfoSet() };

        std::ostringstream ss;
        for (auto const & MUSIC_INFO : MUSIC_INFO_VEC)
        {
            ss << "\"" << MUSIC_INFO.SongName() << "\"\nby " << MUSIC_INFO.ArtistName()
               << "\nLicense: " << MUSIC_INFO.LicenseTitle()
               << "\nDuration: " << MUSIC_INFO.Duration().ToString() << "\n\n";
        }

        if (ss.str().empty())
        {
            ss << " ";
        }

        const sfml_util::gui::TextInfo TEXT_INFO(
            ss.str(),
            sfml_util::FontManager::Instance()->Font_Default1(),
            sfml_util::FontManager::Instance()->Size_Normal(),
            sfml_util::FontManager::Color_Light(),
            sfml_util::Justified::Center);

        const sfml_util::GradientInfo GRADIENT_INFO(
            sf::Color(0, 0, 0, 150), sfml_util::Corner::TopLeft | sfml_util::Corner::BottomRight);

        const sfml_util::gui::BackgroundInfo BACKGROUND_INFO(sf::Color(0, 0, 0, 60), GRADIENT_INFO);

        const sfml_util::gui::box::Info BOX_INFO(
            1.0f, true, RECT, sfml_util::gui::ColorSet(sf::Color(180, 180, 180)), BACKGROUND_INFO);

        if (false == hasStageAlreadyBeenSetup_)
        {
            musicInfoDetailsTextRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
                "SettingsStageMusicInfo",
                TEXT_INFO,
                RECT,
                this,
                sfml_util::gui::TextRegion::DEFAULT_NO_RESIZE_,
                BOX_INFO);

            EntityAdd(musicInfoDetailsTextRegionUPtr_.get());
        }

        musicInfoDetailsTextRegionUPtr_->Setup(
            TEXT_INFO, RECT, sfml_util::gui::TextRegion::DEFAULT_NO_RESIZE_, BOX_INFO);
    }

    void SettingsStage::Setup_RevisionNumber(const sf::FloatRect & BG_BOX_INNER_RECT)
    {
        std::ostringstream ss;
        ss << "Revision: " << game::GameDataFile::Instance()->GetCopyStr("system-revision");

        const sfml_util::gui::TextInfo TEXT_INFO(
            ss.str(),
            sfml_util::FontManager::Instance()->Font_Default1(),
            sfml_util::FontManager::Instance()->Size_Smallish(),
            sfml_util::FontManager::Color_Light(),
            sfml_util::Justified::Left);

        if (false == hasStageAlreadyBeenSetup_)
        {
            revLabelTextRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
                "RevisionNumber", TEXT_INFO, sf::FloatRect());

            EntityAdd(revLabelTextRegionUPtr_.get());
        }

        auto const LEFT{ BG_BOX_INNER_RECT.left };

        auto const TOP{ (
            (BG_BOX_INNER_RECT.top + BG_BOX_INNER_RECT.height)
            - revLabelTextRegionUPtr_->GetEntityRegion().height) };

        revLabelTextRegionUPtr_->Setup(TEXT_INFO, sf::FloatRect(LEFT, TOP, 0.0f, 0.0f));
    }

    const sfml_util::gui::TextInfo
        SettingsStage::CreateLabelTextInfo(const std::string & TITLE) const
    {
        return sfml_util::gui::TextInfo(
            TITLE,
            sfml_util::FontManager::Instance()->Font_Typical(),
            sfml_util::FontManager::Instance()->Size_Normal(),
            sf::Color(255, 255, 255, 200),
            sfml_util::Justified::Center);
    }

    const sfml_util::gui::MouseTextInfo SettingsStage::CreateSliderbarValueTextInfoSet() const
    {
        return sfml_util::gui::MouseTextInfo(
            "",
            sfml_util::FontManager::Instance()->Font_NumbersDefault1(),
            sfml_util::FontManager::Instance()->Size_Small(),
            sf::Color::White,
            sf::Color(255, 200, 200));
    }

    float SettingsStage::HorizPositionOfColumn(
        const std::size_t COLUMN_NUM, const sf::FloatRect & BG_BOX_INNER_RECT)
    {
        auto const LEFT{ resRadioButtonSetUPtr_->GetEntityRegion().left
                         + resRadioButtonSetUPtr_->GetEntityRegion().width };

        auto const REMAINING_RIGHT_SPACE{ (BG_BOX_INNER_RECT.left + BG_BOX_INNER_RECT.width)
                                          - LEFT };

        auto const COLUMN_WIDTH{ REMAINING_RIGHT_SPACE / 4.0f };

        return LEFT + (static_cast<float>(COLUMN_NUM) * COLUMN_WIDTH);
    }

    float SettingsStage::SliderLabelVertPad() const { return sfml_util::MapByRes(5.0f, 15.0f); }
} // namespace stage
} // namespace heroespath
