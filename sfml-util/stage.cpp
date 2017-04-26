//
// stage.cpp
//
#include "stage.hpp"

#include "sfml-util/display.hpp"
#include "sfml-util/font-manager.hpp"
#include "sfml-util/gui/text-info.hpp"
#include "sfml-util/gui/box-info.hpp"
#include "sfml-util/gui/box.hpp"
#include "sfml-util/gui/gui-entity.hpp"

#include <sstream>
#include <exception>


namespace sfml_util
{

    Stage::Stage(const std::string & NAME)
    :
        STAGE_NAME_         (std::string(NAME).append("_Stage")),
        stageRegion_        ( sf::FloatRect(0.0f, 0.0f, Display::Instance()->GetWinWidth(), Display::Instance()->GetWinHeight()) ),
        entitySSet_         (),
        entityWithFocusSPtr_(),
        hoverTextBoxSPtr_   (),
        hoverSfText_        ()
    {}


    Stage::Stage(const std::string &   NAME,
                 const sf::FloatRect & REGION)
    :
        STAGE_NAME_         (std::string(NAME).append("_Stage")),
        stageRegion_        (REGION),
        entitySSet_         (),
        entityWithFocusSPtr_(),
        hoverTextBoxSPtr_   (),
        hoverSfText_        ()
    {}


    Stage::Stage(const std::string & NAME,
                 const float         REGION_LEFT,
                 const float         REGION_TOP,
                 const float         REGION_WIDTH,
                 const float         REGION_HEIGHT)
    :
        STAGE_NAME_         (std::string(NAME).append("_Stage")),
        stageRegion_        ( sf::FloatRect(REGION_LEFT, REGION_TOP, REGION_WIDTH, REGION_HEIGHT) ),
        entitySSet_         (),
        entityWithFocusSPtr_(),
        hoverTextBoxSPtr_   (),
        hoverSfText_        ()
    {}


    Stage::~Stage()
    {}


    void Stage::UpdateTime(const float ELAPSED_TIME_SECONDS)
    {
        for (auto & entitySPtr : entitySSet_)
        {
            M_ASSERT_OR_LOGANDTHROW_SS((entitySPtr.get() != nullptr), "Stage::UpdateTime() encountered a null entitySPtr in entitySSet_.");
            entitySPtr->UpdateTime(ELAPSED_TIME_SECONDS);
        }
    }


    void Stage::UpdateMousePos(const sf::Vector2f & MOUSE_POS_V)
    {
        for (auto & entitySPtr : entitySSet_)
        {
            M_ASSERT_OR_LOGANDTHROW_SS((entitySPtr.get() != nullptr), "Stage::UpdateMousePos() encountered a null entitySPtr in entitySSet_.");
            entitySPtr->UpdateMousePos(MOUSE_POS_V);
        }
    }


    void Stage::UpdateMouseDown(const sf::Vector2f & MOUSE_POS_V)
    {
        for (auto & entitySPtr : entitySSet_)
        {
            M_ASSERT_OR_LOGANDTHROW_SS((entitySPtr.get() != nullptr), "Stage::UpdateMouseDown() encountered a null entitySPtr in entitySSet_.");
            entitySPtr->MouseDown(MOUSE_POS_V);
        }
    }


    gui::IGuiEntitySPtr_t Stage::UpdateMouseUp(const sf::Vector2f & MOUSE_POS_V)
    {
        bool foundFocus(false);

        for (auto & entitySPtr : entitySSet_)
        {
            M_ASSERT_OR_LOGANDTHROW_SS((entitySPtr.get() != nullptr), "Stage::UpdateMouseUp() encountered a null entitySPtr in entitySSet_.");

            if (entitySPtr->MouseUp(MOUSE_POS_V))
            {
                //can only find one entity with focus
                if ((entitySPtr->WillAcceptFocus()) && (false == foundFocus))
                {
                    foundFocus = true;
                    entitySPtr->SetHasFocus(true);
                    entityWithFocusSPtr_ = entitySPtr;
                }
            }
        }

        return entityWithFocusSPtr_;
    }


    void Stage::UpdateMouseWheel(const sf::Vector2f & MOUSE_POS_V, const float MOUSEWHEEL_DELTA)
    {
        for (auto & entitySPtr : entitySSet_)
        {
            M_ASSERT_OR_LOGANDTHROW_SS((entitySPtr.get() != nullptr), "Stage::UpdateMouseWheel() encountered a null entitySPtr in entitySSet_.");
            entitySPtr->UpdateMouseWheel(MOUSE_POS_V, MOUSEWHEEL_DELTA);
        }
    }


    bool Stage::KeyPress(const sf::Event::KeyEvent & KE)
    {
        if (entityWithFocusSPtr_.get() != nullptr)
            if (entityWithFocusSPtr_->KeyPress(KE))
                return true;

        return false;
    }


    bool Stage::KeyRelease(const sf::Event::KeyEvent & KE)
    {
        if (entityWithFocusSPtr_.get() != nullptr)
            if (entityWithFocusSPtr_->KeyRelease(KE))
                return true;

        return false;
    }


    void Stage::RemoveFocus()
    {
        entityWithFocusSPtr_.reset();

        for (auto & entitySPtr : entitySSet_)
            entitySPtr->SetHasFocus(false);
    }


    bool Stage::SetFocus(const gui::IGuiEntitySPtr_t & ENTITY_SPTR)
    {
        entityWithFocusSPtr_.reset();

        bool foundEntityWithFocus(false);

        for(auto & entitySPtr : entitySSet_)
        {
            M_ASSERT_OR_LOGANDTHROW_SS((entitySPtr.get() != nullptr), "Stage::SetFocus() encountered a null entitySPtr in entitySSet_.");

            //only need to confirm that ENTITY_SPTR is in entitySSet_
            if (ENTITY_SPTR.get() == entitySPtr.get())
            {
                entityWithFocusSPtr_ = ENTITY_SPTR;
                foundEntityWithFocus = true;
                break;
            }
        }

        return foundEntityWithFocus;
    }


    void Stage::Draw(sf::RenderTarget & target, sf::RenderStates states)
    {
        for (auto const & ENTITY_SPTR: entitySSet_)
        {
            M_ASSERT_OR_LOGANDTHROW_SS((ENTITY_SPTR.get() != nullptr), "Stage::Draw() encountered a null entitySPtr in entitySSet_.");
            ENTITY_SPTR->draw(target, states);
        }

        DrawHoverText(target, states);
    }


    bool Stage::EntityAdd(const gui::IGuiEntitySPtr_t & ENTITY_SPTR)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((ENTITY_SPTR.get() != nullptr), "sfml_util::Stage::EntityAdd() was given a null ENTITY_SPTR.");

        const std::size_t ORIG_ENTITY_COUNT(entitySSet_.size());
        entitySSet_.insert(ENTITY_SPTR);
        const std::size_t NEW_ENTITY_COUNT(entitySSet_.size());

        //if there is no change in count then the insert() failed because it was already in the set -this should never happen
        if (ORIG_ENTITY_COUNT == NEW_ENTITY_COUNT)
        {
            std::ostringstream ss;
            ss << "sfml_util::Stage::EntityAdd(\"" << ENTITY_SPTR->GetEntityName() << "\") tried to add that entity when it was already in the entitySSet_.";
            throw std::logic_error(ss.str());
        }

        return (ORIG_ENTITY_COUNT != NEW_ENTITY_COUNT);
    }


    bool Stage::EntityRemove(const gui::IGuiEntitySPtr_t & ENTITY_SPTR)
    {
        bool wereAnyRemoved(false);
        const std::size_t ORIG_NUM_ENTITYS(entitySSet_.size());

        if (entityWithFocusSPtr_.get() == ENTITY_SPTR.get())
        {
            wereAnyRemoved = true;
            entityWithFocusSPtr_.reset();
        }

        entitySSet_.erase(ENTITY_SPTR);

        const std::size_t NEW_NUM_ENTITYS(entitySSet_.size());

        if (false == wereAnyRemoved)
            wereAnyRemoved = ! (ORIG_NUM_ENTITYS == NEW_NUM_ENTITYS);

        return wereAnyRemoved;
    }

    
    bool Stage::EntityRemovePtr(gui::IGuiEntity * ptr)
    {
        bool wereAnyRemoved(false);
        const std::size_t ORIG_NUM_ENTITYS(entitySSet_.size());

        if (entityWithFocusSPtr_.get() == ptr)
        {
            wereAnyRemoved = true;
            entityWithFocusSPtr_.reset();
        }

        sfml_util::gui::IGuiEntitySPtr_t foundSPtr{ nullptr };
        for (auto const & NEXT_IGUIENTITY_SPTR : entitySSet_)
        {
            if (NEXT_IGUIENTITY_SPTR.get() == ptr)
            {
                foundSPtr = NEXT_IGUIENTITY_SPTR;
                break;
            }
        }

        if (foundSPtr.get() != nullptr)
        {
            entitySSet_.erase(foundSPtr);
        }

        const std::size_t NEW_NUM_ENTITYS(entitySSet_.size());

        if (false == wereAnyRemoved)
            wereAnyRemoved = ! (ORIG_NUM_ENTITYS == NEW_NUM_ENTITYS);

        return wereAnyRemoved;
    }


    void Stage::SetMouseHover(const sf::Vector2f & MOUSE_POS_V, const bool IS_MOUSE_HOVERING)
    {
        if (IS_MOUSE_HOVERING)
        {
            std::string hoverText("");

            //check if focused entity is hovered first
            if ((entityWithFocusSPtr_.get() != nullptr) && (entityWithFocusSPtr_->GetEntityRegion().contains(MOUSE_POS_V)))
                hoverText = entityWithFocusSPtr_->GetMouseHoverText(MOUSE_POS_V);

            //if focused entity is not hovered, then look for any entity the mouse is hoving over
            if (hoverText.empty())
            {
                for (auto const & IGUIENTITY_SPTR : entitySSet_)
                {
                    M_ASSERT_OR_LOGANDTHROW_SS((IGUIENTITY_SPTR.get() != nullptr), "Stage::MouseHover() encountered a null entitySPtr in entitySSet_.");

                    if (IGUIENTITY_SPTR->GetEntityRegion().contains(MOUSE_POS_V))
                    {
                        hoverText = IGUIENTITY_SPTR->GetMouseHoverText(MOUSE_POS_V);
                        if (hoverText.empty() == false)
                            break;
                    }
                }
            }

            if (hoverText.empty())
            {
                if (hoverTextBoxSPtr_.get() != nullptr)
                    hoverTextBoxSPtr_.reset();

                return;
            }

            const gui::TextInfo TEXT_INFO(hoverText,
                                          FontManager::Instance()->Font_Default2(),
                                          FontManager::Instance()->Size_Smallish(),
                                          sf::Color(50,50,50),
                                          Justified::Left);

            sfml_util::gui::SetupText(hoverSfText_, TEXT_INFO);

            sf::FloatRect region(MOUSE_POS_V.x - 200.0f,
                                 MOUSE_POS_V.y + 10.0f,
                                 hoverSfText_.getGlobalBounds().width + 20.0f,
                                 hoverSfText_.getGlobalBounds().height + 8.0f);

            const float SCREEN_WIDTH(Display::Instance()->GetWinWidth());
            if ((region.left + region.width) > SCREEN_WIDTH)
                region.left = SCREEN_WIDTH - region.width;

            if (region.left < 0.0f)
                region.left = 0.0f;

            hoverSfText_.setPosition(region.left + 10.0f, region.top + 2.0f);
            const gui::BackgroundInfo BG_INFO(FontManager::Instance()->Color_Orange() - sf::Color(20,0,0,0));

            const gui::box::Info BOX_INFO(1, true, region, gui::ColorSet(), BG_INFO);

            hoverTextBoxSPtr_.reset( new gui::box::Box(std::string(GetStageName()).append("'sHoverText"), BOX_INFO) );
        }
        else
        {
            if (hoverTextBoxSPtr_.get() != nullptr)
                hoverTextBoxSPtr_.reset();
        }
    }


    void Stage::DrawHoverText(sf::RenderTarget & target, sf::RenderStates & states)
    {
        if (hoverTextBoxSPtr_.get() != nullptr)
        {
            target.draw( * hoverTextBoxSPtr_, states);
            target.draw(hoverSfText_, states);
        }
    }

}
