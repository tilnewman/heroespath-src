// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_STAGE_ADVENTURESTAGE_HPP_INCLUDED
#define HEROESPATH_STAGE_ADVENTURESTAGE_HPP_INCLUDED
//
// adventure-stage.hpp
//
#include "interact/interaction-manager.hpp"
#include "misc/not-null.hpp"
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/stage.hpp"

namespace heroespath
{
namespace stage
{

    class AdventureDisplayStage;
    using AdventureDisplayStagePtr_t = misc::NotNull<AdventureDisplayStage *>;

    // Responsible for managing all AdventureStage interactions with the player.
    class AdventureStage : public sfml_util::Stage
    {
    public:
        AdventureStage(const AdventureStage &) = delete;
        AdventureStage(AdventureStage &&) = delete;
        AdventureStage & operator=(const AdventureStage &) = delete;
        AdventureStage & operator=(AdventureStage &&) = delete;

    public:
        AdventureStage();
        virtual ~AdventureStage();

        void Setup() override;
        void UpdateTime(const float ELAPSED_TIME_SEC) override;

    private:
        interact::InteractionManager interactionManager_;
        AdventureDisplayStagePtr_t adventureDisplayStagePtr_;
    };

} // namespace stage
} // namespace heroespath

#endif // HEROESPATH_STAGE_ADVENTURESTAGE_HPP_INCLUDED
