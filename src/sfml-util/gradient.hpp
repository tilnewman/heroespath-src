// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_GRADIENT_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_GRADIENT_HPP_INCLUDED
//
// gradient.hpp
//  Gradient drawing helper code.
//
#include "sfml-util/gradient-info.hpp"
#include "sfml-util/sfml-graphics.hpp"

#include <vector>

namespace heroespath
{
namespace sfml_util
{

    // A class that draws simple gradients
    class GradientRect : public sf::Drawable
    {
    public:
        // if using this constructor, Setup() must be called before any other functions.
        GradientRect();

        GradientRect(const sf::FloatRect & REGION, const GradientInfo & INFO);

        GradientRect(
            const float POS_LEFT,
            const float POS_TOP,
            const float WIDTH,
            const float HEIGHT,
            const GradientInfo & INFO);

        virtual ~GradientRect();

        void Setup(const sf::FloatRect & REGION, const GradientInfo & INFO = GradientInfo());

        virtual void SetPosition(const float POS_LEFT, const float POS_TOP);
        virtual void MovePosition(const float HORIZ, const float VERT);

        virtual void draw(sf::RenderTarget & target, sf::RenderStates states) const;

    protected:
        sf::VertexArray quads_;
    };

} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_GRADIENT_HPP_INCLUDED
