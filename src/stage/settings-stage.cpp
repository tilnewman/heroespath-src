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

#include "sfml-util/sfml-util.hpp"
#include "sfml-util/loaders.hpp"
#include "sfml-util/display.hpp"
#include "sfml-util/font-manager.hpp"
#include "sfml-util/tile.hpp"
#include "sfml-util/gui/text-info.hpp"
#include "sfml-util/gui/gui-elements.hpp"
#include "sfml-util/sound-manager.hpp"
#include "sfml-util/radio-button-display.hpp"

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
    :
        Stage("Settings"),
        SLIDER_LABEL_OFFSET_VERT_(165.0f),
        LABEL_TEXT_INFO_(
            "",
            sfml_util::FontManager::Instance()->Font_Typical(),
            sfml_util::FontManager::Instance()->Size_Normal(),
            sf::Color(255, 255, 255, 200)),
        SLIDER_MOUSE_TEXT_INFO_(
            "",
            sfml_util::FontManager::Instance()->Font_NumbersDefault1(),
            sfml_util::FontManager::Instance()->Size_Normal(),
            sf::Color::White,
            sf::Color(255, 200, 200)),
        hasStageAlreadyBeenSetup_(false),
        bgTileImage_("media-images-backgrounds-tile-darkknot"),
        mainMenuTitle_("settings_button_normal.png"),
        bottomSymbol_(),
        backButtonUPtr_(std::make_unique<sfml_util::main_menu_buttons::BackButton>(
            0.0f,
            0.0f,
            1.0f,
            false)),
        bgBox_("SettingsStageWoodBackground"),
        
        resRadioButtonSetUPtr_(),
        resolutionTextRegionUPtr_(),
        musicVolumeTextRegionUPtr_(),
        musicVolumeSliderBarUPtr_(),
        effectsVolumeTextRegionUPtr_(),
        effectsVolumeSliderBarUPtr_(),
        revisionTextRegionUPtr_(),
        aaRadioButtonSetUPtr_(),
        aaTextRegionUPtr_(),
        prevAALevel_(sfml_util::Display::Instance()->AntialiasLevel()),
        musicInfoLabelTextRegionUPtr_(),
        musicInfoTextRegionUPtr_()
    {}


    SettingsStage::~SettingsStage()
    {
        ClearAllEntities();
    }


    bool SettingsStage::HandleCallback(
        const sfml_util::callback::RadioButtonCallbackPackage_t & PACKAGE)
    {
        if (PACKAGE.PTR_ == aaRadioButtonSetUPtr_.get())
        {
            prevAALevel_ = sfml_util::Display::Instance()->AntialiasLevel();

            auto const SELECT_SELECT{ static_cast<unsigned>( std::pow(
                    2.0f,
                    static_cast<double>(PACKAGE.PTR_->GetSelectedNumber() - 1))) };

            auto const NEW_ANTIALIAS_LEVEL{
                ((PACKAGE.PTR_->GetSelectedNumber() == 0) ? 0 : SELECT_SELECT) };

            game::LoopManager::Instance()->ChangeResolution(
                this,
                this,
                sfml_util::Display::GetCurrentResolution(),
                NEW_ANTIALIAS_LEVEL);

            return true;
        }
        else
        {
            return false;
        }
    }


    bool SettingsStage::HandleCallback(const popup::PopupResponse & POPUP)
    {
        M_HP_LOG(GetStageName() << " HandlePopupCallback(response=\""
            << popup::ResponseTypes::ToString(POPUP.Response()) << "\")");

        if (POPUP.Response() == popup::ResponseTypes::No)
        {
            M_HP_LOG(GetStageName() << " Stage.  User rejected the new antialias level.  "
                << "Changing back to the previous.");

            game::LoopManager::Instance()->ChangeResolution(
                this,
                this,
                sfml_util::Display::Instance()->GetCurrentResolution(),
                prevAALevel_);

            HandleResolutionChange();
            return false;
        }
        else if (POPUP.Response() == popup::ResponseTypes::Okay)
        {
            //case where the antialiasing mode was not supported and
            //need to revert back to original value
            switch (prevAALevel_)
            {
                case 1:  { aaRadioButtonSetUPtr_->SetSelectNumber(1); break; }
                case 2:  { aaRadioButtonSetUPtr_->SetSelectNumber(2); break; }
                case 4:  { aaRadioButtonSetUPtr_->SetSelectNumber(3); break; }
                case 8:  { aaRadioButtonSetUPtr_->SetSelectNumber(4); break; }
                case 16: { aaRadioButtonSetUPtr_->SetSelectNumber(5); break; }
                default: { aaRadioButtonSetUPtr_->SetSelectNumber(0); break; }
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
        Setup_MusicVolumeSlider();
        
        Setup_SfxVolumeLabel(BG_BOX_INNER_RECT);
        Setup_SfxVolumeSlider();

        Setup_AntiAliasLabel(BG_BOX_INNER_RECT);
        Setup_AntiAliasRadioButtonSet();

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


    void SettingsStage::HandleResolutionChange()
    {
        Setup();
    }


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


    void SettingsStage::Setup_BackgroundImage()
    {
        bgTileImage_.Reset();
    }


    void SettingsStage::Setup_MainMenuTitle()
    {
        mainMenuTitle_.SetPositionAndSize();
    }


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
        auto const BG_BOX_WIDTH{ StageRegionWidth() * 0.5f };
        auto const BG_BOX_HEIGHT{ StageRegionHeight() * 0.5f };

        auto const BG_BOX_LEFT{
            (StageRegionWidth() * 0.5f) - (BG_BOX_WIDTH * 0.5f) };

        auto const BG_BOX_TOP{ mainMenuTitle_.Bottom() };

        const sf::FloatRect BG_BOX_RECT(
            BG_BOX_LEFT,
            BG_BOX_TOP,
            BG_BOX_WIDTH,
            BG_BOX_HEIGHT);

        auto const BG_BOX_INNER_PAD{ sfml_util::MapByRes(10.0f, 30.0f) };

        const sf::FloatRect BG_BOX_RECT_INNER(
            BG_BOX_LEFT + BG_BOX_INNER_PAD,
            BG_BOX_TOP + BG_BOX_INNER_PAD,
            BG_BOX_WIDTH - (BG_BOX_INNER_PAD * 2.0f),
            BG_BOX_HEIGHT - (BG_BOX_INNER_PAD * 2.0f));

        sfml_util::gui::BackgroundInfo bgBoxbgInfo{
            sfml_util::gui::GuiElements::Instance()->GetTextureWood(), BG_BOX_RECT };

        bgBox_.SetupBox(sfml_util::gui::box::Info(
            true,
            BG_BOX_RECT,
            sfml_util::gui::ColorSet(sf::Color::White),
            bgBoxbgInfo));

        return BG_BOX_RECT_INNER;
    }


    void SettingsStage::Setup_ResolutionChangeLabel(const sf::FloatRect & BG_BOX_INNER_RECT)
    {
        auto const LEFT{ BG_BOX_INNER_RECT.left + sfml_util::MapByRes(60.0f, 125.0f) };
        auto const TOP{ BG_BOX_INNER_RECT.top - sfml_util::MapByRes(5.0f, 20.0f) };

        if (hasStageAlreadyBeenSetup_)
        {
            resolutionTextRegionUPtr_->SetEntityPos(LEFT, TOP);
        }
        else
        {
            sfml_util::gui::TextInfo resolutionTitleTextInfo(LABEL_TEXT_INFO_);
            resolutionTitleTextInfo.text = "Change Resolution";
            resolutionTitleTextInfo.justified = sfml_util::Justified::Center;

            resolutionTextRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
                "ResolutionChangeLabel",
                resolutionTitleTextInfo,
                sf::FloatRect(LEFT, TOP, 0.0f, 0.0f),
                sfml_util::gui::TextRegion::DEFAULT_NO_RESIZE_,
                sfml_util::gui::box::Info());

            EntityAdd(resolutionTextRegionUPtr_.get());
        }
    }


    void SettingsStage::Setup_ResolutionChangeRadioButtonSet(
        const sf::FloatRect & BG_BOX_INNER_RECT)
    {
        if (false == hasStageAlreadyBeenSetup_)
        {
            resRadioButtonSetUPtr_ = std::make_unique<sfml_util::RadioButtonSet_DisplayChange>(
                BG_BOX_INNER_RECT.left,
                BG_BOX_INNER_RECT.top + sfml_util::MapByRes(20.0f, 50.0f),
                this);

            resRadioButtonSetUPtr_->SetEntityColors(
                sfml_util::gui::ColorSet(
                    sf::Color(180, 180, 180),
                    sf::Color(0, 0, 0, 60),
                    sf::Color(180, 180, 180),
                    sf::Color(0, 0, 0, 60)));

            EntityAdd(resRadioButtonSetUPtr_.get());
        }

        auto const LEFT{
            (resRadioButtonSetUPtr_->GetEntityPos().x +
            (resRadioButtonSetUPtr_->GetEntityRegion().width * 0.5f)) -
                (resolutionTextRegionUPtr_->GetEntityRegion().width * 0.5f) };

        auto const TOP{ resolutionTextRegionUPtr_->GetEntityPos().y };

        resolutionTextRegionUPtr_->SetEntityPos(LEFT, TOP);
    }


    void SettingsStage::Setup_MusicVolumeLabel(const sf::FloatRect & BG_BOX_INNER_RECT)
    {
        auto const LEFT{
            resRadioButtonSetUPtr_->GetEntityRegion().left +
            resRadioButtonSetUPtr_->GetEntityRegion().width +
            sfml_util::MapByRes(150.0f, 250.0f) };

        auto const TOP{ BG_BOX_INNER_RECT.top - sfml_util::MapByRes(5.0f, 20.0f) };

        if (hasStageAlreadyBeenSetup_)
        {
            musicVolumeTextRegionUPtr_->SetEntityPos(LEFT, TOP);
        }
        else
        {
            sfml_util::gui::TextInfo musicVolumeTitleTextInfo(LABEL_TEXT_INFO_);
            musicVolumeTitleTextInfo.text = "Music\nVolume";
            musicVolumeTitleTextInfo.justified = sfml_util::Justified::Center;

            musicVolumeTextRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
                "MusicVolumeSettingLabel",
                musicVolumeTitleTextInfo,
                sf::FloatRect(LEFT, TOP, 0.0f, 0.0f));

            EntityAdd(musicVolumeTextRegionUPtr_.get());
        }
    }


    void SettingsStage::Setup_MusicVolumeSlider()
    {
        auto const LEFT{
            musicVolumeTextRegionUPtr_->GetEntityPos().x +
            (musicVolumeTextRegionUPtr_->GetEntityRegion().width  * 0.5f) -
            13.0f };

        auto const TOP{
            musicVolumeTextRegionUPtr_->GetEntityPos().y +
            musicVolumeTextRegionUPtr_->GetEntityRegion().height +
            5.0f };

        if (hasStageAlreadyBeenSetup_)
        {
            musicVolumeSliderBarUPtr_->SetEntityPos(LEFT, TOP);
        }
        else
        {
            musicVolumeSliderBarUPtr_ = std::make_unique<sfml_util::SliderBarLabeled_Music>(
                "MusicVolume",
                LEFT,
                TOP,
                160.0f,
                sfml_util::gui::SliderStyle(),
                SLIDER_MOUSE_TEXT_INFO_,
                0.5f,
                0.0f,
                SLIDER_LABEL_OFFSET_VERT_);

            EntityAdd(musicVolumeSliderBarUPtr_.get());
        }
    }


    void SettingsStage::Setup_SfxVolumeLabel(const sf::FloatRect & BG_BOX_INNER_RECT)
    {
        sfml_util::SliderBarLabeled_Effects::SetPreventSoundEffect(true);

        auto const LEFT{
            resRadioButtonSetUPtr_->GetEntityRegion().left +
            resRadioButtonSetUPtr_->GetEntityRegion().width +
            325.0f };

        auto const TOP{ BG_BOX_INNER_RECT.top - sfml_util::MapByRes(5.0f, 20.0f) };

        if (hasStageAlreadyBeenSetup_)
        {
            effectsVolumeTextRegionUPtr_->SetEntityPos(LEFT, TOP);
        }
        else
        {
            sfml_util::gui::TextInfo effectsVolumeTitleTextInfo(LABEL_TEXT_INFO_);
            effectsVolumeTitleTextInfo.text = "Sound Effects\nVolume";
            effectsVolumeTitleTextInfo.justified = sfml_util::Justified::Center;

            effectsVolumeTextRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
                "EffectsVolumeSettingLabel",
                effectsVolumeTitleTextInfo,
                sf::FloatRect(LEFT, TOP, 0.0f, 0.0f),
                sfml_util::gui::TextRegion::DEFAULT_NO_RESIZE_,
                sfml_util::gui::box::Info());

            EntityAdd(effectsVolumeTextRegionUPtr_.get());
        }

        sfml_util::SliderBarLabeled_Effects::SetPreventSoundEffect(false);
    }
        

    void SettingsStage::Setup_SfxVolumeSlider()
    {
        sfml_util::SliderBarLabeled_Effects::SetPreventSoundEffect(true);

        auto const LEFT{
            effectsVolumeTextRegionUPtr_->GetEntityPos().x +
            (effectsVolumeTextRegionUPtr_->GetEntityRegion().width  * 0.5f) -
            13.0f };

        auto const TOP{
            effectsVolumeTextRegionUPtr_->GetEntityPos().y +
            effectsVolumeTextRegionUPtr_->GetEntityRegion().height +
            5.0f };

        if (hasStageAlreadyBeenSetup_)
        {
            effectsVolumeSliderBarUPtr_->SetEntityPos(LEFT, TOP);
        }
        else
        {   
            effectsVolumeSliderBarUPtr_ = std::make_unique<sfml_util::SliderBarLabeled_Effects>(
                "EffectsVolume",
                LEFT,
                TOP,
                160.0f,
                sfml_util::gui::SliderStyle(),
                SLIDER_MOUSE_TEXT_INFO_,
                0.5f,
                0.0f,
                SLIDER_LABEL_OFFSET_VERT_);

            EntityAdd(effectsVolumeSliderBarUPtr_.get());
        }

        sfml_util::SliderBarLabeled_Effects::SetPreventSoundEffect(false);
    }


    void SettingsStage::Setup_AntiAliasLabel(const sf::FloatRect & BG_BOX_INNER_RECT)
    {
        auto const LEFT{
            resRadioButtonSetUPtr_->GetEntityRegion().left +
            resRadioButtonSetUPtr_->GetEntityRegion().width +
            65.0f };

        auto const TOP{ BG_BOX_INNER_RECT.top - sfml_util::MapByRes(5.0f, 20.0f) };

        if (hasStageAlreadyBeenSetup_)
        {
            aaTextRegionUPtr_->SetEntityPos(LEFT, TOP);
        }
        else
        {
            sfml_util::gui::TextInfo aaLabelTextInfo(LABEL_TEXT_INFO_);
            aaLabelTextInfo.text = "Anti-\nAliasing";
            aaLabelTextInfo.justified = sfml_util::Justified::Center;

            aaTextRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
                "AntiAliasSettingLabel",
                aaLabelTextInfo,
                sf::FloatRect(LEFT, TOP, 0.0f, 0.0f));

            EntityAdd(aaTextRegionUPtr_.get());
        }
    }


    void SettingsStage::Setup_AntiAliasRadioButtonSet()
    {
        auto const LEFT{
            resRadioButtonSetUPtr_->GetEntityRegion().left +
            resRadioButtonSetUPtr_->GetEntityRegion().width +
            70.0f };

        auto const TOP{
            aaTextRegionUPtr_->GetEntityPos().y +
            aaTextRegionUPtr_->GetEntityRegion().height +
            5.0f };

        if (hasStageAlreadyBeenSetup_)
        {
            aaRadioButtonSetUPtr_->SetEntityPos(LEFT, TOP);
        }
        else
        {
            const sfml_util::gui::TextInfo TEXT_INFO(
                " ",
                sfml_util::FontManager::Instance()->Font_Default1(),
                20);

            const std::vector<std::string> LABELS_VEC =
                { "0   ", "1   ", "2   ", "4   ", "8   ", "16  " };

            const sfml_util::GradientInfo GRADIENT_INFO(
                sf::Color(0, 0, 0, 150),
                sfml_util::Corner::TopLeft | sfml_util::Corner::BottomRight);

            const sfml_util::gui::BackgroundInfo BG_INFO(
                sf::Color(0, 0, 0, 60),
                GRADIENT_INFO);

            const sfml_util::gui::box::Info BOX_INFO(
                1.0f,
                true,
                sf::FloatRect(),
                sfml_util::gui::ColorSet(sf::Color(180, 180, 180)),
                BG_INFO);

            //establish which antialiasing level is current
            std::size_t aaInitialSelection(0);
            const unsigned ANTIALIAS_LEVEL(sfml_util::Display::Instance()->AntialiasLevel());
            switch (ANTIALIAS_LEVEL)
            {
                case 0:  { aaInitialSelection = 0; break; }
                case 1:  { aaInitialSelection = 1; break; }
                case 2:  { aaInitialSelection = 2; break; }
                case 4:  { aaInitialSelection = 3; break; }
                case 8:  { aaInitialSelection = 4; break; }
                case 16: { aaInitialSelection = 5; break; }
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
                LEFT,
                TOP,
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
    }


    void SettingsStage::Setup_MusicInfoLabel(const sf::FloatRect & BG_BOX_INNER_RECT)
    {
        auto const PAD{ 30.0f };

        auto const LEFT{
            (aaRadioButtonSetUPtr_->GetEntityRegion().left - PAD) -
            sfml_util::MapByRes(10.0f, 30.0f) };

        auto const TOP{
            aaRadioButtonSetUPtr_->GetEntityRegion().top +
            aaRadioButtonSetUPtr_->GetEntityRegion().height +
            PAD };

        auto const WIDTH{
            (BG_BOX_INNER_RECT.left + BG_BOX_INNER_RECT.width) - LEFT };

        auto const HEIGHT{ 0.0f };

        const sf::FloatRect TEXT_RECT(LEFT, TOP, WIDTH, HEIGHT);

        if (hasStageAlreadyBeenSetup_)
        {
            musicInfoLabelTextRegionUPtr_->SetEntityPos(LEFT, TOP);
        }
        else
        {
            const sfml_util::gui::TextInfo TEXT_INFO(
                "Music Currently Playing",
                sfml_util::FontManager::Instance()->Font_Typical(),
                29,
                sf::Color::White,
                sfml_util::Justified::Center);

            musicInfoLabelTextRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
                "MusicInfoLabel",
                TEXT_INFO,
                TEXT_RECT);

            EntityAdd(musicInfoLabelTextRegionUPtr_.get());
        }
    }


    void SettingsStage::Setup_MusicInfoBox(const sf::FloatRect & BG_BOX_INNER_RECT)
    {
        auto const PAD{ 30.0f };

        auto const LEFT{ aaRadioButtonSetUPtr_->GetEntityRegion().left };

        auto const TOP{
            musicInfoLabelTextRegionUPtr_->GetEntityRegion().top +
            musicInfoLabelTextRegionUPtr_->GetEntityRegion().height +
            PAD };

        auto const WIDTH{
            ((BG_BOX_INNER_RECT.left + BG_BOX_INNER_RECT.width) - LEFT) - PAD };

        auto const HEIGHT{
            (BG_BOX_INNER_RECT.top + BG_BOX_INNER_RECT.height) - TOP };

        const sf::FloatRect RECT(
            LEFT,
            TOP,
            WIDTH,
            HEIGHT);

        if (hasStageAlreadyBeenSetup_)
        {
            musicInfoTextRegionUPtr_->SetEntityPos(LEFT, TOP);
        }
        else
        {   
            auto const MUSIC_INFO_VEC{ sfml_util::SoundManager::Instance()->MusicInfoSet() };

            std::ostringstream ss;
            for (auto const & MUSIC_INFO : MUSIC_INFO_VEC)
            {
                ss << "\"" << MUSIC_INFO.SongName()
                    << "\"\nby " << MUSIC_INFO.ArtistName()
                    << "\nLicense: " << MUSIC_INFO.LicenseTitle()
                    << "\nDuration: " << MUSIC_INFO.Duration().ToString()
                    << "\n\n";
            }

            if (ss.str().empty())
            {
                ss << " ";
            }

            const sfml_util::gui::TextInfo TEXT_INFO(
                ss.str(),
                sfml_util::FontManager::Instance()->Font_Default1(),
                27,
                sfml_util::FontManager::Color_Light(),
                sfml_util::Justified::Center);

            const sfml_util::GradientInfo GRADIENT_INFO(
                sf::Color(0, 0, 0, 150),
                sfml_util::Corner::TopLeft | sfml_util::Corner::BottomRight);

            const sfml_util::gui::BackgroundInfo BACKGROUND_INFO(
                sf::Color(0, 0, 0, 60),
                GRADIENT_INFO);

            const sfml_util::gui::box::Info BOX_INFO(
                1.0f,
                true,
                RECT,
                sfml_util::gui::ColorSet(sf::Color(180, 180, 180)),
                BACKGROUND_INFO);

            musicInfoTextRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
                "SettingsStageMusicInfo",
                TEXT_INFO,
                RECT,
                this,
                sfml_util::gui::TextRegion::DEFAULT_NO_RESIZE_,
                BOX_INFO);

            EntityAdd(musicInfoTextRegionUPtr_.get());
        }
    }


    void SettingsStage::Setup_RevisionNumber(const sf::FloatRect & BG_BOX_INNER_RECT)
    {
        if (false == hasStageAlreadyBeenSetup_)
        {
            std::ostringstream ss;
            ss << "Revision: " << game::GameDataFile::Instance()->GetCopyStr("system-revision");

            const sfml_util::gui::TextInfo TEXT_INFO(
                ss.str(),
                sfml_util::FontManager::Instance()->Font_Default1(),
                sfml_util::FontManager::Instance()->Size_Normal(),
                sfml_util::FontManager::Color_Light(),
                sfml_util::Justified::Left);

            revisionTextRegionUPtr_ = std::make_unique<sfml_util::gui::TextRegion>(
                "Version",
                TEXT_INFO,
                sf::FloatRect());

            EntityAdd(revisionTextRegionUPtr_.get());
        }

        auto const LEFT{ BG_BOX_INNER_RECT.left };

        auto const TOP{
            ((BG_BOX_INNER_RECT.top + BG_BOX_INNER_RECT.height) -
            revisionTextRegionUPtr_->GetEntityRegion().height) };

        revisionTextRegionUPtr_->SetEntityPos(LEFT, TOP);
    }

}
}
