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
#ifndef HEROESPATH_PLAYER_INITIAL_HPP_INCLUDED
#define HEROESPATH_PLAYER_INITIAL_HPP_INCLUDED
//
// initial.hpp
//  A collection of functions that performs initial setup of a player character
//
#include "misc/types.hpp"
#include "stats/trait.hpp"
#include "item/item-type-enum.hpp"


namespace heroespath
{
namespace player
{

    //forward declarations
    class Character;
    using CharacterPtrC_t  = Character * const;
    using CharacterCPtrC_t = const Character * const;


    class Initial
    {
    public:
        static void Setup(CharacterPtrC_t);

    private:
        static void EnsureValidImageFilename(CharacterPtrC_t);
        static void SetupInventory(CharacterPtrC_t);
        static void SetupSpellsAndSongs(CharacterPtrC_t);
        static void EquipBodyParts(CharacterPtrC_t);
        static Health_t GetStartingHealth(CharacterCPtrC_t);
        static void SetStartingHealth(CharacterPtrC_t);
        static void SetStartingMana(CharacterPtrC_t);
        static item::material::Enum HardOrSoftLeatherRand();
    };

}
}

#endif //HEROESPATH_PLAYER_INITIAL_HPP_INCLUDED
