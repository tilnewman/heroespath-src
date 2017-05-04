//
// list-box.cpp
//
#include "list-box.hpp"

#include "sfml-util/sfml-util.hpp"
#include "sfml-util/i-stage.hpp"
#include "sfml-util/sound-manager.hpp"
#include "sfml-util/static-sounds.hpp"
#include "sfml-util/i-stage.hpp"
#include "sfml-util/gui/box.hpp"
#include "sfml-util/gui/list-box-item.hpp"
#include "sfml-util/gui/creature-image-manager.hpp"
#include "sfml-util/gui/item-image-manager.hpp"
#include "sfml-util/gui/title-image-manager.hpp"
#include "sfml-util/gui/spell-image-manager.hpp"
#include "sfml-util/gui/condition-image-manager.hpp"

#include "game/log-macros.hpp"
#include "game/creature/title.hpp"
#include "game/player/character.hpp"
#include "game/creature/condition.hpp"


namespace sfml_util
{
namespace gui
{

    namespace callback
    {
        const int ListBoxEventPackage::INVALID_SELECTION_(-1);
        const sf::Vector2f ListBoxEventPackage::INVALID_MOUSE_POS_V_(-1.0f, -1.0f);
    }


    const int           ListBox::NO_LIMIT_(0);
    const sf::FloatRect ListBox::ERROR_RECT_(-1.0f, -1.0f, -1.0f, -1.0f);


    ListBox::ListBox(const std::string &       NAME,
                     const sf::FloatRect &     REGION,
                     const ListBoxItemSLst_t & ITEM_LIST,
                     IStage * const            stagePtr,
                     const float               MARGIN,
                     const float               BETWEEN_PAD,
                     const box::Info &         BOX_INFO,
                     const sf::Color &         LINE_COLOR,
                     const int                 ITEM_LIMIT,
                     callback::IListBoxCallbackHandler * const callbackPtr)
    :
        GuiEntity       (std::string(NAME).append("_ListBox"), REGION),
        IMAGE_HORIZ_PAD_(10.0f),
        boxSPtr_        (),
        sliderBarSPtr_  (),
        lineColor_      (LINE_COLOR),
        highlightColor_ (BOX_INFO.bg_info.color + sf::Color(20, 20, 20, 20)),
        currentViewPos_ (0.0f),
        margin_         (MARGIN),
        betweenPad_     (BETWEEN_PAD),
        stagePtr_       (stagePtr),
        list_           (ITEM_LIST),
        selectedSPtr_   (),
        itemLimit_      (ITEM_LIMIT),
        imageSize_      (sfml_util::MapByRes(50.0f, 150.0f)),
        imageMap_       (),
        imageColor_     (sf::Color::White),
        callbackPtr_    (callbackPtr)
    {
        Setup(REGION,
              ITEM_LIST,
              stagePtr,
              MARGIN,
              BETWEEN_PAD,
              BOX_INFO,
              LINE_COLOR,
              ITEM_LIMIT,
              callbackPtr);
    }


    ListBox::~ListBox()
    {}


    void ListBox::Setup(const sf::FloatRect &     REGION,
                        const ListBoxItemSLst_t & ITEM_LIST,
                        IStage * const            stagePtr,
                        const float               MARGIN,
                        const float               BETWEEN_PAD,
                        const box::Info &         BOX_INFO,
                        const sf::Color &         LINE_COLOR,
                        const int                 ITEM_LIMIT,
                        callback::IListBoxCallbackHandler * const callbackPtr)
    {
        SetEntityRegion(REGION);

        list_.clear();
        list_ = ITEM_LIST;
        stagePtr_ = stagePtr;
        margin_ = MARGIN;
        betweenPad_ = BETWEEN_PAD;
        lineColor_ = LINE_COLOR;
        currentViewPos_ = 0.0f;
        itemLimit_ = ITEM_LIMIT;
        callbackPtr_ = callbackPtr;

        if (boxSPtr_.get() == nullptr)
        {
            boxSPtr_.reset(new sfml_util::gui::box::Box("ListBox's", BOX_INFO));
            stagePtr->EntityAdd(boxSPtr_);
        }
        else
            boxSPtr_->SetupBox(BOX_INFO);

        boxSPtr_->SetWillAcceptFocus(false);

        if (sliderBarSPtr_.get() == nullptr)
        {
            sliderBarSPtr_.reset( new sfml_util::gui::SliderBar("ListBox's",
                                                                REGION.left + REGION.width + 10.0f,
                                                                REGION.top + 10.0f,
                                                                REGION.height - 20.0f,
                                                                sfml_util::gui::SliderStyle(),
                                                                this) );
            stagePtr->EntityAdd(sliderBarSPtr_);
        }
        else
            sliderBarSPtr_->SetCurrentValue(0.0f);

        //select the first entity in the list
        if (list_.empty() == false)
            selectedSPtr_ = * list_.begin();

        SetupList();
    }


    bool ListBox::HandleCallback(const callback::SliderBarCallbackPackage_t & PACKAGE)
    {
        if (list_.empty())
        {
            currentViewPos_ = 0.0f;
            selectedSPtr_.reset();
        }
        else
        {
            typename ListBoxItemSLst_t::const_iterator itr(list_.begin());
            for (; itr != list_.end(); ++itr)
            {
                typename ListBoxItemSLst_t::const_iterator tempItr(itr);
                ++tempItr;
                if (tempItr == list_.end())
                    break;
            }
            currentViewPos_ = (GetTotalHeight() - (*itr)->GetEntityRegion().height) * PACKAGE.PTR_->GetCurrentValue();
            SetupList();
        }

        return true;
    }


    void ListBox::DrawLine(sf::RenderTarget & target, const float POS_TOP) const
    {
        sf::Vertex line[] =
        {
            sf::Vertex(sf::Vector2f(entityRegion_.left, POS_TOP)),
            sf::Vertex(sf::Vector2f(entityRegion_.left + (entityRegion_.width * 0.5f), POS_TOP)),
            sf::Vertex(sf::Vector2f(entityRegion_.left + entityRegion_.width, POS_TOP)),
        };

        line[0].color = sf::Color::Transparent;
        line[1].color = lineColor_;
        line[2].color = sf::Color::Transparent;
        target.draw(line, 3, sf::LinesStrip);
    }


    void ListBox::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        //use the ListBox's entityWillDraw_ to control whether ListBox is responsible for drawing its entitys
        if (false == entityWillDraw_)
            return;

        typename ListBoxItemSLst_t::const_iterator itr(list_.begin());
        const std::size_t NUM_ENTITYS(list_.size());
        const float BOTTOM(entityRegion_.top + entityRegion_.height);
        float lastDrawnListPosY(entityRegion_.top);
        float lastDrawnEntityHeight(45.0f);
        int itemCount(0);

        for (std::size_t i(0); i<NUM_ENTITYS; ++i)
        {
            if (itr == list_.end())
                break;

            const ListBoxItemSPtr_t nextEntitySPtr( * itr );
            const ImageMapCIter_t IMAGE_MAP_CITER(imageMap_.find(nextEntitySPtr));

            //highlight the current selection
            const bool IS_HIGHLIGHTED((selectedSPtr_.get() != nullptr) &&
                                      (nextEntitySPtr == selectedSPtr_) &&
                                      (nextEntitySPtr->GetEntityWillDraw()));

            if (IS_HIGHLIGHTED)
            {
                sf::FloatRect rect(nextEntitySPtr->GetEntityRegion());
                rect.left -= margin_;
                rect.width = entityRegion_.width;

                if (IMAGE_MAP_CITER != imageMap_.end())
                    rect.width -= imageSize_ + IMAGE_HORIZ_PAD_;

                sfml_util::DrawRectangle<float>(target, states, rect, sf::Color::Transparent, 0, highlightColor_);
            }

            if (nextEntitySPtr->GetEntityWillDraw())
            {
                float imageHeight(0.0f);
                if (IMAGE_MAP_CITER != imageMap_.end())
                {
                    target.draw(IMAGE_MAP_CITER->second.second, states);
                    imageHeight = IMAGE_MAP_CITER->second.second.getGlobalBounds().height;
                }

                target.draw( * nextEntitySPtr, states);
                lastDrawnEntityHeight = std::max(nextEntitySPtr->GetEntityRegion().height, imageHeight);

                //draw between line
                const float LINE_POS_TOP(nextEntitySPtr->GetEntityRegion().top + nextEntitySPtr->GetEntityRegion().height + (betweenPad_ * 0.5f));
                if (LINE_POS_TOP < BOTTOM)
                {
                    const int LIMIT((list_.empty()) ? itemLimit_ : (itemLimit_ - 1));
                    if ((NO_LIMIT_ == itemLimit_) || (itemCount++ <= LIMIT))
                        DrawLine(target, LINE_POS_TOP);

                    lastDrawnListPosY = LINE_POS_TOP + lastDrawnEntityHeight;
                }
                else
                    break;
            }

            ++itr;
        }

        //draw lines in the rest of the box even though there are no more entitys
        while (lastDrawnListPosY < BOTTOM)
        {
            const int LIMIT((list_.empty()) ? itemLimit_ : (itemLimit_ - 1));
            if ((NO_LIMIT_ == itemLimit_) || (itemCount++ <= LIMIT))
                DrawLine(target, lastDrawnListPosY);

            lastDrawnListPosY += lastDrawnEntityHeight;
        }
    }


    ListBoxItemSPtr_t ListBox::GetAtPosition(const sf::Vector2f & POS_V)
    {
        for (auto const & NEXT_ENTITY_SPTR : list_)
            if (NEXT_ENTITY_SPTR->GetEntityWillDraw())
                if ((POS_V.x >= entityRegion_.left) &&
                    (POS_V.x <= entityRegion_.left + entityRegion_.width - (2.0f * margin_)) &&
                    (POS_V.y >= NEXT_ENTITY_SPTR->GetEntityRegion().top) &&
                    (POS_V.y <= NEXT_ENTITY_SPTR->GetEntityRegion().top + NEXT_ENTITY_SPTR->GetEntityRegion().height))
                {
                    return NEXT_ENTITY_SPTR;
                }

        return ListBoxItemSPtr_t();
    }


    std::size_t ListBox::GetSelectedIndex() const
    {
        std::size_t index(0);
        for (auto const & NEXT_ENTITY_SPTR : list_)
            if (NEXT_ENTITY_SPTR == selectedSPtr_)
                return index;
            else
                ++index;

        return list_.size() + 1;//return invalid index, anything that will cause IsIndexValid() to return false will work here
    }


    bool ListBox::IsIndexValid(const std::size_t INDEX) const
    {
        if (list_.empty())
            return false;
        else if (INDEX >= list_.size())
            return false;
        else
            return true;
    }


    bool ListBox::SetSelectedIndex(const std::size_t NEW_INDEX, const bool WILL_PLAY_SOUNDEFFECT)
    {
        if (IsIndexValid(NEW_INDEX) == false)
            return false;

        if (GetSelectedIndex() == NEW_INDEX)
            return false;

        SoundManager::Instance()->StaticSounds_TickOn()->PlayRandom();

        std::size_t indexCounter(0);
        float vertTracker(entityRegion_.height);
        for (typename ListBoxItemSLst_t::const_iterator itr(list_.begin()); itr != list_.end(); ++itr)
        {
            if (indexCounter++ == NEW_INDEX)
            {
                currentViewPos_ = (*itr)->GetEntityRegion().height + betweenPad_;

                if (WILL_PLAY_SOUNDEFFECT)
                    SoundManager::Instance()->StaticSounds_TickOn()->PlayRandom();

                selectedSPtr_ = *itr;
                SetupList();
                CreateSelectionChangePackageAndCallHandler(NEW_INDEX);
                return true;
            }
            else
            {
                if ((*itr)->GetEntityWillDraw())
                    vertTracker += (*itr)->GetEntityRegion().height + betweenPad_;
            }
        }

        return false;
    }


    ListBoxItemSPtr_t ListBox::At(const std::size_t INDEX)
    {
        if (INDEX >= list_.size())
        {
            std::ostringstream ss;
            ss << "Get() was given an index=" << INDEX << " that is out of bounds with the current count=" << list_.size() << ".";
            throw std::range_error(ss.str());
        }
        else
        {
            typename sfml_util::gui::ListBoxItemSLst_t::iterator itr(list_.begin());
            std::advance(itr, INDEX);
            return * itr;
        }
    }


    void ListBox::Add(const ListBoxItemSPtr_t & THING_SPTR,
                      const bool                WILL_INC_CURRENT_SEL)
    {
        if ((NO_LIMIT_ == itemLimit_) || (list_.size() < static_cast<std::size_t>(itemLimit_)))
        {
            list_.push_back(THING_SPTR);

            if (WILL_INC_CURRENT_SEL)
            {
                MoveSelectionDown();
            }

            SetupList();
        }
    }


    bool ListBox::Remove(const ListBoxItemSPtr_t & THING_SPTR)
    {
        typename ListBoxItemSLst_t::iterator FOUND_ITR(std::find(list_.begin(), list_.end(), THING_SPTR));

        if (FOUND_ITR == list_.end())
            return false;

        if (*FOUND_ITR == selectedSPtr_)
            selectedSPtr_.reset();

        list_.erase(FOUND_ITR);
        SetupList();
        return true;
    }


    bool ListBox::MouseUp(const sf::Vector2f & MOUSE_POS_V)
    {
        const bool DID_STATE_CHANGE(GuiEntity::MouseUp(MOUSE_POS_V));

        for (auto const & NEXT_ENTITY_SPTR : list_)
        {
            if (NEXT_ENTITY_SPTR->GetEntityWillDraw())
            {
                if ((MOUSE_POS_V.x >= entityRegion_.left) &&
                    (MOUSE_POS_V.x <= entityRegion_.left + entityRegion_.width - (2.0f * margin_)) &&
                    (MOUSE_POS_V.y >= NEXT_ENTITY_SPTR->GetEntityRegion().top) &&
                    (MOUSE_POS_V.y <= NEXT_ENTITY_SPTR->GetEntityRegion().top + NEXT_ENTITY_SPTR->GetEntityRegion().height))
                {
                    if (selectedSPtr_ != NEXT_ENTITY_SPTR)
                    {
                        SoundManager::Instance()->StaticSounds_Switch()->PlayRandom();
                        selectedSPtr_ = NEXT_ENTITY_SPTR;
                        SetupList();
                        boxSPtr_->FakeColorSetAsIfFocusIs(true);

                        if (callbackPtr_ != nullptr)
                        {
                            const callback::ListBoxPtrPackage_t PTR_PACKAGE(this);
                            const callback::ListBoxEventPackage PACKAGE(PTR_PACKAGE, sfml_util::GuiEvent::Click, MOUSE_POS_V);
                            callbackPtr_->HandleCallback(PACKAGE);
                        }

                        return true;
                    }
                }
            }
        }

        boxSPtr_->FakeColorSetAsIfFocusIs(DID_STATE_CHANGE);

        return DID_STATE_CHANGE;
    }


    bool ListBox::KeyRelease(const sf::Event::KeyEvent & KEY_EVENT)
    {
        if (list_.empty())
        {
            selectedSPtr_.reset();
            CreateKeypressPackageAndCallHandler(KEY_EVENT);
            return false;
        }

        SoundManager::Instance()->StaticSounds_Switch()->PlayRandom();

        if (KEY_EVENT.code == sf::Keyboard::Return)
        {
            CreateKeypressPackageAndCallHandler(KEY_EVENT);
            return true;
        }

        if (KEY_EVENT.code == sf::Keyboard::Up)
        {
            auto const WILL_RETURN_TRUE{ MoveSelectionUp() };
            CreateKeypressPackageAndCallHandler(KEY_EVENT);
            return WILL_RETURN_TRUE;
        }
        else if (KEY_EVENT.code == sf::Keyboard::Down)
        {
            auto const WILL_RETURN_TRUE{ MoveSelectionDown() };
            CreateKeypressPackageAndCallHandler(KEY_EVENT);
            return WILL_RETURN_TRUE;
        }

        CreateKeypressPackageAndCallHandler(KEY_EVENT);
        return false;
    }


    ListBoxItemSPtr_t ListBox::GetItemAtLocation(const sf::Vector2f & MOUSE_POS_V)
    {
        for (auto const & NEXT_ITEM_SPTR : list_)
        {
            if (NEXT_ITEM_SPTR->GetEntityWillDraw())
            {
                if ((MOUSE_POS_V.x >= entityRegion_.left) &&
                    (MOUSE_POS_V.x <= entityRegion_.left + entityRegion_.width - (2.0f * margin_)) &&
                    (MOUSE_POS_V.y >= NEXT_ITEM_SPTR->GetEntityRegion().top) &&
                    (MOUSE_POS_V.y <= NEXT_ITEM_SPTR->GetEntityRegion().top + NEXT_ITEM_SPTR->GetEntityRegion().height))
                {
                    return NEXT_ITEM_SPTR;
                }
            }
        }

        return ListBoxItemSPtr_t();
    }


    const sf::FloatRect ListBox::GetRectAtLocation(const sf::Vector2f & MOUSE_POS_V)
    {
        for (auto const & NEXT_ITEM_SPTR : list_)
        {
            if (NEXT_ITEM_SPTR->GetEntityWillDraw())
            {
                if ((MOUSE_POS_V.x >= entityRegion_.left) &&
                    (MOUSE_POS_V.x <= entityRegion_.left + entityRegion_.width - (2.0f * margin_)) &&
                    (MOUSE_POS_V.y >= NEXT_ITEM_SPTR->GetEntityRegion().top) &&
                    (MOUSE_POS_V.y <= NEXT_ITEM_SPTR->GetEntityRegion().top + NEXT_ITEM_SPTR->GetEntityRegion().height))
                {
                    const ImageMapCIter_t IMAGE_MAP_CITER(imageMap_.find(NEXT_ITEM_SPTR));
                    if (IMAGE_MAP_CITER != imageMap_.end())
                    {
                        return IMAGE_MAP_CITER->second.second.getGlobalBounds();
                    }
                    else
                    {
                        return sf::FloatRect(entityRegion_.left,
                                             NEXT_ITEM_SPTR->GetEntityRegion().top,
                                             entityRegion_.width - (2.0f * margin_),
                                             NEXT_ITEM_SPTR->GetEntityRegion().top + NEXT_ITEM_SPTR->GetEntityRegion().height);
                    }
                }
            }
        }

        return ERROR_RECT_;
    }


    void ListBox::SetEntityPos(const float POS_LEFT, const float POS_TOP)
    {
        const float DIFF_HORIZ(POS_LEFT - GetEntityPos().x);
        const float DIFF_VERT(POS_TOP - GetEntityPos().y);

        for (auto & NEXT_ITEM_SPTR : list_)
            NEXT_ITEM_SPTR->MoveEntityPos(DIFF_HORIZ, DIFF_VERT);

        for (auto & NEXT_IMAGE_PAIR : imageMap_)
            NEXT_IMAGE_PAIR.second.second.move(DIFF_HORIZ, DIFF_VERT);

        if (boxSPtr_.get() != nullptr)
            boxSPtr_->MoveEntityPos(DIFF_HORIZ, DIFF_VERT);

        GuiEntity::SetEntityPos(POS_LEFT, POS_TOP);
    }


    void ListBox::CreateSelectionChangePackageAndCallHandler(const std::size_t NEW_SELECTED_INDEX)
    {
        if (callbackPtr_ != nullptr)
        {
            const callback::ListBoxPtrPackage_t PTR_PACKAGE(this);
            const callback::ListBoxEventPackage PACKAGE(PTR_PACKAGE,
                                                    sfml_util::GuiEvent::SelectionChange,
                                                    callback::ListBoxEventPackage::INVALID_MOUSE_POS_V_,
                                                    NEW_SELECTED_INDEX);
            callbackPtr_->HandleCallback(PACKAGE);
        }
    }


    void ListBox::CreateKeypressPackageAndCallHandler(const sf::Event::KeyEvent & KEY_EVENT)
    {
        if (callbackPtr_ != nullptr)
        {
            const callback::ListBoxPtrPackage_t PTR_PACKAGE(this);
            const callback::ListBoxEventPackage PACKAGE(PTR_PACKAGE,
                                                        sfml_util::GuiEvent::Keypress,
                                                        callback::ListBoxEventPackage::INVALID_MOUSE_POS_V_,
                                                        callback::ListBoxEventPackage::INVALID_SELECTION_,
                                                        false,
                                                        KEY_EVENT);
            callbackPtr_->HandleCallback(PACKAGE);
        }
    }


    void ListBox::OnDoubleClick(const sf::Vector2f & MOUSE_POS_V)
    {
        if (callbackPtr_ != nullptr)
        {
            const callback::ListBoxPtrPackage_t PTR_PACKAGE(this);
            const callback::ListBoxEventPackage PACKAGE(PTR_PACKAGE, sfml_util::GuiEvent::DoubleClick, MOUSE_POS_V);
            callbackPtr_->HandleCallback(PACKAGE);
        }
    }


    void ListBox::SetupList()
    {
        imageMap_.clear();

        //find the first visible (or last) entity
        typename ListBoxItemSLst_t::iterator itr(list_.begin());
        if (list_.empty() == false)
        {
            float vertTracker(0.0f);

            for (; itr != list_.end(); ++itr)
            {
                if (vertTracker < currentViewPos_)
                {
                    //check if at the end of the list
                    typename ListBoxItemSLst_t::const_iterator tempItr(itr);
                    ++tempItr;
                    if (tempItr == list_.end())
                    {
                        currentViewPos_ = vertTracker;
                        break;
                    }
                }
                else
                    break;

                (*itr)->SetEntityWillDraw(false);
                vertTracker += (*itr)->GetEntityRegion().height + betweenPad_;
            }
        }

        //setup first visible entity
        ListBoxItemSPtr_t firstVisibleEntitySPtr;
        float entityPosY(entityRegion_.top + margin_);
        if (list_.empty() == false)
        {
            firstVisibleEntitySPtr = *itr;
            firstVisibleEntitySPtr->SetEntityWillDraw(true);
            PopulateImageMapAndAdjustLeftPos(firstVisibleEntitySPtr, entityRegion_.left + margin_, entityPosY, true);

            if (itr != list_.end())
                ++itr;

            entityPosY += firstVisibleEntitySPtr->GetEntityRegion().height + betweenPad_;
        }

        //set selected entity to the first if none is set
        if (selectedSPtr_.get() == nullptr)
        {
            selectedSPtr_ = firstVisibleEntitySPtr;
        }
        else
        {
            if ((list_.empty() == false) && (selectedSPtr_ != firstVisibleEntitySPtr))
                PopulateImageMapAndAdjustLeftPos(firstVisibleEntitySPtr,
                                                 entityRegion_.left + margin_,
                                                 entityPosY - (firstVisibleEntitySPtr->GetEntityRegion().height + betweenPad_),
                                                 false);
        }


        //establish the visible set of entitys in the box
        if (list_.empty() == false)
        {
            for (; itr != list_.end(); ++itr)
            {
                if ((entityPosY + (*itr)->GetEntityRegion().height) > (entityRegion_.top + entityRegion_.height))
                {
                    (*itr)->SetEntityWillDraw(false);
                    break;
                }
                else
                {
                    (*itr)->SetEntityWillDraw(true);
                    PopulateImageMapAndAdjustLeftPos((*itr), entityRegion_.left + margin_, entityPosY, (selectedSPtr_ == * itr));
                    entityPosY += (*itr)->GetEntityRegion().height + betweenPad_;
                }
            }
        }

        //set all remaining entitys (outside the box's lower limit) to not draw
        if (list_.empty() == false)
        {
            for (; itr != list_.end(); ++itr)
                (*itr)->SetEntityWillDraw(false);
        }
    }


    float ListBox::GetTotalHeight() const
    {
        float totalHeight(0.0f);

        for (auto const & NEXT_ENTITY_SPTR : list_)
            totalHeight += NEXT_ENTITY_SPTR->GetEntityRegion().height + betweenPad_;

        if (totalHeight > 0.0f)
            totalHeight -= betweenPad_;

        return totalHeight;
    }


    void ListBox::PopulateImageMapAndAdjustLeftPos(sfml_util::gui::ListBoxItemSPtr_t & listBoxItemSPtr,
                                                   const float                         POS_LEFT_ORIG,
                                                   const float                         POS_TOP,
                                                   const bool                          IS_SELECTED_ITEM)
    {
        float newPosLeft(POS_LEFT_ORIG);
        sfml_util::TextureSPtr_t textureSPtr;

        if (listBoxItemSPtr->iitem_sptr.get() != nullptr)
        {
            textureSPtr = sfml_util::gui::ItemImageManager::Instance()->Load(listBoxItemSPtr->iitem_sptr);
        }
        else if (listBoxItemSPtr->TITLE_CPTRC != nullptr)
        {
            textureSPtr = sfml_util::gui::TitleImageManager::Instance()->Get(listBoxItemSPtr->TITLE_CPTRC);
        }
        else if (listBoxItemSPtr->character_sptr.get() != nullptr)
        {
            textureSPtr = sfml_util::gui::CreatureImageManager::Instance()->GetImage(listBoxItemSPtr->character_sptr->ImageFilename(), true);
        }
        else if (listBoxItemSPtr->SPELL_PTRC != nullptr)
        {
            textureSPtr = sfml_util::gui::SpellImageManager::Instance()->Get(listBoxItemSPtr->SPELL_PTRC->Which());
        }
        else if (listBoxItemSPtr->cond_sptr.get() != nullptr)
        {
            textureSPtr = sfml_util::gui::ConditionImageManager::Instance()->Get(listBoxItemSPtr->cond_sptr->Which());
        }

        if (textureSPtr.get() != nullptr)
        {
            textureSPtr->setSmooth(true);
            sf::Sprite sprite( * textureSPtr);

            if (IS_SELECTED_ITEM)
                sprite.setColor(sf::Color::White);
            else
                sprite.setColor(imageColor_);

            if (sprite.getLocalBounds().width > sprite.getLocalBounds().height)
            {
                const float SCALE(imageSize_ / sprite.getLocalBounds().width);
                sprite.setScale(SCALE, SCALE);
            }
            else
            {
                const float SCALE(imageSize_ / sprite.getLocalBounds().height);
                sprite.setScale(SCALE, SCALE);
            }

            const float SPRITE_POS_LEFT((POS_LEFT_ORIG + (imageSize_ * 0.5f)) - (sprite.getGlobalBounds().width * 0.5f));
            const float SPRITE_POS_TOP((POS_TOP + (imageSize_ * 0.5f)) - (sprite.getGlobalBounds().height * 0.5f));
            sprite.setPosition(SPRITE_POS_LEFT, SPRITE_POS_TOP);

            imageMap_[listBoxItemSPtr] = std::make_pair(textureSPtr, sprite);
            const float POS_LEFT_ADJ(imageSize_ + IMAGE_HORIZ_PAD_);
            newPosLeft += POS_LEFT_ADJ;

            sf::FloatRect r(listBoxItemSPtr->GetEntityRegion());
            r.width -= POS_LEFT_ADJ;
            r.height = std::max(r.height, imageSize_);
            listBoxItemSPtr->SetEntityRegion(r);
        }

        listBoxItemSPtr->SetEntityPos(newPosLeft, POS_TOP);
    }


    bool ListBox::MoveSelectionUp()
    {
        float vertTracker(entityRegion_.height);
        for (typename ListBoxItemSLst_t::const_iterator itr(list_.begin()); itr != list_.end(); ++itr)
        {
            typename ListBoxItemSLst_t::const_iterator tempItr(itr);
            ++tempItr;
            if (*tempItr == selectedSPtr_)
            {
                if (vertTracker >= entityRegion_.height)
                    currentViewPos_ -= (*itr)->GetEntityRegion().height + betweenPad_;

                SoundManager::Instance()->StaticSounds_TickOn()->PlayRandom();
                selectedSPtr_ = *itr;
                SetupList();
                return true;
            }
            else
            {
                if ((*itr)->GetEntityWillDraw())
                    vertTracker -= (*itr)->GetEntityRegion().height + betweenPad_;
            }
        }

        return false;
    }


    bool ListBox::MoveSelectionDown()
    {
        float vertTracker(0.0f);
        for (typename ListBoxItemSLst_t::const_iterator itr(list_.begin()); itr != list_.end(); ++itr)
        {
            if (itr != list_.begin())
            {
                typename ListBoxItemSLst_t::const_iterator prevItr(itr);
                --prevItr;
                if (*prevItr == selectedSPtr_)
                {
                    if ((vertTracker + ((*itr)->GetEntityRegion().height + margin_)) > entityRegion_.height)
                        currentViewPos_ += (*itr)->GetEntityRegion().height + betweenPad_;

                    SoundManager::Instance()->StaticSounds_TickOff()->PlayRandom();
                    selectedSPtr_ = *itr;
                    SetupList();
                    return true;
                }
            }

            if ((*itr)->GetEntityWillDraw())
                vertTracker += (*itr)->GetEntityRegion().height + betweenPad_;
        }

        return false;
    }

}
}
