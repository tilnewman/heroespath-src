// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_POPUP_POPUPSTAGESPELLBOOK_HPP_INCLUDED
#define HEROESPATH_POPUP_POPUPSTAGESPELLBOOK_HPP_INCLUDED
//
// popup-stage-spellbook.hpp
//
#include "misc/not-null.hpp"
#include "popup/popup-info.hpp"
#include "popup/popup-stage-base.hpp"
#include "sfml-util/cached-texture.hpp"
#include "sfml-util/color-shaker.hpp"
#include "sfml-util/color-slider.hpp"
#include "sfml-util/gui/background-info.hpp"
#include "sfml-util/gui/list-box.hpp"
#include "sfml-util/sliders.hpp"

#include <string>

namespace heroespath
{
namespace spell
{
    class Spell;
    using SpellPtr_t = misc::NotNull<Spell *>;
} // namespace spell
namespace popup
{

    class PopupStageSpellbook
        : public PopupStageBase
        , public sfml_util::gui::callback::
              IListBoxCallbackHandler<PopupStageSpellbook, spell::SpellPtr_t>
    {
    public:
        PopupStageSpellbook(const PopupStageSpellbook &) = delete;
        PopupStageSpellbook(PopupStageSpellbook &&) = delete;
        PopupStageSpellbook & operator=(const PopupStageSpellbook &) = delete;
        PopupStageSpellbook & operator=(PopupStageSpellbook &&) = delete;

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

        const std::string HandlerName() const override { return PopupStageBase::HandlerName(); }

        using PopupStageBase::HandleCallback;

        bool HandleCallback(const sfml_util::gui::callback::ListBoxEventPackage<
                            PopupStageSpellbook,
                            spell::SpellPtr_t> &) override;

        void Setup() override;
        void Draw(sf::RenderTarget & target, const sf::RenderStates &) override;
        void UpdateTime(const float ELAPSED_TIME_SECONDS) override;
        bool KeyRelease(const sf::Event::KeyEvent &) override;

        void SetupOuterAndInnerRegion() override;

        void SetupRegions();
        void SetupLeftAccentImage();
        void SetupRightAccentImage();
        void SetupPlayerImage();
        void SetupPlayerDetailsText();
        void SetupSpellListboxLabel();
        void SetupSpellListbox();

        void SetPageRightColors(const sf::Color & IMAGE_COLOR, const sf::Color & TEXT_COLOR);
        void SetupPageRightText(const spell::SpellPtr_t);
        bool DoesCharacterHaveEnoughManaToCastSpell(const spell::SpellPtr_t) const;
        bool CanCastSpellInPhase(const spell::SpellPtr_t) const;
        bool CanCastSpell(const spell::SpellPtr_t) const;
        bool HandleSpellCast();

    private:
        const spell::SpellPtr_t CurrentSelectedSpell() const;

    private:
        static const float BACKGROUND_WIDTH_RATIO_;
        static const float COLOR_FADE_SPEED_;
        static const sf::Uint8 SPELL_IMAGE_ALPHA_;
        static const sf::Color UNABLE_TEXT_COLOR_;
        static const float WARNING_DURATION_SEC_;

        sfml_util::CachedTextureOpt_t playerCachedTextureOpt_;
        sf::Sprite playerSprite_;
        sf::FloatRect pageRectLeft_;
        sf::FloatRect pageRectRight_;
        sfml_util::gui::TextRegionUPtr_t charDetailsTextRegionUPtr_;
        sfml_util::gui::TextRegionUPtr_t listBoxLabelTextRegionUPtr_;
        sfml_util::gui::ListBoxUPtr_t<PopupStageSpellbook, spell::SpellPtr_t> listBoxUPtr_;
        const sf::Color LISTBOX_IMAGE_COLOR_;
        const sf::Color LISTBOX_LINE_COLOR_;
        const sf::Color LISTBOX_COLOR_FG_;
        const sf::Color LISTBOX_COLOR_BG_;
        const sfml_util::gui::ColorSet LISTBOX_COLORSET_;
        sfml_util::gui::BackgroundInfo LISTBOX_BG_INFO_;
        sfml_util::gui::TextInfo listElementTextInfo_;
        sfml_util::CachedTextureOpt_t spellCachedTextureOpt_;
        sf::Sprite spellSprite_;
        sfml_util::gui::TextRegionUPtr_t spellTitleTextRegionUPtr_;
        sfml_util::gui::TextRegionUPtr_t spellDetailsTextUPtr_;
        sfml_util::gui::TextRegionUPtr_t unableTextUPtr_;
        sfml_util::gui::TextRegionUPtr_t spellDescTextUPtr_;
        sfml_util::ColorShaker warnColorShaker_;
        sfml_util::ColorSlider imageColorSlider_;
        sfml_util::ColorSlider textColorSlider_;
        std::size_t currentListboxIndex_;
    };

} // namespace popup
} // namespace heroespath

#endif // HEROESPATH_POPUP_POPUPSTAGESPELLBOOK_HPP_INCLUDED
