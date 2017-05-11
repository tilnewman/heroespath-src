#ifndef SFMLUTIL_RADIOBUTTONDISPLAY_INCLUDED
#define SFMLUTIL_RADIOBUTTONDISPLAY_INCLUDED
//
// radio-button-display.hpp
//  A specific RadioButtonSet for changing the display resolution.
//
#include "sfml-util/i-stage.hpp"
#include "sfml-util/resolution.hpp"
#include "sfml-util/gui/radio-button.hpp"

#include "game/i-popup-callback.hpp"

#include <memory>


namespace gui_demo
{

    //Represents a RadioButtonSet that changes the display
    class RadioButtonSet_DisplayChange
    :
        public game::callback::IPopupHandler_t,
        public sfml_util::gui::RadioButtonSet
    {
        //prevent copy construction
        RadioButtonSet_DisplayChange(const RadioButtonSet_DisplayChange &) =delete;

        //prevent copy assignment
        RadioButtonSet_DisplayChange & operator=(const RadioButtonSet_DisplayChange &) =delete;

    public:
        //If Using this constructor, then one of the Setup() functions must be called before any other member
        RadioButtonSet_DisplayChange(const float               POS_LEFT,
                                     const float               POS_TOP,
                                     sfml_util::IStage * const OWNER_STAGE_PTR);

        virtual ~RadioButtonSet_DisplayChange();

        inline virtual const std::string HandlerName() const { return GetEntityName(); }

        virtual bool HandleCallback(const game::callback::PopupResponse &);

    protected:
        //if not found, resolutionVec_.size() is returned
        std::size_t FindCurrentResolutionSelection();
        virtual void OnClick(const sf::Vector2f &);

    private:
        static const std::size_t MAX_NUM_RES_DISPLAYABLE_;
        //
        sfml_util::IStage * const  ownerStagePtr_;
        sfml_util::ResolutionVec_t resolutionVec_;
        const std::size_t          ORIG_INVALID_SELECTION_;
        sfml_util::Resolution      prevResolution_;
    };

    using RadioButtonSet_DisplayChangeSPtr_t = std::shared_ptr<RadioButtonSet_DisplayChange>;
}
#endif //SFMLUTIL_RADIOBUTTONDISPLAY_INCLUDED
