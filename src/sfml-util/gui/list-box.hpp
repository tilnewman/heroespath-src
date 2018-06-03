// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_GUI_LISTBOX_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_GUI_LISTBOX_HPP_INCLUDED
//
// list-box.hpp
//  A class that manages a list of some template GuiEntity types wrapped in a box
//
#include "misc/boost-optional-that-throws.hpp"
#include "misc/not-null.hpp"
#include "misc/vector-map.hpp"
#include "sfml-util/gui-event-enum.hpp"
#include "sfml-util/gui/box-info.hpp"
#include "sfml-util/gui/gui-entity.hpp"
#include "sfml-util/gui/sliderbar.hpp"
#include "sfml-util/i-callback-handler.hpp"
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/sfml-system.hpp"

#include <algorithm>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace heroespath
{
namespace sfml_util
{
    class IStage;
    using IStagePtr_t = misc::NotNull<IStage *>;
    using IStagePtrOpt_t = boost::optional<IStagePtr_t>;

    namespace gui
    {

        namespace box
        {
            class Box;
            using BoxUPtr_t = std::unique_ptr<Box>;
        } // namespace box

        class ListBoxItem;
        using ListBoxItemSPtr_t = std::shared_ptr<ListBoxItem>;
        using ListBoxItemSVec_t = std::vector<ListBoxItemSPtr_t>;

        // types required for the ListBoxPtrPackage_t and IListBoxCallbackHandler below
        class ListBox;
        using ListBoxUPtr_t = std::unique_ptr<ListBox>;

        // the callback type for ListBox is a wrapper class called ListBoxItem
        namespace callback
        {
            using ListBoxPtrPackage_t = sfml_util::callback::PtrWrapper<ListBox>;

            struct ListBoxEventPackage
            {
                static const int INVALID_SELECTION_;
                static const sf::Vector2f INVALID_MOUSE_POS_V_;

                explicit ListBoxEventPackage(
                    ListBoxPtrPackage_t PACKAGE,
                    const sfml_util::GuiEvent::Enum GUI_EVENT = sfml_util::GuiEvent::None,
                    const sf::Vector2f & MOUSE_POS_V = INVALID_MOUSE_POS_V_,
                    const int CURR_SELECTION = INVALID_SELECTION_,
                    const bool HAS_FOCUS_CHANGED = false,
                    const sf::Event::KeyEvent & KEYPRESS_EVENT = sf::Event::KeyEvent(),
                    const sf::Event::MouseWheelEvent & MOUSEWHEEL_EVENT
                    = sf::Event::MouseWheelEvent())
                    : package(PACKAGE)
                    , gui_event(GUI_EVENT)
                    , mouse_pos(MOUSE_POS_V)
                    , curr_selection(CURR_SELECTION)
                    , has_focus_changed(HAS_FOCUS_CHANGED)
                    , keypress_event(KEYPRESS_EVENT)
                    , mousewheel_event(MOUSEWHEEL_EVENT)
                {}

                ListBoxPtrPackage_t package;
                sfml_util::GuiEvent::Enum gui_event;
                sf::Vector2f mouse_pos;
                int curr_selection;
                bool has_focus_changed;
                sf::Event::KeyEvent keypress_event;
                sf::Event::MouseWheelEvent mousewheel_event;
            };

            using IListBoxCallbackHandler
                = sfml_util::callback::ICallbackHandler<ListBoxEventPackage, bool>;

            using IListBoxCallbackHandlerPtr_t = misc::NotNull<IListBoxCallbackHandler *>;
            using IListBoxCallbackHandlerPtrOpt_t = boost::optional<IListBoxCallbackHandlerPtr_t>;

        } // namespace callback

        // A class that manages a vertical list of IGuiEntitys that can be scrolled through and
        // selected. The template type Item_t must be/descend/implement IGuiEntity.
        class ListBox
            : public GuiEntity
            , public callback::ISliderBarCallbackHandler_t
        {
        public:
            ListBox(const ListBox &) = delete;
            ListBox(ListBox &&) = delete;
            ListBox & operator=(const ListBox &) = delete;
            ListBox & operator=(ListBox &&) = delete;

            explicit ListBox(
                const std::string & NAME,
                const sf::FloatRect & REGION = sf::FloatRect(),
                const ListBoxItemSVec_t & ITEM_VEC = ListBoxItemSVec_t(),
                const IStagePtrOpt_t STAGE_PTR_OPT = boost::none,
                const float BETWEEN_PAD = 0.0f,
                const box::Info & BOX_INFO = box::Info(),
                const sf::Color & LINE_COLOR = sf::Color::Transparent,
                const callback::IListBoxCallbackHandlerPtrOpt_t CALLBACK_PTR_OPT = boost::none);

            virtual ~ListBox();

            void Setup(
                const sf::FloatRect & REGION = sf::FloatRect(),
                const ListBoxItemSVec_t & ITEM_VEC = ListBoxItemSVec_t(),
                const IStagePtrOpt_t STAGE_PTR_OPT = boost::none,
                const float BETWEEN_PAD = 0.0f,
                const box::Info & BOX_INFO = box::Info(),
                const sf::Color & LINE_COLOR = sf::Color::Transparent,
                const callback::IListBoxCallbackHandlerPtrOpt_t CALLBACK_PTR_OPT = boost::none);

            const std::string HandlerName() const override { return GetEntityName(); }

            bool HandleCallback(const callback::SliderBarCallbackPackage_t & PACKAGE) override;

            void ImageColor(const sf::Color & COLOR)
            {
                imageColor_ = COLOR;
                SetupForDraw();
            }

            const sf::Color ImageColor() const { return imageColor_; }

            void draw(sf::RenderTarget & target, sf::RenderStates states) const override;

            ListBoxItemSPtr_t AtPos(const sf::Vector2f & POS_V);

            const ListBoxItemSPtr_t Selected() const;

            std::size_t DisplayIndex() const { return displayIndex_; }

            void DisplayIndex(const std::size_t);

            std::size_t SelectedIndex() const
            {
                return selectionDisplayIndex_ + selectionOffsetIndex_;
            }

            // sets displayIndex_ to whatever selectionDisplayIndex_ turns out to be
            void SelectedIndex(const std::size_t);

            bool IsIndexValid(const std::size_t INDEX) const { return (INDEX < items_.size()); }

            bool IsIndexVisible_Display(const std::size_t INDEX) const
            {
                return ((INDEX >= displayIndex_) && (INDEX <= CalcLastVisibleIndex_Display()));
            }

            bool IsIndexVisible_Selection(const std::size_t INDEX) const
            {
                return (
                    (INDEX >= selectionDisplayIndex_)
                    && (INDEX <= CalcLastVisibleIndex_Selection()));
            }

            ListBoxItemSPtr_t At(const std::size_t INDEX) { return items_.at(INDEX); }

            std::size_t Size() const { return items_.size(); }
            bool Empty() const { return items_.empty(); }

            void Add(const ListBoxItemSPtr_t & ITEM_SPTR);
            bool Remove(const ListBoxItemSPtr_t & ITEM_SPTR);

            bool RemoveSelected() { return Remove(Selected()); }
            void Clear() { items_.clear(); }

            bool MouseUp(const sf::Vector2f & MOUSE_POS_V) override;

            bool KeyRelease(const sf::Event::KeyEvent & KEY_EVENT) override;

            void SetEntityPos(const float POS_LEFT, const float POS_TOP) override;

            const sf::Color GetHighlightColor() const { return highlightColor_; }
            void SetHighlightColor(const sf::Color & C) { highlightColor_ = C; }

            bool WillPlaySoundEffects() const { return willPlaySfx_; }
            void WillPlaySoundEffects(const bool B) { willPlaySfx_ = B; }

            const ListBoxItemSVec_t Items() const { return items_; }
            void Items(const ListBoxItemSVec_t & NEW_ITEMS_VEC);

            // these two functions return true if the selection changed
            bool SelectPrev();
            bool SelectNext();

            const sf::FloatRect FullItemRect(const ListBoxItemSPtr_t &) const;

        private:
            std::size_t CalcGreatestFirstDisplayedIndex() const;
            void CreateKeypressPackageAndCallHandler(const sf::Event::KeyEvent & KEY_EVENT);
            void OnClick(const sf::Vector2f &) override {}
            void OnDoubleClick(const sf::Vector2f & MOUSE_POS_V) override;
            void SetupForDraw();

            std::size_t CalcVisibleItems() const;
            std::size_t CalcLastVisibleIndex_Display() const;
            std::size_t CalcLastVisibleIndex_Selection() const;

            void PlaySfx() const;

            void DrawLine(sf::RenderTarget & target, const float POS_TOP) const;

            bool IsPosWithinItemRegion(
                const sf::Vector2f & POS_V, const ListBoxItemSPtr_t & ITEM_SPTR) const;

            void ResetItemHeight();

        private:
            static const sf::Color INVALID_ITEM_HIGHLIGHT_COLOR_;
            //
            const float HORIZ_PAD_;
            sf::Vector2f itemSizeV_;
            float betweenPadVert_;
            box::BoxUPtr_t boxUPtr_;
            SliderBarUPtr_t sliderbarUPtr_;
            sf::Color lineColor_;
            sf::Color highlightColor_;
            IStagePtrOpt_t stagePtrOpt_;
            ListBoxItemSVec_t items_;
            sf::Color imageColor_;
            bool willPlaySfx_;
            callback::IListBoxCallbackHandlerPtrOpt_t callbackPtrOpt_;

            // which item is displayed at the top of the list
            std::size_t displayIndex_;

            // which item item is displayed at the top of the list when the selection is displayed
            std::size_t selectionDisplayIndex_;

            // offset from selectionIndexA_ to the actual selection
            std::size_t selectionOffsetIndex_;

            // how many items are listed
            std::size_t visibleCount_;
        };

    } // namespace gui
} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_GUI_LISTBOX_HPP_INCLUDED
