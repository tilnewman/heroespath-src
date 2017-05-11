#ifndef SFMLUTIL_SLIDERBAREFFECTS_INCLUDED
#define SFMLUTIL_SLIDERBAREFFECTS_INCLUDED
//
// sliderbar-effects.hpp
//  A SliderBar that controls the sound effects volume.
//
#include "sfml-util/gui/mouse-text-info.hpp"
#include "sfml-util/gui/sliderbar-labeled.hpp"

#include <string>
#include <memory>


namespace sfml_util
{

    //Encapsulates a gui sliderbar with a label that controls the sound effects volume.
    class SliderBarLabeled_Effects : public gui::SliderBarLabeled
    {
        //prevent copy construction
        SliderBarLabeled_Effects(const SliderBarLabeled_Effects &) =delete;

        //prevent copy assignment
        SliderBarLabeled_Effects & operator=(const SliderBarLabeled_Effects &) =delete;

    public:
        SliderBarLabeled_Effects(const std::string &        NAME,
                                 const float                POS_LEFT,
                                 const float                POS_TOP,
                                 const float                LENGTH,
                                 const gui::SliderStyle &   STYLE,
                                 const gui::MouseTextInfo & THREE_TEXT_INFOS,
                                 const float                INITIAL_VALUE           = 0.0f,
                                 const float                RELATIVE_LABEL_POS_LEFT = 0.0f,
                                 const float                RELATIVE_LABEL_POS_TOP  = 0.0f);//must be [0.0f, 1.0f]

        virtual ~SliderBarLabeled_Effects();

        virtual void OnChange(const float NEW_VALUE);
        virtual void SetPadPosition();
        virtual bool MouseDown(const sf::Vector2f & MOUSE_POS_V);
        virtual void SetCurrentValueFromScreenCoords(const sf::Vector2f & NEW_COORD_V);
        virtual bool UpdateTime(const float);

        static inline void SetPreventSoundEffect(const bool WILL_PREVENT)   { willPreventPlayingSound_ = WILL_PREVENT; }

    private:
        static bool willPreventPlayingSound_;
        bool willPlaySound_;
        float timeSinceLastPlaySec_;
    };

    using SliderBarLabeled_EffectsSPtr_t = std::shared_ptr<SliderBarLabeled_Effects>;
    using SliderBarLabeled_EffectsSVec_t = std::vector<SliderBarLabeled_EffectsSPtr_t>;

}

#endif //SFMLUTIL_SLIDERBAREFFECTS_INCLUDED
