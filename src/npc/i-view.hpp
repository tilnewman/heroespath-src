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
#ifndef HEROESPATH_NPC_IVIEW_HPP_INCLUDED
#define HEROESPATH_NPC_IVIEW_HPP_INCLUDED
//
// i-view.hpp
//
#include "npc/pose-enum.hpp"
#include "sfml-util/direction-enum.hpp"
#include <memory>


namespace sf
{
    class Sprite;
}

namespace heroespath
{
namespace npc
{

    //Pure virtual interface for all NPC classes.
    struct IView
    {
        virtual ~IView() {}

        virtual void Set(const Pose::Enum, const sfml_util::Direction::Enum) = 0;

        //returns true if the current animation finished
        virtual bool Update(const float TIME_ELAPSED) = 0;

        virtual sfml_util::Direction::Enum Direction() const = 0;

        virtual Pose::Enum Pose() const = 0;
        
        virtual void Sprite(sf::Sprite &) const = 0;
    };

    using IViewUPtr_t = std::unique_ptr<IView>;

}
}


#endif //HEROESPATH_NPC_IVIEW_HPP_INCLUDED
