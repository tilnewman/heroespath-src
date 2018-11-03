// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_STAGE_ADVENTURE_STAGE_INTERACT_STAGE_REGION_HPP_INCLUDED
#define HEROESPATH_STAGE_ADVENTURE_STAGE_INTERACT_STAGE_REGION_HPP_INCLUDED
//
// adventure-stage-interact-stage.hpp
//
#include "gui/callback.hpp"
#include "gui/colored-rect.hpp"
#include "gui/text-button.hpp"
#include "interact/interaction-manager.hpp"
#include "interact/lock-interactions.hpp"
#include "map/transition.hpp"
#include "misc/boost-optional-that-throws.hpp"
#include "misc/not-null.hpp"
#include "stage/stage-base.hpp"

#include <SFML/Graphics/Sprite.hpp>

#include <memory>
#include <string>
#include <vector>

namespace heroespath
{
namespace gui
{
    class TextRegion;
    using TextRegionUPtr_t = std::unique_ptr<TextRegion>;
} // namespace gui
namespace map
{
    class Map;
    using MapPtr_t = misc::NotNull<Map *>;
    using MapPtrOpt_t = boost::optional<MapPtr_t>;
} // namespace map
namespace stage
{

    // Responsible for the interaction region of the Adventure Stage.
    class InteractStage
        : public stage::StageBase

        , public gui::TextButton::Callback_t::IHandler_t
        , public gui::PopupCallback_t::IHandler_t
    {
    public:
        InteractStage(const InteractStage &) = delete;
        InteractStage(InteractStage &&) = delete;
        InteractStage & operator=(const InteractStage &) = delete;
        InteractStage & operator=(InteractStage &&) = delete;

        explicit InteractStage(interact::InteractionManager &);

        virtual ~InteractStage();

        bool HandleCallback(const gui::TextButton::Callback_t::PacketPtr_t &) final;
        bool HandleCallback(const gui::PopupCallback_t::PacketPtr_t &) override;

        void PreSetup(const sf::FloatRect & STAGE_REGION, map::MapPtr_t mapPtr);

        void Setup() override;
        void draw(sf::RenderTarget &, sf::RenderStates) const override;
        void UpdateTime(const float ELAPSED_TIME_SECONDS) override;
        bool KeyRelease(const sf::Event::KeyEvent &) override;

        interact::InteractionManager & InteractionManager() { return interactionManager_; }

        interact::LockPicking & LockPick() { return lockPicking_; }

        void MapTransition(const map::Transition &);

    private:
        void SetupInteractionForDrawing(const interact::IInteractionPtrOpt_t);
        void DrawInteraction(sf::RenderTarget &) const;

    private:
        static const sf::Uint8 BACKGROUND_ALPHA_;
        static const sf::Uint8 CONTEXT_IMAGE_ALPHA_;
        static const float SUBJECT_REGION_WIDTH_RATIO_;
        static const float SUBJECT_REGION_HEIGHT_RATIO_;
        static const float SUBJECT_IMAGE_PAD_RATIO_;
        static const float CONTEXT_IMAGE_PAD_RATIO_;

        map::MapPtrOpt_t mapPtrOpt_;
        float regionPad_;
        // sf::FloatRect outerRect_; the outer rect is StageBase::StageRegion()
        sf::FloatRect innerRect_;
        interact::InteractionManager & interactionManager_;
        sf::Sprite subjectSprite_;
        sf::Sprite contextSprite_;
        sf::Sprite npcSprite_;
        gui::TextRegionUPtr_t textRegionUPtr_;
        std::vector<gui::TextButtonUPtr_t> buttons_;
        interact::LockPicking lockPicking_;
        gui::ColoredRect backgroundColoredRect_;
    };

    using InteractStagePtr_t = misc::NotNull<InteractStage *>;
    using InteractStagePtrOpt_t = boost::optional<InteractStagePtr_t>;

} // namespace stage
} // namespace heroespath

#endif // HEROESPATH_STAGE_ADVENTURE_STAGE_INTERACT_STAGE_REGION_HPP_INCLUDED
