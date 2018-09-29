// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_POPUP_POPUPSTAGETREASURETRAP_HPP_INCLUDED
#define HEROESPATH_POPUP_POPUPSTAGETREASURETRAP_HPP_INCLUDED
//
// popup-stage-treasure-trap.hpp
//
#include "popup/popup-stage-base.hpp"

#include "sfml-util/gui/text-region.hpp"

#include <string>

namespace heroespath
{
namespace popup
{

    // Responsible for implementing a treasure trap popup window stage.
    class PopupStageTreasureTrap : public PopupStageBase
    {
    public:
        PopupStageTreasureTrap(const PopupStageTreasureTrap &) = delete;
        PopupStageTreasureTrap(PopupStageTreasureTrap &&) = delete;
        PopupStageTreasureTrap & operator=(const PopupStageTreasureTrap &) = delete;
        PopupStageTreasureTrap & operator=(PopupStageTreasureTrap &&) = delete;

    public:
        explicit PopupStageTreasureTrap(const PopupInfo & POPUP_INFO);
        virtual ~PopupStageTreasureTrap();

        void Setup() override;
        void Draw(sf::RenderTarget & target, const sf::RenderStates &) override;

    private:
        void SetupTitleText();
        void SetupDescriptionText();
        void SetupAccentImage();

    private:
        sf::Texture trapTexture_;
        sf::Sprite trapSprite_;
        sfml_util::TextRegionUPtr_t descTextRegionUPtr_;
    };
} // namespace popup
} // namespace heroespath

#endif // HEROESPATH_POPUP_POPUPSTAGETREASURETRAP_HPP_INCLUDED
