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
#ifndef HEROESPATH_SFMLUTIL_BOX_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_BOX_HPP_INCLUDED
//
// box.hpp
//  Box drawing utility class
//
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/gui/gui-entity.hpp"
#include "sfml-util/gui/line.hpp"
#include "sfml-util/gui/background-image.hpp"
#include "sfml-util/gui/box-info.hpp"

#include <memory>
#include <string>
#include <vector>


namespace heroespath
{
namespace sfml_util
{
namespace gui
{
namespace box
{

    float GetMinWidth_Box(const bool WILL_GROW_TO_FIT);
    float GetMinHeight_Box(const bool WILL_GROW_TO_FIT);


    //A simple box drawing utitily class
    //Assumes corner images are symmetrical
    class Box : public GuiEntity
    {
        Box(const Box &) =delete;
        Box & operator=(const Box &) =delete;

    public:
        //if no box::Info object is given, then Setup() must be called before any other function
        explicit Box(const std::string & NAME, const box::Info & BOX_INFO = box::Info());

        virtual ~Box();

        virtual void draw(sf::RenderTarget & target, sf::RenderStates states) const;

        inline virtual bool UpdateMousePos(const sf::Vector2f &) { return false; }

        void SetupBox(const box::Info & BOX_INFO = box::Info());

        virtual void SetEntityPos(const float POS_LEFT, const float POS_TOP);
        virtual void MoveEntityPos(const float HORIZ, const float VERT);

        inline virtual const box::Info BoxInfo() const { return boxInfo_; }

        virtual bool SetHasFocus(const bool);

    protected:
        inline virtual void OnClick(const sf::Vector2f &) {}
        virtual void OnColorChange();

        box::Info boxInfo_;
        Line topLine_;
        Line rightLine_;
        Line botLine_;
        Line leftLine_;
        sf::Sprite cTopLeftSprite_;
        sf::Sprite cTopRightSprite_;
        sf::Sprite cBotLeftSprite_;
        sf::Sprite cBotRightSprite_;
        BackgroundImage backgroundImage_;
    };

    using BoxUPtr_t = std::unique_ptr<Box>;

}
}
}
}

#endif //HEROESPATH_SFMLUTIL_BOX_HPP_INCLUDED
