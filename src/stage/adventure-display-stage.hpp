// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_STAGE_ADVENTUREDISPLAYSTAGE_HPP_INCLUDED
#define HEROESPATH_STAGE_ADVENTUREDISPLAYSTAGE_HPP_INCLUDED
//
// adventure-display-stage.hpp
//
#include "gui/cached-texture.hpp"
#include "gui/direction-enum.hpp"
#include "gui/horiz-symbol.hpp"
#include "gui/stage-title.hpp"
#include "interact/interaction-manager.hpp"
#include "misc/boost-optional-that-throws.hpp"
#include "misc/not-null.hpp"
#include "stage/adventure-stage-char-list.hpp"
#include "stage/stage-base.hpp"

#include <SFML/Graphics/Sprite.hpp>

#include <memory>

namespace heroespath
{
namespace map
{
    class Map;
    using MapPtr_t = misc::NotNull<Map *>;
    using MapUPtr_t = std::unique_ptr<Map>;
} // namespace map
namespace stage
{

    class InteractStage;
    using InteractStagePtr_t = misc::NotNull<InteractStage *>;

    // Responsible for all drawing operations of the AdventureStage.
    class AdventureDisplayStage : public stage::StageBase

    {
    public:
        AdventureDisplayStage(const AdventureDisplayStage &) = delete;
        AdventureDisplayStage(AdventureDisplayStage &&) = delete;
        AdventureDisplayStage & operator=(const AdventureDisplayStage &) = delete;
        AdventureDisplayStage & operator=(AdventureDisplayStage &&) = delete;

        explicit AdventureDisplayStage();
        virtual ~AdventureDisplayStage();

        void Setup() override;
        void Draw(sf::RenderTarget &, const sf::RenderStates &) override;
        void UpdateTime(const float ELAPSED_TIME_SECONDS) override;

        map::MapPtr_t GetMapForStageFactory() { return mapUPtr_.get(); }

        interact::InteractionManager & GetInteractionManagerForStageFactory()
        {
            return interactionManager_;
        }

        const sf::FloatRect CalcInteractRegionForStageFactory() const;

    private:
        void Setup_CharacterList();
        void Setup_BackgroundImage();
        void Setup_Map();

        void HandleMovementKeypresses(
            const gui::Direction::Enum, bool & wasPressed, const bool IS_PRESSED);

    private:
        static const float TIME_BETWEEN_MAP_MOVES_SEC_;

        interact::InteractionManager interactionManager_;
        gui::StageTitle stageTitle_;
        gui::BottomSymbol bottomImage_;
        const sf::FloatRect MAP_OUTER_REGION_;
        const sf::FloatRect MAP_INNER_REGION_;
        map::MapUPtr_t mapUPtr_;
        AdventureCharacterListUPtr_t characterListUPtr_;
        gui::CachedTexture bgCachedTexture_;
        sf::Sprite bgSprite_;
        float moveTimerSec_;
        bool wasPressedLeft_;
        bool wasPressedRight_;
        bool wasPressedUp_;
        bool wasPressedDown_;
    };

    using AdventureDisplayStagePtr_t = misc::NotNull<AdventureDisplayStage *>;
    using AdventureDisplayStagePtrOpt_t = boost::optional<AdventureDisplayStagePtr_t>;

} // namespace stage
} // namespace heroespath

#endif // HEROESPATH_STAGE_ADVENTUREDISPLAYSTAGE_HPP_INCLUDED
