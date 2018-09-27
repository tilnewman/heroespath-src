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
//  Helper classes and functions for managing Entity's in a collection on screen.
//
#include "misc/boost-optional-that-throws.hpp"
#include "misc/not-null.hpp"
#include "sfml-util/font-enum.hpp"
#include "sfml-util/i-stage.hpp"
#include "sfml-util/sound-effects-enum.hpp"
#include "sfml-util/text.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>

#include <memory>
#include <string>
#include <vector>

namespace heroespath
{
namespace sfml_util
{
    namespace gui
    {
        class IEntity;
        using IEntityPtr_t = misc::NotNull<IEntity *>;
        using IEntityPVec_t = std::vector<IEntityPtr_t>;
        using IEntityPtrOpt_t = boost::optional<IEntityPtr_t>;

        class BoxEntity;
        using BoxEntityUPtr_t = std::unique_ptr<BoxEntity>;

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

        void Setup() override {}

        const std::string GetStageName() const override final { return STAGE_NAME_; }

        const sf::FloatRect StageRegion() const override final { return stageRegion_; }
        void StageRegionSet(const sf::FloatRect & RECT) override final { stageRegion_ = RECT; }

        float StageRegionLeft() const override final { return stageRegion_.left; }
        float StageRegionTop() const override final { return stageRegion_.top; }
        float StageRegionWidth() const override final { return stageRegion_.width; }
        float StageRegionHeight() const override final { return stageRegion_.height; }

        void UpdateTime(const float ELAPSED_TIME_SECONDS) override;
        void UpdateMousePos(const sf::Vector2i & NEW_MOUSE_POS) override;
        void UpdateMouseDown(const sf::Vector2f & MOUSE_POS_V) override;

        void UpdateMouseWheel(
            const sf::Vector2f & MOUSE_POS_V, const float MOUSEWHEEL_DELTA) override;

        const gui::IEntityPtrOpt_t UpdateMouseUp(const sf::Vector2f & MOUSE_POS_V) override;

        bool KeyPress(const sf::Event::KeyEvent & KE) override;
        bool KeyRelease(const sf::Event::KeyEvent & KE) override;

        const gui::IEntityPtrOpt_t GetEntityWithFocus() const override final
        {
            return entityWithFocusPtrOpt_;
        }

        void RemoveFocus() override final;
        void SetFocus(const gui::IEntityPtr_t ENTITY_PTR) override final;

        void Draw(sf::RenderTarget & target, const sf::RenderStates & STATES) override;

        // only a required function for the SettingsStage which can change resolution
        void HandleResolutionChange() override {}

        // throws if the entity to add was already there
        void EntityAdd(
            const gui::IEntityPtr_t,
            const bool WILL_INSERT_AT_FRONT_INSTEAD_OF_BACK = false) override final;

        // returns false if the entity to remove was not found
        bool EntityRemove(const gui::IEntityPtr_t) override final;

        void SetMouseHover(const sf::Vector2f &, const bool IS_MOUSE_HOVERING_NOW) override final;

        void TestingStrAppend(const std::string &) override {}
        void TestingStrIncrement(const std::string &) override {}

        void TestingImageSet(const std::string &, const bool = false) override {}

        void PerformNextTest() override {}
        void ClearAllEntities() override final;

        bool IsMouseHeldDown() const override final { return isMouseHeldDown_; }
        bool IsMouseHeldDownAndMoving() const override final { return isMouseHeldDownAndMoving_; }
        const sf::Vector2f MouseDownPosV() const override final { return mouseDownPosV_; }

    private:
        static const float MOUSE_DRAG_MIN_DISTANCE_;
        //
        const std::string STAGE_NAME_;
        sf::FloatRect stageRegion_;

        // these are observer pointers whose lifetime is not controlled by this class
        gui::IEntityPVec_t entityPVec_;

        // a copy of a ptr in entityPVec_
        gui::IEntityPtrOpt_t entityWithFocusPtrOpt_;

        gui::BoxEntityUPtr_t hoverTextBoxUPtr_;
        sfml_util::Text hoverText_;

    protected:
        bool isMouseHeldDown_;
        bool isMouseHeldDownAndMoving_;
        sf::Vector2f mouseDownPosV_;
        bool willClearCachesOnExit_;
    };

} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_STAGE_HPP_INCLUDED
