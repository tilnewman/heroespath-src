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
#ifndef SFMLUTIL_TEXTBUTTON_INCLUDED
#define SFMLUTIL_TEXTBUTTON_INCLUDED
//
// text-button.hpp
//  Drawing and handling code for buttons that are only text.
//
#include "sfml-util/gui/gui-entity-text.hpp"

#include <memory>
#include <string>
#include <vector>


namespace sfml_util
{
namespace gui
{

    class MouseTextInfo;


    //Base class for a text button that has different text styles for mouse positions
    class TextButton : public GuiText
    {
        //prevent copy construction
        TextButton(const TextButton &) =delete;

        //prevent copy assignment
        TextButton & operator=(const TextButton &) =delete;

    public:
        //if using this constructor, Setup() must be called before any other functions
        explicit TextButton(const std::string & NAME);

        TextButton( const std::string &   NAME,
                    const float           POS_LEFT,
                    const float           POS_TOP,
                    const MouseTextInfo & MOUSE_TEXT_INFO,
                    const float           CLICK_SHIFT = CLICK_SHIFT_DEFAULT_);

        virtual ~TextButton();

        void Setup(const float           POS_LEFT,
                   const float           POS_TOP,
                   const MouseTextInfo & MOUSE_TEXT_INFO,
                   const float           CLICK_SHIFT = CLICK_SHIFT_DEFAULT_);

    protected:
        static float CLICK_SHIFT_DEFAULT_;
        inline virtual void OnClick(const sf::Vector2f &) {}

    private:
        float clickShift_;
    };

    using TextButtonSPtr_t = std::shared_ptr<TextButton>;
    using TextButtonSVec_t = std::vector<TextButtonSPtr_t>;

}
}
#endif //SFMLUTIL_TEXTBUTTON_INCLUDED
