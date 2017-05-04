//
// role-stats.cpp
//
#include "role-stats.hpp"

#include "game/game-data-file.hpp"
#include "game/creature/stat-modifier-loader.hpp"


namespace game
{
namespace creature
{

    RoleStatSetMap_t RoleStatModifier::roleStatSetMap_;


    const stats::StatSet RoleStatModifier::Get(const role::Enum ENUM)
    {
        if (roleStatSetMap_.empty())
        {
            for (int i(0); i < role::PlayerRoleCount; ++i)
            {
                auto const NEXT_ENUM{ static_cast<role::Enum>(i) };
                auto const NEXT_ENUM_STR{ role::ToString(NEXT_ENUM) };
                auto const NEXT_KEY_STR{ "heroespath-creature-role-stat-modifiers-" + NEXT_ENUM_STR };
                std::string nextStatSetStr{ "" };
                game::GameDataFile::Instance()->GetStr(nextStatSetStr, NEXT_KEY_STR);
                roleStatSetMap_[NEXT_ENUM] = StatModifierLoader::ConvertStringToStatSet(nextStatSetStr);
            }
        }

        return roleStatSetMap_[ENUM];
    }

}
}
