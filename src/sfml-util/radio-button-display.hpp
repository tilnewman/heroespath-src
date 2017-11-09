///////////////////////////////////////////////////////////////////////////////
//
// Heroes' Path - Open-source, non-commercial, simple, game in the RPG style.
// Copyright (C) 2017 Ziesche Til Newman (tilnewman@gmail.com)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from
// the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
//  1. The origin of this software must not be misrepresented; you must not
//     claim that you wrote the original software.  If you use this software
//     in a product, an acknowledgment in the product documentation would be
//     appreciated but is not required.
//
//  2. Altered source versions must be plainly marked as such, and must not
//     be misrepresented as being the original software.
//
//  3. This notice may not be removed or altered from any source distribution.
//
///////////////////////////////////////////////////////////////////////////////
#ifndef SFMLUTIL_RADIOBUTTONDISPLAY_HPP_INCLUDED
#define SFMLUTIL_RADIOBUTTONDISPLAY_HPP_INCLUDED
//
// radio-button-display.hpp
//  A specific RadioButtonSet for changing the display resolution.
//
#include "sfml-util/i-stage.hpp"
#include "sfml-util/resolution.hpp"
#include "sfml-util/gui/radio-button.hpp"

#include "popup/i-popup-callback.hpp"

#include <memory>
#include <string>


namespace heroespath
{
namespace sfml_util
{

    //Represents a RadioButtonSet that changes the display
    class RadioButtonSet_DisplayChange
    :
        public heroespath::popup::IPopupHandler_t,
        public sfml_util::gui::RadioButtonSet
    {
        RadioButtonSet_DisplayChange(const RadioButtonSet_DisplayChange &) =delete;
        RadioButtonSet_DisplayChange & operator=(const RadioButtonSet_DisplayChange &) =delete;

    public:
        //If Using this constructor, then one of the Setup()
        //functions must be called before any other member.
        RadioButtonSet_DisplayChange(
            const float POS_LEFT,
            const float POS_TOP,
            sfml_util::IStage * const OWNER_STAGE_PTR);

        virtual ~RadioButtonSet_DisplayChange();

        inline virtual const std::string HandlerName() const { return GetEntityName(); }

        virtual bool HandleCallback(const heroespath::popup::PopupResponse &);

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
}

#endif //SFMLUTIL_RADIOBUTTONDISPLAY_HPP_INCLUDED
