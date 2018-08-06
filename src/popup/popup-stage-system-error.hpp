// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_POPUP_POPUPSTAGESYSTEMERROR_HPP_INCLUDED
#define HEROESPATH_POPUP_POPUPSTAGESYSTEMERROR_HPP_INCLUDED
//
// popup-stage-system-error.hpp
//
#include "popup/popup-stage-base.hpp"

namespace heroespath
{
namespace popup
{

    // Responsible for implementing the System Error Popup Stage.
    class PopupStageSystemError : public PopupStageBase
    {
    public:
        PopupStageSystemError(const PopupStageSystemError &) = delete;
        PopupStageSystemError(PopupStageSystemError &&) = delete;
        PopupStageSystemError & operator=(const PopupStageSystemError &) = delete;
        PopupStageSystemError & operator=(PopupStageSystemError &&) = delete;

    public:
        explicit PopupStageSystemError(const PopupInfo &);
        virtual ~PopupStageSystemError();

        void Setup() override;
        void Draw(sf::RenderTarget &, const sf::RenderStates &) override;

    private:
        sfml_util::CachedTexture bgCachedTexture_;
        sf::Sprite bgSprite_;
    };
} // namespace popup
} // namespace heroespath

#endif // HEROESPATH_POPUP_POPUPSTAGESYSTEMERROR_HPP_INCLUDED
