// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_CREATURE_SPELL_SPELLENUM_HPP_INCLUDED
#define HEROESPATH_CREATURE_SPELL_SPELLENUM_HPP_INCLUDED
//
// spell-enum.hpp
//
#include "misc/enum-common.hpp"

#include <string>
#include <vector>

namespace heroespath
{
namespace spell
{

    struct Spells : public EnumBaseCounting<>
    {
        // Note:  Keep in sync with:
        //          spell::Warehouse::Fill()
        //          CombatAnimation::SpellAnimStart()
        //          CombatSoundEffects::PlaySpell()
        //          Initial::SetupInventory()
        enum Enum : EnumUnderlying_t
        {
            Sparks = 0,
            Bandage,
            Sleep,
            Awaken,
            Trip,
            Lift,
            Daze,
            Panic,
            ClearMind,
            Poison,
            Antidote, // cures poison
            PoisonCloud,
            Count
        };

        static constexpr std::string_view Name(const Spells::Enum ENUM) noexcept
        {
            switch (ENUM)
            {
                case Sparks: return "Sparks";
                case Bandage: return "Bandage";
                case Sleep: return "Sleep";
                case Awaken: return "Awaken";
                case Trip: return "Trip";
                case Lift: return "Lift";
                case Daze: return "Daze";
                case Panic: return "Panic";
                case ClearMind: return "Clear Mind";
                case Poison: return "Poison";
                case Antidote: return "Antidote";
                case PoisonCloud: return "Poison Cloud";
                case Count: return "Count";
                default: return "spell::Spells::Name(ENUM=out_of_range)";
            }
        }

        static const std::string ShortDesc(const Enum);
        static const std::string ExtraDesc(const Enum);
        static const std::string ImageFilename(const Enum);
        static const std::string ImageDirectory();
        static const std::string ImagePath(const Enum);
    };

    using SpellEnumVec_t = std::vector<Spells::Enum>;

} // namespace spell
} // namespace heroespath

#endif // HEROESPATH_CREATURE_SPELL_SPELLENUM_HPP_INCLUDED
