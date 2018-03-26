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
#ifndef HEROESPATH_NONPLAYER_PARTY_HPP_INCLUDED
#define HEROESPATH_NONPLAYER_PARTY_HPP_INCLUDED
//
// party.hpp (non-player)
//  A collection of characters NOT under control of the user.
//
#include "misc/boost-serialize-includes.hpp"

#include <memory>
#include <string>
#include <vector>

namespace heroespath
{
namespace creature
{
    class Creature;
    using CreaturePtr_t = Creature *;
    using CreatureCPtr_t = const Creature *;
    using CreaturePtrC_t = Creature * const;
    using CreatureCPtrC_t = const Creature * const;
    using CreaturePVec_t = std::vector<CreaturePtr_t>;
} // namespace creature
namespace non_player
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
            const creature::CreaturePVec_t & CHARACTER_PVEC = creature::CreaturePVec_t());

        ~Party();

        const creature::CreaturePVec_t Characters() const { return charactersPVec_; }
        void Add(const creature::CreaturePtr_t, const bool WILL_STORE = true);
        bool Remove(creature::CreaturePtr_t, const bool WILL_FREE = true);
        void Clear() { charactersPVec_.clear(); }
        const std::string Summary() const;
        bool Contains(const creature::CreaturePtr_t) const;

    private:
        creature::CreaturePVec_t charactersPVec_;

    private:
        friend class boost::serialization::access;
        template <typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & charactersPVec_;
        }
    };

    using PartyUPtr_t = std::unique_ptr<Party>;

} // namespace non_player
} // namespace heroespath

#endif // HEROESPATH_NONPLAYER_PARTY_HPP_INCLUDED
