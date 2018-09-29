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
#include "sfml-util/cached-texture.hpp"
#include "sfml-util/display.hpp"
#include "sfml-util/gui/box-entity.hpp"
#include "sfml-util/gui/callback.hpp"
#include "sfml-util/gui/main-menu-buttons.hpp"
#include "sfml-util/gui/radio-or-check-set.hpp"
#include "sfml-util/gui/sliderbar-music.hpp"
#include "sfml-util/gui/sliderbar-sfx.hpp"
#include "sfml-util/gui/text-region.hpp"
#include "sfml-util/horiz-symbol.hpp"
#include "sfml-util/stage-title.hpp"
#include "sfml-util/stage.hpp"

#include <memory>
#include <string>

namespace heroespath
{
namespace stage
{

    // A simple class that displays the system and game settings
    class SettingsStage
        : public sfml_util::Stage
        , public sfml_util::PopupCallback_t::IHandler_t
        //, public sfml_util::RadioButtonSet::Callback_t::IHandler_t
        , public sfml_util::ImageTextEntity::Callback_t::IHandler_t
    {
    public:
        SettingsStage(const SettingsStage &) = delete;
        SettingsStage(SettingsStage &&) = delete;
        SettingsStage & operator=(const SettingsStage &) = delete;
        SettingsStage & operator=(SettingsStage &&) = delete;

        SettingsStage();
        virtual ~SettingsStage();

        // bool HandleCallback(const sfml_util::RadioButton::Callback_t::PacketPtr_t &)
        // override;

        bool HandleCallback(const sfml_util::PopupCallback_t::PacketPtr_t &) override;

        bool HandleCallback(const sfml_util::ImageTextEntity::Callback_t::PacketPtr_t &) override
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

        const sfml_util::TextInfo CreateLabelTextInfo(const std::string & TITLE) const;

        const sfml_util::MouseTextInfo CreateSliderbarValueTextInfoSet() const;

        float HorizPositionOfColumn(
            const std::size_t COLUMN_NUM, const sf::FloatRect & BG_BOX_INNER_RECT);

        float SliderLabelVertPad() const;

    private:
        const float SLIDER_LENGTH_VERT_;
        bool hasStageAlreadyBeenSetup_;
        unsigned prevAALevel_;
        sfml_util::BoxEntity screenBgBox_;
        sfml_util::StageTitle stageTitle_;
        sfml_util::BottomSymbol bottomSymbol_;
        sfml_util::MainMenuButtonUPtr_t backButtonUPtr_;
        sfml_util::BoxEntityUPtr_t settingsBoxUPtr_;
        sfml_util::TextRegionUPtr_t resLabelTextRegionUPtr_;
        // sfml_util::RadioButtonSetUPtr_t resRadioButtonSetUPtr_;
        sfml_util::TextRegionUPtr_t aaLabelTextRegionUPtr_;
        // sfml_util::RadioButtonSetUPtr_t aaRadioButtonSetUPtr_;
        sfml_util::TextRegionUPtr_t musicVolLabelTextRegionUPtr_;
        sfml_util::SliderBarMusicUPtr_t musicVolSliderBarUPtr_;
        sfml_util::TextRegionUPtr_t effectsVolLabelTextRegionUPtr_;
        sfml_util::SliderBarSfxUPtr_t effectsVolSliderBarUPtr_;
        sfml_util::TextRegionUPtr_t musicInfoLabelTextRegionUPtr_;
        sfml_util::TextRegionUPtr_t musicInfoDetailsTextRegionUPtr_;
        sfml_util::TextRegionUPtr_t revLabelTextRegionUPtr_;
    };

} // namespace stage
} // namespace heroespath

#endif // HEROESPATH_SETTINGSSTAGE_HPP_INCLUDED
