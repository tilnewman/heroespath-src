#ifndef APPBASE_SFMLUTIL_POPUPSTAGE_INCLUDED
#define APPBASE_SFMLUTIL_POPUPSTAGE_INCLUDED
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
#include "sfml-util/gui/box.hpp"
#include "sfml-util/gui/sliderbar.hpp"
#include "sfml-util/gui/text-entry-box.hpp"

#include "heroespath/popup-info.hpp"

#include <memory>
#include <queue>
#include <string>
#include <vector>
#include <utility>


namespace sfml_util
{

    //forward declaration
    namespace gui
    {
        class TextRegion;
        using TextRegionSPtr_t = std::shared_ptr<TextRegion>;
    }


    //A base class for all Popup Window Stages
    class PopupStage
    :
        public Stage,
        public sfml_util::gui::callback::ISliderBarCallbackHandler_t,
        public sfml_util::gui::callback::ITextEntryBoxCallbackHandler_t
    {
    public:
        PopupStage(const heroespath::PopupInfo & POPUP_INFO,
                   const sf::FloatRect &         REGION,
                   const sf::FloatRect &         INNER_REGION,
                   const TextureSPtr_t &         TEXTURE_SPTR,
                   const sf::Sprite &            ACCENT_SPRITE,
                   const TextureSPtr_t &         ACCENT_TEXTURE_SPTR);

        PopupStage(const heroespath::PopupInfo & POPUP_INFO,
                   const sf::FloatRect &         REGION,
                   const sf::FloatRect &         INNER_REGION,
                   const sf::Sprite &            ACCENT_SPRITE,
                   const TextureSPtr_t &         ACCENT_TEXTURE_SPTR);

        virtual ~PopupStage();

        inline virtual const std::string HandlerName() const { return GetStageName(); }
        virtual bool HandleCallback(const sfml_util::gui::callback::SliderBarCallbackPackage_t &);
        virtual bool HandleCallback(const sfml_util::gui::callback::TextEntryBoxCallbackPackage_t &);

        virtual void Setup();
        virtual void Draw(sf::RenderTarget & target, sf::RenderStates states);
        virtual void UpdateTime(const float ELAPSED_TIME_SECONDS);
        virtual bool KeyRelease(const sf::Event::KeyEvent &);

        inline virtual std::size_t CurrentSelection() const { return imageIndex_; }

    private:
        void SetupSelectImage(const std::size_t NEW_IMAGE_INDEX, const float SLIDER_SPEED);
        void SetupInfoText(const std::string &);
        int GetSelectNumber() const;

        //returns true of the select number is valid
        bool ProcessSelectNumber();

    protected:
        static const float          IMAGE_SLIDER_SPEED_;
        static const int            NUMBER_SELECT_INVALID_;
        static const float          BEFORE_FADE_STARTS_DELAY_SEC_;
        //
        const heroespath::PopupInfo POPUP_INFO_;
        sf::Sprite                  backgroundSprite_;
        TextureSPtr_t               backgroundTexture_;
        sf::FloatRect               innerRegion_;
        gui::TextRegionSPtr_t       textRegionSPtr_;
        sf::FloatRect               textRegion_;
        float                       elapsedTimeCounter_;
        std::size_t                 secondCounter_;
        gui::box::Box               box_;
        GradientRect                gradient_;
        sf::Sprite                  accentSprite_;
        TextureSPtr_t               accentTextureSPtr_;
        PopupButton_SelectSPtr_t    selectPopupButtonSPtr_;

        gui::SliderBarSPtr_t sliderbarSPtr_;
        float sliderbarPosTop_;
        bool willSliderbarUpdate_;
        bool willTextBoxUpdate_;

        //number selection members
        gui::TextRegionSPtr_t   infoTextRegionSPtr_;
        gui::TextEntryBoxSPtr_t textEntryBoxSPtr_;

        //members needed to animate selection images
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
        std::queue<std::size_t>    imageMoveQueue_;
        sliders::ZeroSliderOnce<float> imageSlider_;

        //members used to fade two images
        //imageSpritePrev_ and imageSpriteCurr_ are used to display the two fading images
        //POPUP_INFO_.Images() holds the two sfml_util::TextureSPtr_ts of the two fading images
        float beforeFadeTimerSec_;
        float fadeAlpha_;
    };


    using PopupStageSPtr_t = std::shared_ptr<PopupStage>;

}
#endif //APPBASE_SFMLUTIL_POPUPSTAGE_INCLUDED
