// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_CREATURE_PLAYER_PARTY_FACTORY_HPP_INCLUDED
#define HEROESPATH_CREATURE_PLAYER_PARTY_FACTORY_HPP_INCLUDED
//
// player-party-factory.hpp
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

    class PlayerParty;
    using PlayerPartyUPtr_t = std::unique_ptr<PlayerParty>;

    // Responsible for making party objects full of player creatures.
    class PlayerPartyFactory
    {
    public:
        PlayerPartyFactory(const PlayerPartyFactory &) = delete;
        PlayerPartyFactory(PlayerPartyFactory &&) = delete;
        PlayerPartyFactory & operator=(const PlayerPartyFactory &) = delete;
        PlayerPartyFactory & operator=(PlayerPartyFactory &&) = delete;

        PlayerPartyFactory() = default;

        PlayerPartyUPtr_t Make(const avatar::Avatar::Enum, const creature::CreaturePVec_t &) const;
        PlayerPartyUPtr_t MakeFakeForTesting() const;

    private:
        const std::string MakeCharacterNameForTesting(const std::string & POSTFIX) const;
    };

} // namespace creature
} // namespace heroespath

#endif // HEROESPATH_CREATURE_PLAYER_PARTY_FACTORY_HPP_INCLUDED
