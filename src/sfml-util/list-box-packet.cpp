// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// list-box-packet.cpp
//
#include "list-box-packet.hpp"

#include "sfutil/display.hpp"

namespace heroespath
{
namespace gui
{

    const sf::Color ListBoxPacket::DEFAULT_IMAGE_COLOR_ { sf::Color::White };
    const sf::Color ListBoxPacket::DEFAULT_HIGHLIGHT_COLOR_ADJ_ { sf::Color(20, 20, 20, 20) };
    const sf::Color ListBoxPacket::DEFAULT_HIGHLIGHT_COLOR_INVALID_ { 127, 32, 32, 64 };
    const sf::Color ListBoxPacket::DEFAULT_HIGHLIGHT_IMAGE_COLOR_ADJ_ { 100, 100, 100, 100 };

    // makes a 1.2x1.44 border at 1280x960 and 4.32x2.7 border at 2880x1800
    const sf::Vector2f ListBoxPacket::DEFAULT_ELEMENT_PAD_SCREEN_RATIO_V_ { sf::Vector2f(
        0.001f, 0.0015f) };

    // makes a 1.92x1.44 border at 1280x960 and 4.32x2.7 border at 2880x1800
    const sf::Vector2f ListBoxPacket::DEFAULT_IMAGE_PAD_SCREEN_RATIO_V_ { sf::Vector2f(
        0.0015f, 0.0015f) };

    const float ListBoxPacket::DEFAULT_IMAGE_MAX_SIZE_HORIZ_SCREEN_RATIO_ { 0.03f };

    ListBoxPacket::ListBoxPacket(
        const sf::FloatRect & REGION,
        const BoxEntityInfo & BOX_INFO,
        const sf::Color & LINE_COLOR,
        const sf::Color & IMAGE_COLOR,
        const sf::Color & HIGHLIGHT_COLOR_ADJ,
        const sf::Color & HIGHLIGHT_COLOR_INVALID,
        const sf::Color & HIGHLIGHT_IMAGE_COLOR_ADJ,
        const float IMAGE_MAX_SIZE_HORIZ_SCREEN_RATIO,
        const sf::Vector2f & ELEMENT_PAD_SCREEN_RATIO_V,
        const sf::Vector2f & IMAGE_PAD_SCREEN_RATIO_V)
        : region_(REGION)
        , boxInfo_(BOX_INFO)
        , lineColor_(LINE_COLOR)
        , imageColor_(IMAGE_COLOR)
        , highlightColor_(BOX_INFO.color_from + HIGHLIGHT_COLOR_ADJ)
        , highlightColorInvalid_(HIGHLIGHT_COLOR_INVALID)
        , highlightImageColor_(IMAGE_COLOR + HIGHLIGHT_IMAGE_COLOR_ADJ)
        , imageMaxSize_(sfutil::ScreenRatioToPixelsHoriz(IMAGE_MAX_SIZE_HORIZ_SCREEN_RATIO))
        , elementPadV_(
              sfutil::ScreenRatioToPixelsHoriz(ELEMENT_PAD_SCREEN_RATIO_V.x),
              sfutil::ScreenRatioToPixelsVert(ELEMENT_PAD_SCREEN_RATIO_V.y))
        , imagePadV_(
              sfutil::ScreenRatioToPixelsHoriz(IMAGE_PAD_SCREEN_RATIO_V.x),
              sfutil::ScreenRatioToPixelsVert(IMAGE_PAD_SCREEN_RATIO_V.y))
        , elementSizeWithoutPadV_(
              REGION.width - (elementPadV_.x * 2.0f), imageMaxSize_ + (imagePadV_.y * 2.0f))
    {}

    float ListBoxPacket::ImageMaxLength(const bool WILL_INCLUDE_IMAGE_PAD) const
    {
        if (WILL_INCLUDE_IMAGE_PAD)
        {
            return imageMaxSize_ + (imagePadV_.x * 2.0f);
        }
        else
        {
            return imageMaxSize_;
        }
    }

    const sf::Vector2f ListBoxPacket::ImageMaxSize(const bool WILL_INCLUDE_IMAGE_PAD) const
    {
        return sf::Vector2f(
            ImageMaxLength(WILL_INCLUDE_IMAGE_PAD), ImageMaxLength(WILL_INCLUDE_IMAGE_PAD));
    }

    const sf::Vector2f ListBoxPacket::ElementSize(const bool WILL_INCLUDE_ELEMENT_PAD) const
    {
        if (WILL_INCLUDE_ELEMENT_PAD)
        {
            return elementSizeWithoutPadV_ + (elementPadV_ * 2.0f);
        }
        else
        {
            return elementSizeWithoutPadV_;
        }
    }

} // namespace gui
} // namespace heroespath
