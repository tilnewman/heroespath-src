// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_INTERACT_STATEMENTCOMMONHPP_INCLUDED
#define HEROESPATH_INTERACT_STATEMENTCOMMONHPP_INCLUDED
//
// statement-common.hpp
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

        struct CommonTalk
        {
            static const std::string Compose(const Mood, const creature::PlayerParty &);

        private:
            static const std::string ComposeKind(const creature::PlayerParty &);
            static const std::string ComposeMean(const creature::PlayerParty &);
        };

    } // namespace talk
} // namespace interact
} // namespace heroespath

#endif // HEROESPATH_INTERACT_STATEMENTCOMMONHPP_INCLUDED
