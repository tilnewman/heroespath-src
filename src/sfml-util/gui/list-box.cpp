// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
///////////////////////////////////////////////////////////////////////////////
//
// Heroes' Path - Open-source, non-commercial, simple, game in the RPG style.
// Copyright (C) 2017 Ziesche Til Newman (tilnewman@gmail.com)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from
// the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
//  1. The origin of this software must not be misrepresented; you must not
//     claim that you wrote the original software.  If you use this software
//     in a product, an acknowledgment in the product documentation would be
//     appreciated but is not required.
//
//  2. Altered source versions must be plainly marked as such, and must not
//     be misrepresented as being the original software.
//
//  3. This notice may not be removed or altered from any source distribution.
//
///////////////////////////////////////////////////////////////////////////////
//
// list-box.cpp
//
#include "list-box.hpp"

#include "sfml-util/gui/box.hpp"
#include "sfml-util/gui/condition-image-manager.hpp"
#include "sfml-util/gui/creature-image-manager.hpp"
#include "sfml-util/gui/item-image-manager.hpp"
#include "sfml-util/gui/list-box-item.hpp"
#include "sfml-util/gui/song-image-manager.hpp"
#include "sfml-util/gui/spell-image-manager.hpp"
#include "sfml-util/gui/title-image-manager.hpp"
#include "sfml-util/i-stage.hpp"
#include "sfml-util/sfml-util.hpp"
#include "sfml-util/sound-manager.hpp"

#include "creature/condition.hpp"
#include "creature/title.hpp"
#include "item/item.hpp"
#include "log/log-macros.hpp"
#include "player/character.hpp"
#include "song/song.hpp"
#include "spell/spell.hpp"

#include <vector>

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

        const sf::FloatRect ListBox::ERROR_RECT_(-1.0f, -1.0f, -1.0f, -1.0f);

        ListBox::ListBox(
            const std::string & NAME,
            const sf::FloatRect & REGION,
            const ListBoxItemSVec_t & ITEM_VEC,
            IStage * const stagePtr,
            const float MARGIN,
            const float BETWEEN_PAD,
            const box::Info & BOX_INFO,
            const sf::Color & LINE_COLOR,
            callback::IListBoxCallbackHandler * const callbackPtr)
            : GuiEntity(std::string(NAME).append("_ListBox"), REGION)
            , IMAGE_HORIZ_PAD_(10.0f)
            , IMAGE_SIZE_(sfml_util::MapByRes(50.0f, 150.0f))
            , boxUPtr_()
            , sliderbarUPtr_()
            , lineColor_(LINE_COLOR)
            , highlightColor_(BOX_INFO.bg_info.color + sf::Color(20, 20, 20, 20))
            , margin_(MARGIN)
            , betweenPad_(BETWEEN_PAD)
            , stagePtr_(stagePtr)
            , items_(ITEM_VEC)
            , imageMap_()
            , imageColor_(sf::Color::White)
            , willPlaySfx_(true)
            , callbackPtr_(callbackPtr)
            , displayIndex_(0)
            , selectionDisplayIndex_(0)
            , selectionOffsetIndex_(0)
            , visibleCount_(0)
            , countLimit_(0)
        {
            Setup(
                REGION, ITEM_VEC, stagePtr, MARGIN, BETWEEN_PAD, BOX_INFO, LINE_COLOR, callbackPtr);
        }

        ListBox::~ListBox()
        {
            if (stagePtr_ != nullptr)
            {
                if (boxUPtr_.get() != nullptr)
                {
                    stagePtr_->EntityRemove(boxUPtr_.get());
                }

                if (sliderbarUPtr_.get() != nullptr)
                {
                    stagePtr_->EntityRemove(sliderbarUPtr_.get());
                }
            }
        }

        void ListBox::Setup(
            const sf::FloatRect & REGION,
            const ListBoxItemSVec_t & ITEM_VEC,
            IStage * const stagePtr,
            const float MARGIN,
            const float BETWEEN_PAD,
            const box::Info & BOX_INFO,
            const sf::Color & LINE_COLOR,
            callback::IListBoxCallbackHandler * const callbackPtr)
        {
            SetEntityRegion(REGION);

            items_.clear();
            items_ = ITEM_VEC;
            stagePtr_ = stagePtr;
            margin_ = MARGIN;
            betweenPad_ = BETWEEN_PAD;
            lineColor_ = LINE_COLOR;
            callbackPtr_ = callbackPtr;

            if (boxUPtr_.get() == nullptr)
            {
                boxUPtr_ = std::make_unique<sfml_util::gui::box::Box>("ListBox's", BOX_INFO);
                stagePtr->EntityAdd(boxUPtr_.get());
            }
            else
            {
                boxUPtr_->SetupBox(BOX_INFO);
            }

            boxUPtr_->SetWillAcceptFocus(false);

            if (sliderbarUPtr_.get() == nullptr)
            {
                sliderbarUPtr_ = std::make_unique<sfml_util::gui::SliderBar>(
                    "ListBox's",
                    REGION.left + REGION.width + 10.0f,
                    REGION.top + 10.0f,
                    REGION.height - 20.0f,
                    sfml_util::gui::SliderStyle(),
                    this);

                stagePtr->EntityAdd(sliderbarUPtr_.get());
            }
            else
            {
                sliderbarUPtr_->SetCurrentValue(0.0f);
            }

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

            auto lastDrawnListPosY{ entityRegion_.top };
            auto lastDrawnEntityHeight{ IMAGE_SIZE_ };
            auto const POS_BOTTOM{ entityRegion_.top + entityRegion_.height };
            auto const SELECTION_INDEX{ SelectedIndex() };
            auto const LAST_DRAWN_INDEX{ CalcLastVisibleIndex_Display() };

            std::size_t drawnCount{ 0 };
            if (Empty() == false)
            {
                for (std::size_t i(displayIndex_); i <= LAST_DRAWN_INDEX; ++i)
                {
                    ++drawnCount;

                    auto const ITEM_SPTR{ items_.at(i) };

                    ImagePair_t imagePair;
                    auto const DID_FIND_IMAGE{ imageMap_.Find(ITEM_SPTR, imagePair) };

                    // highlight invalid entries
                    if (ITEM_SPTR->is_valid == false)
                    {
                        sf::FloatRect rect(ITEM_SPTR->GetEntityRegion());
                        rect.left -= margin_;
                        rect.width = entityRegion_.width;

                        if (DID_FIND_IMAGE)
                        {
                            rect.width -= IMAGE_SIZE_ + IMAGE_HORIZ_PAD_;
                        }

                        sfml_util::DrawRectangle<float>(
                            target,
                            states,
                            rect,
                            sf::Color::Transparent,
                            0,
                            sf::Color(127, 32, 32, 64));
                    }

                    // highlight the current selection
                    if (SELECTION_INDEX == i)
                    {
                        sf::FloatRect rect(ITEM_SPTR->GetEntityRegion());
                        rect.left -= margin_;
                        rect.width = entityRegion_.width;

                        if (DID_FIND_IMAGE)
                        {
                            rect.width -= IMAGE_SIZE_ + IMAGE_HORIZ_PAD_;
                        }

                        sfml_util::DrawRectangle<float>(
                            target, states, rect, sf::Color::Transparent, 0, highlightColor_);
                    }

                    if (DID_FIND_IMAGE)
                    {
                        target.draw(imagePair.second, states);
                    }

                    target.draw(*ITEM_SPTR, states);

                    lastDrawnEntityHeight = ITEM_SPTR->GetEntityRegion().height;

                    // draw between line
                    auto const LINE_POS_TOP{ ITEM_SPTR->GetEntityRegion().top
                                             + ITEM_SPTR->GetEntityRegion().height
                                             + (betweenPad_ * 0.5f) };

                    if (LINE_POS_TOP < POS_BOTTOM)
                    {
                        DrawLine(target, LINE_POS_TOP);
                        lastDrawnListPosY = LINE_POS_TOP + lastDrawnEntityHeight + betweenPad_;
                    }
                    else
                    {
                        break;
                    }
                }
            }

            if (drawnCount < countLimit_)
            {
                auto const WERE_ANY_DRAWN{ (drawnCount != 0) };

                // draw lines in the rest of the box even though there are no more entitys
                while (lastDrawnListPosY < POS_BOTTOM)
                {
                    DrawLine(target, lastDrawnListPosY);
                    lastDrawnListPosY += lastDrawnEntityHeight;

                    ++drawnCount;

                    if ((WERE_ANY_DRAWN && (drawnCount >= countLimit_))
                        || ((WERE_ANY_DRAWN == false) && (drawnCount > countLimit_)))
                    {
                        break;
                    }
                }
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

        bool ListBox::Add(const ListBoxItemSPtr_t & ITEM_SPTR)
        {
            if ((0 == countLimit_) || (items_.size() < countLimit_))
            {
                items_.emplace_back(ITEM_SPTR);
                SetupForDraw();
                return true;
            }
            else
            {
                return false;
            }
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
            for (std::size_t i(0); i < NUM_ITEMS; ++i)
            {
                auto const ITEM_SPTR{ items_[i] };

                if (ITEM_SPTR->GetEntityWillDraw() && IsPosWithinItemRegion(MOUSE_POS_V, ITEM_SPTR)
                    && (ORIG_SELECTED_ITEM_SPTR != ITEM_SPTR))
                {
                    SelectedIndex(i);

                    if (willPlaySfx_)
                    {
                        SoundManager::Instance()->PlaySfx_MouseClick();
                    }

                    boxUPtr_->FakeColorSetAsIfFocusIs(true);

                    if (callbackPtr_ != nullptr)
                    {
                        const callback::ListBoxPtrPackage_t PTR_PACKAGE{ this };

                        const callback::ListBoxEventPackage PACKAGE(
                            PTR_PACKAGE, sfml_util::GuiEvent::Click, MOUSE_POS_V);

                        callbackPtr_->HandleCallback(PACKAGE);

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

        const sf::FloatRect ListBox::ImageRectOfItemAtPos(const sf::Vector2f & MOUSE_POS_V)
        {
            auto const ITEM_SPTR{ AtPos(MOUSE_POS_V) };

            if (ITEM_SPTR.get() == nullptr)
            {
                return ERROR_RECT_;
            }

            ImagePair_t imagePair;
            auto const DID_FIND_IMAGE{ imageMap_.Find(ITEM_SPTR, imagePair) };
            if (DID_FIND_IMAGE)
            {
                return imagePair.second.getGlobalBounds();
            }
            else
            {
                return ITEM_SPTR->GetEntityRegion();
            }
        }

        void ListBox::SetEntityPos(const float POS_LEFT, const float POS_TOP)
        {
            auto const DIFF_HORIZ{ POS_LEFT - GetEntityPos().x };
            auto const DIFF_VERT{ POS_TOP - GetEntityPos().y };

            for (auto & itemSPtr : items_)
            {
                itemSPtr->MoveEntityPos(DIFF_HORIZ, DIFF_VERT);
            }

            for (auto & NEXT_IMAGE_PAIR : imageMap_)
            {
                NEXT_IMAGE_PAIR.second.second.move(DIFF_HORIZ, DIFF_VERT);
            }

            if (boxUPtr_.get() != nullptr)
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

        std::size_t ListBox::CalcGreatestFirstDisplayedIndex() const
        {
            return ((items_.size() > visibleCount_) ? (items_.size() - visibleCount_) : 0);
        }

        void ListBox::CreateKeypressPackageAndCallHandler(const sf::Event::KeyEvent & KEY_EVENT)
        {
            if (callbackPtr_ != nullptr)
            {
                const callback::ListBoxPtrPackage_t PTR_PACKAGE(this);

                const callback::ListBoxEventPackage PACKAGE(
                    PTR_PACKAGE,
                    sfml_util::GuiEvent::Keypress,
                    callback::ListBoxEventPackage::INVALID_MOUSE_POS_V_,
                    callback::ListBoxEventPackage::INVALID_SELECTION_,
                    false,
                    KEY_EVENT);

                callbackPtr_->HandleCallback(PACKAGE);

                SetupForDraw();
            }
        }

        void ListBox::OnDoubleClick(const sf::Vector2f & MOUSE_POS_V)
        {
            if (callbackPtr_ != nullptr)
            {
                const callback::ListBoxPtrPackage_t PTR_PACKAGE(this);

                const callback::ListBoxEventPackage PACKAGE(
                    PTR_PACKAGE, sfml_util::GuiEvent::DoubleClick, MOUSE_POS_V);

                callbackPtr_->HandleCallback(PACKAGE);

                SetupForDraw();
            }
        }

        void ListBox::SetupForDraw()
        {
            auto posTop{ entityRegion_.top + margin_ };

            auto const NUM_ITEMS{ items_.size() };
            for (std::size_t i(0); i < NUM_ITEMS; ++i)
            {
                auto itemSPtr{ items_[i] };
                if (IsIndexVisible_Display(i))
                {
                    itemSPtr->SetEntityWillDraw(true);
                    LoadImageAndSetPosition(itemSPtr, posTop, (SelectedIndex() == i));
                    posTop += itemSPtr->GetEntityRegion().height + betweenPad_;
                }
                else
                {
                    itemSPtr->SetEntityWillDraw(false);
                }
            }
        }

        void ListBox::LoadImageAndSetPosition(
            sfml_util::gui::ListBoxItemSPtr_t & listBoxItemSPtr,
            const float POS_TOP,
            const bool IS_SELECTED_ITEM)
        {
            ImagePair_t imagePair;

            auto const DOES_IMAGE_EXIST{ (
                (listBoxItemSPtr->ITEM_CPTR != nullptr) || (listBoxItemSPtr->TITLE_PTR_OPT)
                || (listBoxItemSPtr->CHARACTER_CPTR != nullptr)
                || (listBoxItemSPtr->SPELL_CPTRC != nullptr)
                || (listBoxItemSPtr->COND_CPTRC != nullptr)
                || (listBoxItemSPtr->SONG_CPTRC != nullptr)) };

            if (DOES_IMAGE_EXIST)
            {
                imageMap_.Find(listBoxItemSPtr, imagePair);

                if (imagePair.first.get() == nullptr)
                {
                    imagePair.first = std::make_shared<sf::Texture>();

                    if (listBoxItemSPtr->ITEM_CPTR != nullptr)
                    {
                        sfml_util::gui::ItemImageManager::Instance()->Load(
                            *imagePair.first, listBoxItemSPtr->ITEM_CPTR);
                    }
                    else if (listBoxItemSPtr->TITLE_PTR_OPT)
                    {
                        sfml_util::gui::TitleImageManager::Instance()->Get(
                            *imagePair.first, listBoxItemSPtr->TITLE_PTR_OPT->Obj().Which());
                    }
                    else if (listBoxItemSPtr->CHARACTER_CPTR != nullptr)
                    {
                        sfml_util::gui::CreatureImageManager::Instance()->GetImage(
                            *imagePair.first,
                            listBoxItemSPtr->CHARACTER_CPTR->ImageFilename(),
                            true);
                    }
                    else if (listBoxItemSPtr->SPELL_CPTRC != nullptr)
                    {
                        sfml_util::gui::SpellImageManager::Instance()->Get(
                            *imagePair.first, listBoxItemSPtr->SPELL_CPTRC->Which());
                    }
                    else if (listBoxItemSPtr->COND_CPTRC != nullptr)
                    {
                        sfml_util::gui::ConditionImageManager::Instance()->Get(
                            *imagePair.first, listBoxItemSPtr->COND_CPTRC->Which());
                    }
                    else if (listBoxItemSPtr->SONG_CPTRC != nullptr)
                    {
                        sfml_util::gui::SongImageManager::Instance()->Get(
                            *imagePair.first, listBoxItemSPtr->SONG_CPTRC->Which());
                    }
                }
            }

            auto const ITEM_POS_LEFT{ entityRegion_.left + margin_ };
            auto newPosLeft{ ITEM_POS_LEFT };

            if (imagePair.first.get() != nullptr)
            {
                sf::Sprite & sprite{ imagePair.second };

                sprite.setTexture(*imagePair.first, true);

                if (IS_SELECTED_ITEM)
                {
                    sprite.setColor(sf::Color::White);
                }
                else
                {
                    sprite.setColor(imageColor_);
                }

                if (sprite.getLocalBounds().width > sprite.getLocalBounds().height)
                {
                    const float SCALE(IMAGE_SIZE_ / sprite.getLocalBounds().width);
                    sprite.setScale(SCALE, SCALE);
                }
                else
                {
                    const float SCALE(IMAGE_SIZE_ / sprite.getLocalBounds().height);
                    sprite.setScale(SCALE, SCALE);
                }

                auto const SPRITE_POS_LEFT{ (ITEM_POS_LEFT + (IMAGE_SIZE_ * 0.5f))
                                            - (sprite.getGlobalBounds().width * 0.5f) };

                auto const SPRITE_POS_TOP{ (POS_TOP + (IMAGE_SIZE_ * 0.5f))
                                           - (sprite.getGlobalBounds().height * 0.5f) };

                sprite.setPosition(SPRITE_POS_LEFT, SPRITE_POS_TOP);

                auto const IMAGE_AREA_WIDTH{ IMAGE_SIZE_ + IMAGE_HORIZ_PAD_ };
                newPosLeft += IMAGE_AREA_WIDTH;

                imageMap_[listBoxItemSPtr] = imagePair;

                sf::FloatRect r(listBoxItemSPtr->GetEntityRegion());
                r.width -= IMAGE_AREA_WIDTH;
                r.height = IMAGE_SIZE_;
                listBoxItemSPtr->SetEntityRegion(r);
            }

            listBoxItemSPtr->SetEntityPos(newPosLeft, POS_TOP);
        }

        std::size_t ListBox::CalcVisibleItems() const
        {
            std::size_t count{ 0 };
            auto posVert{ entityRegion_.top + margin_ };
            auto const POS_BOTTOM{ (entityRegion_.top + entityRegion_.height) };

            do
            {
                posVert += IMAGE_SIZE_;

                if (posVert > POS_BOTTOM)
                {
                    break;
                }
                else
                {
                    ++count;
                    posVert += betweenPad_;
                }
            } while (posVert < POS_BOTTOM);

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
            return (
                (POS_V.x >= entityRegion_.left)
                && (POS_V.x <= entityRegion_.left + entityRegion_.width - (2.0f * margin_))
                && (POS_V.y >= ITEM_SPTR->GetEntityRegion().top)
                && (POS_V.y
                    <= ITEM_SPTR->GetEntityRegion().top + ITEM_SPTR->GetEntityRegion().height));
        }
    } // namespace gui
} // namespace sfml_util
} // namespace heroespath
