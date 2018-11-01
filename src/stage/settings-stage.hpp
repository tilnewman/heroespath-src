// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SETTINGSSTAGE_HPP_INCLUDED
#define HEROESPATH_SETTINGSSTAGE_HPP_INCLUDED
//
// settings-stage.hpp
//  A Stage class that allows changing system and game settings.
//
#include "sfml-util/box-entity.hpp"
#include "sfml-util/cached-texture.hpp"
#include "sfml-util/callback.hpp"
#include "sfml-util/display.hpp"
#include "sfml-util/horiz-symbol.hpp"
#include "sfml-util/main-menu-buttons.hpp"
#include "sfml-util/radio-or-check-set.hpp"
#include "sfml-util/sliderbar-music.hpp"
#include "sfml-util/sliderbar-sfx.hpp"
#include "sfml-util/stage-title.hpp"
#include "sfml-util/text-region.hpp"
#include "stage/stage-base.hpp"

#include <memory>
#include <string>

namespace heroespath
{
namespace stage
{

    // A simple class that displays the system and game settings
    class SettingsStage
        : public stage::StageBase

        , public gui::PopupCallback_t::IHandler_t
        //, public gui::RadioButtonSet::Callback_t::IHandler_t
        , public gui::ImageTextEntity::Callback_t::IHandler_t
    {
    public:
        SettingsStage(const SettingsStage &) = delete;
        SettingsStage(SettingsStage &&) = delete;
        SettingsStage & operator=(const SettingsStage &) = delete;
        SettingsStage & operator=(SettingsStage &&) = delete;

        SettingsStage();
        virtual ~SettingsStage();

        // bool HandleCallback(const gui::RadioButton::Callback_t::PacketPtr_t &)
        // override;

        bool HandleCallback(const gui::PopupCallback_t::PacketPtr_t &) override;

        bool HandleCallback(const gui::ImageTextEntity::Callback_t::PacketPtr_t &) override
        {
            return false;
        }

        void Setup() override;
        void Draw(sf::RenderTarget & target, const sf::RenderStates &) override;
        void HandleResolutionChange() override;
        bool KeyRelease(const sf::Event::KeyEvent &) override;

    private:
        void Setup_StageRegionToFullscreen();
        void Setup_BackgroundImage();
        void Setup_StageTitle();
        void Setup_BottomSymbol();
        void Setup_BackButton();
        const sf::FloatRect Setup_WoodBackgroundBoxAndReturnInnerRect();
        void Setup_ResolutionChangeLabel(const sf::FloatRect & BG_BOX_INNER_RECT);
        void Setup_ResolutionChangeRadioButtonSet(const sf::FloatRect & BG_BOX_INNER_RECT);
        void Setup_AntiAliasLabel(const sf::FloatRect & BG_BOX_INNER_RECT);
        void Setup_AntiAliasRadioButtonSet(const sf::FloatRect & BG_BOX_INNER_RECT);
        void Setup_MusicVolumeLabel(const sf::FloatRect & BG_BOX_INNER_RECT);
        void Setup_MusicVolumeSlider(const sf::FloatRect & BG_BOX_INNER_RECT);
        void Setup_SfxVolumeLabel(const sf::FloatRect & BG_BOX_INNER_RECT);
        void Setup_SfxVolumeSlider(const sf::FloatRect & BG_BOX_INNER_RECT);
        void Setup_MusicInfoLabel(const sf::FloatRect & BG_BOX_INNER_RECT);
        void Setup_MusicInfoBox(const sf::FloatRect & BG_BOX_INNER_RECT);
        void Setup_RevisionNumber(const sf::FloatRect & BG_BOX_INNER_RECT);

        const gui::TextInfo CreateLabelTextInfo(const std::string & TITLE) const;

        const gui::MouseTextInfo CreateSliderbarValueTextInfoSet() const;

        float HorizPositionOfColumn(
            const std::size_t COLUMN_NUM, const sf::FloatRect & BG_BOX_INNER_RECT);

        float SliderLabelVertPad() const;

    private:
        const float SLIDER_LENGTH_VERT_;
        bool hasStageAlreadyBeenSetup_;
        unsigned prevAALevel_;
        gui::BoxEntity screenBgBox_;
        gui::StageTitle stageTitle_;
        gui::BottomSymbol bottomSymbol_;
        gui::MainMenuButtonUPtr_t backButtonUPtr_;
        gui::BoxEntityUPtr_t settingsBoxUPtr_;
        gui::TextRegionUPtr_t resLabelTextRegionUPtr_;
        // gui::RadioButtonSetUPtr_t resRadioButtonSetUPtr_;
        gui::TextRegionUPtr_t aaLabelTextRegionUPtr_;
        // gui::RadioButtonSetUPtr_t aaRadioButtonSetUPtr_;
        gui::TextRegionUPtr_t musicVolLabelTextRegionUPtr_;
        gui::SliderBarMusicUPtr_t musicVolSliderBarUPtr_;
        gui::TextRegionUPtr_t effectsVolLabelTextRegionUPtr_;
        gui::SliderBarSfxUPtr_t effectsVolSliderBarUPtr_;
        gui::TextRegionUPtr_t musicInfoLabelTextRegionUPtr_;
        gui::TextRegionUPtr_t musicInfoDetailsTextRegionUPtr_;
        gui::TextRegionUPtr_t revLabelTextRegionUPtr_;
    };

} // namespace stage
} // namespace heroespath

#endif // HEROESPATH_SETTINGSSTAGE_HPP_INCLUDED
