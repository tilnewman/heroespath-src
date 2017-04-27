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
    using IGuiEntityPtr_t = IGuiEntity *;


    //responsible for moving a gui entity back and forth between two points using the slider method
    class GuiEntitySlider : public sliders::PosSlider
    {
    public:
        GuiEntitySlider(IGuiEntityPtr_t      guiEntityPtr = nullptr,
                        const sf::Vector2f & FROM_POS_V   = sf::Vector2f(0.0f, 0.0f),
                        const sf::Vector2f & TO_POS_V     = sf::Vector2f(0.0f, 0.0f),
                        const float          SLIDER_SPEED = 1.0f);

        virtual ~GuiEntitySlider();

        void Setup(IGuiEntityPtr_t      guiEntityPtr  = nullptr,
                   const sf::Vector2f & FROM_POS_V    = sf::Vector2f(0.0f, 0.0f),
                   const sf::Vector2f & TO_POS_V      = sf::Vector2f(0.0f, 0.0f),
                   const float          SLIDER_SPEED  = 1.0f);

        virtual void Reset(const bool WILL_RESET_POSITION);

        virtual bool UpdateTime(const float ELAPSED_TIME_SECONDS);

        inline void ReleasePointer() { guiEntityPtr_ = nullptr; }

    private:
        IGuiEntityPtr_t guiEntityPtr_;
    };

}
}


#endif //SFMLUTIL_GUI_GUI_ENTITY_SLIDER_INCLUDED
