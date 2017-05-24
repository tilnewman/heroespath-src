#ifndef SFMLUTIL_GRADIENT_INCLUDED
#define SFMLUTIL_GRADIENT_INCLUDED
//
// gradient.hpp
//  Gradient drawing helper code.
//
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/gradient-info.hpp"

#include <vector>


namespace sfml_util
{

    //A class that draws simple gradients
    class GradientRect : public sf::Drawable
    {
    public:
        //if using this constructor, Setup() must be called before any other functions.
        GradientRect();

        GradientRect(const sf::FloatRect & REGION,
                     const GradientInfo &  INFO);

        GradientRect(const float          POS_LEFT,
                     const float          POS_TOP,
                     const float          WIDTH,
                     const float          HEIGHT,
                     const GradientInfo & INFO);

        virtual ~GradientRect();

        void Setup(const sf::FloatRect & REGION,
                   const GradientInfo &  INFO = GradientInfo());

        virtual void SetPosition(const float POS_LEFT, const float POS_TOP);
        virtual void MovePosition(const float HORIZ, const float VERT);

        virtual void draw(sf::RenderTarget & target, sf::RenderStates states) const;

    protected:
        sf::VertexArray quads_;
    };


    using GradientRectSPtr_t = std::shared_ptr<GradientRect>;
    using GradientRectSVec_t = std::vector<GradientRectSPtr_t>;

}
#endif //SFMLUTIL_GRADIENT_INCLUDED
