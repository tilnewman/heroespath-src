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

    // Responsible for making party objects full of player creatures.
    class PartyFactory
    {
    public:
        PartyFactory(const PartyFactory &) = delete;
        PartyFactory(PartyFactory &&) = delete;
        PartyFactory & operator=(const PartyFactory &) = delete;
        PartyFactory & operator=(PartyFactory &&) = delete;

        PartyFactory() = default;

        PartyUPtr_t Make(const avatar::Avatar::Enum, const creature::CreaturePVec_t &) const;
        PartyUPtr_t MakeFakeForTesting() const;

    private:
        const std::string MakeCharacterNameForTesting(const std::string & POSTFIX) const;
    };

} // namespace player
} // namespace heroespath

#endif // HEROESPATH_PLAYER_PARTY_FACTORY_HPP_INCLUDED
