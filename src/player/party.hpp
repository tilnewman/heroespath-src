///////////////////////////////////////////////////////////////////////////////
//
// Heroes' Path - Open-source, non-commercial, simple, game in the RPG style.
// Copyright (C) 2017 Ziesche Til Newman (tilnewman@gmail.com)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from
// the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
//  1. The origin of this software must not be misrepresented; you must not
//     claim that you wrote the original software.  If you use this software
//     in a product, an acknowledgment in the product documentation would be
//     appreciated but is not required.
//
//  2. Altered source versions must be plainly marked as such, and must not
//     be misrepresented as being the original software.
//
//  3. This notice may not be removed or altered from any source distribution.
//
///////////////////////////////////////////////////////////////////////////////
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
