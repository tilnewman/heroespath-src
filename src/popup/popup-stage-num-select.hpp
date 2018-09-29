// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_POPUP_POPUPSTAGENUMSELECT_HPP_INCLUDED
#define HEROESPATH_POPUP_POPUPSTAGENUMSELECT_HPP_INCLUDED
//
// popup-stage-num-select.hpp
//
#include "popup/popup-stage-base.hpp"
#include "sfml-util/gui/sliderbar.hpp"
#include "sfml-util/gui/text-entry-box.hpp"
#include "sfml-util/gui/text-region.hpp"

#include <string>

namespace heroespath
{
namespace popup
{

    // Responsible for implementing the Number Selection Popup Stage.
    class PopupStageNumberSelect
        : public PopupStageBase
        , public sfml_util::TextEntryBox::Callback_t::IHandler_t
    {
    public:
        PopupStageNumberSelect(const PopupStageNumberSelect &) = delete;
        PopupStageNumberSelect(PopupStageNumberSelect &&) = delete;
        PopupStageNumberSelect & operator=(const PopupStageNumberSelect &) = delete;
        PopupStageNumberSelect & operator=(PopupStageNumberSelect &&) = delete;

    public:
        explicit PopupStageNumberSelect(const PopupInfo & POPUP_INFO);
        virtual ~PopupStageNumberSelect();

        using PopupStageBase::HandleCallback;

        bool HandleCallback(const sfml_util::SliderBar::Callback_t::PacketPtr_t &) override;

        bool HandleCallback(const sfml_util::TextEntryBox::Callback_t::PacketPtr_t &) override;

        void Setup() override;
        void Draw(sf::RenderTarget &, const sf::RenderStates &) override;
        bool KeyRelease(const sf::Event::KeyEvent &) override;

    private:
        void SetupInfoText(const std::string &);
        int GetSelectNumber() const;
        bool ProcessSelectNumber();
        void SetupSliderbar() override;

    private:
        static const int NUMBER_SELECT_INVALID_;
        //
        sfml_util::TextRegionUPtr_t msgTextRegionUPtr_;
        sfml_util::TextEntryBoxUPtr_t textEntryBoxUPtr_;
        bool willSliderbarUpdate_;
        bool willTextBoxUpdate_;
    };

} // namespace popup
} // namespace heroespath

#endif // HEROESPATH_POPUP_POPUPSTAGENUMSELECT_HPP_INCLUDED
