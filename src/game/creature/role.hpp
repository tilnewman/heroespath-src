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
#ifndef GAME_CREATURE_ROLE_INCLUDED
#define GAME_CREATURE_ROLE_INCLUDED
//
// role.hpp
//  The Role class for all creatures.
//
#include "misc/boost-serialize-includes.hpp"

#include "game/stats/types.hpp"
#include "game/stats/stat-set.hpp"
#include "game/creature/role-enum.hpp"

#include <string>
#include <vector>
#include <tuple>


namespace game
{
namespace creature
{

    //a base class for both Player and Non-Player Role classes
    //Role includes more than a class, but also the concept of experience and rank, etc.
    class Role
    {
    public:
        explicit Role(const role::Enum ROLE = role::PlayerRoleCount);

        virtual ~Role();

        inline const std::string Name() const               { return role::ToString(role_); }
        inline const std::string Desc() const               { return role::Desc(role_); }
        inline role::Enum Which() const                     { return role_; }
        inline const stats::StatSet StatModifiers() const   { return statModifiers_; }

        static const std::string CharacterRoleString(const role::Enum, const bool WILL_WRAP = false);
        static const std::string CharacterRoleString(const RoleVec_t &, const bool WILL_WRAP = false);

        friend bool operator<(const Role & L, const Role & R);
        friend bool operator==(const Role & L, const Role & R);

    public:
        static const RoleVec_t ROLEVEC_ALL_PLAYER_ROLES_;

    protected:
        role::Enum     role_;
        stats::StatSet statModifiers_;

    private:
        friend class boost::serialization::access;
        template<typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & role_;
            ar & statModifiers_;
        }
    };


    inline bool operator<(const Role & L, const Role & R)
    {
        return std::tie(L.role_, L.statModifiers_)
               <
               std::tie(R.role_, R.statModifiers_);
    }


    inline bool operator==(const Role & L, const Role & R)
    {
        return std::tie(L.role_, L.statModifiers_)
               ==
               std::tie(R.role_, R.statModifiers_);
    }


    inline bool operator!=(const Role & L, const Role & R)
    {
        return ! (L == R);
    }

}
}

#endif //GAME_CREATURE_ROLE_INCLUDED
