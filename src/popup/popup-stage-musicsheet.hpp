// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_POPUP_POPUPSTAGEMUSICSHEET_HPP_INCLUDED
#define HEROESPATH_POPUP_POPUPSTAGEMUSICSHEET_HPP_INCLUDED
//
// popup-stage-musicsheet.hpp
//
#include "gui/box-entity-info.hpp"
#include "gui/cached-texture.hpp"
#include "gui/list-box.hpp"
#include "gui/list-element.hpp"
#include "gui/slider-color.hpp"
#include "gui/sliders.hpp"
#include "misc/not-null.hpp"
#include "popup/popup-info.hpp"
#include "popup/popup-stage-base.hpp"

#include <string>

namespace heroespath
{
namespace song
{
    class Song;
    using SongPtr_t = misc::NotNull<Song *>;
} // namespace song

namespace popup
{

    // Responsible for implementing the MusicSheet popup stage
    class PopupStageMusicSheet
        : public PopupStageBase
        , public gui::ListBox<PopupStageMusicSheet, song::SongPtr_t>::Callback_t::IHandler_t
    {
    public:
        using SongListBox_t = gui::ListBox<PopupStageMusicSheet, song::SongPtr_t>;

        PopupStageMusicSheet(const PopupStageMusicSheet &) = delete;
        PopupStageMusicSheet(PopupStageMusicSheet &&) = delete;
        PopupStageMusicSheet & operator=(const PopupStageMusicSheet &) = delete;
        PopupStageMusicSheet & operator=(PopupStageMusicSheet &&) = delete;

        explicit PopupStageMusicSheet(const popup::PopupInfo &);

        virtual ~PopupStageMusicSheet();

        bool HandleCallback(const SongListBox_t::Callback_t::PacketPtr_t &) override;

        using PopupStageBase::HandleCallback;

        void Setup() override;
        void Draw(sf::RenderTarget & target, const sf::RenderStates &) override;
        void UpdateTime(const float ELAPSED_TIME_SECONDS) override;
        bool KeyRelease(const sf::Event::KeyEvent &) override;

        void SetupOuterAndInnerRegion() override;

    private:
        void SetupRegions();
        void SetupLeftAccentImage();
        void SetupRightAccentImage();
        void SetupPlayerImage();
        void SetupPlayerDetailsText();
        void SetupListboxLabelText();
        void SetupListbox();

        void SetupPageRightText(const song::SongPtr_t);
        void SetPageRightColors(const sf::Color & IMAGE_COLOR, const sf::Color & TEXT_COLOR);
        bool DoesCharacterHaveEnoughManaToPlaySong(const song::SongPtr_t) const;
        bool CanPlaySongInPhase(const song::SongPtr_t) const;
        bool CanPlaySong(const song::SongPtr_t) const;
        bool HandleSongPlay();
        const song::SongPtr_t CurrentSelectedSong() const;

    private:
        static const float BORDER_SCREEN_RATIO_;
        static const float COLOR_FADE_SPEED_;
        static const sf::Color UNABLE_TEXT_COLOR_;
        static const sf::Uint8 IMAGE_ALPHA_;
        static const float WARNING_DURATION_SEC_;

        const float PLAYER_IMAGE_HEIGHT_;
        gui::TextRegionUPtr_t charDetailsTextRegionUPtr_;
        gui::TextRegionUPtr_t listBoxLabelTextRegionUPtr_;
        gui::ListBoxUPtr_t<PopupStageMusicSheet, song::SongPtr_t> listBoxUPtr_;
        gui::CachedTexture playerCachedTexture_;
        sf::Sprite playerSprite_;
        sf::FloatRect pageRectLeft_;
        sf::FloatRect pageRectRight_;
        gui::TextRegionUPtr_t titleTextRegionUPtr_;
        gui::TextRegionUPtr_t detailsTextUPtr_;
        gui::TextRegionUPtr_t unableTextUPtr_;
        gui::TextRegionUPtr_t descTextUPtr_;
        gui::CachedTextureOpt_t songCachedTextureOpt_;
        sf::Sprite songSprite_;
        const sf::Color LISTBOX_IMAGE_COLOR_;
        const sf::Color LISTBOX_LINE_COLOR_;
        const sf::Color LISTBOX_COLOR_FG_;
        const sf::Color LISTBOX_COLOR_BG_;
        const gui::FocusColors LISTBOX_COLORSET_;
        gui::BoxEntityInfo listBoxBackgroundInfo_;
        gui::TextInfo listElementTextInfo_;
        std::size_t currentSongIndex_;
        gui::ColorSlider imageColorSlider_;
        gui::ColorSlider textColorSlider_;
        gui::ColorSlider warnTextColorSlider_;
    };

} // namespace popup
} // namespace heroespath

#endif // HEROESPATH_POPUP_POPUPSTAGEMUSICSHEET_HPP_INCLUDED
