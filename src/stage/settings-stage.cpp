// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// settings-stage.cpp
//
#include "settings-stage.hpp"

#include "gui/box-entity.hpp"
#include "gui/display.hpp"
#include "gui/font-manager.hpp"
#include "gui/gui-images.hpp"
#include "gui/sound-manager.hpp"
#include "gui/text-info.hpp"
#include "misc/config-file.hpp"
#include "misc/log-macros.hpp"
#include "misc/real.hpp"
#include "popup/popup-response.hpp"
#include "sfutil/center.hpp"
#include "sfutil/display.hpp"
#include "sfutil/position.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

#include <algorithm>
#include <sstream>
#include <vector>

namespace heroespath
{
namespace stage
{

    SettingsStage::SettingsStage()
        : StageBase(
            "Settings",
            { gui::GuiFont::Default,
              gui::GuiFont::System,
              gui::GuiFont::SystemCondensed,
              gui::GuiFont::Number,
              gui::GuiFont::Handwriting },
            false)
        , SLIDER_LENGTH_VERT_(160.0f)
        , hasStageAlreadyBeenSetup_(false)
        , prevAALevel_(gui::Display::Instance()->AntialiasLevel())
        , screenBgBox_(
              "SettingsStage'sBackground",
              StageRegion(),
              gui::BoxEntityInfo(gui::CachedTexture(
                  "media-images-backgrounds-tile-darkknot",
                  gui::ImageOpt::Default | gui::ImageOpt::Repeated)))
        , stageTitle_("media-images-buttons-mainmenu-settings-normal")
        , bottomSymbol_()
        , backButtonUPtr_(std::make_unique<gui::MainMenuButton>(
              stage::Stage::Previous, gui::ImageTextEntity::Callback_t::IHandlerPtr_t(this), -1.0f))
        , settingsBoxUPtr_()
        , resLabelTextRegionUPtr_()
        //, resRadioButtonSetUPtr_()
        , aaLabelTextRegionUPtr_()
        //, aaRadioButtonSetUPtr_()
        , musicVolLabelTextRegionUPtr_()
        , musicVolSliderBarUPtr_()
        , effectsVolLabelTextRegionUPtr_()
        , effectsVolSliderBarUPtr_()
        , musicInfoLabelTextRegionUPtr_()
        , musicInfoDetailsTextRegionUPtr_()
        , revLabelTextRegionUPtr_()
    {}

    SettingsStage::~SettingsStage() { StageBase::ClearAllEntities(); }

    /*bool SettingsStage::HandleCallback(
        const gui::RadioButton::Callback_t::PacketPtr_t  PACKAGE_PTR)
    {
        if (PACKAGE.PTR_ == aaRadioButtonSetUPtr_.get())
        {
            prevAALevel_ = gui::Display::Instance()->AntialiasLevel();

            const auto SELECT_SELECT { static_cast<unsigned>(
                std::pow(2.0f, static_cast<double>(PACKAGE.PTR_->GetSelectedNumber() - 1))) };

            const auto NEW_ANTIALIAS_LEVEL { (
                (PACKAGE.PTR_->GetSelectedNumber() == 0) ? 0 : SELECT_SELECT) };

            gChangeResolution(
                this, this, gui::Display::GetCurrentResolution(), NEW_ANTIALIAS_LEVEL);

            return true;
        }
        else
        {
            return false;
        }
    }*/

    const std::string SettingsStage::HandleCallback(
        const misc::PopupCallback_t::Packet_t &, const std::string & PACKET_DESCRIPTION)
    {
        /*
        M_HP_LOG(
            GetStageName() << " HandlePopupCallback(response=\""
                           << popup::ResponseTypes::ToString(PACKET_PTR->type) << "\")");

        if (PACKET_PTR->type == popup::ResponseTypes::No)
        {
            M_HP_LOG(
                GetStageName() << " Stage.  User rejected the new antialias level.  "
                               << "Changing back to the previous.");

            ChangeResolution(
                this, this, gui::Display::Instance()->GetCurrentResolution(), prevAALevel_);

            HandleResolutionChange();
            return false;
        }
        else if (PACKET_PTR->type == popup::ResponseTypes::Okay)
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
        */

        return MakeCallbackHandlerMessage(
            PACKET_DESCRIPTION,
            "popup callback NOT HANDLED TODO THIS CODE COMMENTED OUT UNTIL THE SETTINGS STAGE IS "
            "FIXED");
    }

    void SettingsStage::Setup()
    {
        Setup_StageRegionToFullscreen();
        Setup_BackgroundImage();
        Setup_StageTitle();
        Setup_BottomSymbol();
        Setup_BackButton();

        const auto BG_BOX_INNER_RECT { Setup_WoodBackgroundBoxAndReturnInnerRect() };

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

    void SettingsStage::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        target.draw(screenBgBox_, states);
        target.draw(stageTitle_, states);
        target.draw(bottomSymbol_, states);
        target.draw(*settingsBoxUPtr_, states);
        StageBase::draw(target, states);
    }

    void SettingsStage::HandleResolutionChange() { Setup(); }

    bool SettingsStage::KeyRelease(const sf::Event::KeyEvent & KEY_EVENT)
    {
        if (StageBase::KeyRelease(KEY_EVENT))
        {
            return true;
        }
        else if (KEY_EVENT.code == sf::Keyboard::B)
        {
            backButtonUPtr_->SetMouseState(gui::MouseState::Over);
            gui::SoundManager::Instance()->PlaySfx_Keypress();
            TransitionTo(stage::Stage::Previous);
            return true;
        }
        else
        {
            return false;
        }
    }

    void SettingsStage::Setup_StageRegionToFullscreen()
    {
        StageRegion(gui::Display::Instance()->FullScreenRect());
    }

    void SettingsStage::Setup_BackgroundImage() { ; }

    void SettingsStage::Setup_StageTitle() { stageTitle_.SetSizeAndReCenter(); }

    void SettingsStage::Setup_BottomSymbol()
    {
        bottomSymbol_.Setup();

        if (false == hasStageAlreadyBeenSetup_)
        {
            EntityAdd(backButtonUPtr_);
        }
    }

    void SettingsStage::Setup_BackButton()
    {
        backButtonUPtr_->SetEntityPos(
            sfutil::ScreenRatioToPixelsHoriz(0.039f),
            sfutil::CenterOfVert(bottomSymbol_.Region())
                - (backButtonUPtr_->GetEntityRegion().height * 0.5f));
    }

    const sf::FloatRect SettingsStage::Setup_WoodBackgroundBoxAndReturnInnerRect()
    {
        const auto BG_BOX_WIDTH { std::max(StageRegion().width * 0.45f, 1000.0f) };
        const auto BG_BOX_HEIGHT { std::max(StageRegion().height * 0.5f, 500.0f) };

        const auto BG_BOX_LEFT { (StageRegion().width * 0.5f) - (BG_BOX_WIDTH * 0.5f) };

        const auto BG_BOX_TOP { sfutil::Bottom(stageTitle_.Region())
                                + stageTitle_.DefaultBottomPad() };

        const sf::FloatRect BG_BOX_RECT(BG_BOX_LEFT, BG_BOX_TOP, BG_BOX_WIDTH, BG_BOX_HEIGHT);

        const auto BG_BOX_INNER_PAD { sfutil::MapByRes(20.0f, 60.0f) };

        const sf::FloatRect BG_BOX_RECT_INNER(
            BG_BOX_LEFT + BG_BOX_INNER_PAD,
            BG_BOX_TOP + BG_BOX_INNER_PAD,
            BG_BOX_WIDTH - (BG_BOX_INNER_PAD * 2.0f),
            BG_BOX_HEIGHT - (BG_BOX_INNER_PAD * 2.0f));

        gui::BoxEntityInfo bgBoxbgInfo;

        bgBoxbgInfo.SetupBorder(true);

        bgBoxbgInfo.focus_colors = gui::FocusColors(sf::Color::White);

        bgBoxbgInfo.SetupImage(
            gui::CachedTexture(
                "media-images-backgrounds-tile-wood",
                gui::ImageOpt::Default | gui::ImageOpt::Repeated),
            sfutil::ScreenRatioToPixelsHoriz(0.06f));

        settingsBoxUPtr_
            = std::make_unique<gui::BoxEntity>("SettingsStageBackground", BG_BOX_RECT, bgBoxbgInfo);

        return BG_BOX_RECT_INNER;
    }

    void SettingsStage::Setup_ResolutionChangeLabel(const sf::FloatRect & BG_BOX_INNER_RECT)
    {
        if (false == hasStageAlreadyBeenSetup_)
        {
            resLabelTextRegionUPtr_ = std::make_unique<gui::TextRegion>(
                "ResolutionChangeLabel", CreateLabelTextInfo("Change Resolution"), sf::FloatRect());

            EntityAdd(resLabelTextRegionUPtr_);
        }

        const auto LEFT { BG_BOX_INNER_RECT.left + sfutil::MapByRes(60.0f, 125.0f) };
        const auto TOP { BG_BOX_INNER_RECT.top };

        resLabelTextRegionUPtr_->Setup(
            CreateLabelTextInfo("Change Resolution"), sf::FloatRect(LEFT, TOP, 0.0f, 0.0f));
    }

    void SettingsStage::Setup_ResolutionChangeRadioButtonSet(const sf::FloatRect &)
    {
        /*if (false == hasStageAlreadyBeenSetup_)
        {
            resRadioButtonSetUPtr_ = std::make_unique<gui::RadioButtonSet_DisplayChange>(
                BG_BOX_INNER_RECT.left,
                BG_BOX_INNER_RECT.top + sfutil::MapByRes(20.0f, 50.0f),
                gui::RadioButtonSetCallbackHandlerPtr_t(this),
                misc::MakeNotNull(this));

            resRadioButtonSetUPtr_->SetEntityColors(gui::FocusColors(
                sf::Color(180, 180, 180),
                sf::Color(0, 0, 0, 60),
                sf::Color(180, 180, 180),
                sf::Color(0, 0, 0, 60)));

            EntityAdd(resRadioButtonSetUPtr_);
        }

        const auto LEFT { BG_BOX_INNER_RECT.left };

        const auto TOP { resLabelTextRegionUPtr_->GetEntityPos().y
                         + resLabelTextRegionUPtr_->GetEntityRegion().height };

        resRadioButtonSetUPtr_->SetEntityPos(LEFT, TOP);*/
    }

    void SettingsStage::Setup_AntiAliasLabel(const sf::FloatRect & BG_BOX_INNER_RECT)
    {
        if (false == hasStageAlreadyBeenSetup_)
        {
            aaLabelTextRegionUPtr_ = std::make_unique<gui::TextRegion>(
                "AntiAliasSettingLabel", CreateLabelTextInfo("Anti-\nAliasing"), sf::FloatRect());

            EntityAdd(aaLabelTextRegionUPtr_);
        }

        const auto LEFT { HorizPositionOfColumn(1, BG_BOX_INNER_RECT)
                          - (aaLabelTextRegionUPtr_->GetEntityRegion().width * 0.5f) };

        const auto TOP { BG_BOX_INNER_RECT.top };

        aaLabelTextRegionUPtr_->Setup(
            CreateLabelTextInfo("Anti-\nAliasing"), sf::FloatRect(LEFT, TOP, 0.0f, 0.0f));
    }

    void SettingsStage::Setup_AntiAliasRadioButtonSet(const sf::FloatRect &)
    {
        /*if (false == hasStageAlreadyBeenSetup_)
        {
            const gui::TextInfo TEXT_INFO(
                " ",
                gui::GuiFont::Default,
                gui::FontManager::Instance()->Size_Small());

            const std::vector<std::string> LABELS_VEC
                = { "0   ", "1   ", "2   ", "4   ", "8   ", "16  " };

            gui::BoxEntityInfo radioButtonSetBoxInfo;

            radioButtonSetBoxInfo.SetupColor(
                sf::Color::Transparent,
                sf::Color(0, 0, 0, 150),
                gui::Side::None,
                gui::Corner::TopLeft | gui::Corner::BottomRight);

            radioButtonSetBoxInfo.SetupBorder(true, 1.0f);

            radioButtonSetBoxInfo.focus_colors
                = gui::FocusColors(sf::Color(180, 180, 180));

            // establish which antialiasing level is current
            std::size_t aaInitialSelection(0);
            const unsigned ANTIALIAS_LEVEL(gui::Display::Instance()->AntialiasLevel());
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

            aaRadioButtonSetUPtr_ = std::make_unique<gui::RadioButtonSet>(
                gui::RadioButtonSetCallbackHandlerPtr_t(this),
                "Antialias",
                0.0f,
                0.0f,
                TEXT_INFO,
                LABELS_VEC,
                aaInitialSelection,
                gui::Brightness::Bright,
                std::vector<std::size_t>(),
                radioButtonSetBoxInfo);

            aaRadioButtonSetUPtr_->SetEntityColors(gui::FocusColors(
                sf::Color(180, 180, 180),
                sf::Color(0, 0, 0, 60),
                sf::Color(180, 180, 180),
                sf::Color(0, 0, 0, 60)));

            EntityAdd(aaRadioButtonSetUPtr_);
        }

        const auto LEFT { HorizPositionOfColumn(1, BG_BOX_INNER_RECT)
                          - (aaRadioButtonSetUPtr_->GetEntityRegion().width * 0.5f) };

        const auto TOP { aaLabelTextRegionUPtr_->GetEntityPos().y
                         + aaLabelTextRegionUPtr_->GetEntityRegion().height };

        aaRadioButtonSetUPtr_->SetEntityPos(LEFT, TOP);*/
    }

    void SettingsStage::Setup_MusicVolumeLabel(const sf::FloatRect & BG_BOX_INNER_RECT)
    {
        if (false == hasStageAlreadyBeenSetup_)
        {
            musicVolLabelTextRegionUPtr_ = std::make_unique<gui::TextRegion>(
                "MusicVolumeSettingLabel", CreateLabelTextInfo("Music\nVolume"), sf::FloatRect());

            EntityAdd(musicVolLabelTextRegionUPtr_);
        }

        const auto LEFT { HorizPositionOfColumn(2, BG_BOX_INNER_RECT)
                          - (musicVolLabelTextRegionUPtr_->GetEntityRegion().width * 0.5f) };

        const auto TOP { BG_BOX_INNER_RECT.top };

        musicVolLabelTextRegionUPtr_->Setup(
            CreateLabelTextInfo("Music\nVolume"), sf::FloatRect(LEFT, TOP, 0.0f, 0.0f));
    }

    void SettingsStage::Setup_MusicVolumeSlider(const sf::FloatRect & BG_BOX_INNER_RECT)
    {
        if (false == hasStageAlreadyBeenSetup_)
        {
            musicVolSliderBarUPtr_ = std::make_unique<gui::SliderBarMusic>(
                "MusicVolume",
                0.0f,
                0.0f,
                SLIDER_LENGTH_VERT_,
                gui::SliderStyle(),
                CreateSliderbarValueTextInfoSet(),
                0.5f,
                0.0f,
                SLIDER_LENGTH_VERT_ + SliderLabelVertPad());

            EntityAdd(musicVolSliderBarUPtr_);
        }

        const auto LEFT { HorizPositionOfColumn(2, BG_BOX_INNER_RECT)
                          - (musicVolSliderBarUPtr_->GetEntityRegion().width * 0.5f) };

        const auto TOP { musicVolLabelTextRegionUPtr_->GetEntityPos().y
                         + musicVolLabelTextRegionUPtr_->GetEntityRegion().height };

        musicVolSliderBarUPtr_->SetEntityPos(LEFT, TOP);
    }

    void SettingsStage::Setup_SfxVolumeLabel(const sf::FloatRect & BG_BOX_INNER_RECT)
    {
        if (false == hasStageAlreadyBeenSetup_)
        {
            effectsVolLabelTextRegionUPtr_ = std::make_unique<gui::TextRegion>(
                "EffectsVolumeSettingLabel",
                CreateLabelTextInfo("Sound Effects\nVolume"),
                sf::FloatRect(),
                gui::TextRegion::DEFAULT_NO_RESIZE_,
                gui::BoxEntityInfo());

            EntityAdd(effectsVolLabelTextRegionUPtr_);
        }

        const auto LEFT { HorizPositionOfColumn(3, BG_BOX_INNER_RECT)
                          - (effectsVolLabelTextRegionUPtr_->GetEntityRegion().width * 0.5f) };

        const auto TOP { BG_BOX_INNER_RECT.top };

        effectsVolLabelTextRegionUPtr_->Setup(
            CreateLabelTextInfo("Sound Effects\nVolume"), sf::FloatRect(LEFT, TOP, 0.0f, 0.0f));
    }

    void SettingsStage::Setup_SfxVolumeSlider(const sf::FloatRect & BG_BOX_INNER_RECT)
    {
        if (false == hasStageAlreadyBeenSetup_)
        {
            effectsVolSliderBarUPtr_ = std::make_unique<gui::SliderBarSfx>(
                "EffectsVolume",
                0.0f,
                0.0f,
                SLIDER_LENGTH_VERT_,
                gui::SliderStyle(),
                CreateSliderbarValueTextInfoSet(),
                0.5f,
                0.0f,
                SLIDER_LENGTH_VERT_ + SliderLabelVertPad());

            EntityAdd(effectsVolSliderBarUPtr_);
        }

        const auto LEFT { HorizPositionOfColumn(3, BG_BOX_INNER_RECT)
                          - (effectsVolSliderBarUPtr_->GetEntityRegion().width * 0.5f) };

        const auto TOP { effectsVolLabelTextRegionUPtr_->GetEntityPos().y
                         + effectsVolLabelTextRegionUPtr_->GetEntityRegion().height };

        effectsVolSliderBarUPtr_->SetEntityPos(LEFT, TOP);
    }

    void SettingsStage::Setup_MusicInfoLabel(const sf::FloatRect & BG_BOX_INNER_RECT)
    {
        if (false == hasStageAlreadyBeenSetup_)
        {
            musicInfoLabelTextRegionUPtr_ = std::make_unique<gui::TextRegion>(
                "MusicInfoLabel", CreateLabelTextInfo("Music Currently Playing"), sf::FloatRect());

            EntityAdd(musicInfoLabelTextRegionUPtr_);
        }

        // const auto PAD { 30.0f };

        const auto LEFT { sfutil::ScreenRatioToPixelsHoriz(0.5f) };
        // const auto LEFT { (aaRadioButtonSetUPtr_->GetEntityRegion().left - PAD)
        //                  - sfutil::MapByRes(10.0f, 30.0f) };

        const auto TOP { sfutil::ScreenRatioToPixelsVert(0.5f) };
        // const auto TOP { aaRadioButtonSetUPtr_->GetEntityRegion().top
        //                 + aaRadioButtonSetUPtr_->GetEntityRegion().height + PAD };

        const auto WIDTH { (BG_BOX_INNER_RECT.left + BG_BOX_INNER_RECT.width) - LEFT };

        musicInfoLabelTextRegionUPtr_->Setup(
            CreateLabelTextInfo("Music Currently Playing"), sf::FloatRect(LEFT, TOP, WIDTH, 0.0f));
    }

    void SettingsStage::Setup_MusicInfoBox(const sf::FloatRect & BG_BOX_INNER_RECT)
    {
        const auto PAD { sfutil::MapByRes(20.0f, 60.0f) };

        const auto LEFT { sfutil::ScreenRatioToPixelsHoriz(0.5f) };
        // const auto LEFT { aaRadioButtonSetUPtr_->GetEntityPos().x };

        const auto TOP { musicInfoLabelTextRegionUPtr_->GetEntityRegion().top
                         + musicInfoLabelTextRegionUPtr_->GetEntityRegion().height + PAD };

        const auto WIDTH { ((BG_BOX_INNER_RECT.left + BG_BOX_INNER_RECT.width) - LEFT) - PAD };

        const auto HEIGHT { (BG_BOX_INNER_RECT.top + BG_BOX_INNER_RECT.height) - TOP };

        const sf::FloatRect RECT(LEFT, TOP, WIDTH, HEIGHT);

        const auto MUSIC_INFO_VEC { gui::SoundManager::Instance()->MusicInfoSet() };

        std::ostringstream ss;
        for (const auto & MUSIC_INFO : MUSIC_INFO_VEC)
        {
            ss << "\"" << MUSIC_INFO.SongName() << "\"\nby " << MUSIC_INFO.ArtistName()
               << "\nLicense: " << MUSIC_INFO.LicenseTitle()
               << "\nDuration: " << MUSIC_INFO.Duration().ToString() << "\n\n";
        }

        if (ss.str().empty())
        {
            ss << " ";
        }

        const gui::TextInfo TEXT_INFO(
            ss.str(),
            gui::GuiFont::Default,
            gui::FontManager::Instance()->Size_Normal(),
            sfutil::color::Light,
            gui::Justified::Center);

        gui::BoxEntityInfo musicBoxInfo;

        musicBoxInfo.SetupColor(
            sf::Color::Transparent,
            sf::Color(0, 0, 0, 150),
            gui::Side::None,
            gui::Corner::TopLeft | gui::Corner::BottomRight);

        musicBoxInfo.SetupBorder(true, 1.0f);

        musicBoxInfo.focus_colors = gui::FocusColors(sf::Color(180, 180, 180));

        if (false == hasStageAlreadyBeenSetup_)
        {
            musicInfoDetailsTextRegionUPtr_ = std::make_unique<gui::TextRegion>(
                "SettingsStageMusicInfo",
                TEXT_INFO,
                RECT,
                misc::MakeNotNull(this),
                gui::TextRegion::DEFAULT_NO_RESIZE_,
                musicBoxInfo);

            EntityAdd(musicInfoDetailsTextRegionUPtr_);
        }

        musicInfoDetailsTextRegionUPtr_->Setup(
            TEXT_INFO, RECT, gui::TextRegion::DEFAULT_NO_RESIZE_, musicBoxInfo);
    }

    void SettingsStage::Setup_RevisionNumber(const sf::FloatRect & BG_BOX_INNER_RECT)
    {
        std::ostringstream ss;
        ss << "Revision: " << misc::ConfigFile::Instance()->Value("system-revision");

        const gui::TextInfo TEXT_INFO(
            ss.str(),
            gui::GuiFont::Default,
            gui::FontManager::Instance()->Size_Smallish(),
            sfutil::color::Light,
            gui::Justified::Left);

        if (false == hasStageAlreadyBeenSetup_)
        {
            revLabelTextRegionUPtr_
                = std::make_unique<gui::TextRegion>("RevisionNumber", TEXT_INFO, sf::FloatRect());

            EntityAdd(revLabelTextRegionUPtr_);
        }

        const auto LEFT { BG_BOX_INNER_RECT.left };

        const auto TOP { (
            (BG_BOX_INNER_RECT.top + BG_BOX_INNER_RECT.height)
            - revLabelTextRegionUPtr_->GetEntityRegion().height) };

        revLabelTextRegionUPtr_->Setup(TEXT_INFO, sf::FloatRect(LEFT, TOP, 0.0f, 0.0f));
    }

    const gui::TextInfo SettingsStage::CreateLabelTextInfo(const std::string & TITLE) const
    {
        return gui::TextInfo(
            TITLE,
            gui::GuiFont::SystemCondensed,
            gui::FontManager::Instance()->Size_Normal(),
            sf::Color(255, 255, 255, 200),
            gui::Justified::Center);
    }

    const gui::MouseTextInfo SettingsStage::CreateSliderbarValueTextInfoSet() const
    {
        return gui::MouseTextInfo(
            "",
            gui::GuiFont::Number,
            gui::FontManager::Instance()->Size_Small(),
            sf::Color::White,
            sf::Color(255, 200, 200));
    }

    float SettingsStage::HorizPositionOfColumn(
        const std::size_t COLUMN_NUM, const sf::FloatRect & BG_BOX_INNER_RECT)
    {
        const auto LEFT { sfutil::ScreenRatioToPixelsHoriz(0.5f) };

        // const auto LEFT { resRadioButtonSetUPtr_->GetEntityRegion().left
        //                  + resRadioButtonSetUPtr_->GetEntityRegion().width };

        const auto REMAINING_RIGHT_SPACE { (BG_BOX_INNER_RECT.left + BG_BOX_INNER_RECT.width)
                                           - LEFT };

        const auto COLUMN_WIDTH { REMAINING_RIGHT_SPACE / 4.0f };

        return LEFT + (static_cast<float>(COLUMN_NUM) * COLUMN_WIDTH);
    }

    float SettingsStage::SliderLabelVertPad() const { return sfutil::MapByRes(5.0f, 15.0f); }

} // namespace stage
} // namespace heroespath
