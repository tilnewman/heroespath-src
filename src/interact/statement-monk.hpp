// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_INTERACT_STATEMENTMONK_HPP_INCLUDED
#define HEROESPATH_INTERACT_STATEMENTMONK_HPP_INCLUDED
//
// statement-monk.hpp
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
            inline const std::vector<std::string> MonkThings()
            {
                return { "the light", "peace", "wisdom", "enlightenment", "truth", "nirvana" };
            }
        } // namespace compose

        struct MonkTalk
        {
            static const std::string Compose(const Mood);

        private:
            static const std::string ComposeKind();
            static const std::string ComposeMean();
        };

    } // namespace talk
} // namespace interact
} // namespace heroespath

#endif // HEROESPATH_INTERACT_STATEMENTMONK_HPP_INCLUDED
