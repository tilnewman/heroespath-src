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
#ifndef HEROESPATH_SFMLUTIL_LINE_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_LINE_HPP_INCLUDED
//
// line.hpp
//  Line drawing utility class
//
#include "gui-entity.hpp"
#include "sfml-util/orientation-enum.hpp"
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/side-enum.hpp"

#include <memory>
#include <string>
#include <vector>

namespace heroespath
{
namespace sfml_util
{
    namespace gui
    {

        // A simple line drawing gui entity
        class Line : public GuiEntity
        {
            Line(const Line &) = delete;
            Line(const Line &&) = delete;
            Line & operator=(const Line &) = delete;
            Line & operator=(const Line &&) = delete;

        public:
            Line(
                const std::string & NAME,
                const float LEFT,
                const float TOP,
                const std::size_t LENGTH,
                Orientation::Enum ORIENTATION,
                const Side::Enum SIDE,
                const bool WILL_CAP_ENDS = true);

            // Use this constructor when position and length are not initially known,
            // but must still call Setup() before any other functions.
            Line(
                const std::string & NAME,
                Orientation::Enum ORIENTATION,
                const Side::Enum SIDE,
                const bool WILL_CAP_ENDS);

            virtual ~Line() = default;

            void Setup(const float LEFT, const float TOP, const std::size_t LENGTH);

            virtual void draw(sf::RenderTarget & target, sf::RenderStates states) const;

            inline virtual bool UpdateMousePos(const sf::Vector2f &) { return false; }

            virtual void SetEntityPos(const float POS_LEFT, const float POS_TOP);
            virtual void MoveEntityPos(const float HORIZ, const float VERT);

        protected:
            inline virtual void OnClick(const sf::Vector2f &) {}
            virtual void OnColorChange();

            void SetupBaseSprites();
            void SetupMiddleSprites();
            void SetupOffScreenTexture();

        private:
            std::size_t length_;
            const Orientation::Enum ORIENTATION_;
            const Side::Enum SIDE_;
            const bool WILL_CAP_ENDS_;
            std::size_t pixelsOfMiddleToUse_;
            std::size_t middleCount_;
            sf::Sprite middleSprite_;
            sf::Sprite endTopOrLeftSprite_;
            sf::Sprite endBotOrRightSprite_;
            sf::Sprite finalSprite_; // used to draw to screen
            sf::RenderTexture offScreenTexture_;
        };
    } // namespace gui
} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_LINE_HPP_INCLUDED
