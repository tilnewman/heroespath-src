//
// box-info.cpp
//
#include "box-info.hpp"

#include "utilz/real.hpp"


namespace sfml_util
{
namespace gui
{
namespace box
{

    const sf::Uint8 Info::DEFAULT_FRAME_LINES_ALPHA_(127);


    Info::Info(const ColorSet &       COLOR_SET,
               const sf::FloatRect &  REGION,
               const bool             GOLD_BARS,
               const float            LINE_THICKNESS,
               const bool             WILL_FRAME_LINES,
               const bool             WILL_GROW,
               const BackgroundInfo & BACKGROUND_INFO)
    :
        gold_bars     (GOLD_BARS),
        will_grow     (WILL_GROW),
        frame_lines   (WILL_FRAME_LINES),
        line_thickness(LINE_THICKNESS),
        color_set     (COLOR_SET),
        region        (REGION),
        bg_info       (BACKGROUND_INFO)
    {}


    Info::Info(const float            LINE_THICKNESS,
               const bool             WILL_FRAME_LINES,
               const sf::FloatRect &  REGION,
               const ColorSet &       COLOR_SET,
               const BackgroundInfo & BACKGROUND_INFO)
    :
        gold_bars     (false),
        will_grow     (false),
        frame_lines   (WILL_FRAME_LINES),
        line_thickness(LINE_THICKNESS),
        color_set     (COLOR_SET),
        region        (REGION),
        bg_info       (BACKGROUND_INFO)
    {}


    Info::Info(const float            LINE_THICKNESS,
               const bool             WILL_FRAME_LINES,
               const sf::FloatRect &  REGION,
               const std::string &    BG_TEXTURE_PATH,
               const sf::Color &      BG_GRADIENT_TO_COLOR,
               const unsigned int     BG_GRADIENT_SIDES,
               const unsigned int     BG_GRADIENT_CORNERS)
    :
        gold_bars     (false),
        will_grow     (false),
        frame_lines   (WILL_FRAME_LINES),
        line_thickness(LINE_THICKNESS),
        color_set     (sfml_util::gui::ColorSet()),
        region        (REGION),
        bg_info       (BG_TEXTURE_PATH, REGION, sf::Color::White, sfml_util::GradientInfo(BG_GRADIENT_TO_COLOR, BG_GRADIENT_SIDES, BG_GRADIENT_CORNERS))
    {}


    Info::Info(const bool             WILL_GROW,
               const sf::FloatRect &  REGION,
               const ColorSet &       COLOR_SET,
               const BackgroundInfo & BACKGROUND_INFO)
    :
        gold_bars     (true),
        will_grow     (WILL_GROW),
        frame_lines   (false),
        line_thickness(0.0f),
        color_set     (COLOR_SET),
        region        (REGION),
        bg_info       (BACKGROUND_INFO)
    {}


    Info::Info(const bool             WILL_GROW,
               const sf::FloatRect &  REGION,
               const std::string &    BG_TEXTURE_PATH,
               const sf::Color &      BG_GRADIENT_TO_COLOR,
               const unsigned int     BG_GRADIENT_SIDES,
               const unsigned int     BG_GRADIENT_CORNERS)
    :
        gold_bars     (false),
        will_grow     (WILL_GROW),
        frame_lines   (false),
        line_thickness(0),
        color_set     (sfml_util::gui::ColorSet()),
        region        (REGION),
        bg_info       (BG_TEXTURE_PATH, REGION, sf::Color::White, sfml_util::GradientInfo(BG_GRADIENT_TO_COLOR, BG_GRADIENT_SIDES, BG_GRADIENT_CORNERS))
    {}


    bool operator==(const Info & L, const Info & R)
    {
        return ((L.color_set == R.color_set) &&
                (L.gold_bars == R.gold_bars) &&
                (sfml_util::IsRealClose(L.line_thickness, R.line_thickness) ) &&
                (L.region == R.region) &&
                (L.will_grow == R.will_grow) &&
                (L.frame_lines == R.frame_lines) &&
                (L.bg_info == R.bg_info));
    }


    bool operator!=(const Info & L, const Info & R)
    {
        return ! (L == R);
    }

}
}
}
