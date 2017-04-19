//
//  gui-entity-slider.cpp
//
#include "gui-entity-slider.hpp"

#include "sfml-util/gui/i-gui-entity.hpp"


namespace sfml_util
{
namespace gui
{

    GuiEntitySlider::GuiEntitySlider(IGuiEntitySPtr_t     guiEntitySPtr,
                                     const sf::Vector2f & FROM_POS_V,
                                     const sf::Vector2f & TO_POS_V,
                                     const float          SLIDER_SPEED)
    :
        PosSlider     (FROM_POS_V, TO_POS_V, SLIDER_SPEED),
        guiEntitySPtr_(guiEntitySPtr)
    {
        Setup(guiEntitySPtr_, FROM_POS_V, TO_POS_V, SLIDER_SPEED);
    }


    void GuiEntitySlider::Setup(IGuiEntitySPtr_t     guiEntitySPtr,
                                const sf::Vector2f & FROM_POS_V,
                                const sf::Vector2f & TO_POS_V,
                                const float          SLIDER_SPEED)
    {
        guiEntitySPtr_ = guiEntitySPtr;
        PosSlider::Setup(FROM_POS_V, TO_POS_V, SLIDER_SPEED);
    }


    void GuiEntitySlider::Reset(const bool WILL_RESET_POSITION)
    {
        if (WILL_RESET_POSITION)
            guiEntitySPtr_->SetEntityPos(homePosV_);

        PosSlider::Reset(WILL_RESET_POSITION);
    }


    bool GuiEntitySlider::UpdateTime(const float ELAPSED_TIME_SECONDS)
    {
        const bool RESULT(PosSlider::UpdateTime(ELAPSED_TIME_SECONDS));

        if (RESULT)
            guiEntitySPtr_->SetEntityPos(posV_);

        return RESULT;
    }

}
}
