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
#include "gui/cached-texture.hpp"
#include "gui/colored-rect.hpp"
#include "gui/text-button.hpp"
#include "gui/text-region.hpp"
#include "popup/popup-info.hpp"
#include "stage/stage-base.hpp"

#include <SFML/Graphics/Sprite.hpp>

#include <string>

namespace heroespath
{
namespace popup
{

    // Responsible for encapsulating all state and operations common to popup windows.
    class PopupStageBase
        : public stage::StageBase
        , public gui::SliderBar::Callback_t::IHandler_t
        , public gui::TextButton::Callback_t::IHandler_t
    {
    public:
        PopupStageBase(const PopupStageBase &) = delete;
        PopupStageBase(PopupStageBase &&) = delete;
        PopupStageBase & operator=(const PopupStageBase &) = delete;
        PopupStageBase & operator=(PopupStageBase &&) = delete;

        explicit PopupStageBase(const PopupInfo & POPUP_INFO);
        virtual ~PopupStageBase();

        const std::string HandleCallback(
            const gui::SliderBar::Callback_t::Packet_t &, const std::string &) override
        {
            return "";
        }

        const std::string HandleCallback(
            const gui::TextButton::Callback_t::Packet_t &,
            const std::string & PACKET_DESCRIPTION) override;

        void Setup() override;

        void draw(sf::RenderTarget &, sf::RenderStates) const override;

        void DrawRedX(sf::RenderTarget &, sf::RenderStates) const;

        bool KeyRelease(const sf::Event::KeyEvent &) override;

        void UpdateTime(const float ELAPSED_TIME_SECONDS) override;

    protected:
        float ButtonTextHeight() const { return buttonTextHeight_; }

        // returns true if the selection was valid and the popup will be removed
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
        gui::CachedTexture backgroundTexture_;
        gui::TextRegionUPtr_t textRegionUPtr_;
        sf::FloatRect textRegion_;
        gui::TextButtonUPtr_t buttonSelectUPtr_;
        gui::TextButtonUPtr_t buttonYesUPtr_;
        gui::TextButtonUPtr_t buttonNoUPtr_;
        gui::TextButtonUPtr_t buttonCancelUPtr_;
        gui::TextButtonUPtr_t buttonContinueUPtr_;
        gui::TextButtonUPtr_t buttonOkayUPtr_;
        gui::SliderBarUPtr_t sliderbarUPtr_;
        gui::CachedTextureOpt_t accent1CachedTextureOpt_;
        sf::Sprite accentSprite1_;
        gui::CachedTextureOpt_t accent2CachedTextureOpt_;
        sf::Sprite accentSprite2_;
        float sliderbarPosTop_;
        int selection_;
        sf::Sprite xSymbolSprite_;
        bool willShowXImage_;

    private:
        sf::Sprite backgroundSprite_;
        float buttonTextHeight_;
        float buttonVertPos_;
        gui::CachedTextureOpt_t xSymbolCachedTextureOpt_;
        float keepAliveTimerSec_;
    };

} // namespace popup
} // namespace heroespath

#endif // HEROESPATH_POPUP_POPUPSTAGEBASE_HPP_INCLUDED
