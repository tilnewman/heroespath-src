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
#ifndef SFMLUTIL_GUI_TEXTREGION_INCLUDED
#define SFMLUTIL_GUI_TEXTREGION_INCLUDED
//
// text-region.hpp
//  A class that encapsulates text drawn to a bounded region that may need a scrollbar.
//
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/margins.hpp"
#include "sfml-util/i-callback-handler.hpp"
#include "sfml-util/gui/gui-entity.hpp"
#include "sfml-util/gui/box-info.hpp"
#include "sfml-util/gui/sliderbar.hpp"
#include "sfml-util/gui/text-info.hpp"
#include "sfml-util/text-rendering.hpp"

#include <memory>
#include <string>
#include <vector>


namespace sfml_util
{
    class IStage;

namespace gui
{

    namespace box
    {
        class Box;
        using BoxSPtr_t = std::shared_ptr<Box>;
    }


    //Encapsulates text drawn and bounded to an area that might need a scroll bar
    class TextRegion
    :
        public GuiEntity,
        public callback::ISliderBarCallbackHandler_t
    {
        //prevent copy construction
        TextRegion(const TextRegion &) =delete;

        //prevent copy assignment
        TextRegion & operator=(const TextRegion &) =delete;

    public:
        //if using this constructor, Setup() must be called before any other function
        explicit TextRegion(const std::string & NAME);

        //if this constructor is used then no scrollbar will be created, even if the text height exceeds the REGION
        TextRegion( const std::string &     NAME,
                    const TextInfo &        TEXT_INFO,
                    const sf::FloatRect &   REGION,
                    const unsigned int      SMALLER_FONT_SIZE = DEFAULT_NO_RESIZE_,
                    const box::Info &       BOX_INFO          = gui::box::Info(),
                    const Margins &         MARGINS           = Margins());

        TextRegion( const std::string &     NAME,
                    const TextInfo &        TEXT_INFO,
                    const sf::FloatRect &   REGION,
                    IStage * const          stagePtr,
                    const unsigned int      SMALLER_FONT_SIZE = DEFAULT_NO_RESIZE_,
                    const box::Info &       BOX_INFO          = gui::box::Info(),
                    const Margins &         MARGINS           = Margins());

        virtual ~TextRegion();

        //if this Setup() is used then no scrollbar will be created, even if the text height exceeds the REGION
        void Setup( const TextInfo &        TEXT_INFO,
                    const sf::FloatRect &   REGION,
                    const unsigned int      SMALLER_FONT_SIZE = DEFAULT_NO_RESIZE_,
                    const box::Info &       BOX_INFO          = box::Info(),
                    const Margins &         MARGINS           = Margins());

        void Setup( const TextInfo &        TEXT_INFO,
                    const sf::FloatRect &   REGION,
                    IStage * const          stagePtr,
                    const unsigned int      SMALLER_FONT_SIZE    = DEFAULT_NO_RESIZE_,
                    const box::Info &       BOX_INFO             = box::Info(),
                    const Margins &         MARGINS              = Margins(),
                    const bool              WILL_ALLOW_SCROLLBAR = true);

        void HandleSliderBar(sfml_util::gui::SliderBarSPtr_t & newSliderBarSPtr);
        void HandleBox(const box::Info & BOX_INFO);

        virtual void draw(sf::RenderTarget & target, sf::RenderStates states) const;

        virtual void SetEntityPos(const float POS_LEFT, const float POS_TOP);
        virtual void MoveEntityPos(const float HORIZ, const float VERT);

        virtual const std::string HandlerName() const       { return GetEntityName(); }
        virtual bool HandleCallback(const callback::SliderBarCallbackPackage_t &);

        inline const std::string GetText() const            { return text_; }
        void SetText(const std::string &);

        void EstablishWhichLinesToDraw(const float SCROLL_RATIO);
        void EstablishWhichLinesToDraw(const float SCROLL_RATIO, const float REGION_HEIGHT);

        void Append(const TextRegion &);

    protected:
        virtual void OnClick(const sf::Vector2f &) {}

        //changes the font color by applying the current entity foreground color to the sprite and textInfo_ member, but does not re-render the text
        virtual void OnColorChange();

    public:
        static const unsigned int DEFAULT_NO_RESIZE_;

    protected:
        box::Info                 boxInfo_;
        box::BoxSPtr_t            boxSPtr_;
        gui::SliderBarSPtr_t      sliderBarSPtr_;
        IStage *                  stagePtr_;
        std::string               text_;
        text_render::RenderedText renderedText_;
        std::size_t               startLine_;
        sf::FloatRect             regionOrig_;
        sfml_util::gui::TextInfo  textInfoOrig_;
        unsigned int              smallFontSizeOrig_;
        sfml_util::Margins        marginsOrig_;
        bool                      allowScrollbarOrig_;
    };

    using TextRegionUPtr_t = std::unique_ptr<TextRegion>;
    using TextRegionSPtr_t = std::shared_ptr<TextRegion>;
    using TextRegionSVec_t = std::vector<TextRegionSPtr_t>;

}
}
#endif //SFMLUTIL_GUI_TEXTREGION_INCLUDED