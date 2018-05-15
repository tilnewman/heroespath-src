// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_PLAYER_PARTY_HPP_INCLUDED
#define HEROESPATH_PLAYER_PARTY_HPP_INCLUDED
//
// party.hpp (player)
//  A collection of characters under control of the user.
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
} // namespace creature
namespace player
{

    // encapsulates a set of Characters under control of the user
    class Party
    {
    public:
        Party(const Party &) = delete;
        Party(Party &&) = delete;
        Party & operator=(const Party &) = delete;
        Party & operator=(Party &&) = delete;

    public:
        explicit Party(
            const avatar::Avatar::Enum PARTY_AVATAR = avatar::Avatar::Player_First,
            const creature::CreaturePVec_t & CHARACTER_PVEC = creature::CreaturePVec_t());

        ~Party();

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

    using PartyUPtr_t = std::unique_ptr<Party>;

} // namespace player
} // namespace heroespath

#endif // HEROESPATH_PLAYER_PARTY_HPP_INCLUDED
