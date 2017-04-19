#ifndef APPBASE_SFMLUTIL_SLIDERBARMUSIC_INCLUDED
#define APPBASE_SFMLUTIL_SLIDERBARMUSIC_INCLUDED
//
// sliderbar-music.hpp
//  A SliderBar that controls the music volume.
//
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/gui/sliderbar-labeled.hpp"

#include <string>
#include <memory>


namespace sfml_util
{

    //forward declaration
    namespace gui
    {
        class MouseTextInfo;
    }


    //Encapsulates a gui sliderbar with a label that controls the music volume.
    class SliderBarLabeled_Music : public gui::SliderBarLabeled
    {
        //prevent copy construction and copy assignment
        SliderBarLabeled_Music(const SliderBarLabeled_Music &);
        SliderBarLabeled_Music & operator=(const SliderBarLabeled_Music &);

    public:
        SliderBarLabeled_Music(const std::string &        NAME,
                               const float                POS_LEFT,
                               const float                POS_TOP,
                               const float                LENGTH,
                               const gui::SliderStyle &   STYLE,
                               const gui::MouseTextInfo & THREE_TEXT_INFOS,
                               const float                INITIAL_VALUE           = 0.0f,
                               const float                RELATIVE_LABEL_POS_LEFT = 0.0f,
                               const float                RELATIVE_LABEL_POS_TOP  = 0.0f);//must be [0.0f, 1.0f]

        virtual ~SliderBarLabeled_Music();

        virtual void OnChange(const float NEW_VALUE);
        virtual void SetPadPosition();
        virtual bool MouseDown(const sf::Vector2f & MOUSE_POS_V);
        virtual void SetCurrentValueFromScreenCoords(const sf::Vector2f & NEW_COORD_V);
    };

    using SliderBarLabeled_MusicSPtr_t = std::shared_ptr<SliderBarLabeled_Music>;
    using SliderBarLabeled_MusicSVec_t = std::vector<SliderBarLabeled_MusicSPtr_t>;

}

#endif //APPBASE_SFMLUTIL_SLIDERBARMUSIC_INCLUDED
