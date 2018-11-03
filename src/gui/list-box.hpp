// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_GUI_LISTBOX_HPP_INCLUDED
#define HEROESPATH_GUI_LISTBOX_HPP_INCLUDED
//
// list-box.hpp
//
#include "gui/box-entity.hpp"
#include "gui/callback.hpp"
#include "gui/entity.hpp"
#include "gui/gui-event-enum.hpp"
#include "gui/list-box-event-packet.hpp"
#include "gui/list-box-packet.hpp"
#include "gui/list-element.hpp"
#include "gui/sliderbar.hpp"
#include "gui/sound-manager.hpp"
#include "misc/boost-optional-that-throws.hpp"
#include "misc/not-null.hpp"
#include "misc/vector-map.hpp"
#include "sfutil/center-of.hpp"
#include "sfutil/position.hpp"
#include "sfutil/primitives.hpp"
#include "stage/i-stage.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>

#include <boost/type_index.hpp>

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
namespace gui
{

    // A class that manages a vertical list of elements with a sliderbar that can be scrolled
    // through, selected, and activated.  Each element is a type that can hold an observer
    // pointer to a game resoure, an image, text, and a flag for if that element is invalid.
    // Invalid elements are drawn with a red overlay, are selectable, but not activatable.
    // Callbacks to the callbackHandlerPtr_ will not occur if the listbox is empty.
    // If Empty()==false, then the selection index and element pointer must be valid.
    template <typename Stage_t, typename Element_t>
    class ListBox
        : public Entity
        , public SliderBar::Callback_t::IHandler_t
    {
    public:
        using Callback_t = Callback<ListBoxEventPacket<Stage_t, Element_t>>;

        using OwningStagePtr_t = misc::NotNull<Stage_t *>;

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
            const OwningStagePtr_t & OWNING_STAGE_PTR,
            const typename Callback_t::IHandlerPtr_t CALLBACK_HANDLER_PTR,
            const ListBoxPacket & PACKET = ListBoxPacket())
            : Entity(MakeTypeString(NAME), sf::FloatRect(0.0f, 0.0f, 0.0f, 0.0f))
            , owningStagePtr_(OWNING_STAGE_PTR)
            , callbackHandlerPtr_(CALLBACK_HANDLER_PTR)
            , boxUPtr_() // this and all remaining variables will be initialized in Setup()
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

        bool HandleCallback(const SliderBar::Callback_t::PacketPtr_t PACKET_PTR) override
        {
            if (Empty())
            {
                ResetIndexes();
                return false;
            }
            else
            {
                const auto NEW_INDEX_F { static_cast<float>(elements_.size() - 1)
                                         * PACKET_PTR->PositionRatio() };

                DisplayIndex(static_cast<std::size_t>(NEW_INDEX_F));
                return true;
            }
        }

        void draw(sf::RenderTarget & target, sf::RenderStates states) const override
        {
            const auto LIST_BOTTOM { sfutil::Bottom(entityRegion_) };

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
                const auto LAST_INDEX_TO_DRAW { LastVisibleIndex_Display() };

                for (std::size_t i(displayIndex_); i <= LAST_INDEX_TO_DRAW; ++i)
                {
                    const ListElementPtr_t<Element_t> NEXT_ELEMENT_PTR { elements_.at(i).get() };

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
            M_HP_ASSERT_OR_LOG_AND_THROW(
                (Empty() == false), MakeTypeString() << "::Selection() but the listbox was empty.");

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
                    const auto MAX_FIRST_DISPLAYABLE_INDEX { MaxFirstDisplayableIndex() };
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
                            const auto MAX_FIRST_DISPLAYABLE_INDEX { MaxFirstDisplayableIndex() };
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
            return ((INDEX >= selectionDisplayIndex_) && (INDEX <= LastVisibleIndex_Selection()));
        }

        ListElementPtr_t<Element_t> At(const std::size_t INDEX) const
        {
            M_HP_ASSERT_OR_LOG_AND_THROW(
                (Empty() == false),
                MakeTypeString() << "::At(index=" << INDEX << ") but this listbox is empty.");

            M_HP_ASSERT_OR_LOG_AND_THROW(
                (INDEX < elements_.size()),
                MakeTypeString() << "::At(" << INDEX
                                 << ") but that index is out of bounds with actual size="
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
                std::begin(elements_), std::end(elements_), [&](const auto & ELEMENT_UPTR) {
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
            elements_.emplace_back(ListElementUPtr_t<Element_t>(ELEMENT_PTR));
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

            const auto FOUND_ITR { std::find_if(
                std::begin(elements_), std::end(elements_), [&](const auto & UPTR) {
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

            const auto FOUND_ITR { std::find_if(
                std::begin(elements_), std::end(elements_), [&](const auto & UPTR) {
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
        bool Move(const Element_t & RESOURCE, ListBox<OtherStage_t, Element_t> & listBoxToMoveInto)
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
                const auto DID_SELECTION_CHANGE { SelectPrev() };
                wasHandled = DID_SELECTION_CHANGE;

                if (false == willMuteAllSfx_)
                {
                    SoundManager::Instance()->PlaySfx_TickOn();
                }
            }
            else if (KEY_EVENT.code == sf::Keyboard::Down)
            {
                const auto DID_SELECTION_CHANGE { SelectNext() };
                wasHandled = DID_SELECTION_CHANGE;

                if (false == willMuteAllSfx_)
                {
                    SoundManager::Instance()->PlaySfx_TickOff();
                }
            }

            CreateKeypressPacketAndCallHandler(KEY_EVENT);
            return wasHandled;
        }

        // does not actually set the position, instead it moves relative to GetEntityPos()
        void SetEntityPos(const float POS_LEFT, const float POS_TOP) override
        {
            const auto DIFF_V { sf::Vector2f(POS_LEFT, POS_TOP) - GetEntityPos() };
            MoveEntityPos(DIFF_V.x, DIFF_V.y);
        }

        void MoveEntityPos(const float HORIZ, const float VERT) override
        {
            entityRegion_.left += HORIZ;
            entityRegion_.top += VERT;

            for (auto & elementUPtr : elements_)
            {
                elementUPtr->Move(HORIZ, VERT);
            }

            boxUPtr_->MoveEntityPos(HORIZ, VERT);
            sliderbarUPtr_->MoveEntityPos(HORIZ, VERT);
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
            M_HP_ASSERT_OR_LOG_AND_THROW(
                (Empty() == false),
                MakeTypeString() << "::ElementRegion(element={" << ELEMENT_PTR->ToString()
                                 << "}, will_include_element_pad=" << std::boolalpha
                                 << WILL_INCLUDE_ELEMENT_PAD << ") but this listbox is empty.");

            const auto POS_TOP_OPT { ElementPosTopOpt(ELEMENT_PTR, WILL_INCLUDE_ELEMENT_PAD) };

            M_HP_ASSERT_OR_LOG_AND_THROW(
                (!!POS_TOP_OPT),
                MakeTypeString() << "::ElementRegion(element={" << ELEMENT_PTR->ToString()
                                 << "}, will_include_element_pad=" << std::boolalpha
                                 << WILL_INCLUDE_ELEMENT_PAD
                                 << ") but that ListElement was not visible.");

            const sf::Vector2f POS_V(
                entityRegion_.left + ((WILL_INCLUDE_ELEMENT_PAD) ? 0.0f : packet_.ElementPad().x),
                POS_TOP_OPT.value());

            return sf::FloatRect(POS_V, packet_.ElementSize(WILL_INCLUDE_ELEMENT_PAD));
        }

        // returns the max bounding region where the element's image can appear (not the
        // actual sprite's global bounds), throws if the element is not visible
        const sf::FloatRect ElementImageRegionMax(
            const ListElementPtr_t<Element_t> & ELEMENT_PTR,
            const bool WILL_INCLUDE_IMAGE_PAD) const
        {
            M_HP_ASSERT_OR_LOG_AND_THROW(
                (Empty() == false),
                MakeTypeString() << "::ElementImageRegionMax(element={" << ELEMENT_PTR->ToString()
                                 << "}, will_include_image_pad=" << std::boolalpha
                                 << WILL_INCLUDE_IMAGE_PAD << ") but this listbox is empty.");

            M_HP_ASSERT_OR_LOG_AND_THROW(
                (IsElementVisible(ELEMENT_PTR)),
                MakeTypeString() << "::ElementImageRegionMax(element={" << ELEMENT_PTR->ToString()
                                 << "}, will_include_image_pad=" << std::boolalpha
                                 << WILL_INCLUDE_IMAGE_PAD
                                 << ") but that ListElement was not visible.");

            return sf::FloatRect(
                Position(ElementRegion(ELEMENT_PTR, WILL_INCLUDE_IMAGE_PAD)),
                packet_.ImageMaxSize(WILL_INCLUDE_IMAGE_PAD));
        }

        // returns the actual element sprite's global bounds, throws if the element is not
        // visible or if it has no image
        const sf::FloatRect
            ElementImageRegionActual(const ListElementPtr_t<Element_t> & ELEMENT_PTR) const
        {
            M_HP_ASSERT_OR_LOG_AND_THROW(
                (Empty() == false),
                MakeTypeString() << "::ElementImageRegionActual(element={"
                                 << ELEMENT_PTR->ToString() << "}) but this listbox is empty.");

            M_HP_ASSERT_OR_LOG_AND_THROW(
                (IsElementVisible(ELEMENT_PTR)),
                MakeTypeString() << "::ElementImageRegionActual(element={"
                                 << ELEMENT_PTR->ToString()
                                 << "}) but that ListElement was not visible.");

            M_HP_ASSERT_OR_LOG_AND_THROW(
                (ELEMENT_PTR->HasImage()),
                MakeTypeString() << "::ElementImageRegionActual(element={"
                                 << ELEMENT_PTR->ToString()
                                 << "}) but that ListElement has no image.");

            return ELEMENT_PTR->RegionImage();
        }

        // returns the max region where the element's text can appear, throws if the element is
        // not visible or has no text
        const sf::FloatRect ElementTextRegionMax(
            const ListElementPtr_t<Element_t> & ELEMENT_PTR,
            const bool WILL_INCLUDE_ELEMENT_PAD = false) const
        {
            M_HP_ASSERT_OR_LOG_AND_THROW(
                (Empty() == false),
                MakeTypeString() << "::ElementTextRegionMax(element={" << ELEMENT_PTR->ToString()
                                 << "}, will_include_element_pad=" << std::boolalpha
                                 << WILL_INCLUDE_ELEMENT_PAD << ") but this listbox is empty.");

            M_HP_ASSERT_OR_LOG_AND_THROW(
                (IsElementVisible(ELEMENT_PTR)),
                MakeTypeString() << "::ElementTextRegionMax(element={" << ELEMENT_PTR->ToString()
                                 << "}, will_include_element_pad=" << std::boolalpha
                                 << WILL_INCLUDE_ELEMENT_PAD
                                 << ") but that ListElement was not visible.");

            M_HP_ASSERT_OR_LOG_AND_THROW(
                (ELEMENT_PTR->HasText()),
                MakeTypeString() << "::ElementTextRegionMax(element={" << ELEMENT_PTR->ToString()
                                 << "}, will_include_element_pad=" << std::boolalpha
                                 << WILL_INCLUDE_ELEMENT_PAD
                                 << ") but that ListElement has no text.");

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
            M_HP_ASSERT_OR_LOG_AND_THROW(
                (Empty() == false),
                MakeTypeString() << "::ElementTextRegionActual(element={" << ELEMENT_PTR->ToString()
                                 << "}) but this listbox is empty.");

            M_HP_ASSERT_OR_LOG_AND_THROW(
                (IsElementVisible(ELEMENT_PTR)),
                MakeTypeString() << "::ElementTextRegionMax(element={" << ELEMENT_PTR->ToString()
                                 << "}) but that ListElement was not visible.");

            M_HP_ASSERT_OR_LOG_AND_THROW(
                (ELEMENT_PTR->HasText()),
                MakeTypeString() << "::ElementTextRegionMax(element={" << ELEMENT_PTR->ToString()
                                 << "}) but that ListElement has no text.");

            return ELEMENT_PTR->RegionText();
        }

        // throws if ELEMENT_PTR is not in the listbox
        std::size_t IndexOf(const ListElementPtr_t<Element_t> & ELEMENT_PTR) const
        {
            M_HP_ASSERT_OR_LOG_AND_THROW(
                (Empty() == false),
                MakeTypeString() << "::IndexOf(element={" << ELEMENT_PTR->ToString()
                                 << "}) but this listbox is empty.");

            const auto ELEMENT_COUNT { elements_.size() };
            std::size_t finalIndexOf { ELEMENT_COUNT };

            for (std::size_t i(0); i < ELEMENT_COUNT; ++i)
            {
                if (ELEMENT_PTR == elements_.at(i).get())
                {
                    finalIndexOf = i;
                    break;
                }
            }

            M_HP_ASSERT_OR_LOG_AND_THROW(
                (finalIndexOf < elements_.size()),
                MakeTypeString() << "::IndexOf(element={" << ELEMENT_PTR->ToString()
                                 << "}) but that ListElement was not found.");

            return finalIndexOf;
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
            M_HP_ASSERT_OR_LOG_AND_THROW(
                (Empty() == false), MakeTypeString() << "::Front() const, called when empty.");

            return elements_.front();
        }

        const value_type & Back() const
        {
            M_HP_ASSERT_OR_LOG_AND_THROW(
                (Empty() == false), MakeTypeString() << "::Back() const, called when empty.");

            return elements_.back();
        }

        void Sort(std::function<
                  bool(const ListElementUPtr_t<Element_t> &, const ListElementUPtr_t<Element_t> &)>
                      sortFoo)
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

            const auto LIST_BOTTOM { sfutil::Bottom(entityRegion_) };

            const auto SELECTION_INDEX { SelectionIndex() };

            const auto LAST_INDEX_TO_DRAW { LastVisibleIndex_Display() };

            for (std::size_t i(displayIndex_); i <= LAST_INDEX_TO_DRAW; ++i)
            {
                ListElementPtr_t<Element_t> elementPtr { elements_.at(i).get() };

                const auto ELEMENT_POS_LEFT_WITHOUT_PAD { entityRegion_.left
                                                          + packet_.ElementPad().x };

                const auto ELEMENT_POS_TOP_WITH_PAD {
                    entityRegion_.top + (static_cast<float>(i) * packet_.ElementSize(true).y)
                };

                const auto ELEMENT_POS_TOP_WITHOUT_PAD { ELEMENT_POS_TOP_WITH_PAD
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
                    ELEMENT_POS_WITHOUT_PAD_V - packet_.ElementPad(), packet_.ElementSize(true));

                if (elementPtr->IsValid() == false)
                {
                    target.draw(
                        sfutil::MakeRectangleSolid(
                            ELEMENT_RECT_WITH_PAD, packet_.HighlightColorInvalid()),
                        states);
                }

                if (SELECTION_INDEX == i)
                {
                    target.draw(
                        sfutil::MakeRectangleSolid(ELEMENT_RECT_WITH_PAD, packet_.HighlightColor()),
                        states);
                }

                target.draw(*elementPtr, states);

                const auto LINE_POS_TOP { sfutil::Bottom(ELEMENT_RECT_WITH_PAD) };

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
                owningStagePtr_->EntityRemove(boxUPtr_);
                boxUPtr_.reset();
            }

            if (sliderbarUPtr_)
            {
                owningStagePtr_->EntityRemove(sliderbarUPtr_);
                sliderbarUPtr_.reset();
            }
        }

        void Setup()
        {
            const auto REGION { packet_.Region() };
            SetEntityRegion(REGION);

            FreeGuiBoxAndSliderBar();

            elements_.clear();

            // box
            boxUPtr_ = std::make_unique<BoxEntity>(
                "Box_for_{" + MakeTypeString() + "}", REGION, packet_.BoxInfo());

            boxUPtr_->SetWillAcceptFocus(false);
            owningStagePtr_->EntityAdd(boxUPtr_, true);

            // sliderbar
            sliderbarUPtr_ = std::make_unique<SliderBar>(
                "SliderBar_for_{" + MakeTypeString() + "}",
                sfutil::Right(REGION) + 10.0f,
                REGION.top + 10.0f,
                REGION.height - 20.0f,
                SliderStyle(),
                SliderBar::Callback_t::IHandlerPtr_t(this));

            sliderbarUPtr_->PositionRatio(0.0f);
            owningStagePtr_->EntityAdd(sliderbarUPtr_);

            willMuteAllSfx_ = false;
            ResetIndexes();
            maxVisibleCount_ = MaxVisibleElements();
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

        void CreateKeypressPacketAndCallHandler(const sf::Event::KeyEvent & KEY_EVENT)
        {
            if (WillCallback())
            {

                ListBoxEventPacket<Stage_t, Element_t> eventPacket(
                    misc::MakeNotNull(this), GuiEvent::Keypress, KEY_EVENT);

                callbackHandlerPtr_->HandleCallback(misc::MakeNotNull(&eventPacket));
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

            const ListElementPtr_t<Element_t> ELEMENT_PTR { elements_.at(INDEX_OPT.value()).get() };

            if (ELEMENT_PTR != Selection())
            {
                SelectionIndex(INDEX_OPT.value());

                if (false == willMuteAllSfx_)
                {
                    SoundManager::Instance()->PlaySfx_MouseClick();
                }

                boxUPtr_->FakeFocusColorsAsIfFocusIs(true);
            }
        }

        void OnDoubleClick(const sf::Vector2f &) override
        {
            if (WillCallback())
            {
                ListBoxEventPacket<Stage_t, Element_t> eventPacket(
                    misc::MakeNotNull(this), GuiEvent::DoubleClick);

                callbackHandlerPtr_->HandleCallback(misc::MakeNotNull(&eventPacket));
            }
        }

        std::size_t MaxVisibleElements() const
        {
            const auto VISIBLE_COUNT_FLOOR { std::floor(
                entityRegion_.height / packet_.ElementSize(true).y) };

            const auto VISIBLE_COUNT_FLOOR_HEIGHT { (
                VISIBLE_COUNT_FLOOR * packet_.ElementSize(true).y) };

            const auto VISIBLE_COUNT_FLOOR_PLUS1_WITHOUTBOTTOMPAD_HEIGHT {
                (VISIBLE_COUNT_FLOOR_HEIGHT + packet_.ElementSize(true).y) - packet_.ElementPad().y
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

            va[0] = sf::Vertex(sf::Vector2f(entityRegion_.left, POS_TOP), sf::Color::Transparent);

            va[1] = sf::Vertex(
                sf::Vector2f(sfutil::CenterOfHoriz(entityRegion_), POS_TOP), packet_.LineColor());

            va[2] = sf::Vertex(
                sf::Vector2f(sfutil::Right(entityRegion_), POS_TOP), sf::Color::Transparent);

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

    protected:
        void SetEntityRegion(const sf::FloatRect & R) override { Entity::SetEntityRegion(R); }

    private:
        OwningStagePtr_t owningStagePtr_;
        typename Callback_t::IHandlerPtr_t callbackHandlerPtr_;

        // contains color and spacing details of a ListBox
        ListBoxPacket packet_;

        // visible border and background color
        BoxEntityUPtr_t boxUPtr_;

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
    class ListBox;

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
} // namespace heroespath

#endif // HEROESPATH_GUI_LISTBOX_HPP_INCLUDED
