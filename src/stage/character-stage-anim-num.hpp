// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_CHARACTERSTAGEANIMNUM_HPP_INCLUDED
#define HEROESPATH_CHARACTERSTAGEANIMNUM_HPP_INCLUDED
//
// character-stage-anim-num.hpp
//  An animated number that is used on the CharacterCreation Stage
//
#include "creature/stat-set.hpp"
#include "sfml-util/sliders.hpp"
#include "sfml-util/text-info.hpp"
#include "sfml-util/text.hpp"

#include <SFML/Graphics/Drawable.hpp>

#include <memory>
#include <string>
#include <vector>

namespace heroespath
{
namespace stage
{

    // encapsulates an animated number for the CharacterCreation Stage
    class AnimNum : public sf::Drawable
    {
    public:
        AnimNum(
            const creature::Trait_t VALUE,
            const creature::Traits::Enum WHICH_STAT,
            const float START_LEFT,
            const float START_TOP,
            const float TARGET_LEFT,
            const float TARGET_TOP,
            const sfml_util::TextInfo & TEXT_INFO);

        virtual ~AnimNum();

        void CreateNewTextRegion();

        // returns true once the target is reached
        bool UpdateTime(const float ELAPSED_TIME_SEC);

        bool IsDoneMoving() const { return isDoneMoving_; }
        bool IsDoneFading() const { return isDoneFading_; }
        bool WillFade() const { return willFade_; }
        void WillFadeSet(const bool B) { willFade_ = B; }
        bool IgnoreMe() const { return ignoreMe_; }
        void SetIgnoreMe() { ignoreMe_ = true; }
        creature::Traits::Enum Which() const { return whichStat_; }
        creature::Trait_t Value() const { return value_; }
        void ValueSet(const creature::Trait_t S) { value_ = S; }
        bool IsHeldDown() const { return isHeldDown_; }
        void MouseUp() { isHeldDown_ = false; }

        void SetPosY(const float TOP) { text_.setPosition(text_.getPosition().x, TOP); }

        const sf::Vector2f GetPos() const
        {
            return sf::Vector2f(text_.getPosition().x, text_.getPosition().y);
        }

        void SetPos(const float LEFT, const float TOP) { text_.setPosition(LEFT, TOP); }

        bool MouseDown(const float POS_LEFT, const float POS_TOP);

        bool UpdateTimer(const float ELAPSED_TIME_SEC);

        void draw(sf::RenderTarget & target, sf::RenderStates states) const override;

        friend bool operator==(const AnimNum & L, const AnimNum & R);

    private:
        creature::Trait_t value_;
        creature::Traits::Enum whichStat_;
        bool ignoreMe_;
        sf::Uint8 colorVal_;
        float startLeft_;
        float startTop_;
        float distanceX_;
        float distanceY_;
        sf::Color color_;
        bool isDoneMoving_;
        bool isDoneFading_;
        bool willFade_;
        float fadeCounter_;
        bool isHeldDown_;
        sfml_util::TextInfo textInfo_;
        sfml_util::Text text_;
        sfml_util::sliders::ZeroSliderOnce<float> sliderX_;
        float timerSec_;
        float prevPosX_;
    };

    using AnimNumSPtr_t = std::shared_ptr<AnimNum>;
    using AnimNumSVec_t = std::vector<AnimNumSPtr_t>;

    bool operator==(const AnimNum & L, const AnimNum & R);

    inline bool operator!=(const AnimNum & L, const AnimNum & R) { return !(L == R); }

} // namespace stage
} // namespace heroespath

#endif // HEROESPATH_CHARACTERSTAGEANIMNUM_HPP_INCLUDED
