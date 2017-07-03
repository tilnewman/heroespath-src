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
#ifndef SFMLUTIL_POPUPSTAGE_INCLUDED
#define SFMLUTIL_POPUPSTAGE_INCLUDED
//
// popup-stage.hpp
//  This class encapsulates a popup window stage on screen.
//
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/sfml-window.hpp"
#include "sfml-util/stage.hpp"
#include "sfml-util/sliders.hpp"
#include "sfml-util/gradient.hpp"
#include "sfml-util/i-callback-handler.hpp"
#include "sfml-util/color-shaker.hpp"
#include "sfml-util/gui/text-button.hpp"
#include "sfml-util/gui/box.hpp"
#include "sfml-util/gui/sliderbar.hpp"
#include "sfml-util/gui/text-entry-box.hpp"
#include "sfml-util/gui/background-info.hpp"
#include "sfml-util/gui/list-box.hpp"
#include "sfml-util/gui/list-box-item.hpp"

#include "game/popup-info.hpp"

#include <memory>
#include <queue>
#include <string>
#include <vector>
#include <utility>


//forward declarations
namespace game
{
namespace spell
{
    class Spell;
    using SpellPtr_t = Spell *;
    using SpellPtrC_t = Spell * const;
}
namespace song
{
    class Song;
    using SongPtr_t = Song *;
    using SongPtrC_t = Song * const;
}
}
namespace sfml_util
{

    namespace gui
    {
        class TextRegion;
        using TextRegionUPtr_t = std::unique_ptr<TextRegion>;
    }


    //A base class for all Popup Window Stages
    class PopupStage
    :
        public Stage,
        public sfml_util::gui::callback::ISliderBarCallbackHandler_t,
        public sfml_util::gui::callback::ITextEntryBoxCallbackHandler_t,
        public sfml_util::gui::callback::IListBoxCallbackHandler,
        public sfml_util::gui::callback::ITextButtonCallbackHandler_t
    {

        //prevent copy construction
        PopupStage(const PopupStage &) =delete;

        //prevent copy assignment
        PopupStage & operator=(const PopupStage &) =delete;

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
        PopupStage(const game::PopupInfo & POPUP_INFO,
                   const sf::FloatRect &   REGION,
                   const sf::FloatRect &   INNER_REGION,
                   const sf::Texture &     BG_TEXTURE = sf::Texture());

        virtual ~PopupStage();

        inline virtual const std::string HandlerName() const { return GetStageName(); }
        virtual bool HandleCallback(const sfml_util::gui::callback::SliderBarCallbackPackage_t &);
        virtual bool HandleCallback(const sfml_util::gui::callback::TextEntryBoxCallbackPackage_t &);
        virtual bool HandleCallback(const sfml_util::gui::callback::ListBoxEventPackage &);
        virtual bool HandleCallback(const sfml_util::gui::callback::TextButtonCallbackPackage_t &);

        virtual void Setup();
        virtual void Draw(sf::RenderTarget & target, const sf::RenderStates &);
        virtual void UpdateTime(const float ELAPSED_TIME_SECONDS);
        virtual bool KeyRelease(const sf::Event::KeyEvent &);

        inline virtual std::size_t CurrentSelection() const { return imageIndex_; }

    private:
        void SetupSelectImage(const std::size_t NEW_IMAGE_INDEX, const float SLIDER_SPEED);
        void SetupInfoText(const std::string &);
        int GetSelectNumber() const;

        //returns true of the select number is valid
        bool ProcessSelectNumber();

        void SetupSpellbookPageRightForFadeIn();
        void SetupSpellbookPageRightText(const game::spell::SpellPtrC_t);
        void SetupMusicSheetPageRightText(const game::song::SongPtrC_t);
        void MoveSpellbookPageRightColors(const float ELAPSED_TIME_SECONDS);
        void SetSpellbookPageRightColors();

        bool DoesCharacterHaveEnoughManaToCastSpell(const game::spell::SpellPtrC_t) const;
        bool CanCastSpellInPhase(const game::spell::SpellPtrC_t) const;
        bool CanCastSpell(const game::spell::SpellPtrC_t) const;
        bool HandleSpellCast();

        bool DoesCharacterHaveEnoughManaToPlaySong(const game::song::SongPtrC_t) const;
        bool CanPlaySongInPhase(const game::song::SongPtrC_t) const;
        bool CanPlaySong(const game::song::SongPtrC_t) const;
        bool HandleSongPlay();
        
        void SetupCharacterSelectDetailText(const bool WILL_ERASE);

        void SetupCharacterSelectionRejectImage(const bool WILL_ERASE);

        bool HandleSelect();

    public:
        static const float SPELLBOOK_POPUP_BACKGROUND_WIDTH_RATIO_;
        static const float MUSICSHEET_POPUP_BACKGROUND_WIDTH_RATIO_;

    protected:
        static const float     IMAGE_SLIDER_SPEED_;
        static const int       NUMBER_SELECT_INVALID_;
        static const float     BEFORE_FADE_STARTS_DELAY_SEC_;
        static const float     SPELLBOOK_COLOR_FADE_SPEED_;
        static const sf::Uint8 SPELLBOOK_IMAGE_ALPHA_;
        static const sf::Uint8 ACCENT_IMAGE_ALPHA_;
        static const sf::Color SPELL_UNABLE_TEXT_COLOR_;
        static const float     SPELL_WARNING_DURATION_SEC_;
        //
        const game::PopupInfo  POPUP_INFO_;
        sf::Sprite             backgroundSprite_;
        sf::Texture            backgroundTexture_;
        const sf::FloatRect    INNER_REGION_;
        gui::TextRegionUPtr_t  textRegionUPtr_;
        sf::FloatRect          textRegion_;
        float                  elapsedTimeCounter_;
        std::size_t            secondCounter_;
        gui::box::Box          box_;
        GradientRect           gradient_;
        sf::Sprite             accentSprite1_;
        sf::Texture            accentTexture1_;
        sf::Sprite             accentSprite2_;
        sf::Texture            accentTexture2_;
        int                    selection_;
        //
        gui::TextButtonUPtr_t  buttonSelectUPtr_;
        gui::TextButtonUPtr_t  buttonYesUPtr_;
        gui::TextButtonUPtr_t  buttonNoUPtr_;
        gui::TextButtonUPtr_t  buttonCancelUPtr_;
        gui::TextButtonUPtr_t  buttonContinueUPtr_;
        gui::TextButtonUPtr_t  buttonOkayUPtr_;

        //members supporting the image select sliderbar
        gui::SliderBarUPtr_t sliderbarUPtr_;
        float sliderbarPosTop_;
        bool willSliderbarUpdate_;
        bool willTextBoxUpdate_;
        sf::Texture textureCurr_;
        sf::Texture texturePrev_;

        //number selection members
        gui::TextRegionUPtr_t   infoTextRegionUPtr_;
        gui::TextEntryBoxSPtr_t textEntryBoxSPtr_;

        //members needed to animate image select
        bool                  isImageProcAllowed_;
        bool                  isInitialAnimation_;
        bool                  willShowImageCount_;
        sf::Sprite            imageSpriteCurr_;//these two sprites also used for fading
        sf::Sprite            imageSpritePrev_;
        bool                  areImagesMoving_;
        bool                  areImagesMovingLeft_;
        sf::FloatRect         imagesRect_;
        gui::TextRegionUPtr_t imageWrnTextRegionUPtr_;
        gui::TextRegionUPtr_t imageNumTextRegionUPtr_;
        std::size_t           imageIndex_;
        std::size_t           imageIndexLastSoundOn_;
        std::size_t           imageIndexLastSoundOff_;
        float                 imageCurrTargetScale_;
        float                 imagePrevStartScale_;
        float                 imagePrevStartPosX_;
        float                 imageCurrTravelDist_;
        float                 imagePrevTravelDist_;
        std::queue<std::size_t> imageMoveQueue_;
        sliders::ZeroSliderOnce<float> imageSlider_;
        float                 imagePosTop_;

        //members used to fade two images
        //imageSpritePrev_ and imageSpriteCurr_ are used to display the two fading images
        //POPUP_INFO_.Images() holds the two sf::Textures of the two fading images
        float beforeFadeTimerSec_;
        float fadeAlpha_;

        //members supporting the spellbook popup
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
        gui::TextRegionUPtr_t   spellUnableTextUPtr_;
        gui::TextRegionUPtr_t   spellDescTextUPtr_;
        game::spell::SpellPtr_t spellCurrentPtr_;
        sf::Color               spellColorImageCurrent_;
        sf::Color               spellColorImageStart_;
        sf::Color               spellColorImageEnd_;
        sf::Color               spellColorTextCurrent_;
        sf::Color               spellColorTextStart_;
        sf::Color               spellColorTextEnd_;
        bool                    spellUnableTextWillShow_;
        float                   spellWarningTimerSec_;
        ColorShaker             spellWarnColorShaker_;
        sliders::ZeroSliderOnce<float> spellColorSlider_;

        //members supporting character selection
        sf::Texture xSymbolTexture_;
        sf::Sprite xSymbolSprite_;
        bool willShowRejectImage_;

        //members supporting song selection
        //Note:  Many spell members are re-used
        game::song::SongPtr_t songCurrentPtr_;

        //members supporting CombatOver
        sf::Texture combatBgTexture_;
        sf::Sprite combatBgSprite_;
        gui::TextRegionUPtr_t combatTitleUPtr_;
        gui::TextRegionUPtr_t combatDescUPtr_;
    };

    using PopupStagePtr_t = PopupStage *;

}
#endif //SFMLUTIL_POPUPSTAGE_INCLUDED
