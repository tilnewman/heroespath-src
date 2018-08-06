// ----------------------------------------------------------------------------
// "THE BEER-WARE RICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// box-entity-info.cpp
//
#include "box-entity-info.hpp"

namespace heroespath
{
namespace sfml_util
{
    namespace gui
    {

        BoxEntityInfo::BoxEntityInfo()
            : color_from(defaults::None)
            , color_to(defaults::None)
            , color_from_sides(Side::None)
            , color_from_corners(Corner::None)
            , will_draw_border(false)
            , will_grow_border(false)
            , line_thickness(0.0f)
            , line_color(defaults::None)
            , pad_line_color_adj(defaults::None)
            , cached_texture_opt()
            , texture_rect_opt_()
            , will_size_instead_of_fit(false)
            , focus_colors()
            , will_draw_color_over_sprite(true)
        {}

        BoxEntityInfo::BoxEntityInfo(
            const sf::Color & COLOR_FROM,
            const sf::Color & COLOR_TO,
            const Side::Enum SIDES_WITH_FROM_COLOR,
            const Corner::Enum CORNERS_WITH_FROM_COLOR)
            : color_from(defaults::None)
            , color_to(defaults::None)
            , color_from_sides(Side::None)
            , color_from_corners(Corner::None)
            , will_draw_border(false)
            , will_grow_border(false)
            , line_thickness(0.0f)
            , line_color(defaults::None)
            , pad_line_color_adj(defaults::None)
            , cached_texture_opt()
            , texture_rect_opt_()
            , will_size_instead_of_fit(false)
            , focus_colors()
            , will_draw_color_over_sprite(true)
        {
            SetupColor(COLOR_FROM, COLOR_TO, SIDES_WITH_FROM_COLOR, CORNERS_WITH_FROM_COLOR);
        }

        BoxEntityInfo::BoxEntityInfo(
            const bool WILL_GROW_BORDER_TO_CONTAIN_REGION,
            const float LINE_THICKNESS,
            const sf::Color & LINE_COLOR,
            const sf::Color & PAD_LINE_COLOR_ADJ)
            : color_from(defaults::None)
            , color_to(defaults::None)
            , color_from_sides(Side::None)
            , color_from_corners(Corner::None)
            , will_draw_border(false)
            , will_grow_border(false)
            , line_thickness(0.0f)
            , line_color(defaults::None)
            , pad_line_color_adj(defaults::None)
            , cached_texture_opt()
            , texture_rect_opt_()
            , will_size_instead_of_fit(false)
            , focus_colors()
            , will_draw_color_over_sprite(true)
        {
            SetupBorder(
                WILL_GROW_BORDER_TO_CONTAIN_REGION, LINE_THICKNESS, LINE_COLOR, PAD_LINE_COLOR_ADJ);
        }

        BoxEntityInfo::BoxEntityInfo(
            const CachedTexture & CACHED_TEXTURE,
            const IntRectOpt_t & TEXTURE_RECT_OPT,
            const bool WILL_SIZE_INSTEAD_OF_FIT)
            : color_from(defaults::None)
            , color_to(defaults::None)
            , color_from_sides(Side::None)
            , color_from_corners(Corner::None)
            , will_draw_border(false)
            , will_grow_border(false)
            , line_thickness(0.0f)
            , line_color(defaults::None)
            , pad_line_color_adj(defaults::None)
            , cached_texture_opt()
            , texture_rect_opt_(TEXTURE_RECT_OPT)
            , will_size_instead_of_fit(false)
            , focus_colors()
            , will_draw_color_over_sprite(true)
        {
            SetupImage(CACHED_TEXTURE, TEXTURE_RECT_OPT, WILL_SIZE_INSTEAD_OF_FIT);
        }

        void BoxEntityInfo::SetupColor(
            const sf::Color & COLOR_FROM,
            const sf::Color & COLOR_TO,
            const Side::Enum SIDES_WITH_FROM_COLOR,
            const Corner::Enum CORNERS_WITH_FROM_COLOR)
        {
            color_from = COLOR_FROM;
            color_to = COLOR_TO;
            color_from_sides = SIDES_WITH_FROM_COLOR;
            color_from_corners = CORNERS_WITH_FROM_COLOR;
        }

        void BoxEntityInfo::SetupBorder(
            const bool WILL_GROW_BORDER_TO_CONTAIN_REGION,
            const float LINE_THICKNESS,
            const sf::Color & LINE_COLOR,
            const sf::Color & PAD_LINE_COLOR_ADJ)
        {
            will_grow_border = WILL_GROW_BORDER_TO_CONTAIN_REGION;
            line_thickness = LINE_THICKNESS;
            line_color = LINE_COLOR;
            pad_line_color_adj = PAD_LINE_COLOR_ADJ;
        }

        void BoxEntityInfo::SetupImage(
            const CachedTexture & CACHED_TEXTURE,
            const IntRectOpt_t & TEXTURE_RECT_OPT,
            const bool WILL_SIZE_INSTEAD_OF_FIT)
        {
            cached_texture_opt = CACHED_TEXTURE;
            texture_rect_opt_ = TEXTURE_RECT_OPT;
            will_size_instead_of_fit = WILL_SIZE_INSTEAD_OF_FIT;
        }

    } // namespace gui
} // namespace sfml_util
} // namespace heroespath
