// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_STAGE_STAGE_BASE_HPP_INCLUDED
#define HEROESPATH_STAGE_STAGE_BASE_HPP_INCLUDED
//
// stage-base.hpp
//
#include "gui/font-enum.hpp"
#include "gui/sound-effects-enum.hpp"
#include "gui/text.hpp"
#include "misc/boost-optional-that-throws.hpp"
#include "misc/not-null.hpp"
#include "stage/i-stage.hpp"

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
    using IEntityPVec_t = std::vector<IEntityPtr_t>;
    using IEntityPtrOpt_t = boost::optional<IEntityPtr_t>;

    class BoxEntity;
    using BoxEntityUPtr_t = std::unique_ptr<BoxEntity>;
} // namespace gui

namespace stage
{

    // A base class for types that hold and draw a group of on screen resources.
    class StageBase : public IStage
    {
    public:
        StageBase(
            const std::string & NAME,
            const gui::FontEnumVec_t & FONTS_TO_PRELOAD,
            const gui::SfxEnumVec_t & SFX_TO_PRELOAD = {});

        StageBase(
            const std::string & NAME,
            const sf::FloatRect & REGION,
            const gui::FontEnumVec_t & FONTS_TO_PRELOAD,
            const gui::SfxEnumVec_t & SFX_TO_PRELOAD = {});

        virtual ~StageBase();

        StageBase(const StageBase &) = delete;
        StageBase(StageBase &&) = delete;
        StageBase & operator=(const StageBase &) = delete;
        StageBase & operator=(StageBase &&) = delete;

        const std::string GetStageName() const override { return STAGE_NAME_; }

        game::Phase::Enum GetPhase() const override;

        const std::string MakeCallbackHandlerMessage(
            const std::string & EVENT_DESCRIPTION, const std::string & ACTION_TAKEN) const final;

        const sf::FloatRect StageRegion() const override { return stageRegion_; }
        void StageRegion(const sf::FloatRect & RECT) override { stageRegion_ = RECT; }

        void UpdateTime(const float ELAPSED_TIME_SECONDS) override;
        void UpdateMousePos(const sf::Vector2i & NEW_MOUSE_POS) override;
        void UpdateMouseDown(const sf::Vector2f & MOUSE_POS_V) override;

        void UpdateMouseWheel(
            const sf::Vector2f & MOUSE_POS_V, const float MOUSEWHEEL_DELTA) override;

        const gui::IEntityPtrOpt_t UpdateMouseUp(const sf::Vector2f & MOUSE_POS_V) override;

        bool KeyPress(const sf::Event::KeyEvent & KE) override;
        bool KeyRelease(const sf::Event::KeyEvent & KE) override;

        const gui::IEntityPtrOpt_t GetEntityWithFocus() const override
        {
            return entityWithFocusPtrOpt_;
        }

        void RemoveFocus() override;
        void SetFocus(const gui::IEntityPtr_t ENTITY_PTR) override;

        void draw(sf::RenderTarget &, sf::RenderStates) const override;

        void HandleResolutionChange() override {}

        void EntityAdd(
            const gui::IEntityPtr_t,
            const bool WILL_INSERT_AT_FRONT_INSTEAD_OF_BACK = false) override;

        void EntityRemove(const gui::IEntityPtr_t) override;

        void SetMouseHover(const sf::Vector2f &, const bool IS_MOUSE_HOVERING_NOW) override;

        void TestingStrAppend(const std::string &) override;
        void TestingStrIncrement(const std::string &) override;
        void TestingImageSet(const std::string &) override;

        void PerformNextTest() override {}
        void ClearAllEntities() override;

        bool IsMouseHeldDown() const override { return isMouseHeldDown_; }
        bool IsMouseHeldDownAndMoving() const override { return isMouseHeldDownAndMoving_; }
        const sf::Vector2f MouseDownPosV() const override { return mouseDownPosV_; }

        bool IsFading() const override { return isFading_; }
        void IsFading(const bool IS_FADING) override { isFading_ = IS_FADING; }

        void SpawnPopup(
            const misc::PopupCallback_t::IHandlerPtr_t POPUP_HANDLER_PTR,
            const popup::PopupInfo & POPUP_INFO) const override;

        void RemovePopup(
            const popup::PopupButtons::Enum TYPE, const std::size_t SELECTION = 0) const override;

        void TransitionTo(const stage::Stage::Enum NEW_STAGE) const override;

        void TransitionTo(const stage::SetupPacket & SETUP_PACKET) const override;

        const gui::DisplayChangeResult ChangeResolution(
            const misc::PopupCallback_t::IHandlerPtr_t POPUP_HANDLER_PTR,
            const gui::Resolution & NEW_RES,
            const unsigned ANTIALIAS_LEVEL) const override;

    private:
        void StageBaseCommonSetupTasks(
            const gui::FontEnumVec_t & FONTS_TO_PRELOAD, const gui::SfxEnumVec_t & SFX_TO_PRELOAD);

    private:
        static const float MOUSE_DRAG_MIN_DISTANCE_;
        //
        const std::string STAGE_NAME_;
        sf::FloatRect stageRegion_;

        // these are observer pointers whose lifetime is not controlled by this class
        gui::IEntityPVec_t entityPVec_;

        // a copy of an observer ptr in entityPVec_
        gui::IEntityPtrOpt_t entityWithFocusPtrOpt_;

        gui::BoxEntityUPtr_t hoverTextBoxUPtr_;
        gui::Text hoverText_;
        bool isFading_;

    protected:
        bool isMouseHeldDown_;
        bool isMouseHeldDownAndMoving_;
        sf::Vector2f mouseDownPosV_;
    };

} // namespace stage
} // namespace heroespath

#endif // HEROESPATH_STAGE_STAGE_BASE_HPP_INCLUDED
