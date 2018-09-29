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

#include "game/game-data-file.hpp"
#include "game/loop-manager.hpp"
#include "log/log-macros.hpp"
#include "misc/real.hpp"
#include "sfml-util/box-entity.hpp"
#include "sfml-util/display.hpp"
#include "sfml-util/font-manager.hpp"
#include "sfml-util/gui-images.hpp"
#include "sfml-util/sfml-util-center.hpp"
#include "sfml-util/sfml-util-display.hpp"
#include "sfml-util/sfml-util-position.hpp"
#include "sfml-util/sound-manager.hpp"
#include "sfml-util/text-info.hpp"

#include <algorithm>
#include <sstream>
#include <vector>

namespace heroespath
{
namespace stage
{

    SettingsStage::SettingsStage()
        : Stage(
              "Settings",
              { sfml_util::GuiFont::Default,
                sfml_util::GuiFont::System,
                sfml_util::GuiFont::SystemCondensed,
                sfml_util::GuiFont::Number,
                sfml_util::GuiFont::Handwriting },
              false)
        , SLIDER_LENGTH_VERT_(160.0f)
        , hasStageAlreadyBeenSetup_(false)
        , prevAALevel_(sfml_util::Display::Instance()->AntialiasLevel())
        , screenBgBox_(
              "SettingsStage'sBackground",
              StageRegion(),
              sfml_util::BoxEntityInfo(sfml_util::CachedTexture(
                  "media-images-backgrounds-tile-darkknot",
                  sfml_util::ImageOpt::Default | sfml_util::ImageOpt::Repeated)))
        , stageTitle_("media-images-buttons-mainmenu-settings-normal")
        , bottomSymbol_()
        , backButtonUPtr_(std::make_unique<sfml_util::MainMenuButton>(
              sfml_util::LoopState::Previous,
              sfml_util::ImageTextEntity::Callback_t::IHandlerPtr_t(this),
              -1.0f))
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

    SettingsStage::~SettingsStage() { Stage::ClearAllEntities(); }

    /*bool SettingsStage::HandleCallback(
        const sfml_util::RadioButton::Callback_t::PacketPtr_t & PACKAGE_PTR)
    {
        if (PACKAGE.PTR_ == aaRadioButtonSetUPtr_.get())
        {
            prevAALevel_ = sfml_util::Display::Instance()->AntialiasLevel();

            auto const SELECT_SELECT { static_cast<unsigned>(
                std::pow(2.0f, static_cast<double>(PACKAGE.PTR_->GetSelectedNumber() - 1))) };

            auto const NEW_ANTIALIAS_LEVEL { (
                (PACKAGE.PTR_->GetSelectedNumber() == 0) ? 0 : SELECT_SELECT) };

            game::LoopManager::Instance()->ChangeResolution(
                this, this, sfml_util::Display::GetCurrentResolution(), NEW_ANTIALIAS_LEVEL);

            return true;
        }
        else
        {
            return false;
        }
    }*/

    bool SettingsStage::HandleCallback(const sfml_util::PopupCallback_t::PacketPtr_t &)
    {
        /*
        M_HP_LOG(
            GetStageName() << " HandlePopupCallback(response=\""
                           << popup::ResponseTypes::ToString(PACKET_PTR->Response()) << "\")");

        if (PACKET_PTR->Response() == popup::ResponseTypes::No)
        {
            M_HP_LOG(
                GetStageName() << " Stage.  User rejected the new antialias level.  "
                               << "Changing back to the previous.");

            game::LoopManager::Instance()->ChangeResolution(
                this, this, sfml_util::Display::Instance()->GetCurrentResolution(), prevAALevel_);

            HandleResolutionChange();
            return false;
        }
        else if (PACKET_PTR->Response() == popup::ResponseTypes::Okay)
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
        else*/
        {
            return true;
        }
    }

    void SettingsStage::Setup()
    {
        Setup_StageRegionToFullscreen();
        Setup_BackgroundImage();
        Setup_StageTitle();
        Setup_BottomSymbol();
        Setup_BackButton();

        auto const BG_BOX_INNER_RECT { Setup_WoodBackgroundBoxAndReturnInnerRect() };

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
        target.draw(screenBgBox_, STATES);
        target.draw(stageTitle_, STATES);
        target.draw(bottomSymbol_, STATES);
        target.draw(*settingsBoxUPtr_, STATES);
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

    void SettingsStage::Setup_BackgroundImage() { ; }

    void SettingsStage::Setup_StageTitle() { stageTitle_.SetSizeAndReCenter(); }

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
        backButtonUPtr_->SetEntityPos(
            sfml_util::ScreenRatioToPixelsHoriz(0.039f),
            sfml_util::CenterOfVert(bottomSymbol_.Region())
                - (backButtonUPtr_->GetEntityRegion().height * 0.5f));
    }

    const sf::FloatRect SettingsStage::Setup_WoodBackgroundBoxAndReturnInnerRect()
    {
        auto const BG_BOX_WIDTH { std::max(StageRegionWidth() * 0.45f, 1000.0f) };
        auto const BG_BOX_HEIGHT { std::max(StageRegionHeight() * 0.5f, 500.0f) };

        auto const BG_BOX_LEFT { (StageRegionWidth() * 0.5f) - (BG_BOX_WIDTH * 0.5f) };

        auto const BG_BOX_TOP { sfml_util::Bottom(stageTitle_.Region())
                                + stageTitle_.DefaultBottomPad() };

        const sf::FloatRect BG_BOX_RECT(BG_BOX_LEFT, BG_BOX_TOP, BG_BOX_WIDTH, BG_BOX_HEIGHT);

        auto const BG_BOX_INNER_PAD { sfml_util::MapByRes(20.0f, 60.0f) };

        const sf::FloatRect BG_BOX_RECT_INNER(
            BG_BOX_LEFT + BG_BOX_INNER_PAD,
            BG_BOX_TOP + BG_BOX_INNER_PAD,
            BG_BOX_WIDTH - (BG_BOX_INNER_PAD * 2.0f),
            BG_BOX_HEIGHT - (BG_BOX_INNER_PAD * 2.0f));

        sfml_util::BoxEntityInfo bgBoxbgInfo;

        bgBoxbgInfo.SetupBorder(true);

        bgBoxbgInfo.focus_colors = sfml_util::FocusColors(sf::Color::White);

        bgBoxbgInfo.SetupImage(
            sfml_util::CachedTexture(
                "media-images-backgrounds-tile-wood",
                sfml_util::ImageOpt::Default | sfml_util::ImageOpt::Repeated),
            sfml_util::ScreenRatioToPixelsHoriz(0.06f));

        settingsBoxUPtr_ = std::make_unique<sfml_util::BoxEntity>(
            "SettingsStageBackground", BG_BOX_RECT, bgBoxbgInfo);

        return BG_BOX_RECT_INNER;
    }

    void SettingsStage::Setup_ResolutionChangeLabel(const sf::FloatRect & BG_BOX_INNER_RECT)
    {
        if (false == hasStageAlreadyBeenSetup_)
        {
            resLabelTextRegionUPtr_ = std::make_unique<sfml_util::TextRegion>(
                "ResolutionChangeLabel", CreateLabelTextInfo("Change Resolution"), sf::FloatRect());

            EntityAdd(resLabelTextRegionUPtr_.get());
        }

        auto const LEFT { BG_BOX_INNER_RECT.left + sfml_util::MapByRes(60.0f, 125.0f) };
        auto const TOP { BG_BOX_INNER_RECT.top };

        resLabelTextRegionUPtr_->Setup(
            CreateLabelTextInfo("Change Resolution"), sf::FloatRect(LEFT, TOP, 0.0f, 0.0f));
    }

    void SettingsStage::Setup_ResolutionChangeRadioButtonSet(const sf::FloatRect &)
    {
        /*if (false == hasStageAlreadyBeenSetup_)
        {
            resRadioButtonSetUPtr_ = std::make_unique<sfml_util::RadioButtonSet_DisplayChange>(
                BG_BOX_INNER_RECT.left,
                BG_BOX_INNER_RECT.top + sfml_util::MapByRes(20.0f, 50.0f),
                sfml_util::RadioButtonSetCallbackHandlerPtr_t(this),
                sfml_util::IStagePtr_t(this));

            resRadioButtonSetUPtr_->SetEntityColors(sfml_util::FocusColors(
                sf::Color(180, 180, 180),
                sf::Color(0, 0, 0, 60),
                sf::Color(180, 180, 180),
                sf::Color(0, 0, 0, 60)));

            EntityAdd(resRadioButtonSetUPtr_.get());
        }

        auto const LEFT { BG_BOX_INNER_RECT.left };

        auto const TOP { resLabelTextRegionUPtr_->GetEntityPos().y
                         + resLabelTextRegionUPtr_->GetEntityRegion().height };

        resRadioButtonSetUPtr_->SetEntityPos(LEFT, TOP);*/
    }

    void SettingsStage::Setup_AntiAliasLabel(const sf::FloatRect & BG_BOX_INNER_RECT)
    {
        if (false == hasStageAlreadyBeenSetup_)
        {
            aaLabelTextRegionUPtr_ = std::make_unique<sfml_util::TextRegion>(
                "AntiAliasSettingLabel", CreateLabelTextInfo("Anti-\nAliasing"), sf::FloatRect());

            EntityAdd(aaLabelTextRegionUPtr_.get());
        }

        auto const LEFT { HorizPositionOfColumn(1, BG_BOX_INNER_RECT)
                          - (aaLabelTextRegionUPtr_->GetEntityRegion().width * 0.5f) };

        auto const TOP { BG_BOX_INNER_RECT.top };

        aaLabelTextRegionUPtr_->Setup(
            CreateLabelTextInfo("Anti-\nAliasing"), sf::FloatRect(LEFT, TOP, 0.0f, 0.0f));
    }

    void SettingsStage::Setup_AntiAliasRadioButtonSet(const sf::FloatRect &)
    {
        /*if (false == hasStageAlreadyBeenSetup_)
        {
            const sfml_util::TextInfo TEXT_INFO(
                " ",
                sfml_util::GuiFont::Default,
                sfml_util::FontManager::Instance()->Size_Small());

            const std::vector<std::string> LABELS_VEC
                = { "0   ", "1   ", "2   ", "4   ", "8   ", "16  " };

            sfml_util::BoxEntityInfo radioButtonSetBoxInfo;

            radioButtonSetBoxInfo.SetupColor(
                sf::Color::Transparent,
                sf::Color(0, 0, 0, 150),
                sfml_util::Side::None,
                sfml_util::Corner::TopLeft | sfml_util::Corner::BottomRight);

            radioButtonSetBoxInfo.SetupBorder(true, 1.0f);

            radioButtonSetBoxInfo.focus_colors
                = sfml_util::FocusColors(sf::Color(180, 180, 180));

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

            aaRadioButtonSetUPtr_ = std::make_unique<sfml_util::RadioButtonSet>(
                sfml_util::RadioButtonSetCallbackHandlerPtr_t(this),
                "Antialias",
                0.0f,
                0.0f,
                TEXT_INFO,
                LABELS_VEC,
                aaInitialSelection,
                sfml_util::Brightness::Bright,
                std::vector<std::size_t>(),
                radioButtonSetBoxInfo);

            aaRadioButtonSetUPtr_->SetEntityColors(sfml_util::FocusColors(
                sf::Color(180, 180, 180),
                sf::Color(0, 0, 0, 60),
                sf::Color(180, 180, 180),
                sf::Color(0, 0, 0, 60)));

            EntityAdd(aaRadioButtonSetUPtr_.get());
        }

        auto const LEFT { HorizPositionOfColumn(1, BG_BOX_INNER_RECT)
                          - (aaRadioButtonSetUPtr_->GetEntityRegion().width * 0.5f) };

        auto const TOP { aaLabelTextRegionUPtr_->GetEntityPos().y
                         + aaLabelTextRegionUPtr_->GetEntityRegion().height };

        aaRadioButtonSetUPtr_->SetEntityPos(LEFT, TOP);*/
    }

    void SettingsStage::Setup_MusicVolumeLabel(const sf::FloatRect & BG_BOX_INNER_RECT)
    {
        if (false == hasStageAlreadyBeenSetup_)
        {
            musicVolLabelTextRegionUPtr_ = std::make_unique<sfml_util::TextRegion>(
                "MusicVolumeSettingLabel", CreateLabelTextInfo("Music\nVolume"), sf::FloatRect());

            EntityAdd(musicVolLabelTextRegionUPtr_.get());
        }

        auto const LEFT { HorizPositionOfColumn(2, BG_BOX_INNER_RECT)
                          - (musicVolLabelTextRegionUPtr_->GetEntityRegion().width * 0.5f) };

        auto const TOP { BG_BOX_INNER_RECT.top };

        musicVolLabelTextRegionUPtr_->Setup(
            CreateLabelTextInfo("Music\nVolume"), sf::FloatRect(LEFT, TOP, 0.0f, 0.0f));
    }

    void SettingsStage::Setup_MusicVolumeSlider(const sf::FloatRect & BG_BOX_INNER_RECT)
    {
        if (false == hasStageAlreadyBeenSetup_)
        {
            musicVolSliderBarUPtr_ = std::make_unique<sfml_util::SliderBarMusic>(
                "MusicVolume",
                0.0f,
                0.0f,
                SLIDER_LENGTH_VERT_,
                sfml_util::SliderStyle(),
                CreateSliderbarValueTextInfoSet(),
                0.5f,
                0.0f,
                SLIDER_LENGTH_VERT_ + SliderLabelVertPad());

            EntityAdd(musicVolSliderBarUPtr_.get());
        }

        auto const LEFT { HorizPositionOfColumn(2, BG_BOX_INNER_RECT)
                          - (musicVolSliderBarUPtr_->GetEntityRegion().width * 0.5f) };

        auto const TOP { musicVolLabelTextRegionUPtr_->GetEntityPos().y
                         + musicVolLabelTextRegionUPtr_->GetEntityRegion().height };

        musicVolSliderBarUPtr_->SetEntityPos(LEFT, TOP);
    }

    void SettingsStage::Setup_SfxVolumeLabel(const sf::FloatRect & BG_BOX_INNER_RECT)
    {
        if (false == hasStageAlreadyBeenSetup_)
        {
            effectsVolLabelTextRegionUPtr_ = std::make_unique<sfml_util::TextRegion>(
                "EffectsVolumeSettingLabel",
                CreateLabelTextInfo("Sound Effects\nVolume"),
                sf::FloatRect(),
                sfml_util::TextRegion::DEFAULT_NO_RESIZE_,
                sfml_util::BoxEntityInfo());

            EntityAdd(effectsVolLabelTextRegionUPtr_.get());
        }

        auto const LEFT { HorizPositionOfColumn(3, BG_BOX_INNER_RECT)
                          - (effectsVolLabelTextRegionUPtr_->GetEntityRegion().width * 0.5f) };

        auto const TOP { BG_BOX_INNER_RECT.top };

        effectsVolLabelTextRegionUPtr_->Setup(
            CreateLabelTextInfo("Sound Effects\nVolume"), sf::FloatRect(LEFT, TOP, 0.0f, 0.0f));
    }

    void SettingsStage::Setup_SfxVolumeSlider(const sf::FloatRect & BG_BOX_INNER_RECT)
    {
        if (false == hasStageAlreadyBeenSetup_)
        {
            effectsVolSliderBarUPtr_ = std::make_unique<sfml_util::SliderBarSfx>(
                "EffectsVolume",
                0.0f,
                0.0f,
                SLIDER_LENGTH_VERT_,
                sfml_util::SliderStyle(),
                CreateSliderbarValueTextInfoSet(),
                0.5f,
                0.0f,
                SLIDER_LENGTH_VERT_ + SliderLabelVertPad());

            EntityAdd(effectsVolSliderBarUPtr_.get());
        }

        auto const LEFT { HorizPositionOfColumn(3, BG_BOX_INNER_RECT)
                          - (effectsVolSliderBarUPtr_->GetEntityRegion().width * 0.5f) };

        auto const TOP { effectsVolLabelTextRegionUPtr_->GetEntityPos().y
                         + effectsVolLabelTextRegionUPtr_->GetEntityRegion().height };

        effectsVolSliderBarUPtr_->SetEntityPos(LEFT, TOP);
    }

    void SettingsStage::Setup_MusicInfoLabel(const sf::FloatRect & BG_BOX_INNER_RECT)
    {
        if (false == hasStageAlreadyBeenSetup_)
        {
            musicInfoLabelTextRegionUPtr_ = std::make_unique<sfml_util::TextRegion>(
                "MusicInfoLabel", CreateLabelTextInfo("Music Currently Playing"), sf::FloatRect());

            EntityAdd(musicInfoLabelTextRegionUPtr_.get());
        }

        // auto const PAD { 30.0f };

        auto const LEFT { sfml_util::ScreenRatioToPixelsHoriz(0.5f) };
        // auto const LEFT { (aaRadioButtonSetUPtr_->GetEntityRegion().left - PAD)
        //                  - sfml_util::MapByRes(10.0f, 30.0f) };

        auto const TOP { sfml_util::ScreenRatioToPixelsVert(0.5f) };
        // auto const TOP { aaRadioButtonSetUPtr_->GetEntityRegion().top
        //                 + aaRadioButtonSetUPtr_->GetEntityRegion().height + PAD };

        auto const WIDTH { (BG_BOX_INNER_RECT.left + BG_BOX_INNER_RECT.width) - LEFT };

        musicInfoLabelTextRegionUPtr_->Setup(
            CreateLabelTextInfo("Music Currently Playing"), sf::FloatRect(LEFT, TOP, WIDTH, 0.0f));
    }

    void SettingsStage::Setup_MusicInfoBox(const sf::FloatRect & BG_BOX_INNER_RECT)
    {
        auto const PAD { sfml_util::MapByRes(20.0f, 60.0f) };

        auto const LEFT { sfml_util::ScreenRatioToPixelsHoriz(0.5f) };
        // auto const LEFT { aaRadioButtonSetUPtr_->GetEntityPos().x };

        auto const TOP { musicInfoLabelTextRegionUPtr_->GetEntityRegion().top
                         + musicInfoLabelTextRegionUPtr_->GetEntityRegion().height + PAD };

        auto const WIDTH { ((BG_BOX_INNER_RECT.left + BG_BOX_INNER_RECT.width) - LEFT) - PAD };

        auto const HEIGHT { (BG_BOX_INNER_RECT.top + BG_BOX_INNER_RECT.height) - TOP };

        const sf::FloatRect RECT(LEFT, TOP, WIDTH, HEIGHT);

        auto const MUSIC_INFO_VEC { sfml_util::SoundManager::Instance()->MusicInfoSet() };

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

        const sfml_util::TextInfo TEXT_INFO(
            ss.str(),
            sfml_util::GuiFont::Default,
            sfml_util::FontManager::Instance()->Size_Normal(),
            sfml_util::color::Light,
            sfml_util::Justified::Center);

        sfml_util::BoxEntityInfo musicBoxInfo;

        musicBoxInfo.SetupColor(
            sf::Color::Transparent,
            sf::Color(0, 0, 0, 150),
            sfml_util::Side::None,
            sfml_util::Corner::TopLeft | sfml_util::Corner::BottomRight);

        musicBoxInfo.SetupBorder(true, 1.0f);

        musicBoxInfo.focus_colors = sfml_util::FocusColors(sf::Color(180, 180, 180));

        if (false == hasStageAlreadyBeenSetup_)
        {
            musicInfoDetailsTextRegionUPtr_ = std::make_unique<sfml_util::TextRegion>(
                "SettingsStageMusicInfo",
                TEXT_INFO,
                RECT,
                this,
                sfml_util::TextRegion::DEFAULT_NO_RESIZE_,
                musicBoxInfo);

            EntityAdd(musicInfoDetailsTextRegionUPtr_.get());
        }

        musicInfoDetailsTextRegionUPtr_->Setup(
            TEXT_INFO, RECT, sfml_util::TextRegion::DEFAULT_NO_RESIZE_, musicBoxInfo);
    }

    void SettingsStage::Setup_RevisionNumber(const sf::FloatRect & BG_BOX_INNER_RECT)
    {
        std::ostringstream ss;
        ss << "Revision: " << game::GameDataFile::Instance()->GetCopyStr("system-revision");

        const sfml_util::TextInfo TEXT_INFO(
            ss.str(),
            sfml_util::GuiFont::Default,
            sfml_util::FontManager::Instance()->Size_Smallish(),
            sfml_util::color::Light,
            sfml_util::Justified::Left);

        if (false == hasStageAlreadyBeenSetup_)
        {
            revLabelTextRegionUPtr_ = std::make_unique<sfml_util::TextRegion>(
                "RevisionNumber", TEXT_INFO, sf::FloatRect());

            EntityAdd(revLabelTextRegionUPtr_.get());
        }

        auto const LEFT { BG_BOX_INNER_RECT.left };

        auto const TOP { (
            (BG_BOX_INNER_RECT.top + BG_BOX_INNER_RECT.height)
            - revLabelTextRegionUPtr_->GetEntityRegion().height) };

        revLabelTextRegionUPtr_->Setup(TEXT_INFO, sf::FloatRect(LEFT, TOP, 0.0f, 0.0f));
    }

    const sfml_util::TextInfo SettingsStage::CreateLabelTextInfo(const std::string & TITLE) const
    {
        return sfml_util::TextInfo(
            TITLE,
            sfml_util::GuiFont::SystemCondensed,
            sfml_util::FontManager::Instance()->Size_Normal(),
            sf::Color(255, 255, 255, 200),
            sfml_util::Justified::Center);
    }

    const sfml_util::MouseTextInfo SettingsStage::CreateSliderbarValueTextInfoSet() const
    {
        return sfml_util::MouseTextInfo(
            "",
            sfml_util::GuiFont::Number,
            sfml_util::FontManager::Instance()->Size_Small(),
            sf::Color::White,
            sf::Color(255, 200, 200));
    }

    float SettingsStage::HorizPositionOfColumn(
        const std::size_t COLUMN_NUM, const sf::FloatRect & BG_BOX_INNER_RECT)
    {
        auto const LEFT { sfml_util::ScreenRatioToPixelsHoriz(0.5f) };

        // auto const LEFT { resRadioButtonSetUPtr_->GetEntityRegion().left
        //                  + resRadioButtonSetUPtr_->GetEntityRegion().width };

        auto const REMAINING_RIGHT_SPACE { (BG_BOX_INNER_RECT.left + BG_BOX_INNER_RECT.width)
                                           - LEFT };

        auto const COLUMN_WIDTH { REMAINING_RIGHT_SPACE / 4.0f };

        return LEFT + (static_cast<float>(COLUMN_NUM) * COLUMN_WIDTH);
    }

    float SettingsStage::SliderLabelVertPad() const { return sfml_util::MapByRes(5.0f, 15.0f); }

} // namespace stage
} // namespace heroespath
