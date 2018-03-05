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
#ifndef HEROESPATH_INTERACT_BUTTON_ENUM_HPP_INCLUDED
#define HEROESPATH_INTERACT_BUTTON_ENUM_HPP_INCLUDED
//
// interact-button-enum.hpp
//
#include "sfml-util/gui/text-button.hpp"
#include <SFML/Window/Keyboard.hpp>
#include <string>
#include <vector>

namespace heroespath
{
namespace interact
{

    // Responsible for wrapping all the information needed to implement a
    // button used by interactions.
    class Button
    {
    public:
        explicit Button(
            const std::string & NAME, const sf::Keyboard::Key KEY = sf::Keyboard::Unknown);

        const std::string Name() const { return name_; }
        sf::Keyboard::Key Key() const { return key_; }

        bool DoPointersMatch(const sfml_util::gui::TextButton * const P) const
        {
            return (P == ptr_);
        }

        sfml_util::gui::TextButtonUPtr_t
            Make(sfml_util::gui::callback::ITextButtonCallbackHandler_t *);

    private:
        std::string name_;
        sf::Keyboard::Key key_;
        sfml_util::gui::TextButton * ptr_;
    };

    using ButtonVec_t = std::vector<Button>;
}
}

#endif // HEROESPATH_MAP_INTERACT_ENUM_HPP_INCLUDED
