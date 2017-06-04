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
#ifndef SFMLUTIL_ISTAGE_INCLUDED
#define SFMLUTIL_ISTAGE_INCLUDED
//
// i-stage.hpp
//  Interface for all Stage objects.
//
#include "sfml-util/sfml-graphics.hpp"

#include <memory>
#include <string>
#include <vector>


namespace sfml_util
{

    //forward declarations
    namespace gui
    {
        class IGuiEntity;
        using IGuiEntityPtr_t = IGuiEntity *;
    }


    //interface for all Stage classes
    class IStage
    {
    public:
        virtual ~IStage() {}

        virtual void Setup() = 0;

        virtual const std::string GetStageName() const = 0;

        inline virtual const sf::FloatRect StageRegion() const = 0;
        inline virtual void StageRegionSet(const sf::FloatRect & RECT) = 0;

        virtual float StageRegionLeft() const = 0;
        virtual float StageRegionTop() const = 0;
        virtual float StageRegionWidth() const = 0;
        virtual float StageRegionHeight() const = 0;

        virtual void UpdateTime(const float ELAPSED_TIME_SECONDS) = 0;
        virtual void UpdateMousePos(const sf::Vector2f & MOUSE_POS_V) = 0;
        virtual void UpdateMouseDown(const sf::Vector2f & MOUSE_POS_V) = 0;
        virtual void UpdateMouseWheel(const sf::Vector2f & MOUSE_POS_V, const float MOUSEWHEEL_DELTA) = 0;

        virtual gui::IGuiEntityPtr_t UpdateMouseUp(const sf::Vector2f & MOUSE_POS_V) = 0;

        virtual bool KeyPress(const sf::Event::KeyEvent & KE) = 0;
        virtual bool KeyRelease(const sf::Event::KeyEvent & KE) = 0;

        virtual gui::IGuiEntityPtr_t GetEntityWithFocus() const = 0;
        virtual void RemoveFocus() = 0;
        virtual bool SetFocus(const gui::IGuiEntityPtr_t ENTITY_PTR) = 0;

        virtual void Draw(sf::RenderTarget & target, const sf::RenderStates &) = 0;

        virtual void HandleResolutionChange() = 0;

        //throws if the entity to add was already there
        virtual void EntityAdd(const gui::IGuiEntityPtr_t) = 0;

        //returns false if the entity to remove was not found
        virtual bool EntityRemove(const gui::IGuiEntityPtr_t) = 0;

        virtual void SetMouseHover(const sf::Vector2f &, const bool) = 0;

        virtual void TestingStrAppend(const std::string &) = 0;
        virtual void TestingStrIncrement(const std::string &) = 0;
        virtual void TestingImageSet(const sf::Texture &) = 0;
        virtual void PerformNextTest() = 0;

        virtual void ClearAllEntities() = 0;

        virtual bool IsMouseHeldDown() const = 0;
        virtual bool IsMouseHeldDownAndMoving() const = 0;
        virtual const sf::Vector2f MouseDownPosV() const = 0;
    };

    using IStagePtr_t = IStage *;
    using IStagePVec_t = std::vector<IStagePtr_t>;

}

#endif //SFMLUTIL_ISTAGE_INCLUDED
