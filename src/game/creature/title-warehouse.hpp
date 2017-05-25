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
#ifndef GAME_CREATURE_TITLEWAREHOUSE_HPP_INCLUDED
#define GAME_CREATURE_TITLEWAREHOUSE_HPP_INCLUDED
//
// title-warehouse.hpp
//
#include "game/creature/title-enum.hpp"

#include <memory>
#include <vector>


namespace game
{
namespace creature
{
    //forward declarations
    class Title;
    using TitlePtr_t = Title *;
    using TitleUPtr_t = std::unique_ptr<Title>;
    using TitleUVec_t = std::vector<TitleUPtr_t>;

namespace title
{

    class Warehouse
    {
    public:
        static void Fill();
        static void Empty();
        static bool Test();
        static TitlePtr_t Get(const Titles::Enum);

    private:
        static TitleUVec_t titleUVec_;
    };

}
}
}

#endif //GAME_CREATURE_TITLEWAREHOUSE_HPP_INCLUDED
