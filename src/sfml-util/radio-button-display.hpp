// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_RADIOBUTTONDISPLAY_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_RADIOBUTTONDISPLAY_HPP_INCLUDED
//
// radio-button-display.hpp
//  A specific RadioButtonSet for changing the display resolution.
//
#include "popup/i-popup-callback.hpp"
#include "sfml-util/gui/radio-button.hpp"
#include "sfml-util/i-stage.hpp"
#include "sfml-util/resolution.hpp"

#include <string>

namespace heroespath
{
namespace sfml_util
{

    // Represents a RadioButtonSet that changes the display
    class RadioButtonSet_DisplayChange
        : public popup::IPopupHandler_t
        , public sfml_util::gui::RadioButtonSet
    {
    public:
        RadioButtonSet_DisplayChange(const RadioButtonSet_DisplayChange &) = delete;
        RadioButtonSet_DisplayChange(RadioButtonSet_DisplayChange &&) = delete;
        RadioButtonSet_DisplayChange & operator=(const RadioButtonSet_DisplayChange &) = delete;
        RadioButtonSet_DisplayChange & operator=(RadioButtonSet_DisplayChange &&) = delete;

        // If Using this constructor, then one of the Setup()
        // functions must be called before any other member.
        RadioButtonSet_DisplayChange(
            const float POS_LEFT,
            const float POS_TOP,
            const sfml_util::callback::RadioButtonSetCallbackHandlerPtr_t,
            const sfml_util::IStagePtr_t);

        virtual ~RadioButtonSet_DisplayChange();

        virtual const std::string HandlerName() const { return GetEntityName(); }

        virtual bool HandleCallback(const popup::PopupResponse &);

    protected:
        // if not found, resolutionVec_.size() is returned
        std::size_t FindCurrentResolutionSelection();
        virtual void OnClick(const sf::Vector2f &);

    private:
        static const std::size_t MAX_NUM_RES_DISPLAYABLE_;
        //
        sfml_util::IStagePtr_t ownerStagePtr_;
        sfml_util::ResolutionVec_t resolutionVec_;
        const std::size_t ORIG_INVALID_SELECTION_;
        sfml_util::Resolution prevResolution_;
    };

} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_RADIOBUTTONDISPLAY_HPP_INCLUDED
