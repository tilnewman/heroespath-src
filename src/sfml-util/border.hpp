// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_BORDER_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_BORDER_HPP_INCLUDED
//
// border.hpp
//
#include "misc/boost-optional-that-throws.hpp"
#include "sfml-util/cached-texture.hpp"
#include "sfml-util/gold-bar.hpp"
#include "sfutil/rectangle-shape.hpp"

#include <SFML/Graphics/Vertex.hpp>

#include <memory>
#include <tuple>

namespace heroespath
{
namespace sfml_util
{

    // Responsible for keeping and drawing a rectangular border that can be made of lines or
    // gui-styled gold bars.  The default constructor or any constructor with the width or
    // height < 1.0f results in a valid object that will draw nothing.
    class Border : public sf::Drawable
    {
    public:
        // pad lines will not be drawn unless a non-transparent PAD_LINE_COLOR_ADJ is provided
        Border(
            const sf::FloatRect & REGION = sf::FloatRect(),
            const float LINE_THICKNESS = 1.0f,
            const sf::Color & LINE_COLOR = sf::Color::White,
            const sf::Color & PAD_LINE_COLOR_ADJ = sf::Color::Transparent,
            const bool WILL_GROW_BORDER_TO_CONTAIN_REGION = false);

        // use to create a border of gold bars
        Border(const sf::FloatRect & REGION, const bool WILL_GROW_BORDER_TO_CONTAIN_REGION = false);

        Border(const Border &) = default;
        Border(Border &&) = default;
        Border & operator=(const Border &) = default;
        Border & operator=(Border &&) = default;

        // changes to a default invalid state that draws nothing
        void Reset();

        // pad lines will not be drawn unless a non-transparent PAD_LINE_COLOR_ADJ is provided
        void Setup(
            const sf::FloatRect & REGION,
            const float LINE_THICKNESS,
            const sf::Color & LINE_COLOR = sf::Color::White,
            const sf::Color & PAD_LINE_COLOR_ADJ = sf::Color::Transparent,
            const bool WILL_GROW_BORDER_TO_CONTAIN_REGION = false);

        // use to create a border of gold bars
        void Setup(const sf::FloatRect & REGION, const bool WILL_GROW_BORDER_TO_CONTAIN_REGION);

        void draw(sf::RenderTarget &, sf::RenderStates) const override;

        bool IsLine() const { return !!rectangleShapesOpt_; }
        bool IsGoldBars() const { return !!goldBarOpt_; }

        // if WILL_GROW_BORDER_TO_CONTAIN_REGION==false returns the orig region
        const sf::FloatRect OuterRegion() const { return regionOuter_; }

        // if WILL_GROW_BORDER_TO_CONTAIN_REGION==true returns the orig region
        const sf::FloatRect InnerRegion() const { return regionInner_; }

        const sf::Color Color() const;
        void Color(const sf::Color &);

        void SetPos(const float POS_LEFT, const float POS_TOP);
        void MovePos(const float HORIZ, const float VERT);

        bool WasGrown() const;

        const sf::Color LineColor() const;
        const sf::Color PadLineColorAdj() const;

        float LineThickness() const;

        bool WillDraw() const { return willDraw_; }
        void WillDraw(const bool WILL_DRAW) { willDraw_ = WILL_DRAW; }

        friend bool operator<(const Border & L, const Border & R);
        friend bool operator==(const Border & L, const Border & R);

    private:
        boost::optional<std::vector<sf::RectangleShape>> rectangleShapesOpt_;
        GoldBarOpt_t goldBarOpt_;
        sf::FloatRect regionOuter_;
        sf::FloatRect regionInner_;
        bool willDraw_;
    };

    using BorderOpt_t = boost::optional<Border>;
    using BorderUPtr_t = std::unique_ptr<Border>;

    // intentionally ignores willDraw_
    inline bool operator<(const Border & L, const Border & R)
    {
        return std::tie(L.rectangleShapesOpt_, L.goldBarOpt_)
            < std::tie(R.rectangleShapesOpt_, R.goldBarOpt_);
    }

    // intentionally ignores willDraw_
    inline bool operator==(const Border & L, const Border & R)
    {
        return std::tie(L.rectangleShapesOpt_, L.goldBarOpt_)
            == std::tie(R.rectangleShapesOpt_, R.goldBarOpt_);
    }

    // intentionally ignores willDraw_
    inline bool operator!=(const Border & L, const Border & R) { return !(L == R); }

} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_BORDER_HPP_INCLUDED
