// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_GUI_ENTITY_SLIDER_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_GUI_ENTITY_SLIDER_HPP_INCLUDED
//
//  entity-slider.hpp
//
#include "misc/boost-optional-that-throws.hpp"
#include "misc/not-null.hpp"
#include "sfml-util/pos-slider.hpp"

namespace heroespath
{
namespace sfml_util
{
    namespace gui
    {
        class IEntity;
        using IEntityPtr_t = misc::NotNull<IEntity *>;
        using IEntityPtrOpt_t = boost::optional<IEntityPtr_t>;

        // Responsible for moving a Entity back and forth between two points using the slider
        // method.
        class EntitySlider : public sliders::PosSlider
        {
        public:
            explicit EntitySlider(
                const IEntityPtrOpt_t & ENTITY_PTR_OPT = boost::none,
                const sf::Vector2f & FROM_POS_V = sf::Vector2f(0.0f, 0.0f),
                const sf::Vector2f & TO_POS_V = sf::Vector2f(0.0f, 0.0f),
                const float SLIDER_SPEED = 1.0f);

            virtual ~EntitySlider();

            void Setup(
                const IEntityPtrOpt_t & ENTITY_PTR_OPT = boost::none,
                const sf::Vector2f & FROM_POS_V = sf::Vector2f(0.0f, 0.0f),
                const sf::Vector2f & TO_POS_V = sf::Vector2f(0.0f, 0.0f),
                const float SLIDER_SPEED = 1.0f);

            bool UpdateTime(const float ELAPSED_TIME_SECONDS) override;

            const IEntityPtrOpt_t GetEntity() const { return guiEntityPtrOpt_; }
            void SetEntity(const IEntityPtrOpt_t PTR_OPT) { guiEntityPtrOpt_ = PTR_OPT; }

        private:
            // This is an observer pointer that does not control the lifetime of the object.
            IEntityPtrOpt_t guiEntityPtrOpt_;
        };

    } // namespace gui
} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_GUI_GUI_ENTITY_SLIDER_HPP_INCLUDED
