// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_GUIELEMENTS_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_GUIELEMENTS_HPP_INCLUDED
//
// gui-elements.hpp
//
#include "sfml-util/sfml-graphics.hpp"

#include <string>
#include <vector>

namespace heroespath
{
namespace sfml_util
{
    namespace gui
    {

        // Stores the GUI Elements image and provides access to various rects that define sub-images
        // within it.
        class GuiElements
        {
        public:
            GuiElements(const GuiElements &) = delete;
            GuiElements(GuiElements &&) = delete;
            GuiElements & operator=(const GuiElements &) = delete;
            GuiElements & operator=(GuiElements &&) = delete;
            GuiElements() = delete;

            static void LoadTexture();

            // Managed textures
            // these textures are used so often that it saves resources to keep a single instance of
            // them in memory
            static const sf::Texture & GetTexture() { return elementsTexture_; }

            // If the small line box is going to grow to fit inner contents, then these values
            // specity how to grow or shrink each side.
            static float GetLineSmallBoxPadLeft() { return 8.0f; } // subtract this amount from left
            static float GetLineSmallBoxPadRight() { return 9.0f; } // add this amount to right
            static float GetLineSmallBoxPadTop() { return 8.0f; } // subtract this amount from top
            static float GetLineSmallBoxPadBot() { return 9.0f; } // add this amount to bottom

            static const sf::IntRect GetRect_RadioButtonBrightOff()
            {
                return sf::IntRect(0, 0, 20, 20);
            }
            static const sf::IntRect GetRect_RadioButtonBrightOn()
            {
                return sf::IntRect(20, 0, 20, 20);
            }
            static const sf::IntRect GetRect_RadioButtonMedOff()
            {
                return sf::IntRect(40, 0, 20, 20);
            }
            static const sf::IntRect GetRect_RadioButtonMedOn()
            {
                return sf::IntRect(60, 0, 20, 20);
            }
            static const sf::IntRect GetRect_RadioButtonDarkOff()
            {
                return sf::IntRect(80, 0, 20, 20);
            }
            static const sf::IntRect GetRect_RadioButtonDarkOn()
            {
                return sf::IntRect(100, 0, 20, 20);
            }
            static const sf::IntRect GetRect_CheckBoxMedOff()
            {
                return sf::IntRect(120, 0, 26, 23);
            }
            static const sf::IntRect GetRect_CheckBoxMedOn() { return sf::IntRect(146, 0, 26, 23); }
            static const sf::IntRect GetRect_CheckBoxBrightOff()
            {
                return sf::IntRect(172, 0, 26, 23);
            }
            static const sf::IntRect GetRect_CheckBoxBrightOn()
            {
                return sf::IntRect(198, 0, 26, 23);
            }
            static const sf::IntRect GetRect_CheckBoxDarkOff()
            {
                return sf::IntRect(224, 0, 26, 23);
            }
            static const sf::IntRect GetRect_CheckBoxDarkOn() { return sf::IntRect(0, 23, 26, 23); }
            static const sf::IntRect GetRect_ArrowMinusLeftMed()
            {
                return sf::IntRect(26, 23, 27, 28);
            }
            static const sf::IntRect GetRect_ArrowPlusRightMed()
            {
                return sf::IntRect(53, 23, 27, 28);
            }
            static const sf::IntRect GetRect_ArrowMinusLeftBright()
            {
                return sf::IntRect(80, 23, 27, 28);
            }
            static const sf::IntRect GetRect_ArrowPlusRightBright()
            {
                return sf::IntRect(107, 23, 27, 28);
            }
            static const sf::IntRect GetRect_ArrowMinusLeftDark()
            {
                return sf::IntRect(134, 23, 27, 28);
            }
            static const sf::IntRect GetRect_ArrowPlusRightDark()
            {
                return sf::IntRect(161, 23, 27, 28);
            }
            static const sf::IntRect GetRect_ArrowMinusDownBright()
            {
                return sf::IntRect(188, 23, 27, 27);
            }
            static const sf::IntRect GetRect_ArrowPlusUpBright()
            {
                return sf::IntRect(215, 23, 27, 27);
            }
            static const sf::IntRect GetRect_ArrowMinusDownMed()
            {
                return sf::IntRect(0, 51, 27, 27);
            }
            static const sf::IntRect GetRect_ArrowPlusUpMed()
            {
                return sf::IntRect(27, 51, 27, 27);
            }
            static const sf::IntRect GetRect_ArrowMinusDownDark()
            {
                return sf::IntRect(54, 51, 27, 27);
            }
            static const sf::IntRect GetRect_ArrowPlusUpDark()
            {
                return sf::IntRect(81, 51, 27, 27);
            }
            static const sf::IntRect GetRect_ArrowLeftMed() { return sf::IntRect(108, 51, 27, 27); }
            static const sf::IntRect GetRect_ArrowRightMed()
            {
                return sf::IntRect(135, 51, 27, 27);
            }
            static const sf::IntRect GetRect_ArrowDownMed() { return sf::IntRect(162, 51, 27, 27); }
            static const sf::IntRect GetRect_ArrowUpMed() { return sf::IntRect(189, 51, 27, 27); }
            static const sf::IntRect GetRect_ArrowLeftBright()
            {
                return sf::IntRect(216, 51, 27, 27);
            }
            static const sf::IntRect GetRect_ArrowRightBright()
            {
                return sf::IntRect(0, 78, 27, 27);
            }
            static const sf::IntRect GetRect_ArrowDownBright()
            {
                return sf::IntRect(27, 78, 27, 27);
            }
            static const sf::IntRect GetRect_ArrowUpBright() { return sf::IntRect(54, 78, 27, 27); }
            static const sf::IntRect GetRect_ArrowLeftDark() { return sf::IntRect(81, 78, 27, 27); }
            static const sf::IntRect GetRect_ArrowRightDark()
            {
                return sf::IntRect(108, 78, 27, 27);
            }
            static const sf::IntRect GetRect_ArrowDownDark()
            {
                return sf::IntRect(135, 78, 27, 27);
            }
            static const sf::IntRect GetRect_ArrowUpDark() { return sf::IntRect(162, 78, 27, 27); }
            static const sf::IntRect GetRect_ArrowLargeLeft()
            {
                return sf::IntRect(189, 78, 36, 36);
            }
            static const sf::IntRect GetRect_ArrowLargeUp() { return sf::IntRect(0, 114, 36, 36); }
            static const sf::IntRect GetRect_ArrowLargeRight()
            {
                return sf::IntRect(36, 114, 36, 36);
            }
            static const sf::IntRect GetRect_ArrowLargeDown()
            {
                return sf::IntRect(72, 114, 36, 36);
            }
            static const sf::IntRect GetRect_ArrowSmallLeft()
            {
                return sf::IntRect(108, 114, 28, 28);
            }
            static const sf::IntRect GetRect_ArrowSmallUp()
            {
                return sf::IntRect(136, 114, 28, 28);
            }
            static const sf::IntRect GetRect_ArrowSmallRight()
            {
                return sf::IntRect(164, 114, 28, 28);
            }
            static const sf::IntRect GetRect_ArrowSmallDown()
            {
                return sf::IntRect(192, 114, 28, 28);
            }
            static const sf::IntRect GetRect_BoxSmall() { return sf::IntRect(0, 150, 42, 42); }
            static const sf::IntRect GetRect_PadVerticalSmall()
            {
                return sf::IntRect(42, 150, 17, 28);
            }
            static const sf::IntRect GetRect_PadVerticalLarge()
            {
                return sf::IntRect(59, 150, 22, 39);
            }
            static const sf::IntRect GetRect_PadHorizontalLarge()
            {
                return sf::IntRect(81, 150, 40, 21);
            }
            static const sf::IntRect GetRect_HorizontalLineLarge()
            {
                return sf::IntRect(0, 192, 204, 15);
            }
            static const sf::IntRect GetRect_HorizontalLineSmall()
            {
                return sf::IntRect(0, 207, 201, 9);
            }
            static const sf::IntRect GetRect_LineSmallVerticalLeft()
            {
                return sf::IntRect(228, 81, 12, 59);
            }
            static const sf::IntRect GetRect_LineSmallVerticalRight()
            {
                return sf::IntRect(242, 81, 12, 59);
            }
            static const sf::IntRect GetRect_LineSmallEndLeft()
            {
                return sf::IntRect(83, 174, 14, 12);
            }
            static const sf::IntRect GetRect_LineSmallEndRight()
            {
                return sf::IntRect(98, 174, 14, 12);
            }
            static const sf::IntRect GetRect_LineSmallEndTop()
            {
                return sf::IntRect(243, 50, 12, 14);
            }
            static const sf::IntRect GetRect_LineSmallEndBot()
            {
                return sf::IntRect(243, 65, 12, 14);
            }
            static const sf::IntRect GetRect_LineSmallCornerTopLeft()
            {
                return sf::IntRect(0, 217, 19, 19);
            }
            static const sf::IntRect GetRect_LineSmallCornerTopRight()
            {
                return sf::IntRect(20, 217, 19, 19);
            }
            static const sf::IntRect GetRect_LineSmallCornerBotLeft()
            {
                return sf::IntRect(0, 237, 19, 19);
            }
            static const sf::IntRect GetRect_LineSmallCornerBotRight()
            {
                return sf::IntRect(20, 237, 19, 19);
            }
            static const sf::IntRect GetRect_LineSmallHorizontalTop()
            {
                return sf::IntRect(122, 144, 74, 12);
            }
            static const sf::IntRect GetRect_LineSmallHorizontalBot()
            {
                return sf::IntRect(122, 156, 74, 12);
            }
            static const sf::IntRect GetRect_LineSmallCross()
            {
                return sf::IntRect(60, 223, 26, 26);
            }
            static const sf::IntRect GetRect_LineSmallTTop()
            {
                return sf::IntRect(87, 230, 26, 19);
            }
            static const sf::IntRect GetRect_LineSmallTLeft()
            {
                return sf::IntRect(40, 223, 19, 26);
            }
            static const sf::IntRect GetRect_LineSmallTBot()
            {
                return sf::IntRect(114, 223, 26, 19);
            }
            static const sf::IntRect GetRect_LineSmallTRight()
            {
                return sf::IntRect(141, 223, 19, 26);
            }

        private:
            static sf::Texture elementsTexture_;
        };

    } // namespace gui
} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_GUIELEMENTS_HPP_INCLUDED
