#ifndef GAME_CHARACTERSTAGEANIMNUM_INCLUDED
#define GAME_CHARACTERSTAGEANIMNUM_INCLUDED
//
// character-stage-anim-num.hpp
//  An animated number that is used on the CharacterCreation Stage
//
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/sliders.hpp"
#include "sfml-util/gui/text-info.hpp"

#include "game/stats/stat-set.hpp"

#include <memory>
#include <string>
#include <vector>


namespace game
{
namespace stage
{

    //encapsulates an animated number for the CharacterCreation Stage
    class AnimNum : public sf::Drawable
    {
    public:
        AnimNum(const stats::Stat_t            VALUE,
                const stats::stat::Enum        WHICH_STAT,
                const float                    START_LEFT,
                const float                    START_TOP,
                const float                    TARGET_LEFT,
                const float                    TARGET_TOP,
                const sfml_util::gui::TextInfo TEXT_INFO);

        virtual ~AnimNum();

        void CreateNewTextRegion();

        //returns true once the target is reached
        bool UpdateTime(const float ELAPSED_TIME_SEC);

        inline bool IsDoneMoving() const                  { return isDoneMoving_; }
        inline bool IsDoneFading() const                  { return isDoneFading_; }
        inline bool WillFade() const                      { return willFade_; }
        inline void WillFadeSet(const bool B)             { willFade_ = B; }
        inline bool IgnoreMe() const                      { return ignoreMe_; }
        inline void SetIgnoreMe()                         { ignoreMe_ = true; }
        inline stats::stat::Enum Which() const            { return whichStat_; }
        inline stats::Stat_t Value() const                { return value_; }
        inline void ValueSet(const stats::Stat_t NEW_VAL) { value_ = NEW_VAL; }
        inline bool IsHeldDown() const                    { return isHeldDown_; }
        inline void MouseUp()                             { isHeldDown_ = false; }
        inline void SetPosY(const float TOP)              { textObj_.setPosition(textObj_.getPosition().x, TOP); }
        inline const sf::Vector2f GetPos() const              { return sf::Vector2f(textObj_.getPosition().x, textObj_.getPosition().y); }
        inline void SetPos(const float LEFT, const float TOP) { textObj_.setPosition(LEFT, TOP); }

        bool MouseDown(const float POS_LEFT, const float POS_TOP);

        bool UpdateTimer(const float ELAPSED_TIME_SEC);

        inline virtual void draw(sf::RenderTarget & target, sf::RenderStates states) const { target.draw(textObj_, states); }

        friend bool operator==(const AnimNum & L, const AnimNum & R);

    private:
        stats::Stat_t value_;
        stats::stat::Enum whichStat_;
        bool      ignoreMe_;
        sf::Int8  colorVal_;
        float     startLeft_;
        float     startTop_;
        float     distanceX_;
        float     distanceY_;
        sf::Color color_;
        bool      isDoneMoving_;
        bool      isDoneFading_;
        bool      willFade_;
        float     fadeCounter_;
        bool      isHeldDown_;
        sfml_util::gui::TextInfo textInfo_;
        sf::Text  textObj_;
        sfml_util::sliders::ZeroSliderOnce<float> sliderX_;
        float     timerSec_;
        float     prevPosX_;
    };

    using AnimNumSPtr_t = std::shared_ptr<AnimNum>;
    using AnimNumSVec_t = std::vector<AnimNumSPtr_t>;

    bool operator==(const AnimNum & L, const AnimNum & R);

    bool operator!=(const AnimNum & L, const AnimNum & R);

}
}
#endif //GAME_CHARACTERSTAGEANIMNUM_INCLUDED
