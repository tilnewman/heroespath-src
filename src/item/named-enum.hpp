// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_ITEM_NAMED_ENUM_HPP_INCLUDED
#define HEROESPATH_ITEM_NAMED_ENUM_HPP_INCLUDED
//
// named-enum.hpp
//
#include "item/element-enum.hpp"
#include "misc/enum-common.hpp"

#include <string_view>

namespace heroespath
{
namespace item
{

    // all Named are either weapon or armor that are never also Misc, never Set, often Elemental
    struct Named : public EnumBaseCounting<>
    {
        enum Enum : EnumUnderlying_t
        {
            Arctic,
            Army,
            Betrayer,
            Burglar,
            Burning,
            Charlatans,
            Charred,
            Chill,
            Dancing,
            Daring,
            Dark,
            Dawn,
            Diabolic,
            Dusk,
            Elite,
            Fiendish,
            Fiery,
            Focus,
            Frigid,
            Frozen,
            Gladiator,
            Gloom,
            Glory,
            Heroes,
            Honest,
            Icy,
            Imposters,
            Infernal,
            Knaves,
            Light,
            Marauder,
            Misery,
            Moon,
            Mountebank,
            Muggers,
            Nile,
            Noble,
            Pickpocket,
            Pirate,
            Princes,
            Proud,
            Pure,
            Raging,
            Ranger,
            Robbers,
            Samurai,
            Searing,
            Soldiers,
            Sorrow,
            Sun,
            Thief,
            Thors,
            Thugs,
            Twilight,
            Valiant,
            Wardens,
            Wicked,
            Winter,
            Woe,
            Count
        };

        static constexpr std::string_view Name(const Enum NAMED_TYPE) noexcept
        {
            switch (NAMED_TYPE)
            {
                case Charlatans: return "Charlatan's";
                case Heroes: return "Hero's";
                case Imposters: return "Imposter's";
                case Knaves: return "Knave's";
                case Muggers: return "Mugger's";
                case Princes: return "Prince's";
                case Robbers: return "Robber's";
                case Soldiers: return "Soldier's";
                case Thors: return "Thor's";
                case Thugs: return "Thug's";
                case Wardens: return "Warden's";
                case Arctic: return "Arctic";
                case Army: return "Army";
                case Betrayer: return "Betrayer";
                case Burglar: return "Burglar";
                case Burning: return "Burning";
                case Charred: return "Charred";
                case Chill: return "Chill";
                case Dancing: return "Dancing";
                case Daring: return "Daring";
                case Dark: return "Dark";
                case Dawn: return "Dawn";
                case Diabolic: return "Diabolic";
                case Dusk: return "Dusk";
                case Elite: return "Elite";
                case Fiendish: return "Fiendish";
                case Fiery: return "Fiery";
                case Focus: return "Focus";
                case Frigid: return "Frigid";
                case Frozen: return "Frozen";
                case Gladiator: return "Gladiator";
                case Gloom: return "Gloom";
                case Glory: return "Glory";
                case Honest: return "Honest";
                case Icy: return "Icy";
                case Infernal: return "Infernal";
                case Light: return "Light";
                case Marauder: return "Marauder";
                case Misery: return "Misery";
                case Moon: return "Moon";
                case Mountebank: return "Mountebank";
                case Nile: return "Nile";
                case Noble: return "Noble";
                case Pickpocket: return "Pickpocket";
                case Pirate: return "Pirate";
                case Proud: return "Proud";
                case Pure: return "Pure";
                case Searing: return "Searing";
                case Raging: return "Raging";
                case Ranger: return "Ranger";
                case Samurai: return "Samurai";
                case Sorrow: return "Sorrow";
                case Sun: return "Sun";
                case Thief: return "Thief";
                case Twilight: return "Twilight";
                case Valiant: return "Valiant";
                case Wicked: return "Wicked";
                case Winter: return "Winter";
                case Woe: return "Woe";
                case Count: return "Count";
                default: return "item::Named::Name(ENUM=out_of_range)";
            }
        }

        static constexpr ElementList::Enum ElementsAllowed(const Enum ENUM) noexcept
        {
            if (ENUM == Named::Wicked)
            {
                return (ElementList::Fire | ElementList::Shadow);
            }

            if (ENUM == Named::Infernal)
            {
                return ElementList::MakeCombinationsList(Element::Fire | Element::Shadow);
            }

            if ((ENUM == Named::Light) || (ENUM == Named::Dawn) || (ENUM == Named::Sun))
            {
                return ElementList::MakeCombinationsList(Element::Fire | Element::Honor);
            }

            if (ENUM == Named::Dancing)
            {
                return (ElementList::Fire | ElementList::Frost);
            }

            if (ENUM == Named::Princes)
            {
                return (ElementList::Honor | ElementList::Shadow);
            }

            if ((ENUM == Named::Searing) || (ENUM == Named::Burning) || (ENUM == Named::Fiery)
                || (ENUM == Named::Charred))
            {
                return ElementList::Fire;
            }

            if ((ENUM == Named::Icy) || (ENUM == Named::Winter) || (ENUM == Named::Frigid)
                || (ENUM == Named::Chill) || (ENUM == Named::Frozen) || (ENUM == Named::Arctic))
            {
                return ElementList::Frost;
            }

            if ((ENUM == Named::Honest) || (ENUM == Named::Noble) || (ENUM == Named::Daring)
                || (ENUM == Named::Elite) || (ENUM == Named::Valiant) || (ENUM == Named::Heroes)
                || (ENUM == Named::Gladiator) || (ENUM == Named::Proud) || (ENUM == Named::Glory)
                || (ENUM == Named::Pure) || (ENUM == Named::Samurai) || (ENUM == Named::Thors))
            {
                return ElementList::Honor;
            }

            if ((ENUM == Named::Diabolic) || (ENUM == Named::Fiendish) || (ENUM == Named::Nile)
                || (ENUM == Named::Gloom) || (ENUM == Named::Twilight) || (ENUM == Named::Dusk)
                || (ENUM == Named::Sorrow) || (ENUM == Named::Woe) || (ENUM == Named::Misery)
                || (ENUM == Named::Moon) || (ENUM == Named::Dark) || (ENUM == Named::Betrayer)
                || (ENUM == Named::Burglar) || (ENUM == Named::Mountebank)
                || (ENUM == Named::Charlatans))
            {
                return ElementList::Shadow;
            }

            return ElementList::None;
        }
    };

} // namespace item
} // namespace heroespath

#endif // HEROESPATH_ITEM_NAMED_ENUM_HPP_INCLUDED
