// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GUI_GUI_IMAGES_HPP_INCLUDED
#define HEROESPATH_GUI_GUI_IMAGES_HPP_INCLUDED
//
// gui-images.hpp
//
#include <SFML/Graphics/Rect.hpp>

#include <string>

namespace heroespath
{
namespace gui
{

    // Responsible for wrapping all the information about the gui elements image.
    struct GuiImages
    {
        GuiImages() = delete;

        static const std::string PathKey() { return "media-image-misc-gui-elements"; }

        // If the small line box is going to grow to fit inner contents, then these values
        // specify how to grow or shrink each side.
        static float GetLineSmallBoxPadLeft() { return 8.0f; } // subtract this amount from left
        static float GetLineSmallBoxPadRight() { return 9.0f; } // add this amount to right
        static float GetLineSmallBoxPadTop() { return 8.0f; } // subtract this amount from top
        static float GetLineSmallBoxPadBot() { return 9.0f; } // add this amount to bottom

        static const sf::IntRect RadioButtonBrightOffI() { return sf::IntRect(0, 0, 20, 20); }
        static const sf::FloatRect RadioButtonBrightOffF()
        {
            return sf::FloatRect(RadioButtonBrightOffI());
        }
        static const sf::IntRect RadioButtonBrightOnI() { return sf::IntRect(20, 0, 20, 20); }
        static const sf::FloatRect RadioButtonBrightOnF()
        {
            return sf::FloatRect(RadioButtonBrightOnI());
        }
        static const sf::IntRect RadioButtonMedOffI() { return sf::IntRect(40, 0, 20, 20); }
        static const sf::FloatRect RadioButtonMedOffF()
        {
            return sf::FloatRect(RadioButtonMedOffI());
        }
        static const sf::IntRect RadioButtonMedOnI() { return sf::IntRect(60, 0, 20, 20); }
        static const sf::FloatRect RadioButtonMedOnF()
        {
            return sf::FloatRect(RadioButtonMedOnI());
        }
        static const sf::IntRect RadioButtonDarkOffI() { return sf::IntRect(80, 0, 20, 20); }
        static const sf::FloatRect RadioButtonDarkOffF()
        {
            return sf::FloatRect(RadioButtonDarkOffI());
        }
        static const sf::IntRect RadioButtonDarkOnI() { return sf::IntRect(100, 0, 20, 20); }
        static const sf::FloatRect RadioButtonDarkOnF()
        {
            return sf::FloatRect(RadioButtonDarkOnI());
        }
        static const sf::IntRect CheckBoxMedOffI() { return sf::IntRect(120, 0, 26, 23); }
        static const sf::FloatRect CheckBoxMedOffF() { return sf::FloatRect(CheckBoxMedOffI()); }
        static const sf::IntRect CheckBoxMedOnI() { return sf::IntRect(146, 0, 26, 23); }
        static const sf::FloatRect CheckBoxMedOnF() { return sf::FloatRect(CheckBoxMedOnI()); }
        static const sf::IntRect CheckBoxBrightOffI() { return sf::IntRect(172, 0, 26, 23); }
        static const sf::FloatRect CheckBoxBrightOffF()
        {
            return sf::FloatRect(CheckBoxBrightOffI());
        }
        static const sf::IntRect CheckBoxBrightOnI() { return sf::IntRect(198, 0, 26, 23); }
        static const sf::FloatRect CheckBoxBrightOnF()
        {
            return sf::FloatRect(CheckBoxBrightOnI());
        }
        static const sf::IntRect CheckBoxDarkOffI() { return sf::IntRect(224, 0, 26, 23); }
        static const sf::FloatRect CheckBoxDarkOffF() { return sf::FloatRect(CheckBoxDarkOffI()); }
        static const sf::IntRect CheckBoxDarkOnI() { return sf::IntRect(0, 23, 26, 23); }
        static const sf::FloatRect CheckBoxDarkOnF() { return sf::FloatRect(CheckBoxDarkOnI()); }
        static const sf::IntRect ArrowMinusLeftMedI() { return sf::IntRect(26, 23, 27, 28); }
        static const sf::FloatRect ArrowMinusLeftMedF()
        {
            return sf::FloatRect(ArrowMinusLeftMedI());
        }
        static const sf::IntRect ArrowPlusRightMedI() { return sf::IntRect(53, 23, 27, 28); }
        static const sf::FloatRect ArrowPlusRightMedF()
        {
            return sf::FloatRect(ArrowPlusRightMedI());
        }
        static const sf::IntRect ArrowMinusLeftBrightI() { return sf::IntRect(80, 23, 27, 28); }
        static const sf::FloatRect ArrowMinusLeftBrightF()
        {
            return sf::FloatRect(ArrowMinusLeftBrightI());
        }
        static const sf::IntRect ArrowPlusRightBrightI() { return sf::IntRect(107, 23, 27, 28); }
        static const sf::FloatRect ArrowPlusRightBrightF()
        {
            return sf::FloatRect(ArrowPlusRightBrightI());
        }
        static const sf::IntRect ArrowMinusLeftDarkI() { return sf::IntRect(134, 23, 27, 28); }
        static const sf::FloatRect ArrowMinusLeftDarkF()
        {
            return sf::FloatRect(ArrowMinusLeftDarkI());
        }
        static const sf::IntRect ArrowPlusRightDarkI() { return sf::IntRect(161, 23, 27, 28); }
        static const sf::FloatRect ArrowPlusRightDarkF()
        {
            return sf::FloatRect(ArrowPlusRightDarkI());
        }
        static const sf::IntRect ArrowMinusDownBrightI() { return sf::IntRect(188, 23, 27, 27); }
        static const sf::FloatRect ArrowMinusDownBrightF()
        {
            return sf::FloatRect(ArrowMinusDownBrightI());
        }
        static const sf::IntRect ArrowPlusUpBrightI() { return sf::IntRect(215, 23, 27, 27); }
        static const sf::FloatRect ArrowPlusUpBrightF()
        {
            return sf::FloatRect(ArrowPlusUpBrightI());
        }
        static const sf::IntRect ArrowMinusDownMedI() { return sf::IntRect(0, 51, 27, 27); }
        static const sf::FloatRect ArrowMinusDownMedF()
        {
            return sf::FloatRect(ArrowMinusDownMedI());
        }
        static const sf::IntRect ArrowPlusUpMedI() { return sf::IntRect(27, 51, 27, 27); }
        static const sf::FloatRect ArrowPlusUpMedF() { return sf::FloatRect(ArrowPlusUpMedI()); }
        static const sf::IntRect ArrowMinusDownDarkI() { return sf::IntRect(54, 51, 27, 27); }
        static const sf::FloatRect ArrowMinusDownDarkF()
        {
            return sf::FloatRect(ArrowMinusDownDarkI());
        }
        static const sf::IntRect ArrowPlusUpDarkI() { return sf::IntRect(81, 51, 27, 27); }
        static const sf::FloatRect ArrowPlusUpDarkF() { return sf::FloatRect(ArrowPlusUpDarkI()); }
        static const sf::IntRect ArrowLeftMedI() { return sf::IntRect(108, 51, 27, 27); }
        static const sf::FloatRect ArrowLeftMedF() { return sf::FloatRect(ArrowLeftMedI()); }
        static const sf::IntRect ArrowRightMedI() { return sf::IntRect(135, 51, 27, 27); }
        static const sf::FloatRect ArrowRightMedF() { return sf::FloatRect(ArrowRightMedI()); }
        static const sf::IntRect ArrowDownMedI() { return sf::IntRect(162, 51, 27, 27); }
        static const sf::FloatRect ArrowDownMedF() { return sf::FloatRect(ArrowDownMedI()); }
        static const sf::IntRect ArrowUpMedI() { return sf::IntRect(189, 51, 27, 27); }
        static const sf::FloatRect ArrowUpMedF() { return sf::FloatRect(ArrowUpMedI()); }
        static const sf::IntRect ArrowLeftBrightI() { return sf::IntRect(216, 51, 27, 27); }
        static const sf::FloatRect ArrowLeftBrightF() { return sf::FloatRect(ArrowLeftBrightI()); }
        static const sf::IntRect ArrowRightBrightI() { return sf::IntRect(0, 78, 27, 27); }
        static const sf::FloatRect ArrowRightBrightF()
        {
            return sf::FloatRect(ArrowRightBrightI());
        }
        static const sf::IntRect ArrowDownBrightI() { return sf::IntRect(27, 78, 27, 27); }
        static const sf::FloatRect ArrowDownBrightF() { return sf::FloatRect(ArrowDownBrightI()); }
        static const sf::IntRect ArrowUpBrightI() { return sf::IntRect(54, 78, 27, 27); }
        static const sf::FloatRect ArrowUpBrightF() { return sf::FloatRect(ArrowUpBrightI()); }
        static const sf::IntRect ArrowLeftDarkI() { return sf::IntRect(81, 78, 27, 27); }
        static const sf::FloatRect ArrowLeftDarkF() { return sf::FloatRect(ArrowLeftDarkI()); }
        static const sf::IntRect ArrowRightDarkI() { return sf::IntRect(108, 78, 27, 27); }
        static const sf::FloatRect ArrowRightDarkF() { return sf::FloatRect(ArrowRightDarkI()); }
        static const sf::IntRect ArrowDownDarkI() { return sf::IntRect(135, 78, 27, 27); }
        static const sf::FloatRect ArrowDownDarkF() { return sf::FloatRect(ArrowDownDarkI()); }
        static const sf::IntRect ArrowUpDarkI() { return sf::IntRect(162, 78, 27, 27); }
        static const sf::FloatRect ArrowUpDarkF() { return sf::FloatRect(ArrowUpDarkI()); }
        static const sf::IntRect ArrowLargeLeftI() { return sf::IntRect(189, 78, 36, 36); }
        static const sf::FloatRect ArrowLargeLeftF() { return sf::FloatRect(ArrowLargeLeftI()); }
        static const sf::IntRect ArrowLargeUpI() { return sf::IntRect(0, 114, 36, 36); }
        static const sf::FloatRect ArrowLargeUpF() { return sf::FloatRect(ArrowLargeUpI()); }
        static const sf::IntRect ArrowLargeRightI() { return sf::IntRect(36, 114, 36, 36); }
        static const sf::FloatRect ArrowLargeRightF() { return sf::FloatRect(ArrowLargeRightI()); }
        static const sf::IntRect ArrowLargeDownI() { return sf::IntRect(72, 114, 36, 36); }
        static const sf::FloatRect ArrowLargeDownF() { return sf::FloatRect(ArrowLargeDownI()); }
        static const sf::IntRect ArrowSmallLeftI() { return sf::IntRect(108, 114, 28, 28); }
        static const sf::FloatRect ArrowSmallLeftF() { return sf::FloatRect(ArrowSmallLeftI()); }
        static const sf::IntRect ArrowSmallUpI() { return sf::IntRect(136, 114, 28, 28); }
        static const sf::FloatRect ArrowSmallUpF() { return sf::FloatRect(ArrowSmallUpI()); }
        static const sf::IntRect ArrowSmallRightI() { return sf::IntRect(164, 114, 28, 28); }
        static const sf::FloatRect ArrowSmallRightF() { return sf::FloatRect(ArrowSmallRightI()); }
        static const sf::IntRect ArrowSmallDownI() { return sf::IntRect(192, 114, 28, 28); }
        static const sf::FloatRect ArrowSmallDownF() { return sf::FloatRect(ArrowSmallDownI()); }
        static const sf::IntRect BoxSmallI() { return sf::IntRect(0, 150, 42, 42); }
        static const sf::FloatRect BoxSmallF() { return sf::FloatRect(BoxSmallI()); }
        static const sf::IntRect PadVerticalSmallI() { return sf::IntRect(42, 150, 17, 28); }
        static const sf::FloatRect PadVerticalSmallF()
        {
            return sf::FloatRect(PadVerticalSmallI());
        }
        static const sf::IntRect PadVerticalLargeI() { return sf::IntRect(59, 150, 22, 39); }
        static const sf::FloatRect PadVerticalLargeF()
        {
            return sf::FloatRect(PadVerticalLargeI());
        }
        static const sf::IntRect PadHorizontalLargeI() { return sf::IntRect(81, 150, 40, 21); }
        static const sf::FloatRect PadHorizontalLargeF()
        {
            return sf::FloatRect(PadHorizontalLargeI());
        }
        static const sf::IntRect HorizontalLineLargeI() { return sf::IntRect(0, 192, 204, 15); }
        static const sf::FloatRect HorizontalLineLargeF()
        {
            return sf::FloatRect(HorizontalLineLargeI());
        }
        static const sf::IntRect HorizontalLineSmallI() { return sf::IntRect(0, 207, 201, 9); }
        static const sf::FloatRect HorizontalLineSmallF()
        {
            return sf::FloatRect(HorizontalLineSmallI());
        }
        static const sf::IntRect LineSmallVerticalLeftI() { return sf::IntRect(228, 81, 12, 59); }
        static const sf::FloatRect LineSmallVerticalLeftF()
        {
            return sf::FloatRect(LineSmallVerticalLeftI());
        }
        static const sf::IntRect LineSmallVerticalRightI() { return sf::IntRect(242, 81, 12, 59); }
        static const sf::FloatRect LineSmallVerticalRightF()
        {
            return sf::FloatRect(LineSmallVerticalRightI());
        }
        static const sf::IntRect LineSmallEndLeftI() { return sf::IntRect(83, 174, 14, 12); }
        static const sf::FloatRect LineSmallEndLeftF()
        {
            return sf::FloatRect(LineSmallEndLeftI());
        }
        static const sf::IntRect LineSmallEndRightI() { return sf::IntRect(98, 174, 14, 12); }
        static const sf::FloatRect LineSmallEndRightF()
        {
            return sf::FloatRect(LineSmallEndRightI());
        }
        static const sf::IntRect LineSmallEndTopI() { return sf::IntRect(243, 50, 12, 14); }
        static const sf::FloatRect LineSmallEndTopF() { return sf::FloatRect(LineSmallEndTopI()); }
        static const sf::IntRect LineSmallEndBotI() { return sf::IntRect(243, 65, 12, 14); }
        static const sf::FloatRect LineSmallEndBotF() { return sf::FloatRect(LineSmallEndBotI()); }
        static const sf::IntRect LineSmallCornerTopLeftI() { return sf::IntRect(0, 217, 19, 19); }
        static const sf::FloatRect LineSmallCornerTopLeftF()
        {
            return sf::FloatRect(LineSmallCornerTopLeftI());
        }
        static const sf::IntRect LineSmallCornerTopRightI() { return sf::IntRect(20, 217, 19, 19); }
        static const sf::FloatRect LineSmallCornerTopRightF()
        {
            return sf::FloatRect(LineSmallCornerTopRightI());
        }
        static const sf::IntRect LineSmallCornerBotLeftI() { return sf::IntRect(0, 237, 19, 19); }
        static const sf::FloatRect LineSmallCornerBotLeftF()
        {
            return sf::FloatRect(LineSmallCornerBotLeftI());
        }
        static const sf::IntRect LineSmallCornerBotRightI() { return sf::IntRect(20, 237, 19, 19); }
        static const sf::FloatRect LineSmallCornerBotRightF()
        {
            return sf::FloatRect(LineSmallCornerBotRightI());
        }
        static const sf::IntRect LineSmallHorizontalTopI() { return sf::IntRect(122, 144, 74, 12); }
        static const sf::FloatRect LineSmallHorizontalTopF()
        {
            return sf::FloatRect(LineSmallHorizontalTopI());
        }
        static const sf::IntRect LineSmallHorizontalBotI() { return sf::IntRect(122, 156, 74, 12); }
        static const sf::FloatRect LineSmallHorizontalBotF()
        {
            return sf::FloatRect(LineSmallHorizontalBotI());
        }
        static const sf::IntRect LineSmallCrossI() { return sf::IntRect(60, 223, 26, 26); }
        static const sf::FloatRect LineSmallCrossF() { return sf::FloatRect(LineSmallCrossI()); }
        static const sf::IntRect LineSmallTTopI() { return sf::IntRect(87, 230, 26, 19); }
        static const sf::FloatRect LineSmallTTopF() { return sf::FloatRect(LineSmallTTopI()); }
        static const sf::IntRect LineSmallTLeftI() { return sf::IntRect(40, 223, 19, 26); }
        static const sf::FloatRect LineSmallTLeftF() { return sf::FloatRect(LineSmallTLeftI()); }
        static const sf::IntRect LineSmallTBotI() { return sf::IntRect(114, 223, 26, 19); }
        static const sf::FloatRect LineSmallTBotF() { return sf::FloatRect(LineSmallTBotI()); }
        static const sf::IntRect LineSmallTRightI() { return sf::IntRect(141, 223, 19, 26); }
        static const sf::FloatRect LineSmallTRightF() { return sf::FloatRect(LineSmallTRightI()); }
    };

} // namespace gui
} // namespace heroespath

#endif // HEROESPATH_GUI_GUI_IMAGES_HPP_INCLUDED
