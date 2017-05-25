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
#ifndef GAME_ITEM_ARMORFACTORY_INCLUDED
#define GAME_ITEM_ARMORFACTORY_INCLUDED
//
// armor-factory.hpp
//  Code that is responsible for making armor objects.
//
#include "game/item/item-factory-base.hpp"
#include "game/item/armor-types.hpp"
#include "game/stats/types.hpp"

#include <memory>
#include <string>


namespace game
{
namespace item
{
    class Item;
    using ItemSPtr_t = std::shared_ptr<Item>;

namespace armor
{

    //A singleton class responsible for creating armor objects.
    class ArmorFactory : public FactoryBase
    {
        //prevent copy construction
        ArmorFactory(const ArmorFactory &) =delete;

        //prevent copy assignment
        ArmorFactory & operator=(const ArmorFactory &) =delete;

        //prevent non-singleton construction
        ArmorFactory();

    public:
        virtual ~ArmorFactory();

        static ArmorFactory * Instance();
        static void Acquire();
        static void Release();

        //primary material can be wood, secondary material can be Nothing
        static ItemSPtr_t Make_Shield(const shield_type::Enum TYPE,
                                      const material::Enum    MATERIAL_PRI,
                                      const material::Enum    MATERIAL_SEC = material::Nothing);

        //if type==Leather, then the pri and sec materials will be forced to HardLeather and Nothing
        static ItemSPtr_t Make_Helm(const helm_type::Enum TYPE,
                                    const material::Enum  MATERIAL_PRI,
                                    const material::Enum  MATERIAL_SEC = material::Nothing);

        static ItemSPtr_t Make_Gauntlets(const base_type::Enum TYPE,
                                         const material::Enum  MATERIAL_PRI,
                                         const material::Enum  MATERIAL_SEC  = material::Nothing,
                                         const bool            IS_PIXIE_ITEM = false);

        //primary material is always cloth
        static ItemSPtr_t Make_Pants(const base_type::Enum TYPE,
                                     const material::Enum  MATERIAL_SEC = material::Nothing,
                                     const bool            IS_PIXIE_ITEM = false);

        static ItemSPtr_t Make_Boots(const base_type::Enum TYPE,
                                     const material::Enum  MATERIAL_PRI,
                                     const material::Enum  MATERIAL_SEC = material::Nothing,
                                     const bool            IS_PIXIE_ITEM = false);

        //primary material is always cloth, MATERIAL_SEC can be nothing
        static ItemSPtr_t Make_Shirt(const base_type::Enum TYPE,
                                     const material::Enum  MATERIAL_SEC = material::Nothing,
                                     const bool            IS_PIXIE_ITEM = false);

        static ItemSPtr_t Make_Bracer(const base_type::Enum TYPE,
                                      const material::Enum  MATERIAL_PRI,
                                      const material::Enum  MATERIAL_SEC = material::Nothing,
                                      const bool            IS_PIXIE_ITEM = false);

        //primary material should always be metalf
        static ItemSPtr_t Make_Aventail(const base_type::Enum TYPE,
                                        const material::Enum  MATERIAL_PRI,
                                        const material::Enum  MATERIAL_SEC = material::Nothing);

        //primary material should always be either cloth/softleather/hardleather
        static ItemSPtr_t Make_Cover(const cover_type::Enum TYPE,
                                     const material::Enum   MATERIAL_PRI,
                                     const material::Enum   MATERIAL_SEC = material::Nothing,
                                     const bool             IS_PIXIE_ITEM = false);

        static ItemSPtr_t Make_Skin(const material::Enum MATERIAL,
                                    const stats::Rank_t  CREATURE_RANK,
                                    const bool           IS_PIXIE_ITEM);

    private:
        static std::unique_ptr<ArmorFactory> instanceUPtr_;
    };

}//namespace armor
}//namespace item
}//namespace game

#endif //GAME_ITEM_ARMORFACTORY_INCLUDED