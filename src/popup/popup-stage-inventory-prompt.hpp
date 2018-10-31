// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_POPUP_POPUPSTAGEINVENTORYPROMPT_HPP_INCLUDED
#define HEROESPATH_POPUP_POPUPSTAGEINVENTORYPROMPT_HPP_INCLUDED
//
// popup-stage-inventory-prompt.hpp
//
#include "popup/popup-stage-base.hpp"

namespace heroespath
{
namespace popup
{

    // Responsible for implementing the System Error Popup Stage.
    class PopupStageInventoryPrompt : public PopupStageBase
    {
    public:
        PopupStageInventoryPrompt(const PopupStageInventoryPrompt &) = delete;
        PopupStageInventoryPrompt(PopupStageInventoryPrompt &&) = delete;
        PopupStageInventoryPrompt & operator=(const PopupStageInventoryPrompt &) = delete;
        PopupStageInventoryPrompt & operator=(PopupStageInventoryPrompt &&) = delete;

        explicit PopupStageInventoryPrompt(const PopupInfo &);
        virtual ~PopupStageInventoryPrompt();

        void Draw(sf::RenderTarget &, const sf::RenderStates &) override;
        bool KeyRelease(const sf::Event::KeyEvent &) override;

    private:
        bool WillPressingCKeyClosePopup() const override { return true; }
    };
} // namespace popup
} // namespace heroespath

#endif // HEROESPATH_POPUP_POPUPSTAGEINVENTORYPROMPT_HPP_INCLUDED
