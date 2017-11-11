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
#ifndef HEROESPATH_SFMLUTIL_ANIMATIONMULTI_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_ANIMATIONMULTI_HPP_INCLUDED
//
// animation-multi.hpp
//  An animation class sourced from multiples textures.
//
#include "animation-base.hpp"

#include "misc/handy-types.hpp"

#include <vector>


namespace heroespath
{
namespace sfml_util
{

    //An animtion class sourced from multiple textures.
    class AnimationMultiTexture : public Animation
    {
        AnimationMultiTexture(const AnimationMultiTexture &) =delete;
        AnimationMultiTexture & operator=(const AnimationMultiTexture &) =delete;

    public:
        AnimationMultiTexture(const Animations::Enum ENUM,
                              const sf::FloatRect &  REGION,
                              const float            TIME_PER_FRAME_SEC,
                              const sf::BlendMode &  BLEND_MODE,
                              const sf::Color &      COLOR_FROM,
                              const sf::Color &      COLOR_TO);

        virtual ~AnimationMultiTexture();

        virtual void draw(sf::RenderTarget & target, sf::RenderStates states) const;

        virtual void SetEntityPos(const sf::Vector2f & V);
        virtual void SetEntityPos(const float LEFT, const float TOP);

        virtual void SetEntityRegion(const sf::FloatRect & R);

        //returns true if frame count wrapped around back to zero
        virtual bool UpdateTime(const float SECONDS);

        inline virtual std::size_t FrameCount() const
        {
            return textureIdVec_.size();
        }

        inline virtual const sf::Vector2f OrigSize() const
        {
            return origSizeV_;
        }

        virtual void MoveEntityPos(const float HORIZ, const float VERT);

    protected:
        sf::Sprite sprite_;
        sf::Vector2f origSizeV_;

        //the size of this vec acts as a total frame count
        misc::SizetVec_t textureIdVec_;
    };

}
}

#endif //HEROESPATH_SFMLUTIL_ANIMATIONMULTI_HPP_INCLUDED
