// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_MAINMENUTITLE_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_MAINMENUTITLE_HPP_INCLUDED
//
// main-menu-title.hpp
//  Code that draws the title symbol and title text at the top of a main menu screen
//
#include "sfml-util/sfml-util.hpp"

#include <string>

namespace heroespath
{
namespace sfml_util
{

    // Responsible for drawing the title text and symbol at the top of a main menu stage screen.
    class MainMenuTitle : public sf::Drawable
    {
    public:
        // this constructor calls Setup() and SetPositionAndSize()
        explicit MainMenuTitle(
            const std::string & TITLE_IMAGE_FILENAME = "",
            const bool WILL_INVERT_SYMBOL = false,
            const float SYMBOL_SCALE_HORIZ = 1.0f,
            const float SYMBOL_SCALE_VERT = 1.0f);

        virtual ~MainMenuTitle();

        void Setup(
            const std::string & TITLE_IMAGE_FILENAME = "",
            const bool WILL_INVERT_SYMBOL = false,
            const float SYMBOL_SCALE_HORIZ = 1.0f,
            const float SYMBOL_SCALE_VERT = 1.0f);

        void SetPositionAndSize(
            const float SYMBOL_SCALE_HORIZ = 1.0f, const float SYMBOL_SCALE_VERT = 1.0f);

        virtual void draw(sf::RenderTarget & target, sf::RenderStates states) const;

        const sf::FloatRect Region() const { return symbolSprite_.getGlobalBounds(); }

        float BottomPad() const { return sfml_util::MapByRes(35.0f, 135.0f); }

        float Bottom(const bool WILL_PAD = true) const
        {
            return Region().top + Region().height + ((WILL_PAD) ? BottomPad() : 0.0f);
        }

    private:
        sf::Texture symbolTexture_;
        sf::Sprite symbolSprite_;
        sf::Texture titleTexture_;
        sf::Sprite titleSprite_;
        bool willDrawTitle_;
    };
} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_MAINMENUTITLE_HPP_INCLUDED
