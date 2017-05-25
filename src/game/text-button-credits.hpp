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
#ifndef SFMLUTIL_TEXTBUTTONCREDITS_INCLUDED
#define SFMLUTIL_TEXTBUTTONCREDITS_INCLUDED
//
// text-button-credits.hpp
//  A simple button that transfers to the credits screen
//
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/gui/text-button.hpp"

#include <memory>
#include <string>


namespace game
{

    //Base class for a
    class CreditsButton : public sfml_util::gui::TextButton
    {
        //prevent copy construction and copy assignment
        CreditsButton(const CreditsButton &) =delete;
        CreditsButton & operator=(const CreditsButton &) =delete;

    public:
        //if using this constructor, Setup() must be called before any other functions
        explicit CreditsButton(const std::string & NAME);

        CreditsButton(const std::string &                   NAME,
                      const float                           POS_LEFT,
                      const float                           POS_TOP,
                      const sfml_util::gui::MouseTextInfo & MOUSE_TEXT_INFO,
                      const float                           CLICK_SHIFT = TextButton::CLICK_SHIFT_DEFAULT_);

        virtual ~CreditsButton();

        void Setup(const float                           POS_LEFT,
                   const float                           POS_TOP,
                   const sfml_util::gui::MouseTextInfo & MOUSE_TEXT_INFO,
                   const float                           CLICK_SHIFT = TextButton::CLICK_SHIFT_DEFAULT_);

    protected:
        inline virtual void OnClick(const sf::Vector2f &);
    };

    using CreditsButtonSPtr_t = std::shared_ptr<CreditsButton>;

}
#endif //SFMLUTIL_TEXTBUTTONCREDITS_INCLUDED
