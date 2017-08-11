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
#ifndef GAME_ITEM_MISCITEMFACTORY_INCLUDED
#define GAME_ITEM_MISCITEMFACTORY_INCLUDED
//
// misc-item-factory.hpp
//  Responsible for making individual item objects.
//
#include "game/item/item-type-enum.hpp"
#include "game/item/item-factory-base.hpp"

#include <memory>
#include <string>


namespace game
{
namespace item
{
    class Item;
    using ItemPtr_t = Item *;

    class ItemProfile;


namespace misc
{

    //A singleton class responsible for making misc item objects.
    class MiscItemFactory : public FactoryBase
    {
        //prevent copy construction
        MiscItemFactory(const MiscItemFactory &) =delete;

        //prevent copy assignment
        MiscItemFactory & operator=(const MiscItemFactory &) =delete;

        //prevent non-singleton construction
        MiscItemFactory();

    public:
        virtual ~MiscItemFactory();

        static MiscItemFactory * Instance();
        static void Acquire();
        static void Release();

        static ItemPtr_t Make(const ItemProfile &);

        static ItemPtr_t Make_Ring(const material::Enum MATERIAL_PRI,
                                   const material::Enum MATERIAL_SEC);

        static ItemPtr_t Make_Wand(const material::Enum MATERIAL_PRI,
                                   const material::Enum MATERIAL_SEC);

        static ItemPtr_t Make_DrumLute(const bool IS_PIXIE);

    private:
        static std::unique_ptr<MiscItemFactory> instanceUPtr_;
    };

}
}
}

#endif //GAME_ITEM_MISCITEMFACTORY_INCLUDED
