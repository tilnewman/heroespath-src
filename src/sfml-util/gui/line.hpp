// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
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
        public:
            Line(const Line &) = delete;
            Line(Line &&) = delete;
            Line & operator=(const Line &) = delete;
            Line & operator=(Line &&) = delete;

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

            virtual bool UpdateMousePos(const sf::Vector2f &) { return false; }

            virtual void SetEntityPos(const float POS_LEFT, const float POS_TOP);
            virtual void MoveEntityPos(const float HORIZ, const float VERT);

        protected:
            virtual void OnClick(const sf::Vector2f &) {}
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
