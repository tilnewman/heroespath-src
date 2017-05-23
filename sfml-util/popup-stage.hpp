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
#include "sfml-util/popup-buttons.hpp"
#include "sfml-util/i-callback-handler.hpp"
#include "sfml-util/color-shaker.hpp"
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
}
namespace sfml_util
{

    namespace gui
    {
        class TextRegion;
        using TextRegionUPtr_t = std::unique_ptr<TextRegion>;
        using TextRegionSPtr_t = std::shared_ptr<TextRegion>;
    }


    //A base class for all Popup Window Stages
    class PopupStage
    :
        public Stage,
        public sfml_util::gui::callback::ISliderBarCallbackHandler_t,
        public sfml_util::gui::callback::ITextEntryBoxCallbackHandler_t,
        public sfml_util::gui::callback::IListBoxCallbackHandler
    {

        //prevent copy construction
        PopupStage(const PopupStage &) =delete;

        //prevent copy assignment
        PopupStage & operator=(const PopupStage &) =delete;

        //defines what is happening on the spellbook popup
        enum class SpellbookState
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
                   const TextureSPtr_t &   BG_TEXTURE_SPTR = TextureSPtr_t());

        virtual ~PopupStage();

        inline virtual const std::string HandlerName() const { return GetStageName(); }
        virtual bool HandleCallback(const sfml_util::gui::callback::SliderBarCallbackPackage_t &);
        virtual bool HandleCallback(const sfml_util::gui::callback::TextEntryBoxCallbackPackage_t &);
        virtual bool HandleCallback(const sfml_util::gui::callback::ListBoxEventPackage &);

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
        void MoveSpellbookPageRightColors(const float ELAPSED_TIME_SECONDS);
        void SetSpellbookPageRightColors();

        bool DoesCharacterHaveEnoughManaToCastSpell(const game::spell::SpellPtrC_t) const;
        bool CanCastSpellInPhase(const game::spell::SpellPtrC_t) const;
        bool CanCastSpell(const game::spell::SpellPtrC_t) const;

    public:
        static const float SPELLBOOK_POPUP_BACKGROUND_WIDTH_RATIO_;
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
        const game::PopupInfo    POPUP_INFO_;
        sf::Sprite               backgroundSprite_;
        TextureSPtr_t            backgroundTextureSPtr_;
        const sf::FloatRect      INNER_REGION_;
        gui::TextRegionSPtr_t    textRegionSPtr_;
        sf::FloatRect            textRegion_;
        float                    elapsedTimeCounter_;
        std::size_t              secondCounter_;
        gui::box::Box            box_;
        GradientRect             gradient_;
        sf::Sprite               accentSprite1_;
        TextureSPtr_t            accentTexture1SPtr_;
        sf::Sprite               accentSprite2_;
        TextureSPtr_t            accentTexture2SPtr_;
        PopupButton_SelectSPtr_t selectPopupButtonSPtr_;

        //members supporting the image select sliderbar
        gui::SliderBarSPtr_t sliderbarSPtr_;
        float sliderbarPosTop_;
        bool willSliderbarUpdate_;
        bool willTextBoxUpdate_;

        //number selection members
        gui::TextRegionSPtr_t   infoTextRegionSPtr_;
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
        gui::TextRegionSPtr_t imageWrnTextRegionSPtr_;
        gui::TextRegionSPtr_t imageNumTextRegionSPtr_;
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

        //members used to fade two images
        //imageSpritePrev_ and imageSpriteCurr_ are used to display the two fading images
        //POPUP_INFO_.Images() holds the two sfml_util::TextureSPtr_ts of the two fading images
        float beforeFadeTimerSec_;
        float fadeAlpha_;

        //members supporting the spellbook popup
        SpellbookState          spellbookState_;
        TextureSPtr_t           playerTextureSPtr_;
        sf::Sprite              playerSprite_;
        sf::FloatRect           pageRectLeft_;
        sf::FloatRect           pageRectRight_;
        gui::TextRegionUPtr_t   charDetailsTextRegionUPtr_;
        gui::TextRegionUPtr_t   listBoxLabelTextRegionUPtr_;
        gui::ListBoxSPtr_t      spellListBoxSPtr_;
        const sf::Color         LISTBOX_IMAGE_COLOR_;
        const sf::Color         LISTBOX_LINE_COLOR_;
        const sf::Color         LISTBOX_COLOR_FG_;
        const sf::Color         LISTBOX_COLOR_BG_;
        const gui::ColorSet     LISTBOX_COLORSET_;
        gui::BackgroundInfo     LISTBOX_BG_INFO_;
        gui::TextInfo           listBoxItemTextInfo_;
        TextureSPtr_t           spellTextureSPtr_;
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
    };

    using PopupStageSPtr_t = std::shared_ptr<PopupStage>;

}
#endif //SFMLUTIL_POPUPSTAGE_INCLUDED
