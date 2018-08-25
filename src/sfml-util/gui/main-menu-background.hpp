// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_GUI_MAIN_MENU_BACKGROUND_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_GUI_MAIN_MENU_BACKGROUND_HPP_INCLUDED
//
// main-menu-background.hpp
//
#include <SFML/Graphics/Drawable.hpp>

#include <memory>

namespace heroespath
{
namespace sfml_util
{
    namespace gui
    {

        class BoxEntity;
        using BoxEntityUPtr_t = std::unique_ptr<BoxEntity>;

        // Responsible for implementing a BoxEntity that draws the standard menu stage background
        // tiled image with gradient.
        class MainMenuBackground : public sf::Drawable
        {
        public:
            MainMenuBackground();

            MainMenuBackground(const MainMenuBackground &) = delete;
            MainMenuBackground(MainMenuBackground &&) = delete;
            MainMenuBackground & operator=(const MainMenuBackground &) = delete;
            MainMenuBackground & operator=(MainMenuBackground &&) = delete;

            void draw(sf::RenderTarget & target, sf::RenderStates states) const override;

        private:
            BoxEntityUPtr_t boxUPtr_;
        };

    } // namespace gui
} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_GUI_MAIN_MENU_BACKGROUND_HPP_INCLUDED
