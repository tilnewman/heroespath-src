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
#ifndef HEROESPATH_POPUP_POPUPSTAGEMUSICSHEET_HPP_INCLUDED
#define HEROESPATH_POPUP_POPUPSTAGEMUSICSHEET_HPP_INCLUDED
//
// popup-stage-musicsheet.hpp
//
#include "popup/popup-stage-base.hpp"
#include "popup/popup-info.hpp"

#include "sfml-util/gui/background-info.hpp"
#include "sfml-util/gui/list-box.hpp"
#include "sfml-util/gui/list-box-item.hpp"
#include "sfml-util/color-shaker.hpp"
#include "sfml-util/sliders.hpp"
#include "sfml-util/color-slider.hpp"

#include <string>


namespace heroespath
{
namespace song
{
    class Song;
    using SongPtr_t = Song *;
    using SongPtrC_t = Song * const;
}

namespace popup
{

    //Responsible for implementing the MusicSheet popup stage
    class PopupStageMusicSheet
    :
        public PopupStageBase,
        public sfml_util::gui::callback::IListBoxCallbackHandler
    {
        PopupStageMusicSheet(const PopupStageMusicSheet &) =delete;
        PopupStageMusicSheet & operator=(const PopupStageMusicSheet &) =delete;

    public:
        explicit PopupStageMusicSheet(const popup::PopupInfo &);
        virtual ~PopupStageMusicSheet();

        inline const std::string HandlerName() const override
        {
            return PopupStageBase::HandlerName();
        }

        virtual bool HandleCallback(const sfml_util::gui::callback::ListBoxEventPackage &)override;

        using PopupStageBase::HandleCallback;

        virtual void Setup() override;
        virtual void Draw(sf::RenderTarget & target, const sf::RenderStates &) override;
        virtual void UpdateTime(const float ELAPSED_TIME_SECONDS) override;
        virtual bool KeyRelease(const sf::Event::KeyEvent &) override;

        virtual void SetupOuterAndInnerRegion() override;

    private:
        void SetupRegions();
        void SetupLeftAccentImage();
        void SetupRightAccentImage();
        void SetupPlayerImage();
        void SetupPlayerDetailsText();
        void SetupListboxLabelText();
        void SetupListbox();

        void SetupPageRightText(const song::SongPtrC_t);
        void SetPageRightColors(const sf::Color & IMAGE_COLOR, const sf::Color & TEXT_COLOR);
        bool DoesCharacterHaveEnoughManaToPlaySong(const song::SongPtrC_t) const;
        bool CanPlaySongInPhase(const song::SongPtrC_t) const;
        bool CanPlaySong(const song::SongPtrC_t) const;
        bool HandleSongPlay();

    private:
        static const float BACKGROUND_WIDTH_RATIO_;
        static const float COLOR_FADE_SPEED_;
        static const sf::Color UNABLE_TEXT_COLOR_;
        static const sf::Uint8 IMAGE_ALPHA_;
        static const float WARNING_DURATION_SEC_;

        song::SongPtr_t currentSongPtr_;
        sfml_util::gui::TextRegionUPtr_t charDetailsTextRegionUPtr_;
        sfml_util::gui::TextRegionUPtr_t listBoxLabelTextRegionUPtr_;
        sfml_util::gui::ListBoxUPtr_t listBoxUPtr_;
        sf::Texture playerTexture_;
        sf::Sprite playerSprite_;
        sf::FloatRect pageRectLeft_;
        sf::FloatRect pageRectRight_;
        sfml_util::gui::TextRegionUPtr_t titleTextRegionUPtr_;
        sfml_util::gui::TextRegionUPtr_t detailsTextUPtr_;
        sfml_util::gui::TextRegionUPtr_t unableTextUPtr_;
        sfml_util::gui::TextRegionUPtr_t descTextUPtr_;
        sf::Texture songTexture_;
        sf::Sprite songSprite_;
        sfml_util::ColorShaker warnColorShaker_;
        const sf::Color LISTBOX_IMAGE_COLOR_;
        const sf::Color LISTBOX_LINE_COLOR_;
        const sf::Color LISTBOX_COLOR_FG_;
        const sf::Color LISTBOX_COLOR_BG_;
        const sfml_util::gui::ColorSet LISTBOX_COLORSET_;
        sfml_util::gui::BackgroundInfo LISTBOX_BG_INFO_;
        sfml_util::gui::TextInfo listBoxItemTextInfo_;
        sfml_util::ColorSlider imageColorSlider_;
        sfml_util::ColorSlider textColorSlider_;
    };

}
}

#endif //HEROESPATH_POPUP_POPUPSTAGEMUSICSHEET_HPP_INCLUDED
