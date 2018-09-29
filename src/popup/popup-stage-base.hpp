// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_POPUP_POPUPSTAGEBASE_HPP_INCLUDED
#define HEROESPATH_POPUP_POPUPSTAGEBASE_HPP_INCLUDED
//
// popup-stage-base.hpp
//
#include "popup/popup-info.hpp"
#include "sfml-util/cached-texture.hpp"
#include "sfml-util/colored-rect.hpp"
#include "sfml-util/gui/text-button.hpp"
#include "sfml-util/gui/text-region.hpp"
#include "sfml-util/stage.hpp"

#include <SFML/Graphics/Sprite.hpp>

#include <string>

namespace heroespath
{
namespace popup
{

    // Responsible for encapsulating all state and operations common to popup windows.
    class PopupStageBase
        : public sfml_util::Stage
        , public sfml_util::SliderBar::Callback_t::IHandler_t
        , public sfml_util::TextButton::Callback_t::IHandler_t
    {
    public:
        PopupStageBase(const PopupStageBase &) = delete;
        PopupStageBase(PopupStageBase &&) = delete;
        PopupStageBase & operator=(const PopupStageBase &) = delete;
        PopupStageBase & operator=(PopupStageBase &&) = delete;

        explicit PopupStageBase(const PopupInfo & POPUP_INFO);
        virtual ~PopupStageBase();

        bool HandleCallback(const sfml_util::SliderBar::Callback_t::PacketPtr_t &) override;

        bool HandleCallback(const sfml_util::TextButton::Callback_t::PacketPtr_t &) override;

        void Setup() override;

        void Draw(sf::RenderTarget &, const sf::RenderStates &) override;

        void DrawRedX(sf::RenderTarget &, const sf::RenderStates &);

        bool KeyRelease(const sf::Event::KeyEvent &) override;

        void UpdateTime(const float ELAPSED_TIME_SECONDS) override;

    protected:
        float ButtonTextHeight() const { return buttonTextHeight_; }

        virtual bool HandleSelect();

        virtual void SetupOuterAndInnerRegion();

        void SetupForFullScreenWithBorderRatio(const float BORDER_RATIO);

        virtual void SetupSliderbar();

        void PlayValidKeypressSoundEffect() const;
        void PlayInvalidKeypressSoundEffect() const;

        void EndKeepAliveTimer();

    private:
        void SetupVariousButtonPositionValues();
        void SetupButtons();
        void SetupTextRegion();
        void SetupText();
        void SetupAccentSprite();
        void SetupRedXImage();

        const sf::IntRect BackgroundImageRect(const PopupImage::Enum PI, const float SCALE) const;

        virtual bool WillPressingCKeyClosePopup() const { return false; }

        float calcBackgroundImageScale(const PopupImage::Enum) const;

    protected:
        static const sf::Uint8 ACCENT_IMAGE_ALPHA_;
        static const float ACCENT_IMAGE_SCALEDOWN_RATIO_;

    protected:
        PopupInfo popupInfo_;
        sf::FloatRect innerRegion_;
        sfml_util::CachedTexture backgroundTexture_;
        sfml_util::TextRegionUPtr_t textRegionUPtr_;
        sf::FloatRect textRegion_;
        sfml_util::TextButtonUPtr_t buttonSelectUPtr_;
        sfml_util::TextButtonUPtr_t buttonYesUPtr_;
        sfml_util::TextButtonUPtr_t buttonNoUPtr_;
        sfml_util::TextButtonUPtr_t buttonCancelUPtr_;
        sfml_util::TextButtonUPtr_t buttonContinueUPtr_;
        sfml_util::TextButtonUPtr_t buttonOkayUPtr_;
        sfml_util::SliderBarUPtr_t sliderbarUPtr_;
        sfml_util::CachedTextureOpt_t accent1CachedTextureOpt_;
        sf::Sprite accentSprite1_;
        sfml_util::CachedTextureOpt_t accent2CachedTextureOpt_;
        sf::Sprite accentSprite2_;
        float sliderbarPosTop_;
        int selection_;
        sf::Sprite xSymbolSprite_;
        bool willShowXImage_;

    private:
        sf::Sprite backgroundSprite_;
        float buttonTextHeight_;
        float buttonVertPos_;
        sfml_util::CachedTextureOpt_t xSymbolCachedTextureOpt_;
        float keepAliveTimerSec_;
    };

} // namespace popup
} // namespace heroespath

#endif // HEROESPATH_POPUP_POPUPSTAGEBASE_HPP_INCLUDED
