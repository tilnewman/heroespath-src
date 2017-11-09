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
#ifndef SFMLUTIL_BACKGROUNDIMAGE_HPP_INCLUDED
#define SFMLUTIL_BACKGROUNDIMAGE_HPP_INCLUDED
//
// background-image.hpp
//
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/gradient.hpp"
#include "sfml-util/gui/background-info.hpp"

#include <memory>
#include <string>


namespace heroespath
{
namespace sfml_util
{
namespace gui
{

    //Encapsulates a GuiEntity that manages a single background image with possible gradients.
    //If both path.empty() and textureSPtr.get==nullptr, then a simple rectangle will be drawn and filled with the given color.
    class BackgroundImage : public sf::Drawable
    {
        BackgroundImage(const BackgroundImage &) =delete;
        BackgroundImage & operator=(const BackgroundImage &) =delete;

    public:
        //if using this constructor then Setup() must be called before any other function
        BackgroundImage();

        explicit BackgroundImage(const BackgroundInfo & BG_INFO,
                                 const float            IMAGE_SCALE       = 1.0f,
                                 const bool             WILL_SMOOTH_IMAGE = false);

        //use this constructor to create a background tile that is scaled per resolution like is used on the main menu backgrounds
        explicit BackgroundImage(const std::string & MEDIA_PATH_KEY_STR);

        virtual ~BackgroundImage();

        void Setup(const BackgroundInfo & BG_INFO,
                   const float            SCALE             = 1.0f,
                   const bool             WILL_SMOOTH_IMAGE = false);

        void Reset();

        //The only member required by sf::Drawable.
        virtual void draw(sf::RenderTarget & target, sf::RenderStates states) const;

        virtual void SetColor(const sf::Color & NEW_COLOR);

        virtual void SetPosition(const float POS_LEFT, const float POS_TOP);
        virtual void MovePosition(const float HORIZ, const float VERT);

        inline virtual const sf::Color      GetColor() const     { return bgInfo_.color; }
        inline virtual const sf::FloatRect  GetRegion() const    { return bgInfo_.region; }
        inline virtual bool                 IsImage() const      { return (sprite_.getTexture() != nullptr); }
        inline virtual bool                 IsRectangle() const  { return ! IsImage(); }

        void SetImageDetails(const float SCALE = 1.0f, const bool WILL_SMOOTH = false);

        inline const sfml_util::GradientInfo GradientInfo() const { return bgInfo_.gradient_info; }

        inline const BackgroundInfo BgInfo() const { return bgInfo_; }

        inline float ImageScale() const { return sprite_.getScale().x; }

        inline bool IsTextureSmoothed() const { if (false == bgInfo_.hasTexture) return false; else return true; }

    protected:
        GradientRect gradient_;
        sfml_util::gui::BackgroundInfo bgInfo_;
        sf::Sprite sprite_;
    };

}
}
}

#endif //SFMLUTIL_BACKGROUNDIMAGE_HPP_INCLUDED
