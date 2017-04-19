//
// role.cpp
//
#include "role.hpp"

#include "heroespath/creature/role-stats.hpp"

#include <boost/algorithm/string/erase.hpp>

#include <sstream>


namespace heroespath
{
namespace creature
{

    const RoleVec_t Role::ROLEVEC_ALL_PLAYER_ROLES_{ role::Knight, role::Archer, role::Thief, role::Bard, role::Beastmaster, role::Wolfen, role::Firebrand, role::Sylavin, role::Cleric, role::Sorcerer };


    Role::Role(const role::Enum ROLE)
    :
        role_         (ROLE),
        statModifiers_( StatModifierByRole(ROLE) )
    {}


    Role::~Role()
    {}


    const std::string Role::CharacterRoleString(const role::Enum E, const bool WILL_WRAP)
    {
        std::ostringstream ss;

        if (WILL_WRAP)
            ss << "(";

        if (E == role::PlayerRoleCount)
            ss << "ALL player character roles";
        else
            ss << role::Name(E);

        if (WILL_WRAP)
            ss << ")";

        return ss.str();
    }


    const std::string Role::CharacterRoleString(const RoleVec_t & ROLE_VEC, const bool WILL_WRAP)
    {
        const std::string SEP_STR(", ");

        std::ostringstream ss;

        if (WILL_WRAP)
            ss << "(";

        if ((ROLE_VEC.size() == 1) && (ROLE_VEC[0] == role::PlayerRoleCount))
        {
            ss << CharacterRoleString(ROLE_VEC[0], false);
        }
        else
        {
            for (auto const NEXT_ROLE_ENUM : ROLE_VEC)
                ss << role::Name(NEXT_ROLE_ENUM) << SEP_STR;
        }

        if (WILL_WRAP)
            ss << ")";

        return boost::algorithm::erase_all_copy(ss.str(), SEP_STR);
    }

}
}
