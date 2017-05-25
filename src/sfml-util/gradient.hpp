///////////////////////////////////////////////////////////////////////////////
//
// Heroes' Path - Open-source, non-commercial, simple, game in the RPG style.
// Copyright (C) 2017 Ziesche Til Newman (tilnewman@gmail.com)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from
// the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
//  1. The origin of this software must not be misrepresented; you must not
//     claim that you wrote the original software.  If you use this software
//     in a product, an acknowledgment in the product documentation would be
//     appreciated but is not required.
//
//  2. Altered source versions must be plainly marked as such, and must not
//     be misrepresented as being the original software.
//
//  3. This notice may not be removed or altered from any source distribution.
//
///////////////////////////////////////////////////////////////////////////////
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
