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
#include "interact/interaction-manager.hpp"
#include "interact/lock-interactions.hpp"
#include "map/transition.hpp"
#include "misc/boost-optional-that-throws.hpp"
#include "misc/not-null.hpp"
#include "sfml-util/callback.hpp"
#include "sfml-util/colored-rect.hpp"
#include "sfml-util/stage.hpp"
#include "sfml-util/text-button.hpp"

#include <SFML/Graphics/Sprite.hpp>

#include <memory>
#include <string>
#include <vector>

namespace heroespath
{
namespace sfml_util
{
    class TextRegion;
    using TextRegionUPtr_t = std::unique_ptr<TextRegion>;
} // namespace sfml_util
namespace map
{
    class Map;
}
namespace stage
{

    // Responsible for the interaction region of the Adventure Stage.
    class InteractStage
        : public sfml_util::Stage
        , public sfml_util::TextButton::Callback_t::IHandler_t
        , public sfml_util::PopupCallback_t::IHandler_t
    {
    public:
        InteractStage(const InteractStage &) = delete;
        InteractStage(InteractStage &&) = delete;
        InteractStage & operator=(const InteractStage &) = delete;
        InteractStage & operator=(InteractStage &&) = delete;

    public:
        InteractStage(
            map::Map &, const sf::FloatRect & STAGE_REGION, interact::InteractionManager &);

        virtual ~InteractStage();

        bool HandleCallback(const sfml_util::TextButton::Callback_t::PacketPtr_t &) final;

        bool HandleCallback(const sfml_util::PopupCallback_t::PacketPtr_t &) override;

        void Setup() override;
        void Draw(sf::RenderTarget &, const sf::RenderStates &) override;
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

        map::Map & map_;
        float regionPad_;
        // sf::FloatRect outerRect_; the outer rect is Stage::StageRegion()
        sf::FloatRect innerRect_;
        interact::InteractionManager & interactionManager_;
        sf::Sprite subjectSprite_;
        sf::Sprite contextSprite_;
        sf::Sprite npcSprite_;
        sfml_util::TextRegionUPtr_t textRegionUPtr_;
        std::vector<sfml_util::TextButtonUPtr_t> buttons_;
        interact::LockPicking lockPicking_;
        sfml_util::ColoredRect backgroundColoredRect_;
    };

    using InteractStagePtr_t = misc::NotNull<InteractStage *>;
    using InteractStagePtrOpt_t = boost::optional<InteractStagePtr_t>;

} // namespace stage
} // namespace heroespath

#endif // HEROESPATH_STAGE_ADVENTURE_STAGE_INTERACT_STAGE_REGION_HPP_INCLUDED
