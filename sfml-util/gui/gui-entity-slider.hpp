#ifndef SFMLUTIL_GUI_GUI_ENTITY_SLIDER_INCLUDED
#define SFMLUTIL_GUI_GUI_ENTITY_SLIDER_INCLUDED
//
//  gui-entity-slider.hpp
//
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/pos-slider.hpp"


namespace sfml_util
{
namespace gui
{

    //forward declarations
    class IGuiEntity;
    using IGuiEntitySPtr_t = std::shared_ptr<IGuiEntity>;


    //responsible for moving a gui entity back and forth between two points using the slider method
    class GuiEntitySlider : public sliders::PosSlider
    {
        //prevent copy construction
        GuiEntitySlider(const GuiEntitySlider &);

    public:
        GuiEntitySlider(IGuiEntitySPtr_t     guiEntitySPtr    = IGuiEntitySPtr_t(nullptr),
                        const sf::Vector2f & FROM_POS_V       = sf::Vector2f(),
                        const sf::Vector2f & TO_POS_V         = sf::Vector2f(),
                        const float          SLIDER_SPEED     = 1.0f);

        void Setup(IGuiEntitySPtr_t     guiEntitySPtr    = IGuiEntitySPtr_t(nullptr),
                   const sf::Vector2f & FROM_POS_V       = sf::Vector2f(),
                   const sf::Vector2f & TO_POS_V         = sf::Vector2f(),
                   const float          SLIDER_SPEED     = 1.0f);

        virtual void Reset(const bool WILL_RESET_POSITION);

        virtual bool UpdateTime(const float ELAPSED_TIME_SECONDS);

    private:
        IGuiEntitySPtr_t guiEntitySPtr_;
    };

}
}


#endif //SFMLUTIL_GUI_GUI_ENTITY_SLIDER_INCLUDED
