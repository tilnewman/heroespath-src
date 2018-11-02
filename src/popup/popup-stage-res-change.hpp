// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_POPUP_POPUPSTAGERESCHANGE_HPP_INCLUDED
#define HEROESPATH_POPUP_POPUPSTAGERESCHANGE_HPP_INCLUDED
//
// popup-stage-res-change.hpp
//
#include "popup/popup-stage-base.hpp"

namespace heroespath
{
namespace popup
{

    // Responsible for implementing the System Error Popup Stage.
    class PopupStageResChange : public PopupStageBase
    {
    public:
        PopupStageResChange(const PopupStageResChange &) = delete;
        PopupStageResChange(PopupStageResChange &&) = delete;
        PopupStageResChange & operator=(const PopupStageResChange &) = delete;
        PopupStageResChange & operator=(PopupStageResChange &&) = delete;

        explicit PopupStageResChange(const PopupInfo &);
        virtual ~PopupStageResChange();

        void draw(sf::RenderTarget &, sf::RenderStates) const override;
        void UpdateTime(const float ELAPSED_TIME_SECONDS) override;

    private:
        float elapsedTimeCounter_;
        std::size_t secondCounter_;
    };
} // namespace popup
} // namespace heroespath

#endif // HEROESPATH_POPUP_POPUPSTAGERESCHANGE_HPP_INCLUDED
