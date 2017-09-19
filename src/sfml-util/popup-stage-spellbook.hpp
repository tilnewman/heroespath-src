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
#ifndef SFMLUTIL_POPUPSTAGESPELLBOOK_HPP_INCLUDED
#define SFMLUTIL_POPUPSTAGESPELLBOOK_HPP_INCLUDED
//
// popup-stage-spellbook.hpp
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
namespace spell
{
    class Spell;
    using SpellPtr_t = Spell *;
    using SpellPtrC_t = Spell * const;
}
}
namespace sfml_util
{

    class PopupStageSpellbook
    :
        public PopupStageBase,
        public gui::callback::IListBoxCallbackHandler
    {
        //prevent copy construction
        PopupStageSpellbook(const PopupStageSpellbook &) = delete;

        //prevent copy assignment
        PopupStageSpellbook & operator=(const PopupStageSpellbook &) = delete;

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
        explicit PopupStageSpellbook(const game::PopupInfo &);

        virtual ~PopupStageSpellbook();

        inline virtual const std::string HandlerName() const override
        {
            return PopupStageBase::HandlerName();
        }

        using PopupStageBase::HandleCallback;
        bool HandleCallback(const gui::callback::ListBoxEventPackage &) override;

        virtual void Setup() override;
        virtual void Draw(sf::RenderTarget & target, const sf::RenderStates &) override;
        virtual void UpdateTime(const float ELAPSED_TIME_SECONDS) override;
        virtual bool KeyRelease(const sf::Event::KeyEvent &) override;
        
        virtual void SetupOuterAndInnerRegion() override;

        void SetupRegions();
        void SetupLeftAccentImage();
        void SetupRightAccentImage();
        void SetupPlayerImage();
        void SetupPlayerDetailsText();
        void SetupSpellListboxLabel();
        void SetupSpellListbox();

        void SetPageRightColors();
        void SetupPageRightForFadeIn();
        void AdjustPageRightColors(const float ELAPSED_TIME_SECONDS);
        void SetupPageRightText(const game::spell::SpellPtrC_t);
        bool DoesCharacterHaveEnoughManaToCastSpell(const game::spell::SpellPtrC_t) const;
        bool CanCastSpellInPhase(const game::spell::SpellPtrC_t) const;
        bool CanCastSpell(const game::spell::SpellPtrC_t) const;
        bool HandleSpellCast();

    private:
        static const float     BACKGROUND_WIDTH_RATIO_;
        static const float     COLOR_FADE_SPEED_;
        static const sf::Uint8 SPELL_IMAGE_ALPHA_;
        static const sf::Color UNABLE_TEXT_COLOR_;
        static const float     WARNING_DURATION_SEC_;

        FadeState               fadeState_;
        sf::Texture             playerTexture_;
        sf::Sprite              playerSprite_;
        sf::FloatRect           pageRectLeft_;
        sf::FloatRect           pageRectRight_;
        gui::TextRegionUPtr_t   charDetailsTextRegionUPtr_;
        gui::TextRegionUPtr_t   listBoxLabelTextRegionUPtr_;
        gui::ListBoxSPtr_t      listBoxSPtr_;
        const sf::Color         LISTBOX_IMAGE_COLOR_;
        const sf::Color         LISTBOX_LINE_COLOR_;
        const sf::Color         LISTBOX_COLOR_FG_;
        const sf::Color         LISTBOX_COLOR_BG_;
        const gui::ColorSet     LISTBOX_COLORSET_;
        gui::BackgroundInfo     LISTBOX_BG_INFO_;
        gui::TextInfo           listBoxItemTextInfo_;
        sf::Texture             spellTexture_;
        sf::Sprite              spellSprite_;
        gui::TextRegionUPtr_t   spellTitleTextRegionUPtr_;
        gui::TextRegionUPtr_t   spellDetailsTextUPtr_;
        gui::TextRegionUPtr_t   unableTextUPtr_;
        gui::TextRegionUPtr_t   spellDescTextUPtr_;
        game::spell::SpellPtr_t currentSpellPtr_;
        sf::Color               imageColorCurrent_;
        sf::Color               imageColorBegin_;
        sf::Color               imageColorEnd_;
        sf::Color               textColorCurrent_;
        sf::Color               textColorBegin_;
        sf::Color               textColorEnd_;
        bool                    unableTextWillShow_;
        float                   warningTimerSec_;
        ColorShaker             warnColorShaker_;
        sliders::ZeroSliderOnce<float> colorSlider_;
    };

}

#endif //SFMLUTIL_POPUPSTAGESPELLBOOK_HPP_INCLUDED
