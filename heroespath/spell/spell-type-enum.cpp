//
// spell-type-enum.cpp
//
#include "spell-type-enum.hpp"

#include <sstream>
#include <exception>


namespace heroespath
{
namespace spell
{

    const std::string SpellType::ToString(const Enum E)
    {
        switch (E)
        {
            case Attack:                    { return "Attack"; }
            case Heal:                      { return "Heal"; }
            case EnchantItemHelpful:        { return "EnchantItemHelpful"; }
            case EnchantItemHarmful:        { return "EnchantItemHarmful"; }
            case EnchantCreatureHelpful:    { return "EnchantCreatureHelpful"; }
            case EnchantCreatureHarmful:    { return "EnchantCreatureHarmful"; }
            case MiscCombat:                { return "MiscCombat"; }
            case MiscNonCombat:             { return "MiscNonCombat"; }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "heroespath::spell::ToString(" << E << ")_InvalidValueError";
                throw std::runtime_error(ss.str());
            }
        }
    }

}
}
