// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_INTERACT_STATEMENT_FACTORY_HPP_INCLUDED
#define HEROESPATH_INTERACT_STATEMENT_FACTORY_HPP_INCLUDED
//
// statement-factory.hpp
//
#include "interact/statement.hpp"
#include <string>

namespace heroespath
{
namespace interact
{
    namespace talk
    {

        struct Factory
        {
            static const std::string Make(const CategoryVec_t &, const Mood = Mood::Kind);
        };

    } // namespace talk
} // namespace interact
} // namespace heroespath

#endif // HEROESPATH_INTERACT_STATEMENT_FACTORY_HPP_INCLUDED
