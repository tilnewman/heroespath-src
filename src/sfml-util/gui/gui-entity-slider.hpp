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
#ifndef SFMLUTIL_GUI_GUI_ENTITY_SLIDER_HPP_INCLUDED
#define SFMLUTIL_GUI_GUI_ENTITY_SLIDER_HPP_INCLUDED
//
//  gui-entity-slider.hpp
//
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/pos-slider.hpp"


namespace sfml_util
{
namespace gui
{

    //forward declarations
    class IGuiEntity;
    using IGuiEntityPtr_t = IGuiEntity *;


    //Responsible for moving a GuiEntity back and forth between two points using the slider method.
    class GuiEntitySlider : public sliders::PosSlider
    {
    public:
        explicit GuiEntitySlider(
            IGuiEntityPtr_t      guiEntityPtr = nullptr,
            const sf::Vector2f & FROM_POS_V   = sf::Vector2f(0.0f, 0.0f),
            const sf::Vector2f & TO_POS_V     = sf::Vector2f(0.0f, 0.0f),
            const float          SLIDER_SPEED = 1.0f);

        virtual ~GuiEntitySlider();

        void Setup(
            IGuiEntityPtr_t      guiEntityPtr  = nullptr,
            const sf::Vector2f & FROM_POS_V    = sf::Vector2f(0.0f, 0.0f),
            const sf::Vector2f & TO_POS_V      = sf::Vector2f(0.0f, 0.0f),
            const float          SLIDER_SPEED  = 1.0f);

        virtual bool UpdateTime(const float ELAPSED_TIME_SECONDS) override;

        inline IGuiEntityPtr_t GetEntity() const { return guiEntityPtr_; }
        inline void SetEntity(const IGuiEntityPtr_t PTR) { guiEntityPtr_ = PTR; }

    private:

        //This is an observer pointer that does not control the lifetime of the object.
        IGuiEntityPtr_t guiEntityPtr_;
    };

}
}


#endif //SFMLUTIL_GUI_GUI_ENTITY_SLIDER_HPP_INCLUDED
