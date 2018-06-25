// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_STAGE_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_STAGE_HPP_INCLUDED
//
// stage.hpp
//  Helper classes and functions for managing GuiEntity's in a collection on screen.
//
#include "misc/boost-optional-that-throws.hpp"
#include "misc/not-null.hpp"
#include "sfml-util/font-enum.hpp"
#include "sfml-util/i-stage.hpp"
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/sound-effects-enum.hpp"

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
        using IGuiEntityPtr_t = misc::NotNull<IGuiEntity *>;
        using IGuiEntityPVec_t = std::vector<IGuiEntityPtr_t>;
        using IGuiEntityPtrOpt_t = boost::optional<IGuiEntityPtr_t>;

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
        Stage(Stage &&) = delete;
        Stage & operator=(const Stage &) = delete;
        Stage & operator=(Stage &&) = delete;

        Stage(
            const std::string & NAME,
            const FontEnumVec_t & FONTS_TO_PRELOAD,
            const bool WILL_CLEAR_CACHE_ON_EXIT,
            const SfxEnumVec_t & SFX_TO_PRELOAD = {});

        Stage(
            const std::string & NAME,
            const sf::FloatRect & REGION,
            const FontEnumVec_t & FONTS_TO_PRELOAD,
            const bool WILL_CLEAR_CACHE_ON_EXIT,
            const SfxEnumVec_t & SFX_TO_PRELOAD = {});

        Stage(
            const std::string & NAME,
            const float REGION_LEFT,
            const float REGION_TOP,
            const float REGION_WIDTH,
            const float REGION_HEIGHT,
            const FontEnumVec_t & FONTS_TO_PRELOAD,
            const bool WILL_CLEAR_CACHE_ON_EXIT,
            const SfxEnumVec_t & SFX_TO_PRELOAD = {});

        virtual ~Stage();

        virtual void Setup() {}

        virtual const std::string GetStageName() const { return STAGE_NAME_; }

        virtual const sf::FloatRect StageRegion() const { return stageRegion_; }
        virtual void StageRegionSet(const sf::FloatRect & RECT) { stageRegion_ = RECT; }

        virtual float StageRegionLeft() const { return stageRegion_.left; }
        virtual float StageRegionTop() const { return stageRegion_.top; }
        virtual float StageRegionWidth() const { return stageRegion_.width; }
        virtual float StageRegionHeight() const { return stageRegion_.height; }

        virtual void UpdateTime(const float ELAPSED_TIME_SECONDS);
        virtual void UpdateMousePos(const sf::Vector2i & NEW_MOUSE_POS);
        virtual void UpdateMouseDown(const sf::Vector2f & MOUSE_POS_V);

        virtual void
            UpdateMouseWheel(const sf::Vector2f & MOUSE_POS_V, const float MOUSEWHEEL_DELTA);

        virtual const gui::IGuiEntityPtrOpt_t UpdateMouseUp(const sf::Vector2f & MOUSE_POS_V);

        virtual bool KeyPress(const sf::Event::KeyEvent & KE);
        virtual bool KeyRelease(const sf::Event::KeyEvent & KE);

        virtual const gui::IGuiEntityPtrOpt_t GetEntityWithFocus() const
        {
            return entityWithFocusPtrOpt_;
        }

        virtual void RemoveFocus();
        virtual void SetFocus(const gui::IGuiEntityPtr_t ENTITY_PTR);

        virtual void Draw(sf::RenderTarget & target, const sf::RenderStates & STATES);

        // only a required function for the SettingsStage which can change resolution
        virtual void HandleResolutionChange() {}

        // throws if the entity to add was already there
        virtual void EntityAdd(const gui::IGuiEntityPtr_t);

        // returns false if the entity to remove was not found
        virtual bool EntityRemove(const gui::IGuiEntityPtr_t);

        virtual void SetMouseHover(const sf::Vector2f &, const bool);

        virtual void TestingStrAppend(const std::string &) {}
        virtual void TestingStrIncrement(const std::string &) {}

        virtual void TestingImageSet(const std::string &, const bool = false) {}

        virtual void PerformNextTest() {}
        virtual void ClearAllEntities();

        virtual bool IsMouseHeldDown() const { return isMouseHeldDown_; }
        virtual bool IsMouseHeldDownAndMoving() const { return isMouseHeldDownAndMoving_; }
        virtual const sf::Vector2f MouseDownPosV() const { return mouseDownPosV_; }

    protected:
        void DrawHoverText(sf::RenderTarget &, const sf::RenderStates &);

    private:
        static const float MOUSE_DRAG_MIN_DISTANCE_;
        //
        const std::string STAGE_NAME_;
        sf::FloatRect stageRegion_;

        // these are observer pointers whose lifetime is not controlled by this class
        gui::IGuiEntityPVec_t entityPVec_;

        // a copy of a ptr in entityPVec_
        gui::IGuiEntityPtrOpt_t entityWithFocusPtrOpt_;

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
