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
#ifndef HEROESPATH_SFMLUTIL_GUI_CROSSBOX_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_GUI_CROSSBOX_HPP_INCLUDED
//
// crossbox.hpp
//  A crossed box drawing utility class
//
#include "sfml-util/gui/box.hpp"
#include "sfml-util/gui/line.hpp"
#include "sfml-util/sfml-graphics.hpp"

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

            float GetMinWidth_CrossBox(const bool WILL_GROW_TO_FIT);
            float GetMinHeight_CrossBox(const bool WILL_GROW_TO_FIT);

            // A box drawing class for when the box is segmented into four parts.
            class CrossBox : public Box
            {
            public:
                CrossBox(const CrossBox &) = delete;
                CrossBox(const CrossBox &&) = delete;
                CrossBox & operator=(const CrossBox &) = delete;
                CrossBox & operator=(const CrossBox &&) = delete;

            public:
                CrossBox(
                    const std::string & NAME,
                    const float POS_LEFT,
                    const float POS_TOP,
                    const float INTENDED_FIRST_WIDTH,
                    const float INTENDED_FIRST_HEIGHT,
                    const float INTENDED_SECOND_WIDTH,
                    const float INTENDED_SECOND_HEIGHT,
                    const bool WILL_GROW_TO_FIT = false,
                    const BackgroundInfo & BACKGROUND_INFO = BackgroundInfo());

                // For when the initial position and sizes are not known,
                // but calling SetupCrossBox() after construction is required
                // before drawing.
                explicit CrossBox(
                    const std::string & NAME,
                    const bool WILL_GROW_TO_FIT = false,
                    const BackgroundInfo & BACKGROUND_INFO = BackgroundInfo());

                virtual ~CrossBox();

                virtual void draw(sf::RenderTarget & target, sf::RenderStates states) const;

                // CrossBox has it's own Setup function "SetupCrossBox()",
                // so hide Box::SetupBox().
                void SetupCrossBox(
                    const float POS_LEFT,
                    const float POS_TOP,
                    const float INTENDED_FIRST_WIDTH,
                    const float INTENDED_FIRST_HEIGHT,
                    const float INTENDED_SECOND_WIDTH,
                    const float INTENDED_SECOND_HEIGHT,
                    const bool WILL_GROW_TO_FIT,
                    const BackgroundInfo & BACKGROUND_INFO = BackgroundInfo());

            private:
                void SetupBox(
                    const float POS_LEFT,
                    const float POS_TOP,
                    const float INTENDED_WIDTH,
                    const float INTENDED_HEIGHT,
                    const bool WILL_GROW_TO_FIT);

            protected:
                virtual void OnColorChange();

            protected:
                Line topLine2_;
                Line rightLine2_;
                Line botLine2_;
                Line leftLine2_;
                Line centerLeftLine_;
                Line centerRightLine_;
                Line centerTopLine_;
                Line centerBotLine_;
                sf::Sprite crossSprite_;
                sf::Sprite tJointLeftSprite_;
                sf::Sprite tJointRightSprite_;
                sf::Sprite tJointTopSprite_;
                sf::Sprite tJointBotSprite_;
            };

            using CrossBoxSPtr_t = std::shared_ptr<CrossBox>;
            using CrossBoxSVec_t = std::vector<CrossBoxSPtr_t>;
        } // namespace box
    } // namespace gui
} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_GUI_CROSSBOX_HPP_INCLUDED
