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

        const sf::Color ListBox::INVALID_ITEM_HIGHLIGHT_COLOR_{ 127, 32, 32, 64 };

        ListBox::ListBox(
            const std::string & NAME,
            const sf::FloatRect & REGION,
            const ListBoxItemSVec_t & ITEM_VEC,
            const IStagePtrOpt_t ISTAGE_PTR_OPT,
            const float BETWEEN_PAD,
            const box::Info & BOX_INFO,
            const sf::Color & LINE_COLOR,
            const callback::IListBoxCallbackHandlerPtrOpt_t CALLBACK_PTR_OPT)
            : GuiEntity(std::string(NAME).append("_ListBox"), REGION)
            , HORIZ_PAD_(sfml_util::ScreenRatioToPixelsHoriz(0.007f))
            , itemSizeV_(REGION.width, sfml_util::ScreenRatioToPixelsVert(0.04f))
            , betweenPadVert_(BETWEEN_PAD)
            , boxUPtr_()
            , sliderbarUPtr_()
            , lineColor_(LINE_COLOR)
            , highlightColor_(BOX_INFO.bg_info.color + sf::Color(20, 20, 20, 20))
            , stagePtrOpt_(ISTAGE_PTR_OPT)
            , items_(ITEM_VEC)
            , imageColor_(sf::Color::White)
            , willPlaySfx_(true)
            , callbackPtrOpt_(CALLBACK_PTR_OPT)
            , displayIndex_(0)
            , selectionDisplayIndex_(0)
            , selectionOffsetIndex_(0)
            , visibleCount_(0)
        {
            Setup(
                REGION,
                ITEM_VEC,
                ISTAGE_PTR_OPT,
                BETWEEN_PAD,
                BOX_INFO,
                LINE_COLOR,
                callbackPtrOpt_);
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
            const ListBoxItemSVec_t & ITEM_VEC,
            const IStagePtrOpt_t ISTAGE_PTR_OPT,
            const float BETWEEN_PAD,
            const box::Info & BOX_INFO,
            const sf::Color & LINE_COLOR,
            const callback::IListBoxCallbackHandlerPtrOpt_t CALLBACK_PTR_OPT)
        {
            SetEntityRegion(REGION);

            ResetItemHeight();

            items_.clear();
            items_ = ITEM_VEC;
            stagePtrOpt_ = ISTAGE_PTR_OPT;
            betweenPadVert_ = BETWEEN_PAD;
            lineColor_ = LINE_COLOR;
            callbackPtrOpt_ = CALLBACK_PTR_OPT;

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
            SetupForDraw();
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

            auto lastDrawnListItemTop{ entityRegion_.top };
            auto const LIST_BOTTOM{ entityRegion_.top + entityRegion_.height };
            auto const SELECTION_INDEX{ SelectedIndex() };

            if (Empty() == false)
            {
                auto const LAST_INDEX_TO_DRAW{ std::min(
                    CalcLastVisibleIndex_Display(), items_.size() - 1) };

                for (std::size_t i(displayIndex_); i <= LAST_INDEX_TO_DRAW; ++i)
                {
                    auto const ITEM_SPTR{ items_.at(i) };
                    auto const ITEM_RECT{ FullItemRect(ITEM_SPTR) };

                    lastDrawnListItemTop = ITEM_RECT.top;

                    if (SELECTION_INDEX == i)
                    {
                        sfml_util::DrawRectangle<float>(
                            target, states, ITEM_RECT, sf::Color::Transparent, 0, highlightColor_);
                    }

                    target.draw(*ITEM_SPTR, states);

                    if (ITEM_SPTR->IsValid() == false)
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
                    }
                    else
                    {
                        break;
                    }
                }
            }

            // draw lines in the rest of the box even though there are no more items
            lastDrawnListItemTop += (itemSizeV_.y + betweenPadVert_);
            while (lastDrawnListItemTop < LIST_BOTTOM)
            {
                DrawLine(target, lastDrawnListItemTop);
                lastDrawnListItemTop += (itemSizeV_.y + betweenPadVert_);
            }
        }

        ListBoxItemSPtr_t ListBox::AtPos(const sf::Vector2f & POS_V)
        {
            for (auto const & NEXT_ITEM_SPTR : items_)
            {
                if (NEXT_ITEM_SPTR->GetEntityWillDraw()
                    && IsPosWithinItemRegion(POS_V, NEXT_ITEM_SPTR))
                {
                    return NEXT_ITEM_SPTR;
                }
            }

            return ListBoxItemSPtr_t();
        }

        const ListBoxItemSPtr_t ListBox::Selected() const
        {
            auto const INDEX{ SelectedIndex() };
            if (INDEX < items_.size())
            {
                return items_[INDEX];
            }
            else
            {
                return ListBoxItemSPtr_t();
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

                SetupForDraw();
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
            SetupForDraw();
        }

        void ListBox::Add(const ListBoxItemSPtr_t & ITEM_SPTR)
        {
            items_.emplace_back(ITEM_SPTR);

            if (items_.size() == 1)
            {
                ResetItemHeight();
            }

            SetupForDraw();
        }

        bool ListBox::Remove(const ListBoxItemSPtr_t & ITEM_SPTR)
        {
            auto const FOUND_ITR{ std::find(items_.begin(), items_.end(), ITEM_SPTR) };

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

                SetupForDraw();
                return true;
            }
        }

        bool ListBox::MouseUp(const sf::Vector2f & MOUSE_POS_V)
        {
            auto const DID_STATE_CHANGE{ GuiEntity::MouseUp(MOUSE_POS_V) };

            auto const ORIG_SELECTED_ITEM_SPTR{ Selected() };

            auto const NUM_ITEMS{ items_.size() };
            for (std::size_t i(displayIndex_); i < NUM_ITEMS; ++i)
            {
                auto const ITEM_SPTR{ items_[i] };

                if (ITEM_SPTR->GetEntityWillDraw() == false)
                {
                    break;
                }

                if (IsPosWithinItemRegion(MOUSE_POS_V, ITEM_SPTR)
                    && (ORIG_SELECTED_ITEM_SPTR != ITEM_SPTR))
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

                        SetupForDraw();
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

        void ListBox::Items(const ListBoxItemSVec_t & NEW_ITEMS_VEC)
        {
            items_ = NEW_ITEMS_VEC;
            DisplayIndex(0);
            SelectedIndex(0);
            sliderbarUPtr_->SetCurrentValue(0.0f);
            SetupForDraw();
        }

        bool ListBox::SelectPrev()
        {
            if (selectionOffsetIndex_ > 0)
            {
                --selectionOffsetIndex_;
                SetupForDraw();
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

                    SetupForDraw();
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

        const sf::FloatRect ListBox::FullItemRect(const ListBoxItemSPtr_t & ITEM_SPTR) const
        {
            sf::FloatRect rect(ITEM_SPTR->GetEntityRegion());

            rect.left -= HORIZ_PAD_;

            if (ITEM_SPTR->HasImage())
            {
                rect.left -= (itemSizeV_.y + HORIZ_PAD_);
            }

            rect.top = ITEM_SPTR->GetEntityRegion().top
                + ((itemSizeV_.y - ITEM_SPTR->GetEntityRegion().height) * 0.5f);

            rect.width = itemSizeV_.x;
            rect.height = itemSizeV_.y;

            return rect;
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

                SetupForDraw();
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

                SetupForDraw();
            }
        }

        void ListBox::SetupForDraw()
        {
            std::size_t index{ 0 };
            for (auto & itemSPtr : items_)
            {
                if (IsIndexVisible_Display(index))
                {
                    itemSPtr->SetEntityWillDraw(true);

                    auto const TEXT_LEFT{ entityRegion_.left + HORIZ_PAD_ };

                    auto const TEXT_TOP{ entityRegion_.top
                                         + (static_cast<float>(index)
                                            * (itemSizeV_.y + betweenPadVert_)) };

                    itemSPtr->SetEntityPos(TEXT_LEFT, TEXT_TOP);

                    if (itemSPtr->HasImage())
                    {
                        itemSPtr->MoveEntityPos(itemSizeV_.y + HORIZ_PAD_, 0.0f);

                        auto & sprite{ itemSPtr->Sprite() };

                        const sf::FloatRect IMAGE_RECT(
                            entityRegion_.left + HORIZ_PAD_, TEXT_TOP, itemSizeV_.y, itemSizeV_.y);

                        sfml_util::CenterAndScaleSpriteToFit(sprite, IMAGE_RECT);

                        if (SelectedIndex() == index)
                        {
                            sprite.setColor(imageColor_ + sf::Color(100, 100, 100, 100));
                        }
                        else
                        {
                            sprite.setColor(imageColor_);
                        }
                    }
                }
                else
                {
                    itemSPtr->SetEntityWillDraw(false);
                }

                ++index;
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
            const sf::Vector2f & POS_V, const ListBoxItemSPtr_t & ITEM_SPTR) const
        {
            return FullItemRect(ITEM_SPTR).contains(POS_V);
        }

        void ListBox::ResetItemHeight()
        {
            itemSizeV_.y
                = ((items_.empty()) ? sfml_util::ScreenRatioToPixelsVert(0.04f)
                                    : items_.at(0)->GetEntityRegion().height);
        }

    } // namespace gui
} // namespace sfml_util
} // namespace heroespath
