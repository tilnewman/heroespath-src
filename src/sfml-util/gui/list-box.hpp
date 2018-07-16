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
#include "sfml-util/gui/box.hpp"
#include "sfml-util/gui/gui-entity.hpp"
#include "sfml-util/gui/list-box-item-factory.hpp"
#include "sfml-util/gui/list-box-item.hpp"
#include "sfml-util/gui/sliderbar.hpp"
#include "sfml-util/i-callback-handler.hpp"
#include "sfml-util/i-stage.hpp"
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/sfml-system.hpp"
#include "sfml-util/sfml-util.hpp"
#include "sfml-util/sound-manager.hpp"

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

        template <typename Stage_t>
        class ListBox;

        class ListBoxItem;
        using ListBoxItemPtr_t = misc::NotNull<ListBoxItem *>;
        using ListBoxItemPtrOpt_t = boost::optional<ListBoxItemPtr_t>;
        using ListBoxItemUPtr_t = std::unique_ptr<ListBoxItem>;
        using ListBoxItemUVec_t = std::vector<ListBoxItemUPtr_t>;

        // the callback type for ListBox is a wrapper class called ListBoxItem
        namespace callback
        {
            template <typename Stage_t>
            using ListBoxPtrPackage_t = sfml_util::callback::PtrWrapper<ListBox<Stage_t>>;

            template <typename Stage_t>
            struct ListBoxEventPackage
            {
                static const int INVALID_SELECTION_ = -1;

                explicit ListBoxEventPackage(
                    ListBoxPtrPackage_t<Stage_t> PACKAGE,
                    const sfml_util::GuiEvent::Enum GUI_EVENT = sfml_util::GuiEvent::None,
                    const sf::Vector2f & MOUSE_POS_V = sf::Vector2f(-1.0f, -1.0f),
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

                ListBoxPtrPackage_t<Stage_t> package;
                sfml_util::GuiEvent::Enum gui_event;
                sf::Vector2f mouse_pos;
                int curr_selection;
                bool has_focus_changed;
                sf::Event::KeyEvent keypress_event;
                sf::Event::MouseWheelEvent mousewheel_event;
            };

            template <typename Stage_t>
            using IListBoxCallbackHandler
                = sfml_util::callback::ICallbackHandler<ListBoxEventPackage<Stage_t>, bool>;

            template <typename Stage_t>
            using IListBoxCallbackHandlerPtr_t = misc::NotNull<IListBoxCallbackHandler<Stage_t> *>;

            template <typename Stage_t>
            using IListBoxCallbackHandlerPtrOpt_t
                = boost::optional<IListBoxCallbackHandlerPtr_t<Stage_t>>;

        } // namespace callback

        // A class that manages a vertical list of IGuiEntitys that can be scrolled through and
        // selected. The template type Item_t must be/descend/implement IGuiEntity.
        template <typename Stage_t>
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

            ListBox(
                const std::string & NAME,
                Stage_t * owningStagePtr,
                const sf::FloatRect & REGION = sf::FloatRect(),
                const box::Info & BOX_INFO = box::Info(),
                const sf::Color & LINE_COLOR = sf::Color::Transparent,
                const sf::Color & IMAGE_COLOR = sf::Color::White,
                const sf::Color & HIGHLIGHT_COLOR = sf::Color(0, 0, 0, 0),
                const float BETWEEN_PAD = -1.0f)
                : GuiEntity(std::string(NAME).append("_ListBox"), REGION)
                , owningStagePtr_(owningStagePtr)
                , itemElementPadHoriz_(sfml_util::ScreenRatioToPixelsHoriz(0.007f))
                , itemImageVertPad_(1.0f)
                , itemSizeV_(0.0f, 0.0f) // remaining members are initailized in Setup()
                , itemImageMaxSize_(0.0f)
                , betweenItemsPadVert_(0.0f)
                , boxUPtr_()
                , sliderbarUPtr_()
                , lineColor_(sf::Color::Transparent)
                , highlightColor_(sf::Color::Transparent)
                , items_()
                , imageColor_(sf::Color::White)
                , willPlaySfx_(true)
                , displayIndex_(0)
                , selectionDisplayIndex_(0)
                , selectionOffsetIndex_(0)
                , visibleCount_(0)
            {
                Setup(REGION, BOX_INFO, LINE_COLOR, IMAGE_COLOR, HIGHLIGHT_COLOR, BETWEEN_PAD);
            }

            virtual ~ListBox()
            {
                if (boxUPtr_)
                {
                    owningStagePtr_->EntityRemove(boxUPtr_.get());
                }

                if (sliderbarUPtr_)
                {
                    owningStagePtr_->EntityRemove(sliderbarUPtr_.get());
                }
            }

            void Setup(
                const sf::FloatRect & REGION = sf::FloatRect(),
                const box::Info & BOX_INFO = box::Info(),
                const sf::Color & LINE_COLOR = sf::Color::Transparent,
                const sf::Color & IMAGE_COLOR = sf::Color::White,
                const sf::Color & HIGHLIGHT_COLOR = sf::Color(0, 0, 0, 0),
                const float BETWEEN_PAD = -1.0f)
            {
                SetEntityRegion(REGION);

                itemImageMaxSize_ = sfml_util::ScreenRatioToPixelsVert(0.04f);
                itemSizeV_.x = GetEntityRegion().width - (itemElementPadHoriz_ * 2.0f);
                itemSizeV_.y = itemImageMaxSize_ + (itemImageVertPad_ * 2.0f);

                items_.clear();

                betweenItemsPadVert_
                    = ((BETWEEN_PAD < 0.0f) ? sfml_util::ScreenRatioToPixelsVert(0.00667f)
                                            : BETWEEN_PAD);

                lineColor_ = LINE_COLOR;

                imageColor_ = IMAGE_COLOR;

                highlightColor_
                    = ((HIGHLIGHT_COLOR == sf::Color(0, 0, 0, 0))
                           ? (BOX_INFO.bg_info.color + sf::Color(20, 20, 20, 20))
                           : HIGHLIGHT_COLOR);

                if (!boxUPtr_)
                {
                    boxUPtr_ = std::make_unique<sfml_util::gui::box::Box>("ListBox's", BOX_INFO);
                    owningStagePtr_->EntityAdd(boxUPtr_.get());
                }

                boxUPtr_->SetupBox(BOX_INFO);
                boxUPtr_->SetWillAcceptFocus(false);

                if (!sliderbarUPtr_)
                {
                    sliderbarUPtr_ = std::make_unique<sfml_util::gui::SliderBar>(
                        "ListBox's",
                        REGION.left + REGION.width + 10.0f,
                        REGION.top + 10.0f,
                        REGION.height - 20.0f,
                        sfml_util::gui::SliderStyle(),
                        sfml_util::gui::callback::ISliderBarCallbackHandlerPtr_t(this));

                    owningStagePtr_->EntityAdd(sliderbarUPtr_.get());
                }

                sliderbarUPtr_->SetCurrentValue(0.0f);
                displayIndex_ = 0;
                selectionDisplayIndex_ = 0;
                selectionOffsetIndex_ = 0;
                visibleCount_ = CalcVisibleItems();
            }

            const std::string HandlerName() const override { return GetEntityName(); }

            bool HandleCallback(const callback::SliderBarCallbackPackage_t & PACKAGE) override
            {
                if (Empty())
                {
                    return false;
                }
                else
                {
                    auto const NEW_INDEX_F { static_cast<float>(items_.size() - 1)
                                             * PACKAGE.PTR_->GetCurrentValue() };

                    DisplayIndex(static_cast<std::size_t>(NEW_INDEX_F));
                    return true;
                }
            }

            const sf::Color ImageColor() const { return imageColor_; }
            void ImageColor(const sf::Color & COLOR) { imageColor_ = COLOR; }

            void draw(sf::RenderTarget & target, sf::RenderStates states) const override
            {
                // use the ListBox's entityWillDraw_ to control whether
                // ListBox is responsible for drawing its items
                if (false == entityWillDraw_)
                {
                    return;
                }

                auto lastDrawnLinePosVert { entityRegion_.top };
                auto const LIST_BOTTOM { sfml_util::Bottom(entityRegion_) };
                auto const SELECTION_INDEX { SelectedIndex() };

                if (Empty() == false)
                {
                    std::size_t i { 0 };
                    for (; i < displayIndex_; ++i)
                    {
                        items_.at(i)->SetEntityWillDraw(false);
                    }

                    auto const LAST_INDEX_TO_DRAW { std::min(
                        CalcLastVisibleIndex_Display(), items_.size() - 1) };

                    for (; i <= LAST_INDEX_TO_DRAW; ++i)
                    {
                        auto itemPtr { ListBoxItemPtr_t(items_.at(i).get()) };

                        itemPtr->SetEntityWillDraw(true);

                        auto const ITEM_POS_LEFT { entityRegion_.left + itemElementPadHoriz_ };

                        auto const ITEM_POS_TOP { entityRegion_.top
                                                  + (static_cast<float>(i)
                                                     * (itemSizeV_.y + betweenItemsPadVert_)) };

                        auto const TEXT_POS_TOP { (ITEM_POS_TOP + (itemSizeV_.y * 0.5f))
                                                  - (itemPtr->GetEntityRegion().height * 0.5f) };

                        const sf::FloatRect ITEM_RECT(
                            ITEM_POS_LEFT, ITEM_POS_TOP, itemSizeV_.x, itemSizeV_.y);

                        if (itemPtr->HasImage())
                        {
                            itemPtr->SetEntityPos(
                                ITEM_POS_LEFT + (itemImageMaxSize_ + itemElementPadHoriz_),
                                TEXT_POS_TOP);

                            auto & sprite { itemPtr->Sprite() };

                            const sf::FloatRect IMAGE_RECT(
                                ITEM_POS_LEFT,
                                ITEM_POS_TOP + itemImageVertPad_,
                                itemImageMaxSize_,
                                itemImageMaxSize_);

                            sfml_util::FitAndCenter(sprite, IMAGE_RECT);

                            if (SelectedIndex() == i)
                            {
                                sprite.setColor(imageColor_ + sf::Color(100, 100, 100, 100));
                            }
                            else
                            {
                                sprite.setColor(imageColor_);
                            }
                        }
                        else
                        {
                            itemPtr->SetEntityPos(ITEM_POS_LEFT, TEXT_POS_TOP);
                        }

                        if (SELECTION_INDEX == i)
                        {
                            sfml_util::DrawRectangle(
                                target,
                                states,
                                ITEM_RECT,
                                sf::Color::Transparent,
                                0,
                                highlightColor_);
                        }

                        target.draw(*itemPtr, states);

                        if (itemPtr->IsValid() == false)
                        {
                            const sf::Color INVALID_ITEM_HIGHLIGHT_COLOR { 127, 32, 32, 64 };

                            sfml_util::DrawRectangle(
                                target,
                                states,
                                ITEM_RECT,
                                sf::Color::Transparent,
                                0,
                                INVALID_ITEM_HIGHLIGHT_COLOR);
                        }

                        auto const LINE_POS_TOP { sfml_util::Bottom(ITEM_RECT)
                                                  + (betweenItemsPadVert_ * 0.5f) };

                        if (LINE_POS_TOP < LIST_BOTTOM)
                        {
                            DrawLine(target, LINE_POS_TOP);
                            lastDrawnLinePosVert = LINE_POS_TOP;
                        }
                        else
                        {
                            break;
                        }
                    }

                    for (; i < items_.size(); ++i)
                    {
                        items_.at(i)->SetEntityWillDraw(false);
                    }
                }

                // draw lines in the rest of the box even though there are no more items
                auto linePosVert { lastDrawnLinePosVert + (itemSizeV_.y + betweenItemsPadVert_) };
                while (linePosVert < LIST_BOTTOM)
                {
                    DrawLine(target, linePosVert);
                    linePosVert += (itemSizeV_.y + betweenItemsPadVert_);
                }
            }

            ListBoxItemPtrOpt_t AtPos(const sf::Vector2f & POS_V)
            {
                for (auto const & NEXT_ITEM_UPTR : items_)
                {
                    if (NEXT_ITEM_UPTR->GetEntityWillDraw()
                        && FullItemRect(NEXT_ITEM_UPTR.get()).contains(POS_V))
                    {
                        return ListBoxItemPtr_t(NEXT_ITEM_UPTR.get());
                    }
                }

                return boost::none;
            }

            const ListBoxItemPtrOpt_t Selected() const
            {
                auto const INDEX { SelectedIndex() };
                if (INDEX < items_.size())
                {
                    return ListBoxItemPtr_t(items_[INDEX].get());
                }
                else
                {
                    return boost::none;
                }
            }

            std::size_t DisplayIndex() const { return displayIndex_; }

            void DisplayIndex(const std::size_t NEW_INDEX)
            {
                if (IsIndexValid(NEW_INDEX) && (DisplayIndex() != NEW_INDEX))
                {
                    if (NEW_INDEX < displayIndex_)
                    {
                        displayIndex_ = NEW_INDEX;
                    }
                    else
                    {
                        auto const GREATEST_FIRST_INDEX { CalcGreatestFirstDisplayedIndex() };
                        if (NEW_INDEX >= GREATEST_FIRST_INDEX)
                        {
                            displayIndex_ = GREATEST_FIRST_INDEX;
                        }
                        else
                        {
                            displayIndex_ = NEW_INDEX - visibleCount_;
                        }
                    }
                }
            }

            std::size_t SelectedIndex() const
            {
                return selectionDisplayIndex_ + selectionOffsetIndex_;
            }

            // sets displayIndex_ to whatever selectionDisplayIndex_ turns out to be
            void SelectedIndex(const std::size_t NEW_INDEX)
            {
                if (IsIndexValid(NEW_INDEX) && (SelectedIndex() != NEW_INDEX))
                {
                    if (IsIndexVisible_Selection(NEW_INDEX))
                    {
                        selectionOffsetIndex_ = NEW_INDEX - selectionDisplayIndex_;
                    }
                    else
                    {
                        if (NEW_INDEX < selectionDisplayIndex_)
                        {
                            selectionDisplayIndex_ = NEW_INDEX;
                            selectionOffsetIndex_ = 0;
                        }
                        else
                        {
                            if ((NEW_INDEX > 0) && (SelectedIndex() == (NEW_INDEX - 1)))
                            {
                                ++selectionDisplayIndex_;
                            }
                            else
                            {
                                auto const GREATEST_FIRST_INDEX {
                                    CalcGreatestFirstDisplayedIndex()
                                };
                                if (NEW_INDEX >= GREATEST_FIRST_INDEX)
                                {
                                    selectionDisplayIndex_ = GREATEST_FIRST_INDEX;
                                    selectionOffsetIndex_ = NEW_INDEX - GREATEST_FIRST_INDEX;
                                }
                                else
                                {
                                    selectionDisplayIndex_ = NEW_INDEX - visibleCount_;
                                    selectionOffsetIndex_ = visibleCount_ - 1;
                                }
                            }
                        }
                    }
                }

                displayIndex_ = selectionDisplayIndex_;
            }

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

            ListBoxItemPtr_t At(const std::size_t INDEX)
            {
                M_ASSERT_OR_LOGANDTHROW_SS(
                    (INDEX < items_.size()),
                    "sfml_util::gui::ListBox::At("
                        << INDEX << ")(listbox_name=" << GetEntityName()
                        << ") but that index is out of bounds.  Actual size is "
                        << items_.size() - 1 << ".");

                return ListBoxItemPtr_t(items_[INDEX].get());
            }

            void Add(ListBoxItemUPtr_t itemUPtr) { items_.emplace_back(std::move(itemUPtr)); }

            bool Remove(const ListBoxItemPtr_t ITEM_PTR)
            {
                auto const FOUND_ITR { std::find_if(
                    items_.begin(), items_.end(), [&](auto const & UPTR) {
                        return (UPTR.get() == ITEM_PTR);
                    }) };

                if (FOUND_ITR == items_.end())
                {
                    return false;
                }
                else
                {
                    items_.erase(FOUND_ITR);

                    if (SelectedIndex() >= items_.size())
                    {
                        SelectPrev();
                    }

                    return true;
                }
            }

            bool RemoveSelected()
            {
                auto const SELECTED_PTR_OPT { Selected() };

                if (SELECTED_PTR_OPT)
                {
                    return Remove(SELECTED_PTR_OPT.value());
                }
                else
                {
                    return false;
                }
            }

            void Clear() { items_.clear(); }

            std::size_t Size() const { return items_.size(); }

            bool Empty() const { return items_.empty(); }

            bool MouseUp(const sf::Vector2f & MOUSE_POS_V) override
            {
                auto const DID_STATE_CHANGE { GuiEntity::MouseUp(MOUSE_POS_V) };

                auto const ORIG_SELECTED_ITEM_PTR_OPT { Selected() };

                auto const NUM_ITEMS { items_.size() };
                for (std::size_t i(displayIndex_); i < NUM_ITEMS; ++i)
                {
                    auto const ITEM_PTR { ListBoxItemPtr_t(items_[i].get()) };

                    if (ITEM_PTR->GetEntityWillDraw() == false)
                    {
                        break;
                    }

                    if (FullItemRect(ITEM_PTR).contains(MOUSE_POS_V)
                        && (ORIG_SELECTED_ITEM_PTR_OPT != ITEM_PTR))
                    {
                        SelectedIndex(i);

                        if (willPlaySfx_)
                        {
                            SoundManager::Instance()->PlaySfx_MouseClick();
                        }

                        boxUPtr_->FakeColorSetAsIfFocusIs(true);

                        const callback::ListBoxPtrPackage_t<Stage_t> PTR_PACKAGE { this };

                        const callback::ListBoxEventPackage<Stage_t> PACKAGE(
                            PTR_PACKAGE, sfml_util::GuiEvent::Click, MOUSE_POS_V);

                        owningStagePtr_->HandleCallback(PACKAGE);

                        return true;
                    }
                }

                boxUPtr_->FakeColorSetAsIfFocusIs(DID_STATE_CHANGE);
                return DID_STATE_CHANGE;
            }

            bool KeyRelease(const sf::Event::KeyEvent & KEY_EVENT) override
            {
                if (Empty())
                {
                    CreateKeypressPackageAndCallHandler(KEY_EVENT);
                    return false;
                }

                if (KEY_EVENT.code == sf::Keyboard::Return)
                {
                    PlaySfx();
                    CreateKeypressPackageAndCallHandler(KEY_EVENT);
                    return true;
                }

                if (KEY_EVENT.code == sf::Keyboard::Up)
                {
                    auto const DID_SELECTION_CHANGE { SelectPrev() };
                    PlaySfx();
                    CreateKeypressPackageAndCallHandler(KEY_EVENT);
                    return DID_SELECTION_CHANGE;
                }
                else if (KEY_EVENT.code == sf::Keyboard::Down)
                {
                    auto const DID_SELECTION_CHANGE { SelectNext() };
                    PlaySfx();
                    CreateKeypressPackageAndCallHandler(KEY_EVENT);
                    return DID_SELECTION_CHANGE;
                }

                CreateKeypressPackageAndCallHandler(KEY_EVENT);
                return false;
            }

            void SetEntityPos(const float POS_LEFT, const float POS_TOP) override
            {
                auto const DIFF_HORIZ { POS_LEFT - GetEntityPos().x };
                auto const DIFF_VERT { POS_TOP - GetEntityPos().y };

                for (auto & itemUPtr : items_)
                {
                    itemUPtr->MoveEntityPos(DIFF_HORIZ, DIFF_VERT);
                }

                if (boxUPtr_)
                {
                    boxUPtr_->MoveEntityPos(DIFF_HORIZ, DIFF_VERT);
                }

                GuiEntity::SetEntityPos(POS_LEFT, POS_TOP);
            }

            const sf::Color GetHighlightColor() const { return highlightColor_; }
            void SetHighlightColor(const sf::Color & NEW_COLOR) { highlightColor_ = NEW_COLOR; }

            bool WillPlaySoundEffects() const { return willPlaySfx_; }
            void WillPlaySoundEffects(const bool WILL_PLAY_SFX) { willPlaySfx_ = WILL_PLAY_SFX; }

            // these two functions return true if the selection changed
            bool SelectPrev()
            {
                if (selectionOffsetIndex_ > 0)
                {
                    --selectionOffsetIndex_;
                    return true;
                }
                else
                {
                    if (selectionDisplayIndex_ > 0)
                    {
                        if (displayIndex_ == selectionDisplayIndex_)
                        {
                            --displayIndex_;
                        }

                        --selectionDisplayIndex_;
                        return true;
                    }
                }

                return false;
            }

            bool SelectNext()
            {
                if ((Empty() == false) && (SelectedIndex() < (items_.size() - 1)))
                {
                    SelectedIndex(SelectedIndex() + 1);
                    return true;
                }
                else
                {
                    return false;
                }
            }

            const sf::FloatRect FullItemRect(const ListBoxItemPtr_t ITEM_PTR) const
            {
                auto posV { ITEM_PTR->GetEntityPos() };

                if (ITEM_PTR->HasImage())
                {
                    posV.x -= (itemImageMaxSize_ + itemElementPadHoriz_);
                }

                return sf::FloatRect(posV, itemSizeV_);
            }

            void
                MoveItemToOtherListBox(const std::size_t ITEM_TO_MOVE_INDEX, ListBox & otherListBox)
            {
                if (ITEM_TO_MOVE_INDEX < items_.size())
                {
                    ListBoxItemPtr_t itemToMovePtr { items_.at(ITEM_TO_MOVE_INDEX).get() };

                    ListBoxItemFactory listBoxItemFactory;

                    otherListBox.Add(
                        listBoxItemFactory.MakeCopy(otherListBox.GetEntityName(), itemToMovePtr));

                    Remove(itemToMovePtr);
                }
            }

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
            std::size_t CalcGreatestFirstDisplayedIndex() const
            {
                return ((items_.size() > visibleCount_) ? (items_.size() - visibleCount_) : 0);
            }

            void CreateKeypressPackageAndCallHandler(const sf::Event::KeyEvent & KEY_EVENT)
            {
                const callback::ListBoxPtrPackage_t<Stage_t> PTR_PACKAGE(this);

                const callback::ListBoxEventPackage<Stage_t> PACKAGE(
                    PTR_PACKAGE,
                    sfml_util::GuiEvent::Keypress,
                    sf::Vector2f(-1.0f, -1.0f),
                    callback::ListBoxEventPackage<Stage_t>::INVALID_SELECTION_,
                    false,
                    KEY_EVENT);

                owningStagePtr_->HandleCallback(PACKAGE);
            }

            void OnClick(const sf::Vector2f &) override {}

            void OnDoubleClick(const sf::Vector2f & MOUSE_POS_V) override
            {
                const callback::ListBoxPtrPackage_t<Stage_t> PTR_PACKAGE(this);

                const callback::ListBoxEventPackage<Stage_t> PACKAGE(
                    PTR_PACKAGE, sfml_util::GuiEvent::DoubleClick, MOUSE_POS_V);

                owningStagePtr_->HandleCallback(PACKAGE);
            }

            std::size_t CalcVisibleItems() const
            {
                std::size_t count { 0 };
                auto posVert { entityRegion_.top };
                auto const LIST_BOTTOM { (entityRegion_.top + entityRegion_.height) };

                do
                {
                    posVert += itemSizeV_.y;

                    if (posVert > LIST_BOTTOM)
                    {
                        break;
                    }
                    else
                    {
                        ++count;
                        posVert += betweenItemsPadVert_;
                    }
                } while (posVert < LIST_BOTTOM);

                return count;
            }

            std::size_t CalcLastVisibleIndex_Display() const
            {
                if (Empty())
                {
                    return 0;
                }
                else
                {
                    return std::min((displayIndex_ + visibleCount_) - 1, items_.size() - 1);
                }
            }

            std::size_t CalcLastVisibleIndex_Selection() const
            {
                if (Empty())
                {
                    return 0;
                }
                else
                {
                    return std::min(
                        (selectionDisplayIndex_ + visibleCount_) - 1, items_.size() - 1);
                }
            }

            void PlaySfx() const
            {
                if (willPlaySfx_)
                {
                    SoundManager::Instance()->PlaySfx_Keypress();
                }
            }

            void DrawLine(sf::RenderTarget & target, const float POS_TOP) const
            {
                sf::Vertex line[] = {
                    sf::Vertex(sf::Vector2f(entityRegion_.left, POS_TOP)),
                    sf::Vertex(
                        sf::Vector2f(entityRegion_.left + (entityRegion_.width * 0.5f), POS_TOP)),
                    sf::Vertex(sf::Vector2f(entityRegion_.left + entityRegion_.width, POS_TOP)),
                };

                line[0].color = sf::Color::Transparent;
                line[1].color = lineColor_;
                line[2].color = sf::Color::Transparent;
                target.draw(line, 3, sf::LinesStrip);
            }

        private:
            misc::NotNull<Stage_t *> owningStagePtr_;

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

            // which item is displayed at the top of the list
            std::size_t displayIndex_;

            // which item item is displayed at the top of the list when the selection is displayed
            std::size_t selectionDisplayIndex_;

            // offset from selectionIndexA_ to the actual selection
            std::size_t selectionOffsetIndex_;

            // how many items are listed
            std::size_t visibleCount_;
        };

        template <typename Stage_t>
        using ListBoxUPtr_t = std::unique_ptr<ListBox<Stage_t>>;

        template <typename Stage_t>
        inline auto begin(ListBox<Stage_t> & lb) noexcept
        {
            return lb.begin();
        }

        template <typename Stage_t>
        inline auto begin(const ListBox<Stage_t> & LB) noexcept
        {
            return LB.begin();
        }

        template <typename Stage_t>
        inline auto cbegin(const ListBox<Stage_t> & LB) noexcept
        {
            return begin(LB);
        }

        template <typename Stage_t>
        inline auto rbegin(ListBox<Stage_t> & lb) noexcept
        {
            return lb.rbegin();
        }

        template <typename Stage_t>
        inline auto rbegin(const ListBox<Stage_t> & LB) noexcept
        {
            return LB.rbegin();
        }

        template <typename Stage_t>
        inline auto crbegin(const ListBox<Stage_t> & LB) noexcept
        {
            return rbegin(LB);
        }

        template <typename Stage_t>
        inline auto end(ListBox<Stage_t> & lb) noexcept
        {
            return lb.end();
        }

        template <typename Stage_t>
        inline auto end(const ListBox<Stage_t> & LB) noexcept
        {
            return LB.end();
        }

        template <typename Stage_t>
        inline auto cend(const ListBox<Stage_t> & LB) noexcept
        {
            return end(LB);
        }

        template <typename Stage_t>
        inline auto rend(ListBox<Stage_t> & lb) noexcept
        {
            return lb.rend();
        }

        template <typename Stage_t>
        inline auto rend(const ListBox<Stage_t> & LB) noexcept
        {
            return LB.rend();
        }

        template <typename Stage_t>
        inline auto crend(const ListBox<Stage_t> & LB) noexcept
        {
            return rend(LB);
        }

    } // namespace gui
} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_GUI_LISTBOX_HPP_INCLUDED
