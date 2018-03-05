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
#ifndef HEROESPATH_PLAYER_PARTY_FACTORY_HPP_INCLUDED
#define HEROESPATH_PLAYER_PARTY_FACTORY_HPP_INCLUDED
//
// party-factory.hpp
//
#include "avatar/avatar-enum.hpp"
#include <memory>
#include <vector>

namespace heroespath
{
namespace player
{

    class Character;
    using CharacterPtr_t = Character *;
    using CharacterPVec_t = std::vector<CharacterPtr_t>;

    class Party;
    using PartyUPtr_t = std::unique_ptr<Party>;

    struct PartyFactory
    {
        static PartyUPtr_t Make(const avatar::Avatar::Enum, const CharacterPVec_t &);

        static PartyUPtr_t MakeFakeForTesting();
    };
}
}

#endif // HEROESPATH_PLAYER_PARTY_FACTORY_HPP_INCLUDED
