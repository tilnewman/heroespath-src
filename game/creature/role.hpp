#ifndef GAME_CREATURE_ROLE_INCLUDED
#define GAME_CREATURE_ROLE_INCLUDED
//
// role.hpp
//  The Role class for all creatures.
//
#include "utilz/boost-serialize-includes.hpp"

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
        Role(const role::Enum ROLE = role::PlayerRoleCount);

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
