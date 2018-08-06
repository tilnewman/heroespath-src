// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_POPUP_POPUPSTAGEIMAGEFADE_HPP_INCLUDED
#define HEROESPATH_POPUP_POPUPSTAGEIMAGEFADE_HPP_INCLUDED
//
// popup-stage-image-fade.hpp
//
#include "popup/popup-stage-base.hpp"

#include <SFML/Graphics/Sprite.hpp>

namespace heroespath
{
namespace sfml_util
{
    namespace gui
    {
        class TextRegion;
        using TextRegionUPtr_t = std::unique_ptr<TextRegion>;
    } // namespace gui
} // namespace sfml_util

namespace popup
{

    // Responsible for implementing the Image Fade Popup Stage.
    class PopupStageImageFade : public PopupStageBase
    {
    public:
        PopupStageImageFade(const PopupStageImageFade &) = delete;
        PopupStageImageFade(PopupStageImageFade &&) = delete;
        PopupStageImageFade & operator=(const PopupStageImageFade &) = delete;
        PopupStageImageFade & operator=(PopupStageImageFade &&) = delete;

        explicit PopupStageImageFade(const PopupInfo &);
        virtual ~PopupStageImageFade();

        void Setup() override;
        void Draw(sf::RenderTarget &, const sf::RenderStates &) override;
        void UpdateTime(const float ELAPSED_TIME_SECONDS) override;
        bool KeyRelease(const sf::Event::KeyEvent &) override;

    private:
        static const float BEFORE_FADE_STARTS_DELAY_SEC_;

        sf::Sprite spriteCurr_;
        sf::Sprite spritePrev_;
        float beforeFadeTimerSec_;
        float fadeAlpha_;
        sfml_util::gui::TextRegionUPtr_t titleTextRegionUPtr_;
        sfml_util::gui::TextRegionUPtr_t descTextRegionUPtr_;
    };
} // namespace popup
} // namespace heroespath

#endif // HEROESPATH_POPUP_POPUPSTAGEIMAGEFADE_HPP_INCLUDED
