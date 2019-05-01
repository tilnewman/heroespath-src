// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_STAGE_ISTAGE_HPP_INCLUDED
#define HEROESPATH_STAGE_ISTAGE_HPP_INCLUDED
//
// i-stage.hpp
//
#include "game/phase-enum.hpp"
#include "gui/display-change-result.hpp"
#include "misc/boost-optional-that-throws.hpp"
#include "misc/callback.hpp"
#include "misc/not-null.hpp"
#include "popup/popup-enums.hpp"
#include "stage/stage-enum.hpp"

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Window/Event.hpp>

#include <memory>
#include <string>
#include <vector>

namespace sf
{
class RenderTarget;
class RenderStates;
} // namespace sf

namespace heroespath
{

namespace gui
{
    class IEntity;
    using IEntityPtr_t = misc::NotNull<IEntity *>;
    using IEntityPtrOpt_t = boost::optional<IEntityPtr_t>;

    struct Resolution;
} // namespace gui

namespace popup
{
    class PopupInfo;
}
namespace stage
{
    struct SetupPacket;

    // A common pure-virtual interface for all Stage classes.
    struct IStage : public sf::Drawable
    {
        virtual ~IStage() = default;

        virtual void Setup() = 0;

        virtual const std::string GetStageName() const = 0;

        virtual game::Phase::Enum GetPhase() const = 0;

        virtual const std::string MakeCallbackHandlerMessage(
            const std::string & EVENT_DESCRIPTION, const std::string & ACTION_TAKEN) const = 0;

        virtual const sf::FloatRect StageRegion() const = 0;
        virtual void StageRegion(const sf::FloatRect & RECT) = 0;

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

        template <typename T>
        void SetFocus(const std::unique_ptr<T> & UPTR)
        {
            SetFocus(gui::IEntityPtr_t(UPTR.get()));
        }

        virtual void draw(sf::RenderTarget &, sf::RenderStates) const = 0;

        virtual void HandleResolutionChange() = 0;

        virtual void EntityAdd(
            const gui::IEntityPtr_t, const bool WILL_INSERT_AT_FRONT_INSTEAD_OF_BACK = false)
            = 0;

        template <typename T>
        void EntityAdd(
            const std::unique_ptr<T> & UPTR,
            const bool WILL_INSERT_AT_FRONT_INSTEAD_OF_BACK = false)
        {
            EntityAdd(gui::IEntityPtr_t(UPTR.get()), WILL_INSERT_AT_FRONT_INSTEAD_OF_BACK);
        }

        virtual void EntityRemove(const gui::IEntityPtr_t) = 0;

        template <typename T>
        void EntityRemove(const std::unique_ptr<T> & UPTR)
        {
            EntityRemove(gui::IEntityPtr_t(UPTR.get()));
        }

        virtual void SetMouseHover(const sf::Vector2f &, const bool IS_MOUSE_HOVERING_NOW) = 0;

        // all of these testing functions are temp until all tests are moved to unit tests
        virtual void TestingStrAppend(const std::string &) = 0;
        virtual void TestingStrIncrement(const std::string &) = 0;
        virtual void TestingImageSet(const std::string & PATH_STR) = 0;

        virtual void PerformNextTest() = 0;

        virtual void ClearAllEntities() = 0;
        virtual bool IsMouseHeldDown() const = 0;
        virtual bool IsMouseHeldDownAndMoving() const = 0;
        virtual const sf::Vector2f MouseDownPosV() const = 0;

        virtual bool IsFading() const = 0;
        virtual void IsFading(const bool IS_FADING) = 0;

        virtual void SpawnPopup(
            const misc::PopupCallback_t::IHandlerPtr_t POPUP_HANDLER_PTR,
            const popup::PopupInfo & POPUP_INFO) const = 0;

        virtual void RemovePopup(
            const popup::PopupButtons::Enum TYPE, const std::size_t SELECTION = 0) const = 0;

        virtual void TransitionTo(const stage::Stage::Enum NEW_STAGE) const = 0;
        virtual void TransitionTo(const stage::SetupPacket & SETUP_PACKET) const = 0;

        virtual const gui::DisplayChangeResult ChangeResolution(
            const misc::PopupCallback_t::IHandlerPtr_t POPUP_HANDLER_PTR,
            const gui::Resolution & NEW_RES,
            const unsigned ANTIALIAS_LEVEL) const = 0;
    };

    using IStageUPtr_t = std::unique_ptr<IStage>;
    using IStageUVec_t = std::vector<IStageUPtr_t>;
    using IStagePtr_t = misc::NotNull<IStage *>;
    using IStagePVec_t = std::vector<IStagePtr_t>;
    using IStagePtrOpt_t = boost::optional<IStagePtr_t>;

} // namespace stage
} // namespace heroespath

#endif // HEROESPATH_STAGE_ISTAGE_HPP_INCLUDED
