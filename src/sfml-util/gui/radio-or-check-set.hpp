// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_GUI_RADIO_OR_CHECK_SET_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_GUI_RADIO_OR_CHECK_SET_HPP_INCLUDED
//
// radio-or-check-set.hpp
//
#include "misc/boost-optional-that-throws.hpp"
#include "misc/not-null.hpp"
#include "sfml-util/brightness-enum.hpp"
#include "sfml-util/gui/callback.hpp"
#include "sfml-util/gui/entity.hpp"
#include "sfml-util/gui/mouse-text-info.hpp"
#include "sfml-util/i-stage.hpp"

#include <SFML/Graphics/Color.hpp>

#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace heroespath
{
namespace sfml_util
{

    class RadioOrCheckEntity;
    using RadioOrCheckEntityUPtr_t = std::unique_ptr<RadioOrCheckEntity>;
    using RadioOrCheckEntityUVec_t = std::vector<RadioOrCheckEntityUPtr_t>;

    using TitleValidPair_t = std::pair<std::string, bool>;
    using TitleValidPairVec_t = std::vector<TitleValidPair_t>;

    // Responsible for implementing either a set of radio buttons or check boxes.
    class RadioOrCheckSet : public Entity
    {
    public:
        struct EventPacket
        {
            EventPacket(
                const misc::NotNull<RadioOrCheckSet *> SET_PTR, const std::size_t CHANGED_INDEX)
                : set_ptr(SET_PTR)
                , has_changed_index(CHANGED_INDEX < SET_PTR->Size())
                , changed_index(
                      ((CHANGED_INDEX < SET_PTR->Size()) ? CHANGED_INDEX : SET_PTR->Size()))
                , is_changed_index_selected(SET_PTR->IsSelected(CHANGED_INDEX))
            {}

            EventPacket(const EventPacket &) = default;
            EventPacket(EventPacket &&) = default;
            EventPacket & operator=(const EventPacket &) = default;
            EventPacket & operator=(EventPacket &&) = default;

            misc::NotNull<RadioOrCheckSet *> set_ptr;
            bool has_changed_index;
            std::size_t changed_index;
            bool is_changed_index_selected;
        };

        using Callback_t = Callback<EventPacket>;

        RadioOrCheckSet(const RadioOrCheckSet &) = delete;
        RadioOrCheckSet(RadioOrCheckSet &&) = delete;
        RadioOrCheckSet & operator=(const RadioOrCheckSet &) = delete;
        RadioOrCheckSet & operator=(RadioOrCheckSet &&) = delete;

        RadioOrCheckSet(
            const std::string & NAME,
            const Callback_t::IHandlerPtrOpt_t & CALLBACK_HANDLER_PTR_OPT,
            RadioOrCheckEntityUVec_t,
            const float TOP_AND_BOTTOM_SPACER = 0.0f,
            const float LEFT_AND_RIGHT_SPACER = 0.0f,
            const float BETWEEN_IMAGE_AND_TEXT_HORIZ_SPACER = 0.0f,
            const float BETWEEN_ELEMENTS_VERT_SPACER = 0.0f);

        RadioOrCheckSet(
            const std::string & NAME,
            const Callback_t::IHandlerPtrOpt_t & CALLBACK_HANDLER_PTR_OPT,
            const bool IS_RADIO_BUTTON, // if not then check-box is assumed
            const Brightness::Enum BRIGHTNESS,
            const MouseTextInfo & MOUSE_TEXT_INFO,
            const TitleValidPairVec_t & TITLE_VALID_PAIR_VEC,
            const float TOP_AND_BOTTOM_SPACER = 0.0f,
            const float LEFT_AND_RIGHT_SPACER = 0.0f,
            const float BETWEEN_IMAGE_AND_TEXT_HORIZ_SPACER = 0.0f,
            const float BETWEEN_ELEMENTS_VERT_SPACER = 0.0f);

        virtual ~RadioOrCheckSet();

        // no-opt, no sense in setting the mouse state of a set of entities
        bool SetMouseState(const MouseState::Enum) override { return false; }

        // returns true if an entity changed mouse state
        bool MouseUp(const sf::Vector2f & MOUSE_POS_V) override;

        // returns true if the mouse click was on an entity's image, mouse states are never
        // changed
        bool MouseDown(const sf::Vector2f & MOUSE_POS_V) override;

        // returns true if the mouse entered or exited over any entities (implies text overlay
        // color was changed)
        bool UpdateMousePos(const sf::Vector2f &) override;

        std::size_t Size() const { return entityUVec_.size(); }

        const std::vector<std::size_t> SelectedIndexes() const;

        const SizetOpt_t FirstSelectedIndex() const;

        // returns true if entity at INDEX is selected, returns false not only if not selected
        // but also if INDEX is out of range
        bool IsSelected(const std::size_t INDEX) const;

        // returns true if any entities are selected, always returns true if this is a radio
        // button set
        bool AreAnySelected() const { return !!FirstSelectedIndex(); }

        // returns true if entity at INDEX was changed from un-selected to selected
        bool SelectIndex(const std::size_t INDEX);

        // returns true if the entity at INDEX was changed from selected to un-selected, returns
        // false if INDEX is out of range or if INDEX was a selected radio button
        bool UnSelectIndex(const std::size_t INDEX);

        // returns true if the selection was changed at INDEX, returns false if INDEX is out of
        // range or if INDEX was a selected radio button
        bool ChangeIndex(const std::size_t INDEX);

    protected:
        void SetEntityRegion(const sf::FloatRect & R) override { Entity::SetEntityRegion(R); }

    private:
        void PositionElements(
            const float TOP_AND_BOTTOM_SPACER,
            const float LEFT_AND_RIGHT_SPACER,
            const float BETWEEN_IMAGE_AND_TEXT_HORIZ_SPACER,
            const float BETWEEN_ELEMENTS_VERT_SPACER);

        void UnSelectAllExcept(const std::size_t INDEX);

        void TriggerCallbackForChangedIndex(const std::size_t INDEX);

        RadioOrCheckEntityUVec_t entityUVec_;
        Callback_t::IHandlerPtrOpt_t callbackHandlerPtrOpt_;
    };

    using RadioOrCheckSetPtr_t = misc::NotNull<RadioOrCheckSet *>;
    using RadioOrCheckSetUPtr_t = std::unique_ptr<RadioOrCheckSet>;

} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_GUI_RADIO_OR_CHECK_SET_HPP_INCLUDED
