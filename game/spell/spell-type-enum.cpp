//
// spell-type-enum.cpp
//
#include "spell-type-enum.hpp"

#include <sstream>
#include <exception>


namespace game
{
namespace spell
{

    const std::string SpellType::ToString(const Enum E)
    {
        switch (E)
        {
            case Attack:                    { return "Attack"; }
            case Heal:                      { return "Heal"; }
            case EffectItem:                { return "EffectItem"; }
            case EnchantItemHelpful:        { return "EnchantItemHelpful"; }
            case EnchantItemHarmful:        { return "EnchantItemHarmful"; }
            case EffectCreature:            { return "EffectCreature"; }
            case EnchantCreatureHelpful:    { return "EnchantCreatureHelpful"; }
            case EnchantCreatureHarmful:    { return "EnchantCreatureHarmful"; }
            case Misc:                      { return "Misc"; }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "game::spell::ToString(" << E << ")_InvalidValueError";
                throw std::runtime_error(ss.str());
            }
        }
    }


    const std::string SpellType::Name(const Enum E)
    {
        switch (E)
        {
            case Attack:                    { return "attack"; }
            case Heal:                      { return "heal"; }
            case EffectItem:                { return "effect item"; }
            case EnchantItemHelpful:        { return "helpful enchant item"; }
            case EnchantItemHarmful:        { return "harmful enchant item"; }
            case EffectCreature:            { return "effect creature"; }
            case EnchantCreatureHelpful:    { return "helpful enchant creature"; }
            case EnchantCreatureHarmful:    { return "harmful enchant creature"; }
            case Misc:                      { return "miscellaneous"; }
            case Count:
            default:
            {
                std::ostringstream ss;
                ss << "game::spell::ToString(" << E << ")_InvalidValueError";
                throw std::runtime_error(ss.str());
            }
        }
    }

}
}
