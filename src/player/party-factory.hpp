// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_PLAYER_PARTY_FACTORY_HPP_INCLUDED
#define HEROESPATH_PLAYER_PARTY_FACTORY_HPP_INCLUDED
//
// party-factory.hpp
//
#include "avatar/avatar-enum.hpp"
#include "misc/not-null.hpp"

#include <memory>
#include <string>
#include <vector>

namespace heroespath
{
namespace creature
{
    class Creature;
    using CreaturePtr_t = misc::NotNull<Creature *>;
    using CreaturePVec_t = std::vector<CreaturePtr_t>;
} // namespace creature
namespace player
{

    class Party;
    using PartyUPtr_t = std::unique_ptr<Party>;

    struct PartyFactory
    {
        static PartyUPtr_t Make(const avatar::Avatar::Enum, const creature::CreaturePVec_t &);
        static PartyUPtr_t MakeFakeForTesting();

    private:
        static const std::string MakeCharacterNameForTesting(const std::string & POSTFIX);
    };

} // namespace player
} // namespace heroespath

#endif // HEROESPATH_PLAYER_PARTY_FACTORY_HPP_INCLUDED
