#ifndef APPBASE_SFMLUTIL_GUI_TEXTREGION_INCLUDED
#define APPBASE_SFMLUTIL_GUI_TEXTREGION_INCLUDED
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
#include "sfml-util/gui/gui-entity.hpp"
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
        TextRegion(const TextRegion &);

        //prevent copy assignment
        TextRegion & operator=(const TextRegion &);

    public:
        //if using this constructor, Setup() must be called before any other function
        TextRegion(const std::string & NAME);

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

        void HandleSliderBar(sfml_util::gui::SliderBarSPtr_t & tempSliderBarSPtr);
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

    using TextRegionSPtr_t = std::shared_ptr<TextRegion>;
    using TextRegionSVec_t = std::vector<TextRegionSPtr_t>;
}
}
#endif //APPBASE_SFMLUTIL_GUI_TEXTREGION_INCLUDED
