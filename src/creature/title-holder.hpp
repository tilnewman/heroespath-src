// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_CREATURE_TITLE_HOLDER_HPP_INCLUDED
#define HEROESPATH_CREATURE_TITLE_HOLDER_HPP_INCLUDED
//
// title-holder.hpp
//
#include "creature/role-enum.hpp"
#include "creature/title-enum.hpp"
#include "creature/title.hpp"

#include <memory>
#include <vector>

namespace heroespath
{
namespace creature
{

    using TitleUPtr_t = std::unique_ptr<Title>;
    using TitleUVec_t = std::vector<TitleUPtr_t>;

    namespace title
    {

        // Responsible for the lifetime of all Titles in the game.
        class Holder
        {
        public:
            static void Fill();
            static void Empty();
            static TitlePtr_t Get(const Titles::Enum);

        private:
            static TitleUVec_t titleUVec_;
        };

    } // namespace title
} // namespace creature
} // namespace heroespath

#endif // HEROESPATH_CREATURE_TITLE_HOLDER_HPP_INCLUDED
