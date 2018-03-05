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
#ifndef HEROESPATH_SFMLUTIL_STAGE_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_STAGE_HPP_INCLUDED
//
// stage.hpp
//  Helper classes and functions for managing GuiEntity's in a collection on screen.
//
#include "sfml-util/i-stage.hpp"
#include "sfml-util/sfml-graphics.hpp"

#include <memory>
#include <set>
#include <string>
#include <vector>

namespace heroespath
{
namespace sfml_util
{
    namespace gui
    {
        class IGuiEntity;
        using IGuiEntityPtr_t = IGuiEntity *;
        using IGuiEntityPVec_t = std::vector<IGuiEntityPtr_t>;

        namespace box
        {
            class Box;
            using BoxUPtr_t = std::unique_ptr<Box>;
        } // namespace box
    } // namespace gui

    // A base class for types that hold and draw a group of on screen resources.
    class Stage : public IStage
    {
    public:
        Stage(const Stage &) = delete;
        Stage(const Stage &&) = delete;
        Stage & operator=(const Stage &) = delete;
        Stage & operator=(const Stage &&) = delete;

        explicit Stage(const std::string & NAME, const bool WILL_CLEAR_CACHE_ON_EXIT = true);

        Stage(
            const std::string & NAME,
            const sf::FloatRect & REGION,
            const bool WILL_CLEAR_CACHE_ON_EXIT = true);

        Stage(
            const std::string & NAME,
            const float REGION_LEFT,
            const float REGION_TOP,
            const float REGION_WIDTH,
            const float REGION_HEIGHT,
            const bool WILL_CLEAR_CACHE_ON_EXIT = true);

        virtual ~Stage();

        inline virtual void Setup() {}

        inline virtual const std::string GetStageName() const { return STAGE_NAME_; }

        inline virtual const sf::FloatRect StageRegion() const { return stageRegion_; }
        inline virtual void StageRegionSet(const sf::FloatRect & RECT) { stageRegion_ = RECT; }

        inline virtual float StageRegionLeft() const { return stageRegion_.left; }
        inline virtual float StageRegionTop() const { return stageRegion_.top; }
        inline virtual float StageRegionWidth() const { return stageRegion_.width; }
        inline virtual float StageRegionHeight() const { return stageRegion_.height; }

        virtual void UpdateTime(const float ELAPSED_TIME_SECONDS);
        virtual void UpdateMousePos(const sf::Vector2i & NEW_MOUSE_POS);
        virtual void UpdateMouseDown(const sf::Vector2f & MOUSE_POS_V);
        virtual void
            UpdateMouseWheel(const sf::Vector2f & MOUSE_POS_V, const float MOUSEWHEEL_DELTA);

        virtual gui::IGuiEntityPtr_t UpdateMouseUp(const sf::Vector2f & MOUSE_POS_V);

        virtual bool KeyPress(const sf::Event::KeyEvent & KE);
        virtual bool KeyRelease(const sf::Event::KeyEvent & KE);

        inline virtual gui::IGuiEntityPtr_t GetEntityWithFocus() const
        {
            return entityWithFocusPtr_;
        }

        virtual void RemoveFocus();
        virtual bool SetFocus(const gui::IGuiEntityPtr_t ENTITY_PTR);

        virtual void Draw(sf::RenderTarget & target, const sf::RenderStates & STATES);

        // only a required function for the SettingsStage which can change resolution
        virtual void HandleResolutionChange() {}

        // throws if the entity to add was already there
        virtual void EntityAdd(const gui::IGuiEntityPtr_t);

        // returns false if the entity to remove was not found
        virtual bool EntityRemove(const gui::IGuiEntityPtr_t);

        virtual void SetMouseHover(const sf::Vector2f &, const bool);

        inline virtual void TestingStrAppend(const std::string &) {}
        inline virtual void TestingStrIncrement(const std::string &) {}

        inline virtual void TestingImageSet(
            const sf::Texture &,
            const bool = false,
            const std::string & = "",
            const std::string & = "",
            const std::string & = "")
        {}

        inline virtual void PerformNextTest() {}

        virtual void ClearAllEntities();

        inline virtual bool IsMouseHeldDown() const { return isMouseHeldDown_; }
        inline virtual bool IsMouseHeldDownAndMoving() const { return isMouseHeldDownAndMoving_; }
        inline virtual const sf::Vector2f MouseDownPosV() const { return mouseDownPosV_; }

    protected:
        void DrawHoverText(sf::RenderTarget &, const sf::RenderStates &);

    private:
        static const float MOUSE_DRAG_MIN_DISTANCE_;
        //
        const std::string STAGE_NAME_;
        sf::FloatRect stageRegion_;
        gui::IGuiEntityPVec_t entityPVec_;
        gui::IGuiEntityPtr_t entityWithFocusPtr_; // a copy of a ptr in entityPVec_
        gui::box::BoxUPtr_t hoverTextBoxUPtr_;
        sf::Text hoverSfText_;

    protected:
        bool isMouseHeldDown_;
        bool isMouseHeldDownAndMoving_;
        sf::Vector2f mouseDownPosV_;
        bool willClearCachesOnExit_;
    };
} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_STAGE_HPP_INCLUDED
