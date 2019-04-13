// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// statement-factory.hpp
//
#include "statement-factory.hpp"

#include "creature/creature.hpp"
#include "creature/player-party.hpp"
#include "game/game-state.hpp"
#include "game/game.hpp"
#include "game/world.hpp"
#include "interact/statement-child.hpp"
#include "interact/statement-common.hpp"
#include "interact/statement-guard.hpp"
#include "interact/statement-monk.hpp"
#include "interact/statement-tavern.hpp"
#include "interact/statement-town.hpp"
#include "map/level-enum.hpp"
#include "misc/assertlogandthrow.hpp"

namespace heroespath
{
namespace interact
{
    namespace talk
    {

        const std::string Factory::Make(const CategoryVec_t & CATEGORIES, const Mood MOOD)
        {
            M_HP_ASSERT_OR_LOG_AND_THROW(
                (CATEGORIES.empty() == false),
                "interact::talk::Factory::Make() was given an empty category vector.");

            const auto CATEGORY { misc::RandomSelect(CATEGORIES) };

            const auto & PARTY { game::Game::Instance()->State().Party() };

            const auto LEVEL {
                game::Game::Instance()->State().GetWorld().GetMaps().Current().Which()
            };

            switch (CATEGORY)
            {
                case Category::Town: return TownTalk::Compose(MOOD, PARTY, LEVEL);
                case Category::Child: return ChildTalk::Compose(MOOD, PARTY);
                case Category::Monk: return MonkTalk::Compose(MOOD);
                case Category::Drunk: return TavernTalk::Compose(MOOD, PARTY);
                case Category::Guard: return GuardTalk::Compose(MOOD, PARTY);
                case Category::Common:
                default: return CommonTalk::Compose(MOOD, PARTY);
            }
        }

    } // namespace talk
} // namespace interact
} // namespace heroespath
