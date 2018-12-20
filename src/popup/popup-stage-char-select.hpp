// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_POPUP_POPUPSTAGECHARSELECT_HPP_INCLUDED
#define HEROESPATH_POPUP_POPUPSTAGECHARSELECT_HPP_INCLUDED
//
// popup-stage-char-select.hpp
//
#include "gui/text-region.hpp"
#include "misc/vector-map.hpp"
#include "popup/popup-info.hpp"
#include "popup/popup-stage-image-select.hpp"

#include <string>

namespace heroespath
{
namespace popup
{

    // Responsible for implementing the Character Select Popup Stage.
    class PopupStageCharacterSelect : public PopupStageImageSelect
    {
    public:
        explicit PopupStageCharacterSelect(const PopupInfo &);
        virtual ~PopupStageCharacterSelect();

        PopupStageCharacterSelect(const PopupStageCharacterSelect &) = delete;
        PopupStageCharacterSelect(PopupStageCharacterSelect &&) = delete;
        PopupStageCharacterSelect & operator=(const PopupStageCharacterSelect &) = delete;
        PopupStageCharacterSelect & operator=(PopupStageCharacterSelect &&) = delete;

        void draw(sf::RenderTarget &, sf::RenderStates) const override;

    private:
        void SetupCharDetailsText();

        bool HandleSelect() override;

        void SetupContentForNewImage(const bool WILL_ERASE) override;
        void SetupCharacterSelectDetailText(const bool WILL_ERASE);
        void SetupCharacterSelectionRejectImage(const bool WILL_ERASE);

    private:
        gui::TextRegionUPtr_t charDetailsTextRegionUPtr_;
    };

} // namespace popup
} // namespace heroespath

#endif // HEROESPATH_POPUP_POPUPSTAGECHARSELECT_HPP_INCLUDED
