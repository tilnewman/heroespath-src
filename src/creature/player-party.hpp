// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_CREATURE_PLAYER_PARTY_HPP_INCLUDED
#define HEROESPATH_CREATURE_PLAYER_PARTY_HPP_INCLUDED
//
// player-party.hpp
//
#include "avatar/avatar-enum.hpp"
#include "misc/boost-serialize-includes.hpp"
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

    // encapsulates a set of Characters under control of the user
    class PlayerParty
    {
    public:
        PlayerParty(const PlayerParty &) = delete;
        PlayerParty(PlayerParty &&) = delete;
        PlayerParty & operator=(const PlayerParty &) = delete;
        PlayerParty & operator=(PlayerParty &&) = delete;

        explicit PlayerParty(
            const avatar::Avatar::Enum PARTY_AVATAR = avatar::Avatar::Player_First,
            const creature::CreaturePVec_t & CHARACTER_PVEC = creature::CreaturePVec_t());

        ~PlayerParty();

        const creature::CreaturePVec_t Characters() const { return charactersPVec_; }

        const creature::CreaturePtr_t GetNextInOrderAfter(const creature::CreaturePtr_t C_PTR)
        {
            return GetNextInOrder(C_PTR, true);
        }

        const creature::CreaturePtr_t GetNextInOrderBefore(const creature::CreaturePtr_t C_PTR)
        {
            return GetNextInOrder(C_PTR, false);
        }

        const creature::CreaturePtr_t GetNextInOrder(const creature::CreaturePtr_t, const bool);

        const creature::CreaturePtr_t GetAtOrderPos(const std::size_t); // zero indexed

        std::size_t GetOrderNum(const creature::CreaturePtr_t) const; // zero indexed

        std::size_t GetNumHumanoid() const;

        avatar::Avatar::Enum Avatar() const { return avatar_; }

        void BeforeSerialize();
        void AfterSerialize();
        void AfterDeserialize();

    public:
        static const std::size_t MAX_CHARACTER_COUNT_;

    private:
        avatar::Avatar::Enum avatar_;

        // this class is NOT responsible for calling CreatureWarehouse::Store() on these observer
        // pointers but it IS responsible for calling CreatureWarehouse::Free() on them.  See the
        // destructor.
        creature::CreaturePVec_t charactersPVec_;

        // the misc::NotNulls in charactersPVec_ cannot be serialized so the vector of raw pointers
        // is required
        std::vector<creature::Creature *> charactersToSerializePVec_;

    private:
        friend class boost::serialization::access;
        template <typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & charactersToSerializePVec_;
        }
    };

    using PlayerPartyUPtr_t = std::unique_ptr<PlayerParty>;

} // namespace creature
} // namespace heroespath

#endif // HEROESPATH_CREATURE_PLAYER_PARTY_HPP_INCLUDED
