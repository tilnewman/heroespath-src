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
#ifndef SFMLUTIL_GUI_SCROLLREGION_INCLUDED
#define SFMLUTIL_GUI_SCROLLREGION_INCLUDED
//
// scroll-region.hpp
//  A class that allows scrolling through a vertical list of images
//
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/i-callback-handler.hpp"
#include "sfml-util/i-stage.hpp"
#include "sfml-util/gui/box-info.hpp"
#include "sfml-util/gui/sliderbar.hpp"

#include <memory>
#include <string>


namespace sfml_util
{
namespace gui
{

    namespace box
    {
        class Box;
        using BoxSPtr_t = std::shared_ptr<Box>;
    }


    //Encapsulates text drawn and bounded to an area that might need a scroll bar
    class ScrollRegion
    :
        public GuiEntity,
        public callback::ISliderBarCallbackHandler_t
    {
        //prevent copy construction
        ScrollRegion(const ScrollRegion &) =delete;

        //prevent copy assignment
        ScrollRegion & operator=(const ScrollRegion &) =delete;

    public:
        //if using this constructor with default values then Setup() must be called before any other function
        explicit ScrollRegion(const std::string &       NAME,
                              const sf::FloatRect &     REGION       = sf::FloatRect(),
                              const IGuiEntitySList_t & ENTITY_SLIST = IGuiEntitySList_t(),
                              IStage * const            stagePtr     = nullptr,
                              const float               BETWEEN_PAD  = 0.0f,
                              const box::Info &         BOX_INFO     = box::Info());

        virtual ~ScrollRegion();

        void Setup(const sf::FloatRect &      REGION,
                   const IGuiEntitySList_t &  ENTITY_SLIST,
                   IStage * const             stagePtr,
                   const float                BETWEEN_PAD,
                   const box::Info &          BOX_INFO = box::Info());

        virtual void draw(sf::RenderTarget & target, sf::RenderStates states) const;

        virtual const std::string HandlerName() const { return GetEntityName(); }
        virtual bool HandleCallback(const callback::SliderBarCallbackPackage_t &);

        virtual void Add(const IGuiEntitySPtr_t &);
        virtual bool Remove(const IGuiEntitySPtr_t &);
        virtual void RemoveAll();

    protected:
        void SetupOffScreenTexture();
        void PositionSprite();
        float GetTotalContentHeight() const;
        void OnClick(const sf::Vector2f &) {}

    protected:
        box::BoxSPtr_t    boxSPtr_;
        SliderBarSPtr_t   sliderBarSPtr_;
        IGuiEntitySList_t list_;
        float             pad_;
        float             currentVertPos_;
        sf::Sprite        sprite_;
        sfml_util::RendTextSPtr_t renderTextureSPtr_;
    };

    using ScrollRegionSPtr_t = std::shared_ptr<ScrollRegion>;
}
}
#endif //SFMLUTIL_GUI_SCROLLREGION_INCLUDED
