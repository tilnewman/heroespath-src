// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_BOX_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_BOX_HPP_INCLUDED
//
// box.hpp
//  Box drawing utility class
//
#include "sfml-util/gui/background-image.hpp"
#include "sfml-util/gui/box-info.hpp"
#include "sfml-util/gui/gui-entity.hpp"
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

            float GetMinWidth_Box(const bool WILL_GROW_TO_FIT);
            float GetMinHeight_Box(const bool WILL_GROW_TO_FIT);

            // A simple box drawing utitily class
            // Assumes corner images are symmetrical
            class Box : public GuiEntity
            {
            public:
                Box(const Box &) = delete;
                Box(Box &&) = delete;
                Box & operator=(const Box &) = delete;
                Box & operator=(Box &&) = delete;

            public:
                // if no box::Info object is given, then Setup() must be called before any other
                // function
                explicit Box(const std::string & NAME, const box::Info & BOX_INFO = box::Info());

                virtual ~Box();

                virtual void draw(sf::RenderTarget & target, sf::RenderStates states) const;

                virtual bool UpdateMousePos(const sf::Vector2f &) { return false; }

                void SetupBox(const box::Info & BOX_INFO = box::Info());

                virtual void SetEntityPos(const float POS_LEFT, const float POS_TOP);
                virtual void MoveEntityPos(const float HORIZ, const float VERT);

                virtual const box::Info BoxInfo() const { return boxInfo_; }

                virtual bool SetHasFocus(const bool);

            protected:
                virtual void OnClick(const sf::Vector2f &) {}
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
        } // namespace box
    } // namespace gui
} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_BOX_HPP_INCLUDED
