// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// list-box.cpp
//
#include "list-box.hpp"

#include "log/log-macros.hpp"
#include "sfml-util/gui/box.hpp"
#include "sfml-util/gui/list-box-item-factory.hpp"
#include "sfml-util/gui/list-box-item.hpp"
#include "sfml-util/i-stage.hpp"
#include "sfml-util/sfml-util.hpp"
#include "sfml-util/sound-manager.hpp"

namespace heroespath
{
namespace sfml_util
{
    namespace gui
    {

        namespace callback
        {
            const int ListBoxEventPackage::INVALID_SELECTION_{ -1 };
            const sf::Vector2f ListBoxEventPackage::INVALID_MOUSE_POS_V_{ -1.0f, -1.0f };
        } // namespace callback

        const sf::Color ListBox::NO_COLOR_{ 0, 0, 0, 0 };
        const sf::Color ListBox::INVALID_ITEM_HIGHLIGHT_COLOR_{ 127, 32, 32, 64 };

        ListBox::ListBox(
            const std::string & NAME,
            const sf::FloatRect & REGION,
            const IStagePtrOpt_t ISTAGE_PTR_OPT,
            const box::Info & BOX_INFO,
            const sf::Color & LINE_COLOR,
            const callback::IListBoxCallbackHandlerPtrOpt_t CALLBACK_PTR_OPT,
            const sf::Color & IMAGE_COLOR,
            const sf::Color & HIGHLIGHT_COLOR,
            const float BETWEEN_PAD)
            : GuiEntity(std::string(NAME).append("_ListBox"), REGION)
            , HORIZ_PAD_(sfml_util::ScreenRatioToPixelsHoriz(0.007f))
            , itemSizeV_(0.0f, 0.0f) // remaining members are initailized in Setup()
            , betweenPadVert_(0.0f)
            , boxUPtr_()
            , sliderbarUPtr_()
            , lineColor_(sf::Color::Transparent)
            , highlightColor_(sf::Color::Transparent)
            , stagePtrOpt_(boost::none)
            , items_()
            , imageColor_(sf::Color::White)
            , willPlaySfx_(true)
            , callbackPtrOpt_(boost::none)
            , displayIndex_(0)
            , selectionDisplayIndex_(0)
            , selectionOffsetIndex_(0)
            , visibleCount_(0)
        {
            Setup(
                REGION,
                ISTAGE_PTR_OPT,
                BOX_INFO,
                LINE_COLOR,
                CALLBACK_PTR_OPT,
                IMAGE_COLOR,
                HIGHLIGHT_COLOR,
                BETWEEN_PAD);
        }

        ListBox::~ListBox()
        {
            if (stagePtrOpt_)
            {
                if (boxUPtr_)
                {
                    stagePtrOpt_->Obj().EntityRemove(boxUPtr_.get());
                }

                if (sliderbarUPtr_)
                {
                    stagePtrOpt_->Obj().EntityRemove(sliderbarUPtr_.get());
                }
            }
        }

        void ListBox::Setup(
            const sf::FloatRect & REGION,
            const IStagePtrOpt_t ISTAGE_PTR_OPT,
            const box::Info & BOX_INFO,
            const sf::Color & LINE_COLOR,
            const callback::IListBoxCallbackHandlerPtrOpt_t CALLBACK_PTR_OPT,
            const sf::Color & IMAGE_COLOR,
            const sf::Color & HIGHLIGHT_COLOR,
            const float BETWEEN_PAD)
        {
            SetEntityRegion(REGION);

            ResetItemSize();

            items_.clear();
            stagePtrOpt_ = ISTAGE_PTR_OPT;
            betweenPadVert_ = BETWEEN_PAD;
            lineColor_ = LINE_COLOR;
            callbackPtrOpt_ = CALLBACK_PTR_OPT;
            imageColor_ = IMAGE_COLOR;

            highlightColor_
                = ((HIGHLIGHT_COLOR == NO_COLOR_)
                       ? (BOX_INFO.bg_info.color + sf::Color(20, 20, 20, 20))
                       : HIGHLIGHT_COLOR);

            betweenPadVert_
                = ((BETWEEN_PAD < 0.0f) ? sfml_util::ScreenRatioToPixelsVert(0.00667f)
                                        : BETWEEN_PAD);

            if (!boxUPtr_)
            {
                boxUPtr_ = std::make_unique<sfml_util::gui::box::Box>("ListBox's", BOX_INFO);

                if (stagePtrOpt_)
                {
                    stagePtrOpt_->Obj().EntityAdd(boxUPtr_.get());
                }
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

                if (stagePtrOpt_)
                {
                    stagePtrOpt_->Obj().EntityAdd(sliderbarUPtr_.get());
                }
            }

            sliderbarUPtr_->SetCurrentValue(0.0f);
            displayIndex_ = 0;
            selectionDisplayIndex_ = 0;
            selectionOffsetIndex_ = 0;
            visibleCount_ = CalcVisibleItems();
        }

        bool ListBox::HandleCallback(const callback::SliderBarCallbackPackage_t & PACKAGE)
        {
            if (Empty())
            {
                return false;
            }
            else
            {
                auto const NEW_INDEX_F{ static_cast<float>(items_.size() - 1)
                                        * PACKAGE.PTR_->GetCurrentValue() };

                DisplayIndex(static_cast<std::size_t>(NEW_INDEX_F));
                return true;
            }
        }

        void ListBox::draw(sf::RenderTarget & target, sf::RenderStates states) const
        {
            // use the ListBox's entityWillDraw_ to control whether
            // ListBox is responsible for drawing its entitys
            if (false == entityWillDraw_)
            {
                return;
            }

            auto lastDrawnLinePosVert{ entityRegion_.top };
            auto const LIST_BOTTOM{ entityRegion_.top + entityRegion_.height };
            auto const SELECTION_INDEX{ SelectedIndex() };

            if (Empty() == false)
            {
                std::size_t i{ 0 };
                for (; i < displayIndex_; ++i)
                {
                    items_.at(i)->SetEntityWillDraw(false);
                }

                auto const LAST_INDEX_TO_DRAW{ std::min(
                    CalcLastVisibleIndex_Display(), items_.size() - 1) };

                for (; i <= LAST_INDEX_TO_DRAW; ++i)
                {
                    auto itemPtr{ ListBoxItemPtr_t(items_.at(i).get()) };
                    auto const ITEM_RECT{ FullItemRect(itemPtr) };

                    itemPtr->SetEntityWillDraw(true);

                    auto const TEXT_LEFT{ entityRegion_.left + HORIZ_PAD_ };

                    auto const TEXT_TOP{ entityRegion_.top
                                         + (static_cast<float>(i)
                                            * (itemSizeV_.y + betweenPadVert_)) };

                    itemPtr->SetEntityPos(TEXT_LEFT, TEXT_TOP);

                    if (itemPtr->HasImage())
                    {
                        itemPtr->MoveEntityPos(itemSizeV_.y + HORIZ_PAD_, 0.0f);

                        auto & sprite{ itemPtr->Sprite() };

                        const sf::FloatRect IMAGE_RECT(
                            entityRegion_.left + HORIZ_PAD_, TEXT_TOP, itemSizeV_.y, itemSizeV_.y);

                        sfml_util::CenterAndScaleSpriteToFit(sprite, IMAGE_RECT);

                        if (SelectedIndex() == i)
                        {
                            sprite.setColor(imageColor_ + sf::Color(100, 100, 100, 100));
                        }
                        else
                        {
                            sprite.setColor(imageColor_);
                        }
                    }

                    if (SELECTION_INDEX == i)
                    {
                        sfml_util::DrawRectangle<float>(
                            target, states, ITEM_RECT, sf::Color::Transparent, 0, highlightColor_);
                    }

                    target.draw(*itemPtr, states);

                    if (itemPtr->IsValid() == false)
                    {
                        sfml_util::DrawRectangle<float>(
                            target,
                            states,
                            ITEM_RECT,
                            sf::Color::Transparent,
                            0,
                            INVALID_ITEM_HIGHLIGHT_COLOR_);
                    }

                    auto const LINE_POS_TOP{ ITEM_RECT.top + ITEM_RECT.height
                                             + (betweenPadVert_ * 0.5f) };

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
            auto linePosVert{ lastDrawnLinePosVert + (itemSizeV_.y + betweenPadVert_) };
            while (linePosVert < LIST_BOTTOM)
            {
                DrawLine(target, linePosVert);
                linePosVert += (itemSizeV_.y + betweenPadVert_);
            }
        }

        ListBoxItemPtrOpt_t ListBox::AtPos(const sf::Vector2f & POS_V)
        {
            for (auto const & NEXT_ITEM_UPTR : items_)
            {
                if (NEXT_ITEM_UPTR->GetEntityWillDraw()
                    && IsPosWithinItemRegion(POS_V, NEXT_ITEM_UPTR.get()))
                {
                    return ListBoxItemPtr_t(NEXT_ITEM_UPTR.get());
                }
            }

            return boost::none;
        }

        const ListBoxItemPtrOpt_t ListBox::Selected() const
        {
            auto const INDEX{ SelectedIndex() };
            if (INDEX < items_.size())
            {
                return ListBoxItemPtr_t(items_[INDEX].get());
            }
            else
            {
                return boost::none;
            }
        }

        void ListBox::DisplayIndex(const std::size_t NEW_INDEX)
        {
            if (IsIndexValid(NEW_INDEX) && (DisplayIndex() != NEW_INDEX))
            {
                if (NEW_INDEX < displayIndex_)
                {
                    displayIndex_ = NEW_INDEX;
                }
                else
                {
                    auto const GREATEST_FIRST_INDEX{ CalcGreatestFirstDisplayedIndex() };
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

        void ListBox::SelectedIndex(const std::size_t NEW_INDEX)
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
                            auto const GREATEST_FIRST_INDEX{ CalcGreatestFirstDisplayedIndex() };
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

        ListBoxItemPtr_t ListBox::At(const std::size_t INDEX)
        {
            M_ASSERT_OR_LOGANDTHROW_SS(
                (INDEX < items_.size()),
                "sfml_util::gui::ListBox::At("
                    << INDEX << ")(listbox_name=" << GetEntityName()
                    << ") but that index is out of bounds.  Actual size is " << items_.size() - 1
                    << ".");

            return ListBoxItemPtr_t(items_[INDEX].get());
        }

        void ListBox::Add(ListBoxItemUPtr_t itemUPtr)
        {
            items_.emplace_back(std::move(itemUPtr));

            if (items_.size() == 1)
            {
                ResetItemSize();
            }
        }

        bool ListBox::Remove(const ListBoxItemPtr_t ITEM_PTR)
        {
            auto const FOUND_ITR{ std::find_if(
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

        bool ListBox::RemoveSelected()
        {
            auto const SELECTED_PTR_OPT{ Selected() };

            if (SELECTED_PTR_OPT)
            {
                return Remove(SELECTED_PTR_OPT.value());
            }
            else
            {
                return false;
            }
        }

        void ListBox::Clear() { items_.clear(); }

        std::size_t ListBox::Size() const { return items_.size(); }

        bool ListBox::Empty() const { return items_.empty(); }

        bool ListBox::MouseUp(const sf::Vector2f & MOUSE_POS_V)
        {
            auto const DID_STATE_CHANGE{ GuiEntity::MouseUp(MOUSE_POS_V) };

            auto const ORIG_SELECTED_ITEM_PTR_OPT{ Selected() };

            auto const NUM_ITEMS{ items_.size() };
            for (std::size_t i(displayIndex_); i < NUM_ITEMS; ++i)
            {
                auto const ITEM_PTR{ ListBoxItemPtr_t(items_[i].get()) };

                if (ITEM_PTR->GetEntityWillDraw() == false)
                {
                    break;
                }

                if (IsPosWithinItemRegion(MOUSE_POS_V, ITEM_PTR)
                    && (ORIG_SELECTED_ITEM_PTR_OPT != ITEM_PTR))
                {
                    SelectedIndex(i);

                    if (willPlaySfx_)
                    {
                        SoundManager::Instance()->PlaySfx_MouseClick();
                    }

                    boxUPtr_->FakeColorSetAsIfFocusIs(true);

                    if (callbackPtrOpt_)
                    {
                        const callback::ListBoxPtrPackage_t PTR_PACKAGE{ this };

                        const callback::ListBoxEventPackage PACKAGE(
                            PTR_PACKAGE, sfml_util::GuiEvent::Click, MOUSE_POS_V);

                        callbackPtrOpt_->Obj().HandleCallback(PACKAGE);
                    }

                    return true;
                }
            }

            boxUPtr_->FakeColorSetAsIfFocusIs(DID_STATE_CHANGE);
            return DID_STATE_CHANGE;
        }

        bool ListBox::KeyRelease(const sf::Event::KeyEvent & KEY_EVENT)
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
                auto const DID_SELECTION_CHANGE{ SelectPrev() };
                PlaySfx();
                CreateKeypressPackageAndCallHandler(KEY_EVENT);
                return DID_SELECTION_CHANGE;
            }
            else if (KEY_EVENT.code == sf::Keyboard::Down)
            {
                auto const DID_SELECTION_CHANGE{ SelectNext() };
                PlaySfx();
                CreateKeypressPackageAndCallHandler(KEY_EVENT);
                return DID_SELECTION_CHANGE;
            }

            CreateKeypressPackageAndCallHandler(KEY_EVENT);
            return false;
        }

        void ListBox::SetEntityPos(const float POS_LEFT, const float POS_TOP)
        {
            auto const DIFF_HORIZ{ POS_LEFT - GetEntityPos().x };
            auto const DIFF_VERT{ POS_TOP - GetEntityPos().y };

            for (auto & itemSPtr : items_)
            {
                itemSPtr->MoveEntityPos(DIFF_HORIZ, DIFF_VERT);
            }

            if (boxUPtr_)
            {
                boxUPtr_->MoveEntityPos(DIFF_HORIZ, DIFF_VERT);
            }

            GuiEntity::SetEntityPos(POS_LEFT, POS_TOP);
        }

        bool ListBox::SelectPrev()
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

        bool ListBox::SelectNext()
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

        const sf::FloatRect ListBox::FullItemRect(const ListBoxItemPtr_t ITEM_PTR) const
        {
            sf::FloatRect rect(ITEM_PTR->GetEntityRegion());

            rect.left -= HORIZ_PAD_;

            if (ITEM_PTR->HasImage())
            {
                rect.left -= (itemSizeV_.y + HORIZ_PAD_);
            }

            rect.top = ITEM_PTR->GetEntityRegion().top
                + ((itemSizeV_.y - ITEM_PTR->GetEntityRegion().height) * 0.5f);

            rect.width = itemSizeV_.x;
            rect.height = itemSizeV_.y;

            return rect;
        }

        void ListBox::MoveItemToOtherListBox(
            const std::size_t ITEM_TO_MOVE_INDEX, ListBox & otherListBox)
        {
            if (ITEM_TO_MOVE_INDEX < items_.size())
            {
                ListBoxItemPtr_t itemToMovePtr{ items_.at(ITEM_TO_MOVE_INDEX).get() };

                ListBoxItemFactory listBoxItemFactory;

                otherListBox.Add(
                    listBoxItemFactory.MakeCopy(otherListBox.GetEntityName(), itemToMovePtr));

                Remove(itemToMovePtr);
            }
        }

        std::size_t ListBox::CalcGreatestFirstDisplayedIndex() const
        {
            return ((items_.size() > visibleCount_) ? (items_.size() - visibleCount_) : 0);
        }

        void ListBox::CreateKeypressPackageAndCallHandler(const sf::Event::KeyEvent & KEY_EVENT)
        {
            if (callbackPtrOpt_)
            {
                const callback::ListBoxPtrPackage_t PTR_PACKAGE(this);

                const callback::ListBoxEventPackage PACKAGE(
                    PTR_PACKAGE,
                    sfml_util::GuiEvent::Keypress,
                    callback::ListBoxEventPackage::INVALID_MOUSE_POS_V_,
                    callback::ListBoxEventPackage::INVALID_SELECTION_,
                    false,
                    KEY_EVENT);

                callbackPtrOpt_->Obj().HandleCallback(PACKAGE);
            }
        }

        void ListBox::OnDoubleClick(const sf::Vector2f & MOUSE_POS_V)
        {
            if (callbackPtrOpt_)
            {
                const callback::ListBoxPtrPackage_t PTR_PACKAGE(this);

                const callback::ListBoxEventPackage PACKAGE(
                    PTR_PACKAGE, sfml_util::GuiEvent::DoubleClick, MOUSE_POS_V);

                callbackPtrOpt_->Obj().HandleCallback(PACKAGE);
            }
        }

        std::size_t ListBox::CalcVisibleItems() const
        {
            std::size_t count{ 0 };
            auto posVert{ entityRegion_.top };
            auto const LIST_BOTTOM{ (entityRegion_.top + entityRegion_.height) };

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
                    posVert += betweenPadVert_;
                }
            } while (posVert < LIST_BOTTOM);

            return count;
        }

        std::size_t ListBox::CalcLastVisibleIndex_Display() const
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

        std::size_t ListBox::CalcLastVisibleIndex_Selection() const
        {
            if (Empty())
            {
                return 0;
            }
            else
            {
                return std::min((selectionDisplayIndex_ + visibleCount_) - 1, items_.size() - 1);
            }
        }

        void ListBox::PlaySfx() const
        {
            if (willPlaySfx_)
            {
                SoundManager::Instance()->PlaySfx_Keypress();
            }
        }

        void ListBox::DrawLine(sf::RenderTarget & target, const float POS_TOP) const
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

        bool ListBox::IsPosWithinItemRegion(
            const sf::Vector2f & POS_V, const ListBoxItemPtr_t ITEM_PTR) const
        {
            return FullItemRect(ITEM_PTR).contains(POS_V);
        }

        void ListBox::ResetItemSize()
        {
            itemSizeV_.x = GetEntityRegion().width;

            if (items_.empty())
            {
                itemSizeV_.y = sfml_util::ScreenRatioToPixelsVert(0.04f);
            }
            else
            {
                // find and use the tallest item
                auto tallestItemHeight{ items_.front()->GetEntityRegion().height };

                for (auto const & LISTBOX_ITEM_UPTR : items_)
                {
                    auto const ITEM_HEIGHT{ LISTBOX_ITEM_UPTR->GetEntityRegion().height };

                    if (ITEM_HEIGHT > tallestItemHeight)
                    {
                        tallestItemHeight = ITEM_HEIGHT;
                    }
                }

                itemSizeV_.y = tallestItemHeight;
            }
        }

    } // namespace gui
} // namespace sfml_util
} // namespace heroespath
