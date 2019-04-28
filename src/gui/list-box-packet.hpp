// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GUI_LISTBOX_PACKET_HPP_INCLUDED
#define HEROESPATH_GUI_LISTBOX_PACKET_HPP_INCLUDED
//
// list-box-packet.hpp
//
#include "gui/box-entity-info.hpp"
#include "sfutil/color.hpp"
#include "sfutil/common.hpp"

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>

#include <memory>

namespace heroespath
{
namespace gui
{

    // Responsible for wrapping the details required to create a ListBox.
    // Note that the constructor takes pad values by screen ratio and then stores the actual
    // distances in pixels.  Also note that the highlight color is specified as an adjustment to
    // the BOX_INFO's background color, but the stored color is the result of that adjustment.
    class ListBoxPacket
    {
    public:
        explicit ListBoxPacket(
            const sf::FloatRect & REGION = sf::FloatRect(0.0f, 0.0f, 0.0f, 0.0f),
            const BoxEntityInfo & BOX_INFO = BoxEntityInfo(),
            const sf::Color & LINE_COLOR = sf::Color::Transparent,
            const sf::Color & IMAGE_COLOR = DEFAULT_IMAGE_COLOR_,
            const sf::Color & HIGHLIGHT_COLOR_ADJ = DEFAULT_HIGHLIGHT_COLOR_ADJ_,
            const sf::Color & HIGHLIGHT_COLOR_INVALID = DEFAULT_HIGHLIGHT_COLOR_INVALID_,
            const sf::Color & HIGHLIGHT_IMAGE_COLOR_ADJ = DEFAULT_HIGHLIGHT_IMAGE_COLOR_ADJ_,
            const float IMAGE_MAX_SIZE_HORIZ_SCREEN_RATIO
            = DEFAULT_IMAGE_MAX_SIZE_HORIZ_SCREEN_RATIO_,
            const sf::Vector2f & ELEMENT_PAD_SCREEN_RATIO_V = DEFAULT_ELEMENT_PAD_SCREEN_RATIO_V_,
            const sf::Vector2f & IMAGE_PAD_SCREEN_RATIO_V = DEFAULT_IMAGE_PAD_SCREEN_RATIO_V_);

        const sf::FloatRect Region() const { return region_; }

        bool HasBoxInfo() const { return (BoxEntityInfo() != boxInfo_); }
        const BoxEntityInfo BoxInfo() const { return boxInfo_; }

        bool HasLineColor() const { return (sf::Color::Transparent != lineColor_); }
        const sf::Color LineColor() const { return lineColor_; }

        const sf::Color ImageColor() const { return imageColor_; }
        const sf::Color HighlightColor() const { return highlightColor_; }
        const sf::Color HighlightColorInvalid() const { return highlightColorInvalid_; }
        const sf::Color HighlightImageColor() const { return highlightImageColor_; }

        // max size of both dimensions of an element image WITHOUT image pad
        float ImageMaxLength(const bool WILL_INCLUDE_IMAGE_PAD = false) const;

        // max size of an element image WITHOUT image pad
        const sf::Vector2f ImageMaxSize(const bool WILL_INCLUDE_IMAGE_PAD = false) const;

        // size of the empty space surrounding an element
        const sf::Vector2f ElementPad() const { return elementPadV_; }

        // size of an element displayed in the ListBox WITHOUT pad
        const sf::Vector2f ElementSize(const bool WILL_INCLUDE_ELEMENT_PAD = false) const;

        // size of the empty space surrounding an element image
        const sf::Vector2f ImagePad() const { return imagePadV_; }

        static const sf::Color DEFAULT_IMAGE_COLOR_;
        static const sf::Color DEFAULT_HIGHLIGHT_COLOR_ADJ_;
        static const sf::Color DEFAULT_HIGHLIGHT_COLOR_INVALID_;
        static const sf::Color DEFAULT_HIGHLIGHT_IMAGE_COLOR_ADJ_;
        static const sf::Vector2f DEFAULT_ELEMENT_PAD_SCREEN_RATIO_V_;
        static const sf::Vector2f DEFAULT_IMAGE_PAD_SCREEN_RATIO_V_;
        static const float DEFAULT_IMAGE_MAX_SIZE_HORIZ_SCREEN_RATIO_;

    private:
        sf::FloatRect region_;
        BoxEntityInfo boxInfo_;
        sf::Color lineColor_;
        sf::Color imageColor_;
        sf::Color highlightColor_;
        sf::Color highlightColorInvalid_;
        sf::Color highlightImageColor_;
        float imageMaxSize_;
        sf::Vector2f elementPadV_;
        sf::Vector2f imagePadV_;
        sf::Vector2f elementSizeWithoutPadV_;
    };

} // namespace gui
} // namespace heroespath

#endif // HEROESPATH_GUI_LISTBOX_PACKET_HPP_INCLUDED
