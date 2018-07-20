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
//
#include "misc/boost-optional-that-throws.hpp"
#include "misc/not-null.hpp"
#include "misc/vector-map.hpp"
#include "sfml-util/gui-event-enum.hpp"
#include "sfml-util/gui/box-info.hpp"
#include "sfml-util/gui/box.hpp"
#include "sfml-util/gui/gui-entity.hpp"
#include "sfml-util/gui/list-box-packet.hpp"
#include "sfml-util/gui/list-element.hpp"
#include "sfml-util/gui/sliderbar.hpp"
#include "sfml-util/i-callback-handler.hpp"
#include "sfml-util/i-stage.hpp"
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/sfml-util.hpp"
#include "sfml-util/sound-manager.hpp"

#include <boost/type_traits.hpp>

#include <algorithm>
#include <array>
#include <cmath>
#include <functional>
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

        template <typename Stage_t, typename Element_t>
        class ListBox;

        namespace callback
        {
            template <typename Stage_t, typename Element_t>
            using ListBoxPtrPackage_t
                = sfml_util::callback::PtrWrapper<ListBox<Stage_t, Element_t>>;

            // Callbacks to the owningStagePtr_ will not occur if the listbox is empty.
            // Since this object is only created when not empty, then selection must be valid.
            // If gui_event is Click or DoubleClick then mouse_pos_opt is initialized
            template <typename Stage_t, typename Element_t>
            struct ListBoxEventPackage
            {
                ListBoxEventPackage(
                    ListBoxPtrPackage_t<Stage_t, Element_t> PACKAGE,
                    const sfml_util::GuiEvent::Enum GUI_EVENT,
                    const sf::Event::KeyEvent & KEYPRESS_EVENT,
                    const Vector2fOpt_t & MOUSE_POS_V_OPT = boost::none,
                    const bool HAS_FOCUS_CHANGED = false)
                    : package(PACKAGE)
                    , gui_event(GUI_EVENT)
                    , keypress_event(KEYPRESS_EVENT)
                    , mouse_pos_opt(MOUSE_POS_V_OPT)
                    , has_focus_changed(HAS_FOCUS_CHANGED)
                    , selected_element_ptr(PACKAGE.PTR_->Selection())
                {}

                ListBoxEventPackage(
                    ListBoxPtrPackage_t<Stage_t, Element_t> PACKAGE,
                    const sf::Event::KeyEvent & KEYPRESS_EVENT,
                    const bool HAS_FOCUS_CHANGED = false)
                    : ListBoxEventPackage(
                          PACKAGE,
                          sfml_util::GuiEvent::Keypress,
                          KEYPRESS_EVENT,
                          boost::none,
                          HAS_FOCUS_CHANGED)
                {}

                ListBoxEventPackage(
                    ListBoxPtrPackage_t<Stage_t, Element_t> PACKAGE,
                    const sfml_util::GuiEvent::Enum GUI_EVENT,
                    const sf::Vector2f & MOUSE_POS_V,
                    const bool HAS_FOCUS_CHANGED = false)
                    : ListBoxEventPackage(
                          PACKAGE, GUI_EVENT, sf::Event::KeyEvent(), MOUSE_POS_V, HAS_FOCUS_CHANGED)
                {}

                ListBoxPtrPackage_t<Stage_t, Element_t> package;
                sfml_util::GuiEvent::Enum gui_event;
                sf::Event::KeyEvent keypress_event;
                Vector2fOpt_t mouse_pos_opt;
                bool has_focus_changed;
                ListElementPtr_t<Element_t> selected_element_ptr;
            };

            template <typename Stage_t, typename Element_t>
            using IListBoxCallbackHandler = sfml_util::callback::
                ICallbackHandler<ListBoxEventPackage<Stage_t, Element_t>, bool>;

            template <typename Stage_t, typename Element_t>
            using IListBoxCallbackHandlerPtr_t
                = misc::NotNull<IListBoxCallbackHandler<Stage_t, Element_t> *>;

            template <typename Stage_t, typename Element_t>
            using IListBoxCallbackHandlerPtrOpt_t
                = boost::optional<IListBoxCallbackHandlerPtr_t<Stage_t, Element_t>>;

        } // namespace callback

        // A class that manages a vertical list of elements with a sliderbar that can be scrolled
        // through, selected, and activated.  Each element is a type that can hold an observer
        // pointer to a game resoure, an image, text, and a flag for if that element is invalid.
        // Invalid elements are drawn with a red overlay, are selectable, but not activatable.
        // Callbacks to the owningStagePtr_ will not occur if the listbox is empty.
        // If Empty()==false, then the selection index and element pointer must be valid.
        template <typename Stage_t, typename Element_t>
        class ListBox
            : public GuiEntity
            , public callback::ISliderBarCallbackHandler_t
        {
        public:
            using value_type = ListElementUPtr_t<Element_t>;
            using container_type = std::vector<value_type>;
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
                const ListBoxPacket & PACKET = ListBoxPacket())
                : GuiEntity(MakeTypeString(NAME), PACKET.BoxInfo().region)
                , owningStagePtr_(owningStagePtr)

                // all remaining variables will be initialized in Setup()
                , boxUPtr_()
                , sliderbarUPtr_()
                , elements_()
                , willMuteAllSfx_()
                , displayIndex_()
                , selectionDisplayIndex_()
                , selectionOffsetIndex_()
                , maxVisibleCount_()
                , willPreventCallbacks_(false)
            {
                Setup(PACKET);
            }

            virtual ~ListBox() { FreeGuiBoxAndSliderBar(); }

            void Setup(const ListBoxPacket & PACKET)
            {
                packet_ = PACKET;
                Setup();
            }

            const std::string HandlerName() const override { return GetEntityName(); }

            bool HandleCallback(const callback::SliderBarCallbackPackage_t & PACKAGE) override
            {
                if (Empty())
                {
                    ResetIndexes();
                    return false;
                }
                else
                {
                    auto const NEW_INDEX_F { static_cast<float>(elements_.size() - 1)
                                             * PACKAGE.PTR_->GetCurrentValue() };

                    DisplayIndex(static_cast<std::size_t>(NEW_INDEX_F));
                    return true;
                }
            }

            void draw(sf::RenderTarget & target, sf::RenderStates states) const override
            {
                auto const LIST_BOTTOM { sfml_util::Bottom(entityRegion_) };

                auto lastDrawnLinePosVert { drawElements(target, states)
                                            + packet_.ElementSize(true).y };

                // draw lines in the rest of the listbox even if there are no more elements
                while (lastDrawnLinePosVert < LIST_BOTTOM)
                {
                    DrawLine(target, lastDrawnLinePosVert);
                    lastDrawnLinePosVert += packet_.ElementSize(true).y;
                }
            }

            // returns boost::none if the element is not visible
            const FloatOpt_t ElementPosTopOpt(
                const ListElementPtr_t<Element_t> & ELEMENT_TO_FIND_PTR,
                const bool WILL_INCLUDE_PAD = false) const
            {
                if (Empty() == false)
                {
                    auto const LAST_INDEX_TO_DRAW { LastVisibleIndex_Display() };

                    for (std::size_t i(displayIndex_); i <= LAST_INDEX_TO_DRAW; ++i)
                    {
                        const ListElementPtr_t<Element_t> NEXT_ELEMENT_PTR {
                            elements_.at(i).get()
                        };

                        if (ELEMENT_TO_FIND_PTR == NEXT_ELEMENT_PTR)
                        {
                            const auto TOP_INCLUDING_PAD { entityRegion_.top
                                                           + (static_cast<float>(i - displayIndex_)
                                                              * packet_.ElementSize(true).y) };

                            return (
                                (WILL_INCLUDE_PAD) ? TOP_INCLUDING_PAD
                                                   : (TOP_INCLUDING_PAD + packet_.ElementPad().y));
                        }
                    }
                }

                return boost::none;
            }

            bool IsElementVisible(const ListElementPtr_t<Element_t> & ELEMENT_PTR) const
            {
                return !!ElementPosTopOpt(ELEMENT_PTR);
            }

            SizetOpt_t IndexAtPos(const sf::Vector2f & POS_V) const
            {
                if ((Empty() == false) && (entityRegion_.contains(POS_V)))
                {
                    const auto DISTANCE_FROM_TOP { POS_V.y - entityRegion_.top };

                    const auto INDEX_FLOAT { std::floor(
                        DISTANCE_FROM_TOP / packet_.ElementSize(true).y) };

                    const auto INDEX { static_cast<std::size_t>(INDEX_FLOAT) };

                    if (INDEX < elements_.size())
                    {
                        return INDEX;
                    }
                }

                return boost::none;
            }

            ListElementPtrOpt_t<Element_t> AtPos(const sf::Vector2f & POS_V) const
            {
                const auto INDEX_OPT { IndexAtPos(POS_V) };

                if (INDEX_OPT)
                {
                    return ListElementPtr_t<Element_t>(elements_.at(INDEX_OPT.value()).get());
                }

                return boost::none;
            }

            // throws if Empty()
            const ListElementPtr_t<Element_t> Selection() const
            {
                M_ASSERT_OR_LOGANDTHROW_SS(
                    (Empty() == false),
                    MakeTypeString() << "::Selection() but the listbox was empty.");

                return ListElementPtr_t<Element_t>(elements_.at(SelectionIndex()).get());
            }

            std::size_t DisplayIndex() const { return displayIndex_; }

            bool DisplayIndex(const std::size_t NEW_INDEX)
            {
                if (Empty())
                {
                    ResetIndexes();
                }
                else if ((DisplayIndex() != NEW_INDEX) && (NEW_INDEX < elements_.size()))
                {
                    if (NEW_INDEX < displayIndex_)
                    {
                        displayIndex_ = NEW_INDEX;
                    }
                    else
                    {
                        auto const MAX_FIRST_DISPLAYABLE_INDEX { MaxFirstDisplayableIndex() };
                        if (NEW_INDEX >= MAX_FIRST_DISPLAYABLE_INDEX)
                        {
                            displayIndex_ = MAX_FIRST_DISPLAYABLE_INDEX;
                        }
                        else
                        {
                            displayIndex_ = NEW_INDEX - maxVisibleCount_;
                        }
                    }

                    return true;
                }

                return false;
            }

            std::size_t SelectionIndex() const
            {
                return selectionDisplayIndex_ + selectionOffsetIndex_;
            }

            // if returns true then displayIndex_ is set to the resulting selectionDisplayIndex_
            bool SelectionIndex(const std::size_t NEW_INDEX)
            {
                if (Empty())
                {
                    ResetIndexes();
                    return false;
                }

                if (NEW_INDEX >= elements_.size())
                {
                    return false;
                }

                if (NEW_INDEX != SelectionIndex())
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
                            if ((NEW_INDEX > 0) && (SelectionIndex() == (NEW_INDEX - 1)))
                            {
                                ++selectionDisplayIndex_;
                            }
                            else
                            {
                                auto const MAX_FIRST_DISPLAYABLE_INDEX {
                                    MaxFirstDisplayableIndex()
                                };
                                if (NEW_INDEX >= MAX_FIRST_DISPLAYABLE_INDEX)
                                {
                                    selectionDisplayIndex_ = MAX_FIRST_DISPLAYABLE_INDEX;
                                    selectionOffsetIndex_ = NEW_INDEX - MAX_FIRST_DISPLAYABLE_INDEX;
                                }
                                else
                                {
                                    selectionDisplayIndex_ = NEW_INDEX - maxVisibleCount_;
                                    selectionOffsetIndex_ = maxVisibleCount_ - 1;
                                }
                            }
                        }
                    }
                }

                displayIndex_ = selectionDisplayIndex_;
                return true;
            }

            bool IsIndexVisible_Display(const std::size_t INDEX) const
            {
                return ((INDEX >= displayIndex_) && (INDEX <= LastVisibleIndex_Display()));
            }

            bool IsIndexVisible_Selection(const std::size_t INDEX) const
            {
                return (
                    (INDEX >= selectionDisplayIndex_) && (INDEX <= LastVisibleIndex_Selection()));
            }

            ListElementPtr_t<Element_t> At(const std::size_t INDEX) const
            {
                M_ASSERT_OR_LOGANDTHROW_SS(
                    (Empty() == false),
                    MakeTypeString() << "::At(index=" << INDEX << ") but this listbox is empty.");

                M_ASSERT_OR_LOGANDTHROW_SS(
                    (INDEX < elements_.size()),
                    MakeTypeString()
                        << "::At(" << INDEX << ") but that index is out of bounds with actual size="
                        << elements_.size() << ".");

                return ListElementPtr_t<Element_t>(elements_.at(INDEX).get());
            }

            // returns the ListElementPtr_t of the RESOURCE or boost::none if not found
            const ListElementPtrOpt_t<Element_t> Find(const Element_t & RESOURCE)
            {
                if (Empty())
                {
                    ResetIndexes();
                    return boost::none;
                }

                const auto FOUND_ITER { std::find_if(
                    std::begin(elements_), std::end(elements_), [&](auto const & ELEMENT_UPTR) {
                        return (ELEMENT_UPTR->Element() == RESOURCE);
                    }) };

                if (FOUND_ITER == std::end(elements_))
                {
                    return boost::none;
                }
                else
                {
                    return ListElementPtrOpt_t<Element_t>(FOUND_ITER->get());
                }
            }

            void Append(ListElementUPtr_t<Element_t> ELEMENT_UPTR)
            {
                elements_.emplace_back(std::move(ELEMENT_UPTR));
            }

            void AppendAndOwn(const ListElementPtr_t<Element_t> & ELEMENT_PTR)
            {
                elements_.emplace_back(ListElementUPtr_t<Element_t>(ELEMENT_PTR.Ptr()));
            }

            bool AppendAndOwn(const ListElementPtrOpt_t<Element_t> & ELEMENT_PTR_OPT)
            {
                if (ELEMENT_PTR_OPT)
                {
                    AppendAndOwn(ELEMENT_PTR_OPT.value());
                    return true;
                }
                else
                {
                    return false;
                }
            }

            // if returns true then the caller is responsible for the lifetime of ELEMENT_PTR
            bool RemoveRelease(const ListElementPtr_t<Element_t> & ELEMENT_PTR)
            {
                if (Empty())
                {
                    ResetIndexes();
                    return false;
                }

                auto const FOUND_ITR { std::find_if(
                    std::begin(elements_), std::end(elements_), [&](auto const & UPTR) {
                        return (UPTR.get() == ELEMENT_PTR);
                    }) };

                if (FOUND_ITR == elements_.end())
                {
                    return false;
                }
                else
                {
                    FOUND_ITR->release();
                    elements_.erase(FOUND_ITR);

                    if (SelectionIndex() >= elements_.size())
                    {
                        SelectPrev();
                    }

                    if (Empty())
                    {
                        ResetIndexes();
                    }

                    return true;
                }
            }

            // if returns true then the caller is responsible for the lifetime of ELEMENT_PTR,
            // returns false if optional is empty
            bool RemoveRelease(const ListElementPtrOpt_t<Element_t> & ELEMENT_PTR_OPT)
            {
                if (ELEMENT_PTR_OPT)
                {
                    return RemoveRelease(ELEMENT_PTR_OPT.value());
                }

                return false;
            }

            // returns a ListElementPtr_t that the caller is responsible for the lifetime of, or
            // boost::none if the RESOURCE was not found in the listbox
            const ListElementPtrOpt_t<Element_t> RemoveRelease(const Element_t & RESOURCE)
            {
                if (Empty())
                {
                    ResetIndexes();
                    return boost::none;
                }

                const auto FOUND_ELEMENT_PTR_OPT { Find(RESOURCE) };
                RemoveRelease(FOUND_ELEMENT_PTR_OPT);
                return FOUND_ELEMENT_PTR_OPT;
            }

            // note that the ELEMENT_PTR is left dangling after this call
            bool RemoveFree(const ListElementPtr_t<Element_t> & ELEMENT_PTR)
            {
                if (Empty())
                {
                    ResetIndexes();
                    return false;
                }

                auto const FOUND_ITR { std::find_if(
                    std::begin(elements_), std::end(elements_), [&](auto const & UPTR) {
                        return (UPTR.get() == ELEMENT_PTR);
                    }) };

                if (FOUND_ITR == elements_.end())
                {
                    return false;
                }
                else
                {
                    elements_.erase(FOUND_ITR);

                    if (SelectionIndex() >= elements_.size())
                    {
                        SelectPrev();
                    }

                    if (Empty())
                    {
                        ResetIndexes();
                    }

                    return true;
                }
            }

            // elementPtrOpt is set to boost::none if successful, returns false if optional is empty
            bool RemoveFree(ListElementPtrOpt_t<Element_t> & elementPtrOpt)
            {
                if (elementPtrOpt)
                {
                    if (RemoveFree(elementPtrOpt.value()))
                    {
                        elementPtrOpt = boost::none;
                        return true;
                    }
                }

                return false;
            }

            // returns true if the RESOURCE was found and it's owning ListElementUPtr_t was free'd
            bool RemoveFree(const Element_t & RESOURCE)
            {
                if (Empty())
                {
                    ResetIndexes();
                    return false;
                }

                auto foundElementPtrOpt { Find(RESOURCE) };
                return RemoveFree(foundElementPtrOpt);
            }

            // returns true if there was a selected element and it was removed and free'd
            bool RemoveFreeSelection()
            {
                if (Empty())
                {
                    ResetIndexes();
                    return false;
                }

                return RemoveFree(Selection());
            }

            // returns true if the element was moved
            template <typename OtherStage_t>
            bool Move(
                const ListElementPtr_t<Element_t> & ELEMENT_PTR,
                ListBox<OtherStage_t, Element_t> & listBoxToMoveInto)
            {
                if (RemoveRelease(ELEMENT_PTR))
                {
                    listBoxToMoveInto.AppendAndOwn(ELEMENT_PTR);
                    return true;
                }
                else
                {
                    return false;
                }
            }

            // returns true if the element was moved
            template <typename OtherStage_t>
            bool Move(
                const ListElementPtrOpt_t<Element_t> & ELEMENT_PTR_OPT,
                ListBox<OtherStage_t, Element_t> & listBoxToMoveInto)
            {
                if (RemoveRelease(ELEMENT_PTR_OPT))
                {
                    return listBoxToMoveInto.AppendAndOwn(ELEMENT_PTR_OPT);
                }
                else
                {
                    return false;
                }
            }

            // returns true if the element was moved
            template <typename OtherStage_t>
            bool Move(
                const Element_t & RESOURCE, ListBox<OtherStage_t, Element_t> & listBoxToMoveInto)
            {
                return listBoxToMoveInto.AppendAndOwn(RemoveRelease(RESOURCE));
            }

            // returns true if the element was moved
            template <typename OtherStage_t>
            bool MoveSelection(ListBox<OtherStage_t, Element_t> & listBoxToMoveInto)
            {
                if (Empty())
                {
                    ResetIndexes();
                    return false;
                }
                else
                {
                    return Move(Selection(), listBoxToMoveInto);
                }
            }

            void Clear() { elements_.clear(); }

            std::size_t Size() const { return elements_.size(); }

            bool Empty() const { return elements_.empty(); }

            /*
            bool MouseUp(const sf::Vector2f & MOUSE_POS_V) override
            {
                auto const DID_STATE_CHANGE { GuiEntity::MouseUp(MOUSE_POS_V) };

                const auto INDEX_OPT { IndexAtPos(MOUSE_POS_V) };
                if (INDEX_OPT)
                {
                    const auto INDEX { INDEX_OPT.value() };
                    const auto ELEMENT_AT_POS_PTR { elements_.at(INDEX) };
                    auto const ORIG_SELECTED_ELEMENT_PTR_OPT { Selection() };

                    if (ORIG_SELECTED_ELEMENT_PTR_OPT != ELEMENT_AT_POS_PTR)
                    {
                        SelectionIndex(INDEX);

                        if (false == willMuteAllSfx_)
                        {
                            SoundManager::Instance()->PlaySfx_MouseClick();
                        }

                        boxUPtr_->FakeColorSetAsIfFocusIs(true);

                        const callback::ListBoxPtrPackage_t<Stage_t, Element_t> PTR_PACKAGE {
                            this
                        };

                        const callback::ListBoxEventPackage<Stage_t, Element_t> PACKAGE(
                            PTR_PACKAGE, sfml_util::GuiEvent::Click, MOUSE_POS_V);

                        owningStagePtr_->HandleCallback(PACKAGE);
                        return true;
                    }
                }

                boxUPtr_->FakeColorSetAsIfFocusIs(DID_STATE_CHANGE);
                return DID_STATE_CHANGE;
            }
            */

            bool KeyRelease(const sf::Event::KeyEvent & KEY_EVENT) override
            {
                if (Empty())
                {
                    ResetIndexes();
                    return false;
                }

                auto wasHandled { false };
                if (KEY_EVENT.code == sf::Keyboard::Return)
                {
                    if (false == willMuteAllSfx_)
                    {
                        SoundManager::Instance()->PlaySfx_Keypress();
                    }
                    wasHandled = true;
                }
                else if (KEY_EVENT.code == sf::Keyboard::Up)
                {
                    auto const DID_SELECTION_CHANGE { SelectPrev() };
                    wasHandled = DID_SELECTION_CHANGE;

                    if (false == willMuteAllSfx_)
                    {
                        SoundManager::Instance()->PlaySfx_TickOn();
                    }
                }
                else if (KEY_EVENT.code == sf::Keyboard::Down)
                {
                    auto const DID_SELECTION_CHANGE { SelectNext() };
                    wasHandled = DID_SELECTION_CHANGE;

                    if (false == willMuteAllSfx_)
                    {
                        SoundManager::Instance()->PlaySfx_TickOff();
                    }
                }

                CreateKeypressPackageAndCallHandler(KEY_EVENT);
                return wasHandled;
            }

            void SetEntityPos(const float POS_LEFT, const float POS_TOP) override
            {
                auto const DIFF_HORIZ { POS_LEFT - GetEntityPos().x };
                auto const DIFF_VERT { POS_TOP - GetEntityPos().y };

                for (auto & elementUPtr : elements_)
                {
                    elementUPtr->Move(DIFF_HORIZ, DIFF_VERT);
                }

                boxUPtr_->MoveEntityPos(DIFF_HORIZ, DIFF_VERT);
                GuiEntity::SetEntityPos(POS_LEFT, POS_TOP);
            }

            bool AreAllSfxMute() const { return willMuteAllSfx_; }

            void MuteAllSfx(const bool WILL_MUTE) { willMuteAllSfx_ = WILL_MUTE; }

            // returns true if the selection changed
            bool SelectPrev()
            {
                if (Empty())
                {
                    ResetIndexes();
                    return false;
                }

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

            // returns true if the selection changed
            bool SelectNext()
            {
                if (Empty())
                {
                    ResetIndexes();
                    return false;
                }

                if (SelectionIndex() < (elements_.size() - 1))
                {
                    SelectionIndex(SelectionIndex() + 1);
                    return true;
                }
                else
                {
                    return false;
                }
            }

            // throws if the element is not visible
            const sf::FloatRect ElementRegion(
                const ListElementPtr_t<Element_t> & ELEMENT_PTR,
                const bool WILL_INCLUDE_ELEMENT_PAD = false) const
            {
                M_ASSERT_OR_LOGANDTHROW_SS(
                    (Empty() == false),
                    MakeTypeString() << "::ElementRegion(element={" << ELEMENT_PTR->ToString()
                                     << "}, will_include_element_pad=" << std::boolalpha
                                     << WILL_INCLUDE_ELEMENT_PAD << ") but this listbox is empty.");

                const auto POS_TOP_OPT { ElementPosTopOpt(ELEMENT_PTR, WILL_INCLUDE_ELEMENT_PAD) };

                M_ASSERT_OR_LOGANDTHROW_SS(
                    (!!POS_TOP_OPT),
                    MakeTypeString()
                        << "::ElementRegion(element={" << ELEMENT_PTR->ToString()
                        << "}, will_include_element_pad=" << std::boolalpha
                        << WILL_INCLUDE_ELEMENT_PAD << ") but that ListElement was not visible.");

                const sf::Vector2f POS_V(
                    entityRegion_.left
                        + ((WILL_INCLUDE_ELEMENT_PAD) ? 0.0f : packet_.ElementPad().x),
                    POS_TOP_OPT.value());

                return sf::FloatRect(POS_V, packet_.ElementSize(WILL_INCLUDE_ELEMENT_PAD));
            }

            // returns the max bounding region where the element's image can appear (not the
            // actual sprite's global bounds), throws if the element is not visible
            const sf::FloatRect ElementImageRegionMax(
                const ListElementPtr_t<Element_t> & ELEMENT_PTR,
                const bool WILL_INCLUDE_IMAGE_PAD) const
            {
                M_ASSERT_OR_LOGANDTHROW_SS(
                    (Empty() == false),
                    MakeTypeString()
                        << "::ElementImageRegionMax(element={" << ELEMENT_PTR->ToString()
                        << "}, will_include_image_pad=" << std::boolalpha << WILL_INCLUDE_IMAGE_PAD
                        << ") but this listbox is empty.");

                M_ASSERT_OR_LOGANDTHROW_SS(
                    (IsElementVisible(ELEMENT_PTR)),
                    MakeTypeString()
                        << "::ElementImageRegionMax(element={" << ELEMENT_PTR->ToString()
                        << "}, will_include_image_pad=" << std::boolalpha << WILL_INCLUDE_IMAGE_PAD
                        << ") but that ListElement was not visible.");

                return sf::FloatRect(
                    sfml_util::Position(ElementRegion(ELEMENT_PTR, WILL_INCLUDE_IMAGE_PAD)),
                    packet_.ImageMaxSize(WILL_INCLUDE_IMAGE_PAD));
            }

            // returns the actual element sprite's global bounds, throws if the element is not
            // visible or if it has no image
            const sf::FloatRect
                ElementImageRegionActual(const ListElementPtr_t<Element_t> & ELEMENT_PTR) const
            {
                M_ASSERT_OR_LOGANDTHROW_SS(
                    (Empty() == false),
                    MakeTypeString() << "::ElementImageRegionActual(element={"
                                     << ELEMENT_PTR->ToString() << "}) but this listbox is empty.");

                M_ASSERT_OR_LOGANDTHROW_SS(
                    (IsElementVisible(ELEMENT_PTR)),
                    MakeTypeString()
                        << "::ElementImageRegionActual(element={" << ELEMENT_PTR->ToString()
                        << "}) but that ListElement was not visible.");

                M_ASSERT_OR_LOGANDTHROW_SS(
                    (ELEMENT_PTR->HasImage()),
                    MakeTypeString()
                        << "::ElementImageRegionActual(element={" << ELEMENT_PTR->ToString()
                        << "}) but that ListElement has no image.");

                return ELEMENT_PTR->RegionImage();
            }

            // returns the max region where the element's text can appear, throws if the element is
            // not visible or has no text
            const sf::FloatRect ElementTextRegionMax(
                const ListElementPtr_t<Element_t> & ELEMENT_PTR,
                const bool WILL_INCLUDE_ELEMENT_PAD = false) const
            {
                M_ASSERT_OR_LOGANDTHROW_SS(
                    (Empty() == false),
                    MakeTypeString()
                        << "::ElementTextRegionMax(element={" << ELEMENT_PTR->ToString()
                        << "}, will_include_element_pad=" << std::boolalpha
                        << WILL_INCLUDE_ELEMENT_PAD << ") but this listbox is empty.");

                M_ASSERT_OR_LOGANDTHROW_SS(
                    (IsElementVisible(ELEMENT_PTR)),
                    MakeTypeString()
                        << "::ElementTextRegionMax(element={" << ELEMENT_PTR->ToString()
                        << "}, will_include_element_pad=" << std::boolalpha
                        << WILL_INCLUDE_ELEMENT_PAD << ") but that ListElement was not visible.");

                M_ASSERT_OR_LOGANDTHROW_SS(
                    (ELEMENT_PTR->HasText()),
                    MakeTypeString()
                        << "::ElementTextRegionMax(element={" << ELEMENT_PTR->ToString()
                        << "}, will_include_element_pad=" << std::boolalpha
                        << WILL_INCLUDE_ELEMENT_PAD << ") but that ListElement has no text.");

                auto region { ElementRegionOpt(ELEMENT_PTR, WILL_INCLUDE_ELEMENT_PAD) };

                const auto RIGHT_SHIFT { packet_.ImageMaxLength(true) };
                region.left += RIGHT_SHIFT;
                region.width -= RIGHT_SHIFT;
                return region;
            }

            // returns the element's actual TextRegion's rect, throws if the element is not visible
            // or if it has no text
            const sf::FloatRect
                ElementTextRegionActual(const ListElementPtr_t<Element_t> & ELEMENT_PTR) const
            {
                M_ASSERT_OR_LOGANDTHROW_SS(
                    (Empty() == false),
                    MakeTypeString() << "::ElementTextRegionActual(element={"
                                     << ELEMENT_PTR->ToString() << "}) but this listbox is empty.");

                M_ASSERT_OR_LOGANDTHROW_SS(
                    (IsElementVisible(ELEMENT_PTR)),
                    MakeTypeString()
                        << "::ElementTextRegionMax(element={" << ELEMENT_PTR->ToString()
                        << "}) but that ListElement was not visible.");

                M_ASSERT_OR_LOGANDTHROW_SS(
                    (ELEMENT_PTR->HasText()),
                    MakeTypeString()
                        << "::ElementTextRegionMax(element={" << ELEMENT_PTR->ToString()
                        << "}) but that ListElement has no text.");

                return ELEMENT_PTR->RegionText();
            }

            // throws if ELEMENT_PTR is not in the listbox
            std::size_t IndexOf(const ListElementPtr_t<Element_t> & ELEMENT_PTR) const
            {
                M_ASSERT_OR_LOGANDTHROW_SS(
                    (Empty() == false),
                    MakeTypeString() << "::IndexOf(element={" << ELEMENT_PTR->ToString()
                                     << "}) but this listbox is empty.");

                std::size_t index { elements_.size() };

                const auto ELEMENT_COUNT { elements_.size() };
                for (std::size_t i(0); i < ELEMENT_COUNT; ++i)
                {
                    if (ELEMENT_PTR == elements_.at(i).get())
                    {
                        index = i;
                        break;
                    }
                }

                M_ASSERT_OR_LOGANDTHROW_SS(
                    (index < elements_.size()),
                    MakeTypeString() << "::IndexOf(element={" << ELEMENT_PTR->ToString()
                                     << "}) but that ListElement was not found.");

                return index;
            }

            void SilentExerciseHack()
            {
                if (Empty())
                {
                    ResetIndexes();
                    return;
                }

                willPreventCallbacks_ = true;
                MuteAllSfx(true);
                sf::Event::KeyEvent keyEvent;
                keyEvent.code = sf::Keyboard::Down;
                KeyRelease(keyEvent);
                keyEvent.code = sf::Keyboard::Up;
                KeyRelease(keyEvent);
                MuteAllSfx(false);
                willPreventCallbacks_ = false;
            }

            const_iterator begin() const noexcept { return std::begin(elements_); }
            const_iterator end() const noexcept { return std::end(elements_); }

            const_iterator cbegin() const noexcept { return begin(); }
            const_iterator cend() const noexcept { return end(); }

            const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }
            const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }

            const_reverse_iterator crbegin() const noexcept { return rbegin(); }
            const_reverse_iterator crend() const noexcept { return rend(); }

            const value_type & Front() const
            {
                M_ASSERT_OR_LOGANDTHROW_SS(
                    (Empty() == false), MakeTypeString() << "::Front() const, called when empty.");

                return elements_.front();
            }

            const value_type & Back() const
            {
                M_ASSERT_OR_LOGANDTHROW_SS(
                    (Empty() == false), MakeTypeString() << "::Back() const, called when empty.");

                return elements_.back();
            }

            void Sort(std::function<bool(
                          const ListElementUPtr_t<Element_t> &,
                          const ListElementUPtr_t<Element_t> &)> sortFoo)
            {
                if (Empty())
                {
                    ResetIndexes();
                }
                else
                {
                    const auto SELECTED_ELEMENT_PTR { Selection() };
                    std::sort(std::begin(elements_), std::end(elements_), sortFoo);
                    SelectionIndex(IndexOf(SELECTED_ELEMENT_PTR));
                }
            }

        private:
            // returns the last drawn line vertical position
            float drawElements(sf::RenderTarget & target, sf::RenderStates states) const
            {
                auto lastDrawnLinePosVert { entityRegion_.top };

                if (Empty())
                {
                    return lastDrawnLinePosVert;
                }

                auto const LIST_BOTTOM { sfml_util::Bottom(entityRegion_) };

                auto const SELECTION_INDEX { SelectionIndex() };

                auto const LAST_INDEX_TO_DRAW { LastVisibleIndex_Display() };

                for (std::size_t i(displayIndex_); i <= LAST_INDEX_TO_DRAW; ++i)
                {
                    ListElementPtr_t<Element_t> elementPtr { elements_.at(i).get() };

                    auto const ELEMENT_POS_LEFT_WITHOUT_PAD { entityRegion_.left
                                                              + packet_.ElementPad().x };

                    auto const ELEMENT_POS_TOP_WITH_PAD {
                        entityRegion_.top + (static_cast<float>(i) * packet_.ElementSize(true).y)
                    };

                    auto const ELEMENT_POS_TOP_WITHOUT_PAD { ELEMENT_POS_TOP_WITH_PAD
                                                             + packet_.ElementPad().y };

                    const sf::Vector2f ELEMENT_POS_WITHOUT_PAD_V { ELEMENT_POS_LEFT_WITHOUT_PAD,
                                                                   ELEMENT_POS_TOP_WITHOUT_PAD };

                    if (elementPtr->HasImage())
                    {
                        const sf::FloatRect MAX_IMAGE_RECT(
                            ELEMENT_POS_WITHOUT_PAD_V + packet_.ImagePad(), packet_.ImageMaxSize());

                        if (SelectionIndex() == i)
                        {
                            elementPtr->ImageFitCenterColor(
                                MAX_IMAGE_RECT, packet_.HighlightImageColor());
                        }
                        else
                        {
                            elementPtr->ImageFitCenterColor(MAX_IMAGE_RECT, packet_.ImageColor());
                        }
                    }

                    if (elementPtr->HasText())
                    {
                        sf::Vector2f textPosV { ELEMENT_POS_WITHOUT_PAD_V };

                        textPosV.y
                            += ((packet_.ElementSize(false).y * 0.5f)
                                - (elementPtr->RegionText().height * 0.5f));

                        if (elementPtr->HasImage())
                        {
                            textPosV.x += packet_.ImageMaxLength(true);
                        }

                        elementPtr->TextPos(textPosV);
                    }

                    const sf::FloatRect ELEMENT_RECT_WITH_PAD(
                        ELEMENT_POS_WITHOUT_PAD_V - packet_.ElementPad(),
                        packet_.ElementSize(true));

                    if (elementPtr->IsValid() == false)
                    {
                        sfml_util::DrawRectangle(
                            target,
                            states,
                            ELEMENT_RECT_WITH_PAD,
                            sf::Color::Transparent,
                            0,
                            packet_.HighlightColorInvalid());
                    }

                    if (SELECTION_INDEX == i)
                    {
                        sfml_util::DrawRectangle(
                            target,
                            states,
                            ELEMENT_RECT_WITH_PAD,
                            sf::Color::Transparent,
                            0,
                            packet_.HighlightColor());
                    }

                    target.draw(*elementPtr, states);

                    auto const LINE_POS_TOP { sfml_util::Bottom(ELEMENT_RECT_WITH_PAD) };

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

                return lastDrawnLinePosVert;
            }

            void FreeGuiBoxAndSliderBar()
            {
                if (boxUPtr_)
                {
                    owningStagePtr_->EntityRemove(boxUPtr_.get());
                    boxUPtr_.reset();
                }

                if (sliderbarUPtr_)
                {
                    owningStagePtr_->EntityRemove(sliderbarUPtr_.get());
                    sliderbarUPtr_.reset();
                }
            }

            void Setup()
            {
                const auto REGION { packet_.BoxInfo().region };
                SetEntityRegion(REGION);

                FreeGuiBoxAndSliderBar();

                elements_.clear();

                // box
                boxUPtr_ = std::make_unique<sfml_util::gui::box::Box>(
                    "Box_for_{" + MakeTypeString() + "}", packet_.BoxInfo());

                boxUPtr_->SetupBox(packet_.BoxInfo());
                boxUPtr_->SetWillAcceptFocus(false);
                owningStagePtr_->EntityAdd(boxUPtr_.get());

                // sliderbar
                sliderbarUPtr_ = std::make_unique<sfml_util::gui::SliderBar>(
                    "SliderBar_for_{" + MakeTypeString() + "}",
                    sfml_util::Right(REGION) + 10.0f,
                    REGION.top + 10.0f,
                    REGION.height - 20.0f,
                    sfml_util::gui::SliderStyle(),
                    sfml_util::gui::callback::ISliderBarCallbackHandlerPtr_t(this));

                sliderbarUPtr_->SetCurrentValue(0.0f);
                owningStagePtr_->EntityAdd(sliderbarUPtr_.get());

                willMuteAllSfx_ = false;
                ResetIndexes();
                maxVisibleCount_ = MaxVisibleElements();
                SilentExerciseHack();
                willPreventCallbacks_ = false;
            }

            const std::string MakeTypeString(const std::string & ENTITY_NAME = "") const
            {
                std::ostringstream ss;

                ss << "ListBox<" << boost::typeindex::type_id<Stage_t>().pretty_name() << ", "
                   << boost::typeindex::type_id<Element_t>().pretty_name() << ">(entity_name=\"";

                if (ENTITY_NAME.empty())
                {
                    ss << GetEntityName();
                }
                else
                {
                    ss << ENTITY_NAME;
                }

                ss << "\")";

                return ss.str();
            }

            std::size_t MaxFirstDisplayableIndex() const
            {
                if (elements_.size() > maxVisibleCount_)
                {
                    return (elements_.size() - maxVisibleCount_);
                }
                else
                {
                    return 0;
                }
            }

            void CreateKeypressPackageAndCallHandler(const sf::Event::KeyEvent & KEY_EVENT)
            {
                if (WillCallback())
                {
                    const callback::ListBoxPtrPackage_t<Stage_t, Element_t> PTR_PACKAGE(this);

                    const callback::ListBoxEventPackage<Stage_t, Element_t> PACKAGE(
                        PTR_PACKAGE, KEY_EVENT);

                    owningStagePtr_->HandleCallback(PACKAGE);
                }
            }

            void OnClick(const sf::Vector2f & MOUSE_POS_V) override
            {
                if (Empty())
                {
                    ResetIndexes();
                    return;
                }

                const auto INDEX_OPT { IndexAtPos(MOUSE_POS_V) };
                if (!INDEX_OPT)
                {
                    return;
                }

                const ListElementPtr_t<Element_t> ELEMENT_PTR {
                    elements_.at(INDEX_OPT.value()).get()
                };

                if (ELEMENT_PTR != Selection())
                {
                    SelectionIndex(INDEX_OPT.value());

                    if (false == willMuteAllSfx_)
                    {
                        SoundManager::Instance()->PlaySfx_MouseClick();
                    }

                    boxUPtr_->FakeColorSetAsIfFocusIs(true);

                    /*
                     * nobody is using this -zTn 2018-7-20
                     *
                    if (WillCallback())
                    {
                        const callback::ListBoxPtrPackage_t<Stage_t, Element_t> PTR_PACKAGE {
                            this
                        };

                        const callback::ListBoxEventPackage<Stage_t, Element_t> PACKAGE(
                            PTR_PACKAGE, sfml_util::GuiEvent::Click, MOUSE_POS_V);

                        owningStagePtr_->HandleCallback(PACKAGE);
                    }
                    */
                }
            }

            void OnDoubleClick(const sf::Vector2f & MOUSE_POS_V) override
            {
                if (WillCallback())
                {
                    const callback::ListBoxPtrPackage_t<Stage_t, Element_t> PTR_PACKAGE(this);

                    const callback::ListBoxEventPackage<Stage_t, Element_t> PACKAGE(
                        PTR_PACKAGE, sfml_util::GuiEvent::DoubleClick, MOUSE_POS_V);

                    owningStagePtr_->HandleCallback(PACKAGE);
                }
            }

            std::size_t MaxVisibleElements() const
            {
                const auto VISIBLE_COUNT_FLOOR { std::floor(
                    entityRegion_.height / packet_.ElementSize(true).y) };

                const auto VISIBLE_COUNT_FLOOR_HEIGHT { (
                    VISIBLE_COUNT_FLOOR * packet_.ElementSize(true).y) };

                const auto VISIBLE_COUNT_FLOOR_PLUS1_WITHOUTBOTTOMPAD_HEIGHT {
                    (VISIBLE_COUNT_FLOOR_HEIGHT + packet_.ElementSize(true).y)
                    - packet_.ElementPad().y
                };

                if (VISIBLE_COUNT_FLOOR_PLUS1_WITHOUTBOTTOMPAD_HEIGHT < entityRegion_.height)
                {
                    return static_cast<std::size_t>(VISIBLE_COUNT_FLOOR) + 1;
                }
                else
                {
                    return static_cast<std::size_t>(VISIBLE_COUNT_FLOOR);
                }
            }

            std::size_t LastVisibleIndex_Display() const
            {
                if (Empty())
                {
                    return 0;
                }
                else
                {
                    return std::min((displayIndex_ + maxVisibleCount_) - 1, elements_.size() - 1);
                }
            }

            std::size_t LastVisibleIndex_Selection() const
            {
                if (Empty())
                {
                    return 0;
                }
                else
                {
                    return std::min(
                        (selectionDisplayIndex_ + maxVisibleCount_) - 1, elements_.size() - 1);
                }
            }

            void DrawLine(sf::RenderTarget & target, const float POS_TOP) const
            {
                std::array<sf::Vertex, 3> va;

                va[0]
                    = sf::Vertex(sf::Vector2f(entityRegion_.left, POS_TOP), sf::Color::Transparent);

                va[1] = sf::Vertex(
                    sf::Vector2f(sfml_util::CenterOfHoriz(entityRegion_), POS_TOP),
                    packet_.LineColor());

                va[2] = sf::Vertex(
                    sf::Vector2f(sfml_util::Right(entityRegion_), POS_TOP), sf::Color::Transparent);

                target.draw(va.data(), va.size(), sf::LinesStrip);
            }

            void ResetIndexes()
            {
                displayIndex_ = 0;
                selectionDisplayIndex_ = 0;
                selectionOffsetIndex_ = 0;
            }

            bool WillCallback() const
            {
                return ((false == willPreventCallbacks_) && (Empty() == false));
            }

        private:
            misc::NotNull<Stage_t *> owningStagePtr_;

            // contains color and spacing details of a ListBox
            ListBoxPacket packet_;

            // visible border that surrounds the this ListBox
            box::BoxUPtr_t boxUPtr_;

            // sliderbar widget
            SliderBarUPtr_t sliderbarUPtr_;

            // elements contained in this ListBox
            container_type elements_;

            // true if this ListBox will play sound effects in response to player interaction
            bool willMuteAllSfx_;

            // which element is displayed at the top of the list
            std::size_t displayIndex_;

            // which element is displayed at the top of the list when the selection is displayed
            std::size_t selectionDisplayIndex_;

            // offset from selectionDisplayIndex_ to the actual selection
            std::size_t selectionOffsetIndex_;

            // how many elements can be visible at once (how many fit vertically in the ListBox)
            std::size_t maxVisibleCount_;

            // prevents callbacks from happening
            bool willPreventCallbacks_;
        };

        template <typename Stage_t, typename Element_t>
        using ListBoxPtr_t = misc::NotNull<ListBox<Stage_t, Element_t> *>;

        template <typename Stage_t, typename Element_t>
        using ListBoxUPtr_t = std::unique_ptr<ListBox<Stage_t, Element_t>>;

        template <typename Stage_t, typename Element_t>
        inline auto begin(const ListBox<Stage_t, Element_t> & LB) noexcept
        {
            return LB.begin();
        }

        template <typename Stage_t, typename Element_t>
        inline auto cbegin(const ListBox<Stage_t, Element_t> & LB) noexcept
        {
            return begin(LB);
        }

        template <typename Stage_t, typename Element_t>
        inline auto rbegin(const ListBox<Stage_t, Element_t> & LB) noexcept
        {
            return LB.rbegin();
        }

        template <typename Stage_t, typename Element_t>
        inline auto crbegin(const ListBox<Stage_t, Element_t> & LB) noexcept
        {
            return rbegin(LB);
        }

        template <typename Stage_t, typename Element_t>
        inline auto end(const ListBox<Stage_t, Element_t> & LB) noexcept
        {
            return LB.end();
        }

        template <typename Stage_t, typename Element_t>
        inline auto cend(const ListBox<Stage_t, Element_t> & LB) noexcept
        {
            return end(LB);
        }

        template <typename Stage_t, typename Element_t>
        inline auto rend(const ListBox<Stage_t, Element_t> & LB) noexcept
        {
            return LB.rend();
        }

        template <typename Stage_t, typename Element_t>
        inline auto crend(const ListBox<Stage_t, Element_t> & LB) noexcept
        {
            return rend(LB);
        }

    } // namespace gui
} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_GUI_LISTBOX_HPP_INCLUDED
