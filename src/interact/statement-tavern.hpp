// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_INTERACT_STATEMENTTAVERN_HPP_INCLUDED
#define HEROESPATH_INTERACT_STATEMENTTAVERN_HPP_INCLUDED
//
// statement-tavern.hpp
//
#include "creature/player-party.hpp"
#include "interact/statement.hpp"
#include <string>

namespace heroespath
{
namespace interact
{
    namespace talk
    {

        namespace compose
        {
            inline const StrVec_t TavernServers()
            {
                return { "server", "barkeep",      "barwhipe", "bartender",
                         "wench",  "serving girl", "bar maid" };
            }

            inline const StrVec_t TavernDrinks()
            {
                return { "ale", "drink", "beer", "mead", "brew", "swill" };
            }

            inline const StrVec_t TavernDrinkAdjectives()
            {
                return { "delightful", "dreadful", "horrid",   "stinking",  "festering",
                         "rotting",    "murky",    "bubbling", "tasteless", "sour" };
            }

            inline const StrVec_t TavernNoises()
            {
                return { "(hiccup)", "(burp)", "(grumble)", "(mutter)", "(laugh)" };
            }
        } // namespace compose

        struct TavernTalk
        {
            static const std::string Compose(const Mood, const creature::PlayerParty &);

        private:
            static const std::string ComposeKind(const creature::PlayerParty &);
            static const std::string ComposeMean(const creature::PlayerParty &);
        };
    } // namespace talk
} // namespace interact
} // namespace heroespath

#endif // HEROESPATH_INTERACT_STATEMENTTAVERN_HPP_INCLUDED
