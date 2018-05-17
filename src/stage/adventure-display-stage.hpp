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
#include "interact/interaction-manager.hpp"
#include "misc/not-null.hpp"
#include "sfml-util/direction-enum.hpp"
#include "sfml-util/gui/picture-frame.hpp"
#include "sfml-util/horiz-symbol.hpp"
#include "sfml-util/main-menu-title.hpp"
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/sfml-system.hpp"
#include "sfml-util/stage.hpp"
#include "stage/adventure-stage-char-list.hpp"

#include <memory>

namespace heroespath
{
namespace map
{
    class Map;
    using MapUPtr_t = std::unique_ptr<Map>;
} // namespace map
namespace stage
{

    class InteractStage;
    using InteractStagePtr_t = misc::NotNull<InteractStage *>;

    // Responsible for all drawing operations of the AdventureStage.
    class AdventureDisplayStage : public sfml_util::Stage
    {
    public:
        AdventureDisplayStage(const AdventureDisplayStage &) = delete;
        AdventureDisplayStage(AdventureDisplayStage &&) = delete;
        AdventureDisplayStage & operator=(const AdventureDisplayStage &) = delete;
        AdventureDisplayStage & operator=(AdventureDisplayStage &&) = delete;

    public:
        explicit AdventureDisplayStage(interact::InteractionManager &);
        virtual ~AdventureDisplayStage();

        void Setup() override;
        void Draw(sf::RenderTarget &, const sf::RenderStates &) override;
        void UpdateTime(const float ELAPSED_TIME_SECONDS) override;

    private:
        void Setup_CharacterList();
        void Setup_BackgroundImage();
        void Setup_Map();

        void HandleMovementKeypresses(
            const sfml_util::Direction::Enum, bool & wasPressed, const bool IS_PRESSED);

        const sf::FloatRect CalcInteractRegion(const sf::FloatRect & MAP_REGION) const;

    private:
        static const float TIME_BETWEEN_MAP_MOVES_SEC_;

        interact::InteractionManager & interactionManager_;
        sfml_util::MainMenuTitle topImage_;
        sfml_util::BottomSymbol bottomImage_;
        const sf::FloatRect MAP_OUTER_REGION_;
        sfml_util::gui::PictureFrame mapFrame_;
        const sf::FloatRect MAP_INNER_REGION_;
        map::MapUPtr_t mapUPtr_;
        InteractStagePtr_t interactStagePtr_;
        AdventureCharacterListUPtr_t characterListUPtr_;
        sf::Texture backgroundTexture_;
        sf::Sprite backgroundSprite_;
        float moveTimerSec_;
        bool wasPressedLeft_;
        bool wasPressedRight_;
        bool wasPressedUp_;
        bool wasPressedDown_;
    };

} // namespace stage
} // namespace heroespath

#endif // HEROESPATH_STAGE_ADVENTUREDISPLAYSTAGE_HPP_INCLUDED
