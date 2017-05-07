//
// spell-type-enum.cpp
//
#include "spell-type-enum.hpp"

#include <boost/algorithm/string.hpp>

#include <sstream>
#include <exception>


namespace game
{
namespace spell
{

    const std::string SpellClass::ToString(const Enum E, const bool WILL_WRAP)
    {
        std::ostringstream ss;

        if (E & SpellClass::Combat)          ss << "Combat";
        if (E & SpellClass::Exploring)       ss << ((ss.str().empty()) ? "" : ", ") << "Exploring";
        if (E & SpellClass::Conversation)    ss << ((ss.str().empty()) ? "" : ", ") << "Conversation";
        if (E & SpellClass::Quest)           ss << ((ss.str().empty()) ? "" : ", ") << "Quest";
        if (E & SpellClass::Inventory)       ss << ((ss.str().empty()) ? "" : ", ") << "Inventory";

        if (ss.str().empty())
        {
            std::ostringstream ssErr;
            ssErr << "game::spell::SpellClass::ToString(" << E << ")_InvalidValueError";
            throw std::range_error(ssErr.str());
        }

        if (WILL_WRAP)
            return "(" + ss.str() + ")";
        else
            return ss.str();
    }


    SpellClass::Enum SpellClass::FromString(const std::string & S)
    {
        auto lowerCaseStr{ boost::algorithm::to_lower_copy(S) };

        unsigned int x{ 0 };
        if (boost::contains(lowerCaseStr, boost::to_lower_copy(SpellClass::ToString(SpellClass::Combat, false))))         x = x & SpellClass::Combat;
        if (boost::contains(lowerCaseStr, boost::to_lower_copy(SpellClass::ToString(SpellClass::Exploring, false))))      x = x & SpellClass::Exploring;
        if (boost::contains(lowerCaseStr, boost::to_lower_copy(SpellClass::ToString(SpellClass::Conversation, false))))   x = x & SpellClass::Conversation;
        if (boost::contains(lowerCaseStr, boost::to_lower_copy(SpellClass::ToString(SpellClass::Quest, false))))          x = x & SpellClass::Quest;
        if (boost::contains(lowerCaseStr, boost::to_lower_copy(SpellClass::ToString(SpellClass::Inventory, false))))      x = x & SpellClass::Inventory;

        if (0 == x)
        {
            std::ostringstream ssErr;
            ssErr << "game::spell::SpellClass::FromString(\"" << S << "\") unable to convert that string into a set of SpellClass::Enum flags.";
            throw std::runtime_error(ssErr.str());
        }

        return static_cast<SpellClass::Enum>(x);
    }


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
