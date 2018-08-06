// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_ISTAGE_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_ISTAGE_HPP_INCLUDED
//
// i-stage.hpp
//
#include "misc/boost-optional-that-throws.hpp"
#include "misc/not-null.hpp"

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Window/Event.hpp>

#include <string>

namespace sf
{
class RenderTarget;
class RenderStates;
} // namespace sf

namespace heroespath
{
namespace sfml_util
{

    namespace gui
    {
        class IEntity;
        using IEntityPtr_t = misc::NotNull<IEntity *>;
        using IEntityPtrOpt_t = boost::optional<IEntityPtr_t>;
    } // namespace gui

    // interface for all Stage classes
    class IStage
    {
    public:
        virtual ~IStage() = default;

        virtual void Setup() = 0;

        virtual const std::string GetStageName() const = 0;

        virtual const sf::FloatRect StageRegion() const = 0;
        virtual void StageRegionSet(const sf::FloatRect & RECT) = 0;

        virtual float StageRegionLeft() const = 0;
        virtual float StageRegionTop() const = 0;
        virtual float StageRegionWidth() const = 0;
        virtual float StageRegionHeight() const = 0;

        virtual void UpdateTime(const float ELAPSED_TIME_SECONDS) = 0;
        virtual void UpdateMousePos(const sf::Vector2i & MOUSE_POS_V) = 0;
        virtual void UpdateMouseDown(const sf::Vector2f & MOUSE_POS_V) = 0;

        virtual void
            UpdateMouseWheel(const sf::Vector2f & MOUSE_POS_V, const float MOUSEWHEEL_DELTA)
            = 0;

        virtual const gui::IEntityPtrOpt_t UpdateMouseUp(const sf::Vector2f & MOUSE_POS_V) = 0;

        virtual bool KeyPress(const sf::Event::KeyEvent & KE) = 0;
        virtual bool KeyRelease(const sf::Event::KeyEvent & KE) = 0;

        virtual const gui::IEntityPtrOpt_t GetEntityWithFocus() const = 0;
        virtual void RemoveFocus() = 0;
        virtual void SetFocus(const gui::IEntityPtr_t ENTITY_PTR) = 0;

        virtual void Draw(sf::RenderTarget & target, const sf::RenderStates &) = 0;

        virtual void HandleResolutionChange() = 0;

        // throws if the entity to add was already there
        virtual void EntityAdd(
            const gui::IEntityPtr_t, const bool WILL_INSERT_AT_FRONT_INSTEAD_OF_BACK = false)
            = 0;

        // returns false if the entity to remove was not found
        virtual bool EntityRemove(const gui::IEntityPtr_t) = 0;

        virtual void SetMouseHover(const sf::Vector2f &, const bool IS_MOUSE_HOVERING_NOW) = 0;

        virtual void TestingStrAppend(const std::string &) = 0;
        virtual void TestingStrIncrement(const std::string &) = 0;

        virtual void
            TestingImageSet(const std::string & PATH_STR, const bool WILL_CHECK_FOR_OUTLINE = false)
            = 0;

        virtual void PerformNextTest() = 0;
        virtual void ClearAllEntities() = 0;
        virtual bool IsMouseHeldDown() const = 0;
        virtual bool IsMouseHeldDownAndMoving() const = 0;
        virtual const sf::Vector2f MouseDownPosV() const = 0;
    };

    using IStagePtr_t = misc::NotNull<IStage *>;
    using IStagePtrOpt_t = boost::optional<IStagePtr_t>;

} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_ISTAGE_HPP_INCLUDED
