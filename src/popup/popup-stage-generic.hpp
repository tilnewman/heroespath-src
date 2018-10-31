// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_POPUP_POPUPSTAGEGENERIC_HPP_INCLUDED
#define HEROESPATH_POPUP_POPUPSTAGEGENERIC_HPP_INCLUDED
//
// popup-stage-generic.hpp
//  This class encapsulates a popup window stage on screen.
//
#include "popup/popup-stage-base.hpp"

#include <string>

namespace heroespath
{
namespace popup
{

    // Responsible for implementing a generic popup window stage.
    class PopupStageGeneric : public PopupStageBase
    {
    public:
        PopupStageGeneric(const PopupStageGeneric &) = delete;
        PopupStageGeneric(PopupStageGeneric &&) = delete;
        PopupStageGeneric & operator=(const PopupStageGeneric &) = delete;
        PopupStageGeneric & operator=(PopupStageGeneric &&) = delete;

        explicit PopupStageGeneric(const PopupInfo & POPUP_INFO);
        virtual ~PopupStageGeneric();

        void Draw(sf::RenderTarget & target, const sf::RenderStates &) override;
    };
} // namespace popup
} // namespace heroespath

#endif // HEROESPATH_POPUP_POPUPSTAGEGENERIC_HPP_INCLUDED
