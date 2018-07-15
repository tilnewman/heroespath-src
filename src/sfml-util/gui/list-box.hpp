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

        class ListBox;

        class ListBoxItem;
        using ListBoxItemPtr_t = misc::NotNull<ListBoxItem *>;
        using ListBoxItemPtrOpt_t = boost::optional<ListBoxItemPtr_t>;
        using ListBoxItemUPtr_t = std::unique_ptr<ListBoxItem>;
        using ListBoxItemUVec_t = std::vector<ListBoxItemUPtr_t>;

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
            using value_type = ListBoxItemUPtr_t;
            using container_type = ListBoxItemUVec_t;
            using iterator = typename container_type::iterator;
            using const_iterator = typename container_type::const_iterator;
            using reverse_iterator = std::reverse_iterator<iterator>;
            using const_reverse_iterator = std::reverse_iterator<const_iterator>;

            ListBox(const ListBox &) = delete;
            ListBox(ListBox &&) = delete;
            ListBox & operator=(const ListBox &) = delete;
            ListBox & operator=(ListBox &&) = delete;

            explicit ListBox(
                const std::string & NAME,
                const sf::FloatRect & REGION = sf::FloatRect(),
                const IStagePtrOpt_t STAGE_PTR_OPT = boost::none,
                const box::Info & BOX_INFO = box::Info(),
                const sf::Color & LINE_COLOR = sf::Color::Transparent,
                const callback::IListBoxCallbackHandlerPtrOpt_t CALLBACK_PTR_OPT = boost::none,
                const sf::Color & IMAGE_COLOR = sf::Color::White,
                const sf::Color & HIGHLIGHT_COLOR = NO_COLOR_,
                const float BETWEEN_PAD = -1.0f);

            virtual ~ListBox();

            void Setup(
                const sf::FloatRect & REGION = sf::FloatRect(),
                const IStagePtrOpt_t STAGE_PTR_OPT = boost::none,
                const box::Info & BOX_INFO = box::Info(),
                const sf::Color & LINE_COLOR = sf::Color::Transparent,
                const callback::IListBoxCallbackHandlerPtrOpt_t CALLBACK_PTR_OPT = boost::none,
                const sf::Color & IMAGE_COLOR = sf::Color::White,
                const sf::Color & HIGHLIGHT_COLOR = NO_COLOR_,
                const float BETWEEN_PAD = -1.0f);

            const std::string HandlerName() const override { return GetEntityName(); }

            bool HandleCallback(const callback::SliderBarCallbackPackage_t & PACKAGE) override;

            void ImageColor(const sf::Color & COLOR) { imageColor_ = COLOR; }

            const sf::Color ImageColor() const { return imageColor_; }

            void draw(sf::RenderTarget & target, sf::RenderStates states) const override;

            ListBoxItemPtrOpt_t AtPos(const sf::Vector2f & POS_V);

            const ListBoxItemPtrOpt_t Selected() const;

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

            ListBoxItemPtr_t At(const std::size_t);
            void Add(ListBoxItemUPtr_t);
            bool Remove(const ListBoxItemPtr_t);
            bool RemoveSelected();
            void Clear();
            std::size_t Size() const;
            bool Empty() const;

            bool MouseUp(const sf::Vector2f & MOUSE_POS_V) override;

            bool KeyRelease(const sf::Event::KeyEvent & KEY_EVENT) override;

            void SetEntityPos(const float POS_LEFT, const float POS_TOP) override;

            const sf::Color GetHighlightColor() const { return highlightColor_; }
            void SetHighlightColor(const sf::Color & NEW_COLOR) { highlightColor_ = NEW_COLOR; }

            bool WillPlaySoundEffects() const { return willPlaySfx_; }
            void WillPlaySoundEffects(const bool WILL_PLAY_SFX) { willPlaySfx_ = WILL_PLAY_SFX; }

            // these two functions return true if the selection changed
            bool SelectPrev();
            bool SelectNext();

            const sf::FloatRect FullItemRect(const ListBoxItemPtr_t) const;

            void MoveItemToOtherListBox(
                const std::size_t ITEM_TO_MOVE_INDEX, ListBox & otherListBox);

            iterator begin() noexcept { return std::begin(items_); }
            iterator end() noexcept { return std::end(items_); }

            const_iterator begin() const noexcept { return std::begin(items_); }
            const_iterator end() const noexcept { return std::end(items_); }

            const_iterator cbegin() const noexcept { return begin(); }
            const_iterator cend() const noexcept { return end(); }

            reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
            reverse_iterator rend() noexcept { return reverse_iterator(begin()); }

            const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }
            const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }

            const_reverse_iterator crbegin() const noexcept { return rbegin(); }
            const_reverse_iterator crend() const noexcept { return rend(); }

            value_type & Front()
            {
                M_ASSERT_OR_LOGANDTHROW_SS(
                    (Empty() == false), "misc::VectorMap::Front() non-const, called when empty.");

                return items_.front();
            }

            value_type & Back()
            {
                M_ASSERT_OR_LOGANDTHROW_SS(
                    (Empty() == false), "misc::VectorMap::Back() non-const, called when empty.");

                return items_.back();
            }

            const value_type & Front() const
            {
                M_ASSERT_OR_LOGANDTHROW_SS(
                    (Empty() == false), "misc::VectorMap::Front() const, called when empty.");

                return items_.front();
            }

            const value_type & Back() const
            {
                M_ASSERT_OR_LOGANDTHROW_SS(
                    (Empty() == false), "misc::VectorMap::Back() const, called when empty.");

                return items_.back();
            }

        private:
            std::size_t CalcGreatestFirstDisplayedIndex() const;
            void CreateKeypressPackageAndCallHandler(const sf::Event::KeyEvent & KEY_EVENT);
            void OnClick(const sf::Vector2f &) override {}
            void OnDoubleClick(const sf::Vector2f & MOUSE_POS_V) override;

            std::size_t CalcVisibleItems() const;
            std::size_t CalcLastVisibleIndex_Display() const;
            std::size_t CalcLastVisibleIndex_Selection() const;

            void PlaySfx() const;

            void DrawLine(sf::RenderTarget & target, const float POS_TOP) const;

        private:
            static const sf::Color NO_COLOR_;
            static const sf::Color INVALID_ITEM_HIGHLIGHT_COLOR_;
            //

            // the size of the space between the left edge and an item's image and between that
            // image and the item's text, etc.
            float itemElementPadHoriz_;

            // the amount of empty space between the top and bottom of an item's image
            float itemImageVertPad_;

            // the total size of each item on screen
            sf::Vector2f itemSizeV_;

            // the max side length of an item's image
            float itemImageMaxSize_;

            // size of the empty space between items drawn on the screen
            float betweenItemsPadVert_;

            // defines the visible border, or box, that surrounds the this ListBox
            box::BoxUPtr_t boxUPtr_;

            SliderBarUPtr_t sliderbarUPtr_;

            // color of the lines drawn between items
            sf::Color lineColor_;

            // color of the overlay drawn over each item
            sf::Color highlightColor_;

            // each ListBox needs to be able to access its owning stage
            IStagePtrOpt_t stagePtrOpt_;

            // the items contained in this ListBox
            ListBoxItemUVec_t items_;

            // the color applied to all item images (not the item's text)
            sf::Color imageColor_;

            // true if this ListBox will play default sound effects
            bool willPlaySfx_;

            //
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

        using ListBoxUPtr_t = std::unique_ptr<ListBox>;

        inline auto begin(ListBox & lb) noexcept { return lb.begin(); }
        inline auto begin(const ListBox & LB) noexcept { return LB.begin(); }
        inline auto cbegin(const ListBox & LB) noexcept { return begin(LB); }
        inline auto rbegin(ListBox & lb) noexcept { return lb.rbegin(); }
        inline auto rbegin(const ListBox & LB) noexcept { return LB.rbegin(); }
        inline auto crbegin(const ListBox & LB) noexcept { return rbegin(LB); }

        inline auto end(ListBox & lb) noexcept { return lb.end(); }
        inline auto end(const ListBox & LB) noexcept { return LB.end(); }
        inline auto cend(const ListBox & LB) noexcept { return end(LB); }
        inline auto rend(ListBox & lb) noexcept { return lb.rend(); }
        inline auto rend(const ListBox & LB) noexcept { return LB.rend(); }
        inline auto crend(const ListBox & LB) noexcept { return rend(LB); }

    } // namespace gui
} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_GUI_LISTBOX_HPP_INCLUDED
