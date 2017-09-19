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
#ifndef SFMLUTIL_POPUPSTAGEMUSICSHEET_HPP_INCLUDED
#define SFMLUTIL_POPUPSTAGEMUSICSHEET_HPP_INCLUDED
//
// popup-stage-musicsheet.hpp
//
#include "sfml-util/popup-stage-base.hpp"

#include "game/popup-info.hpp"

#include "sfml-util/gui/background-info.hpp"
#include "sfml-util/gui/list-box.hpp"
#include "sfml-util/gui/list-box-item.hpp"
#include "sfml-util/color-shaker.hpp"
#include "sfml-util/sliders.hpp"


namespace game
{
namespace song
{
    class Song;
    using SongPtr_t = Song *;
    using SongPtrC_t = Song * const;
}
}
namespace sfml_util
{

    //Responsible for implementing the MusicSheet popup stage
    class PopupStageMusicSheet
        :
        public PopupStageBase,
        public gui::callback::IListBoxCallbackHandler
    {
        //prevent copy construction
        PopupStageMusicSheet(const PopupStageMusicSheet &) =delete;

        //prevent copy assignment
        PopupStageMusicSheet & operator=(const PopupStageMusicSheet &) =delete;

        //defines what is happening on the spellbook popup
        enum class FadeState
        {
            Initial = 0,
            Waiting,
            FadingOut,
            FadingIn,
            Warning,
            Count
        };

    public:
        explicit PopupStageMusicSheet(const game::PopupInfo &);
        virtual ~PopupStageMusicSheet();

        inline virtual const std::string HandlerName() const override
        {
            return PopupStageBase::HandlerName();
        }

        bool HandleCallback(const gui::callback::ListBoxEventPackage &) override;
        
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

        void SetupPageRightText(const game::song::SongPtrC_t);
        void MovePageRightColors(const float ELAPSED_TIME_SECONDS);
        void SetupPageRightForFadeIn();
        void SetPageRightColors();
        bool DoesCharacterHaveEnoughManaToPlaySong(const game::song::SongPtrC_t) const;
        bool CanPlaySongInPhase(const game::song::SongPtrC_t) const;
        bool CanPlaySong(const game::song::SongPtrC_t) const;
        bool HandleSongPlay();

    private:
        static const float BACKGROUND_WIDTH_RATIO_;
        static const float COLOR_FADE_SPEED_;
        static const sf::Color UNABLE_TEXT_COLOR_;
        static const sf::Uint8 IMAGE_ALPHA_;
        static const float WARNING_DURATION_SEC_;

        game::song::SongPtr_t currentSongPtr_;
        gui::TextRegionUPtr_t charDetailsTextRegionUPtr_;
        gui::TextRegionUPtr_t listBoxLabelTextRegionUPtr_;
        gui::ListBoxSPtr_t listBoxSPtr_;
        FadeState fadeState_;
        sf::Texture playerTexture_;
        sf::Sprite playerSprite_;
        sf::FloatRect pageRectLeft_;
        sf::FloatRect pageRectRight_;
        sf::Color imageColorCurrent_;
        sf::Color imageColorBegin_;
        sf::Color imageColorEnd_;
        sf::Color textColorCurrent_;
        sf::Color textColorBegin_;
        sf::Color textColorEnd_;
        sliders::ZeroSliderOnce<float> colorSlider_;
        gui::TextRegionUPtr_t songTitleTextRegionUPtr_;
        gui::TextRegionUPtr_t songDetailsTextUPtr_;
        gui::TextRegionUPtr_t songUnableTextUPtr_;
        gui::TextRegionUPtr_t songDescTextUPtr_;
        sf::Texture songTexture_;
        sf::Sprite songSprite_;
        bool willShowUnableText_;
        float warningTimerSec_;
        ColorShaker warnColorShaker_;
        const sf::Color LISTBOX_IMAGE_COLOR_;
        const sf::Color LISTBOX_LINE_COLOR_;
        const sf::Color LISTBOX_COLOR_FG_;
        const sf::Color LISTBOX_COLOR_BG_;
        const gui::ColorSet LISTBOX_COLORSET_;
        gui::BackgroundInfo LISTBOX_BG_INFO_;
        gui::TextInfo listBoxItemTextInfo_;
    };

}

#endif //SFMLUTIL_POPUPSTAGEMUSICSHEET_HPP_INCLUDED
