// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_INTERACT_STATEMENTTOWN_HPP_INCLUDED
#define HEROESPATH_INTERACT_STATEMENTTOWN_HPP_INCLUDED
//
// statement-town.hpp
//
#include "creature/player-party.hpp"
#include "interact/statement.hpp"
#include "map/level-enum.hpp"
#include <string>

namespace heroespath
{
namespace interact
{
    namespace talk
    {

        struct TownTalk
        {
            static const std::string
                Compose(const Mood, const creature::PlayerParty &, const map::Level::Enum);

        private:
            static const std::string
                ComposeKind(const creature::PlayerParty &, const map::Level::Enum);
            static const std::string
                ComposeMean(const creature::PlayerParty &, const map::Level::Enum);
        };

    } // namespace talk
} // namespace interact
} // namespace heroespath

#endif // HEROESPATH_INTERACT_STATEMENTTOWN_HPP_INCLUDED
