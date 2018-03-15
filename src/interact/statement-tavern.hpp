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
#ifndef HEROESPATH_INTERACT_STATEMENTTAVERN_HPP_INCLUDED
#define HEROESPATH_INTERACT_STATEMENTTAVERN_HPP_INCLUDED
//
// statement-tavern.hpp
//
#include "interact/statement.hpp"
#include "player/party.hpp"
#include <string>

namespace heroespath
{
namespace interact
{
    namespace talk
    {

        namespace compose
        {
            static inline const StrVec_t TavernServers()
            {
                return { "server", "barkeep",      "barwhipe", "bartender",
                         "wench",  "serving girl", "bar maid" };
            }

            static inline const StrVec_t TavernDrinks()
            {
                return { "ale", "drink", "beer", "mead", "brew", "swill" };
            }

            static inline const StrVec_t TavernDrinkAdjectives()
            {
                return { "delightful", "dreadful", "horrid",   "stinking",  "festering",
                         "rotting",    "murky",    "bubbling", "tasteless", "sour" };
            }

            static inline const StrVec_t TavernNoises()
            {
                return { "(hiccup)", "(burp)", "(grumble)", "(mutter)", "(laugh)" };
            }
        } // namespace compose

        struct TavernTalk
        {
            static const std::string Compose(const Mood, const player::Party &);

        private:
            static const std::string ComposeKind(const player::Party &);
            static const std::string ComposeMean(const player::Party &);
        };
    } // namespace talk
} // namespace interact
} // namespace heroespath

#endif // HEROESPATH_INTERACT_STATEMENTTAVERN_HPP_INCLUDED
