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
        explicit PopupStageBase(const PopupInfo & POPUP_INFO);
        virtual ~PopupStageBase();

        PopupStageBase(const PopupStageBase &) = delete;
        PopupStageBase(PopupStageBase &&) = delete;
        PopupStageBase & operator=(const PopupStageBase &) = delete;
        PopupStageBase & operator=(PopupStageBase &&) = delete;

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
        bool KeyRelease(const sf::Event::KeyEvent &) override;
        void UpdateTime(const float ELAPSED_TIME_SECONDS) override;

    protected:
        const sf::FloatRect MakeBackgroundImageContentRegion(
            const PopupImage::Enum,
            const bool IS_LEFT_IMAGE,
            const sf::FloatRect & BG_REGION) const;

        // returns true if the selection was valid and the popup will be removed
        virtual bool HandleSelect();

        virtual void SetupRegions();
        void SetupRegionsForFullScreen(const float BORDER_RATIO);

        void PlayValidKeypressSoundEffect() const;
        void PlayInvalidKeypressSoundEffect() const;

        void EndKeepAliveTimer();

        const gui::ImageOptions AccentImageOptions() const;

        const sf::Vector2f BackgroundImageLocalSize() const;

        const sf::FloatRect ContentRegion() const { return contentRegion_; }

        void ClosePopup(
            const PopupButtons::Enum BUTTON,
            const std::size_t SELECTION = 0,
            const bool WILL_PLAY_SFX = true);

        const sf::FloatRect ActualTextRegion() const;

    private:
        void SetupBackgroundImage();
        void SetupButtons();
        void SetupText();
        void SetupAccentSprite();
        void SetupRedXImage();

        virtual bool WillPressingCKeyClosePopup() const { return false; }

        // if POS_LEFT is negative then the button is centered
        void MakeTextButton(
            gui::TextButtonUPtr_t & buttonUPtr, const std::string & NAME, const float POS_LEFT);

        float CalcTextButtonMaxHeight() const;

    protected:
        static const sf::Uint8 ACCENT_IMAGE_ALPHA_;

        const PopupInfo popupInfo_;
        const std::string BACKGROUND_IMAGE_CONFIG_KEY_;
        const float TEXT_BUTTON_VERT_PAD_;
        const float TEXT_BUTTON_REGION_HEIGHT_;

        gui::SliderBarUPtr_t sliderbarUPtr_;
        gui::CachedTextureOpt_t accent1CachedTextureOpt_;
        sf::Sprite accentSprite1_;
        gui::CachedTextureOpt_t accent2CachedTextureOpt_;
        sf::Sprite accentSprite2_;
        int selection_;
        sf::Sprite xSymbolSprite_;
        bool willShowXImage_;
        gui::TextRegionUPtr_t textRegionUPtr_;

    private:
        gui::CachedTexture backgroundTexture_;
        sf::Sprite backgroundSprite_;
        sf::FloatRect contentRegion_;
        sf::FloatRect buttonRegion_;
        gui::TextButtonUPtr_t buttonSelectUPtr_;
        gui::TextButtonUPtr_t buttonYesUPtr_;
        gui::TextButtonUPtr_t buttonNoUPtr_;
        gui::TextButtonUPtr_t buttonCancelUPtr_;
        gui::TextButtonUPtr_t buttonContinueUPtr_;
        gui::TextButtonUPtr_t buttonOkayUPtr_;
        gui::CachedTextureOpt_t xSymbolCachedTextureOpt_;
        float keepAliveTimerSec_;
    };

} // namespace popup
} // namespace heroespath

#endif // HEROESPATH_POPUP_POPUPSTAGEBASE_HPP_INCLUDED
