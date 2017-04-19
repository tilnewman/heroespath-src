#ifndef APPBASE_SFMLUTIL_GUI_SCROLLREGION_INCLUDED
#define APPBASE_SFMLUTIL_GUI_SCROLLREGION_INCLUDED
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
        ScrollRegion(const ScrollRegion &);

        //prevent copy assignment
        ScrollRegion & operator=(const ScrollRegion &);

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
#endif //APPBASE_SFMLUTIL_GUI_SCROLLREGION_INCLUDED
