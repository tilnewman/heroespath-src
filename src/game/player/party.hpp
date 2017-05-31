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
#ifndef GAME_PLAYER_PARTY_INCLUDED
#define GAME_PLAYER_PARTY_INCLUDED
//
// party.hpp (player)
//  A collection of characters under control of the user.
//
#include "misc/boost-serialize-includes.hpp"

#include <memory>
#include <string>
#include <vector>


namespace game
{
namespace creature
{
    class Creature;
    using CreaturePtr_t   = Creature *;
    using CreatureCPtrC_t = const Creature * const;
}

namespace player
{

    //forward declarations
    class Character;
    using CharacterPtr_t  = Character *;
    using CharacterPVec_t = std::vector<CharacterPtr_t>;


    //encapsulates a set of Characters under control of the user
    class Party
    {
        //prevent copy construction
        Party(const Party &) =delete;

        //prevent copy assignment
        Party & operator=(const Party &) =delete;

    public:
        explicit Party(const CharacterPVec_t & CHARACTER_PVEC = CharacterPVec_t());
        virtual ~Party();

        inline const CharacterPVec_t Characters() const { return charactersPVec_; }

        //Sets error_msg to a message describing why upon failure,
        //otherwise error_msg is not changed.
        bool Add(const CharacterPtr_t CHARACTER_PTR, std::string & error_msg);

        bool IsAddAllowed(const CharacterPtr_t CHARACTER_PTR, std::string & error_msg);

        //returns true if the character existed in the charactersSVec_ and was removed.
        bool Remove(CharacterPtr_t);

        inline creature::CreaturePtr_t GetNextInOrderAfter(const creature::CreaturePtr_t C_PTR)
        {
            return GetNextInOrder(C_PTR, true);
        }

        inline creature::CreaturePtr_t GetNextInOrderAfter(creature::CreatureCPtrC_t C_CPTRC)
        {
            return GetNextInOrder(C_CPTRC, true);
        }

        inline creature::CreaturePtr_t GetNextInOrderBefore(const creature::CreaturePtr_t C_PTR)
        {
            return GetNextInOrder(C_PTR, false);
        }

        inline creature::CreaturePtr_t GetNextInOrderBefore(creature::CreatureCPtrC_t C_CPTRC)
        {
            return GetNextInOrder(C_CPTRC, false);
        }

        creature::CreaturePtr_t GetNextInOrder(creature::CreatureCPtrC_t, const bool);

        creature::CreaturePtr_t GetAtOrderPos(const std::size_t);//zero indexed

        std::size_t GetOrderNum(creature::CreatureCPtrC_t) const;//zero indexed

        std::size_t GetNumHumanoid() const;

    public:
        static const std::size_t MAX_CHARACTER_COUNT_;

    private:
        CharacterPVec_t charactersPVec_;

    private:
        friend class boost::serialization::access;
        template<typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & charactersPVec_;
        }
    };

    using PartyPtr_t  = Party *;
    using PartyUPtr_t = std::unique_ptr<Party>;

}
}

#endif //GAME_PLAYER_PARTY_INCLUDED
