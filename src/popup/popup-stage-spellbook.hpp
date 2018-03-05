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
#ifndef HEROESPATH_POPUP_POPUPSTAGESPELLBOOK_HPP_INCLUDED
#define HEROESPATH_POPUP_POPUPSTAGESPELLBOOK_HPP_INCLUDED
//
// popup-stage-spellbook.hpp
//
#include "popup/popup-stage-base.hpp"

#include "popup/popup-info.hpp"

#include "sfml-util/color-shaker.hpp"
#include "sfml-util/color-slider.hpp"
#include "sfml-util/gui/background-info.hpp"
#include "sfml-util/gui/list-box-item.hpp"
#include "sfml-util/gui/list-box.hpp"
#include "sfml-util/sliders.hpp"

#include <string>

namespace heroespath
{
namespace spell
{
    class Spell;
    using SpellPtr_t = Spell *;
    using SpellPtrC_t = Spell * const;
} // namespace spell

namespace popup
{

    class PopupStageSpellbook
        : public PopupStageBase
        , public sfml_util::gui::callback::IListBoxCallbackHandler
    {
    public:
        PopupStageSpellbook(const PopupStageSpellbook &) = delete;
        PopupStageSpellbook(const PopupStageSpellbook &&) = delete;
        PopupStageSpellbook & operator=(const PopupStageSpellbook &) = delete;
        PopupStageSpellbook & operator=(const PopupStageSpellbook &&) = delete;

        // defines what is happening on the spellbook popup
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
        explicit PopupStageSpellbook(const PopupInfo &);

        virtual ~PopupStageSpellbook();

        inline virtual const std::string HandlerName() const override
        {
            return PopupStageBase::HandlerName();
        }

        using PopupStageBase::HandleCallback;
        virtual bool HandleCallback(const sfml_util::gui::callback::ListBoxEventPackage &) override;

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

        void SetPageRightColors(const sf::Color & IMAGE_COLOR, const sf::Color & TEXT_COLOR);
        void SetupPageRightText(const spell::SpellPtrC_t);
        bool DoesCharacterHaveEnoughManaToCastSpell(const spell::SpellPtrC_t) const;
        bool CanCastSpellInPhase(const spell::SpellPtrC_t) const;
        bool CanCastSpell(const spell::SpellPtrC_t) const;
        bool HandleSpellCast();

    private:
        static const float BACKGROUND_WIDTH_RATIO_;
        static const float COLOR_FADE_SPEED_;
        static const sf::Uint8 SPELL_IMAGE_ALPHA_;
        static const sf::Color UNABLE_TEXT_COLOR_;
        static const float WARNING_DURATION_SEC_;

        sf::Texture playerTexture_;
        sf::Sprite playerSprite_;
        sf::FloatRect pageRectLeft_;
        sf::FloatRect pageRectRight_;
        sfml_util::gui::TextRegionUPtr_t charDetailsTextRegionUPtr_;
        sfml_util::gui::TextRegionUPtr_t listBoxLabelTextRegionUPtr_;
        sfml_util::gui::ListBoxUPtr_t listBoxUPtr_;
        const sf::Color LISTBOX_IMAGE_COLOR_;
        const sf::Color LISTBOX_LINE_COLOR_;
        const sf::Color LISTBOX_COLOR_FG_;
        const sf::Color LISTBOX_COLOR_BG_;
        const sfml_util::gui::ColorSet LISTBOX_COLORSET_;
        sfml_util::gui::BackgroundInfo LISTBOX_BG_INFO_;
        sfml_util::gui::TextInfo listBoxItemTextInfo_;
        sf::Texture spellTexture_;
        sf::Sprite spellSprite_;
        sfml_util::gui::TextRegionUPtr_t spellTitleTextRegionUPtr_;
        sfml_util::gui::TextRegionUPtr_t spellDetailsTextUPtr_;
        sfml_util::gui::TextRegionUPtr_t unableTextUPtr_;
        sfml_util::gui::TextRegionUPtr_t spellDescTextUPtr_;
        spell::SpellPtr_t currentSpellPtr_;
        sfml_util::ColorShaker warnColorShaker_;
        sfml_util::ColorSlider imageColorSlider_;
        sfml_util::ColorSlider textColorSlider_;
    };
} // namespace popup
} // namespace heroespath

#endif // HEROESPATH_POPUP_POPUPSTAGESPELLBOOK_HPP_INCLUDED
