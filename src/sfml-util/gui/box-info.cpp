// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// box-info.cpp
//
#include "box-info.hpp"

#include "misc/real.hpp"

#include <string>
#include <tuple>

namespace heroespath
{
namespace sfml_util
{
    namespace gui
    {
        namespace box
        {

            const sf::Uint8 Info::DEFAULT_FRAME_LINES_ALPHA_(127);

            Info::Info(
                const ColorSet & COLOR_SET,
                const sf::FloatRect & REGION,
                const bool GOLD_BARS,
                const float LINE_THICKNESS,
                const bool WILL_FRAME_LINES,
                const bool WILL_GROW,
                const BackgroundInfo & BACKGROUND_INFO)
                : gold_bars(GOLD_BARS)
                , will_grow(WILL_GROW)
                , frame_lines(WILL_FRAME_LINES)
                , line_thickness(LINE_THICKNESS)
                , color_set(COLOR_SET)
                , region(REGION)
                , bg_info(BACKGROUND_INFO)
            {}

            Info::Info(
                const float LINE_THICKNESS,
                const bool WILL_FRAME_LINES,
                const sf::FloatRect & REGION,
                const ColorSet & COLOR_SET,
                const BackgroundInfo & BACKGROUND_INFO)
                : gold_bars(false)
                , will_grow(false)
                , frame_lines(WILL_FRAME_LINES)
                , line_thickness(LINE_THICKNESS)
                , color_set(COLOR_SET)
                , region(REGION)
                , bg_info(BACKGROUND_INFO)
            {}

            Info::Info(
                const float LINE_THICKNESS,
                const bool WILL_FRAME_LINES,
                const sf::FloatRect & REGION,
                const std::string & BG_TEXTURE_PATH,
                const sf::Color & BG_GRADIENT_TO_COLOR,
                const unsigned int BG_GRADIENT_SIDES,
                const unsigned int BG_GRADIENT_CORNERS)
                : gold_bars(false)
                , will_grow(false)
                , frame_lines(WILL_FRAME_LINES)
                , line_thickness(LINE_THICKNESS)
                , color_set(sfml_util::gui::ColorSet())
                , region(REGION)
                , bg_info(
                      BG_TEXTURE_PATH,
                      REGION,
                      sf::Color::White,
                      sfml_util::GradientInfo(
                          BG_GRADIENT_TO_COLOR, BG_GRADIENT_SIDES, BG_GRADIENT_CORNERS))
            {}

            Info::Info(
                const bool WILL_GROW,
                const sf::FloatRect & REGION,
                const ColorSet & COLOR_SET,
                const BackgroundInfo & BACKGROUND_INFO)
                : gold_bars(true)
                , will_grow(WILL_GROW)
                , frame_lines(false)
                , line_thickness(0.0f)
                , color_set(COLOR_SET)
                , region(REGION)
                , bg_info(BACKGROUND_INFO)
            {}

            Info::Info(
                const bool WILL_GROW,
                const sf::FloatRect & REGION,
                const std::string & BG_TEXTURE_PATH,
                const sf::Color & BG_GRADIENT_TO_COLOR,
                const unsigned int BG_GRADIENT_SIDES,
                const unsigned int BG_GRADIENT_CORNERS)
                : gold_bars(false)
                , will_grow(WILL_GROW)
                , frame_lines(false)
                , line_thickness(0)
                , color_set(sfml_util::gui::ColorSet())
                , region(REGION)
                , bg_info(
                      BG_TEXTURE_PATH,
                      REGION,
                      sf::Color::White,
                      sfml_util::GradientInfo(
                          BG_GRADIENT_TO_COLOR, BG_GRADIENT_SIDES, BG_GRADIENT_CORNERS))
            {}

            bool operator==(const Info & L, const Info & R)
            {
                if (misc::IsRealClose(L.line_thickness, R.line_thickness) == false)
                {
                    return false;
                }
                else
                {
                    return std::tie(
                               L.gold_bars,
                               L.will_grow,
                               L.frame_lines,
                               L.color_set,
                               L.region,
                               L.bg_info)
                        == std::tie(
                               R.gold_bars,
                               R.will_grow,
                               R.frame_lines,
                               R.color_set,
                               R.region,
                               R.bg_info);
                }
            }
        } // namespace box
    } // namespace gui
} // namespace sfml_util
} // namespace heroespath
