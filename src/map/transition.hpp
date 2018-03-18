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
#ifndef HEROESPATH_MAP_TRANSITION_HPP_INCLUDED
#define HEROESPATH_MAP_TRANSITION_HPP_INCLUDED
//
// transition.hpp
//
#include "map/level-enum.hpp"
#include "sfml-util/sound-effects-enum.hpp"

#include <SFML/Graphics/Rect.hpp>

#include <vector>

namespace heroespath
{
namespace map
{

    // Responsible for storing all the information about a transition between two maps.
    class Transition
    {
    public:
        Transition(
            const bool IS_ENTRY = true,
            const Level::Enum LEVEL = Level::Count,
            const sf::FloatRect & RECT = sf::FloatRect(),
            const sfml_util::sound_effect::MapTransition TRANS_TYPE
            = sfml_util::sound_effect::MapTransition::Count)
            : isEntry_(IS_ENTRY)
            , level_(LEVEL)
            , rect_(RECT)
            , transSfxType_(TRANS_TYPE)
        {}

        inline bool IsEntry() const { return isEntry_; }
        inline Level::Enum WhichLevel() const { return level_; }
        inline const sf::FloatRect Rect() const { return rect_; }
        inline sfml_util::sound_effect::MapTransition TransType() const { return transSfxType_; }

        inline const sf::Vector2f Center() const
        {
            return sf::Vector2f(
                rect_.left + (rect_.width * 0.5f), rect_.top + (rect_.height * 0.5f));
        }

    private:
        bool isEntry_;
        Level::Enum level_;
        sf::FloatRect rect_;
        sfml_util::sound_effect::MapTransition transSfxType_;
    };

    using TransitionVec_t = std::vector<Transition>;
} // namespace map
} // namespace heroespath

#endif // HEROESPATH_MAP_TRANSITION_HPP_INCLUDED
