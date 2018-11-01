// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GUI_SLIDER_ENTITY_HPP_INCLUDED
#define HEROESPATH_GUI_SLIDER_ENTITY_HPP_INCLUDED
//
//  slider-entity.hpp
//
#include "gui/slider-position.hpp"
#include "misc/boost-optional-that-throws.hpp"
#include "misc/not-null.hpp"

namespace heroespath
{
namespace gui
{
    class IEntity;
    using IEntityPtr_t = misc::NotNull<IEntity *>;
    using IEntityPtrOpt_t = boost::optional<IEntityPtr_t>;

    // Responsible for moving a Entity back and forth between two points.
    class EntitySlider : public PosSlider
    {
    public:
        // default constructor has all zero values and is stopped
        EntitySlider();

        EntitySlider(
            const IEntityPtrOpt_t & ENTITY_PTR_OPT,
            const sf::Vector2f & FROM_POS_V,
            const sf::Vector2f & TO_POS_V,
            const float SLIDER_SPEED,
            const WillOscillate WILL_OSCILLATE = WillOscillate::No,
            const WillAutoStart WILL_AUTO_START = WillAutoStart::No,
            const std::size_t CYCLE_COUNT_LIMIT = 0);

        virtual ~EntitySlider();

        EntitySlider(const EntitySlider &) = default;
        EntitySlider(EntitySlider &&) = default;
        EntitySlider & operator=(const EntitySlider &) = default;
        EntitySlider & operator=(EntitySlider &&) = default;

        const IEntityPtrOpt_t GetEntity() const { return guiEntityPtrOpt_; }

        void SetEntity(const IEntityPtrOpt_t PTR_OPT);

    protected:
        void OnUpdateChange() override;

    private:
        // This is an observer pointer that does not control the lifetime of the object.
        IEntityPtrOpt_t guiEntityPtrOpt_;
    };

} // namespace gui
} // namespace heroespath

#endif // HEROESPATH_GUI_SLIDER_ENTITY_HPP_INCLUDED
