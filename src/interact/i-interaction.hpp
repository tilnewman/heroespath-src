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
#ifndef HEROESPATH_INTERACT_I_INTERACTION_HPP_INCLUDED
#define HEROESPATH_INTERACT_I_INTERACTION_HPP_INCLUDED
//
// i-interaction.hpp
//
#include "interact/interact-enum.hpp"
#include "sfml-util/sfml-graphics.hpp"
#include <memory>


namespace heroespath
{
namespace interact
{

    //Pure virtual interface for all Interactions objects.
    struct IInteraction
    {
        virtual ~IInteraction() {}
        virtual Interact::Enum Type() const = 0;
        virtual const std::string Text() const = 0;
        virtual sf::Sprite & SubjectSprite() = 0;
        virtual sf::Sprite & ContextSprite() = 0;
    };

    using InteractionUPtr_t = std::unique_ptr<IInteraction>;

}
}

#endif //HEROESPATH_INTERACT_I_INTERACTION_HPP_INCLUDED