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
// stage.cpp
//
#include "stage.hpp"

#include "sfml-util/sfml-util.hpp"
#include "sfml-util/display.hpp"
#include "sfml-util/font-manager.hpp"
#include "sfml-util/sound-manager.hpp"
#include "sfml-util/texture-cache.hpp"
#include "sfml-util/gui/text-info.hpp"
#include "sfml-util/gui/box-info.hpp"
#include "sfml-util/gui/box.hpp"
#include "sfml-util/gui/gui-entity.hpp"

#include "game/loop-manager.hpp"

#include <sstream>
#include <exception>


namespace sfml_util
{

    const float Stage::MOUSE_DRAG_MIN_DISTANCE_{ 3.0f };


    Stage::Stage(const std::string & NAME,
                 const bool          WILL_CLEAR_CACHE_ON_EXIT)
    :
        STAGE_NAME_              (std::string(NAME).append("_Stage")),
        stageRegion_             (sf::FloatRect(0.0f,
                                                0.0f,
                                                Display::Instance()->GetWinWidth(),
                                                Display::Instance()->GetWinHeight())),
        entityPVec_              (),
        entityWithFocusPtr_      (),
        hoverTextBoxUPtr_        (),
        hoverSfText_             (),
        isMouseHeldDown_         (false),
        isMouseHeldDownAndMoving_(false),
        mouseDownPosV_           (0.0f, 0.0f),
        willClearCachesOnExit_   (WILL_CLEAR_CACHE_ON_EXIT)
    {}


    Stage::Stage(const std::string &   NAME,
                 const sf::FloatRect & REGION,
                 const bool            WILL_CLEAR_CACHE_ON_EXIT)
    :
        STAGE_NAME_              (std::string(NAME).append("_Stage")),
        stageRegion_             (REGION),
        entityPVec_              (),
        entityWithFocusPtr_      (),
        hoverTextBoxUPtr_        (),
        hoverSfText_             (),
        isMouseHeldDown_         (false),
        isMouseHeldDownAndMoving_(false),
        mouseDownPosV_           (0.0f, 0.0f),
        willClearCachesOnExit_   (WILL_CLEAR_CACHE_ON_EXIT)
    {}


    Stage::Stage(const std::string & NAME,
                 const float         REGION_LEFT,
                 const float         REGION_TOP,
                 const float         REGION_WIDTH,
                 const float         REGION_HEIGHT,
                 const bool          WILL_CLEAR_CACHE_ON_EXIT)
    :
        STAGE_NAME_              (std::string(NAME).append("_Stage")),
        stageRegion_             (sf::FloatRect(REGION_LEFT,
                                                REGION_TOP,
                                                REGION_WIDTH,
                                                REGION_HEIGHT)),
        entityPVec_              (),
        entityWithFocusPtr_      (),
        hoverTextBoxUPtr_        (),
        hoverSfText_             (),
        isMouseHeldDown_         (false),
        isMouseHeldDownAndMoving_(false),
        mouseDownPosV_           (0.0f, 0.0f),
        willClearCachesOnExit_   (WILL_CLEAR_CACHE_ON_EXIT)
    {}


    Stage::~Stage()
    {
        if (willClearCachesOnExit_)
        {
            SoundManager::Instance()->ClearSoundEffectsCache();
            TextureCache::Instance()->RemoveAll();
        }
    }


    void Stage::UpdateTime(const float ELAPSED_TIME_SECONDS)
    {
        for (auto entityPtr : entityPVec_)
        {
            entityPtr->UpdateTime(ELAPSED_TIME_SECONDS);
        }
    }


    void Stage::UpdateMousePos(const sf::Vector2f & MOUSE_POS_V)
    {
        isMouseHeldDownAndMoving_ = (isMouseHeldDown_ &&
            (sfml_util::Distance(mouseDownPosV_, MOUSE_POS_V) > MOUSE_DRAG_MIN_DISTANCE_));

        for (auto entityPtr : entityPVec_)
        {
            entityPtr->UpdateMousePos(MOUSE_POS_V);
        }
    }


    void Stage::UpdateMouseDown(const sf::Vector2f & MOUSE_POS_V)
    {
        isMouseHeldDown_ = true;
        mouseDownPosV_ = MOUSE_POS_V;

        for (auto entityPtr : entityPVec_)
        {
            entityPtr->MouseDown(MOUSE_POS_V);
        }
    }


    gui::IGuiEntityPtr_t Stage::UpdateMouseUp(const sf::Vector2f & MOUSE_POS_V)
    {
        isMouseHeldDown_ = false;
        isMouseHeldDownAndMoving_ = false;

        bool foundFocus(false);

        for (auto entityPtr : entityPVec_)
        {
            if (entityPtr->MouseUp(MOUSE_POS_V))
            {
                //can only find one entity with focus
                if ((entityPtr->WillAcceptFocus()) && (false == foundFocus))
                {
                    foundFocus = true;
                    entityPtr->SetHasFocus(true);
                    entityWithFocusPtr_ = entityPtr;
                }
            }
        }

        return entityWithFocusPtr_;
    }


    void Stage::UpdateMouseWheel(const sf::Vector2f & MOUSE_POS_V, const float MOUSEWHEEL_DELTA)
    {
        for (auto entityPtr : entityPVec_)
        {
            entityPtr->UpdateMouseWheel(MOUSE_POS_V, MOUSEWHEEL_DELTA);
        }
    }


    bool Stage::KeyPress(const sf::Event::KeyEvent & KE)
    {
        return ((entityWithFocusPtr_ != nullptr) && (entityWithFocusPtr_->KeyPress(KE)));
    }


    bool Stage::KeyRelease(const sf::Event::KeyEvent & KE)
    {
        return ((entityWithFocusPtr_ != nullptr) && (entityWithFocusPtr_->KeyRelease(KE)));
    }


    void Stage::RemoveFocus()
    {
        entityWithFocusPtr_ = nullptr;

        for (auto entityPtr : entityPVec_)
        {
            entityPtr->SetHasFocus(false);
        }
    }


    bool Stage::SetFocus(const gui::IGuiEntityPtr_t ENTITY_PTR)
    {
        //TODO Should we eliminate the current focus before we know if we are setting a new focus?
        entityWithFocusPtr_ = nullptr;

        bool foundEntityWithFocus(false);

        for(auto entityPtr : entityPVec_)
        {
            //only need to confirm that ENTITY_SPTR is in entityPVec_
            if (ENTITY_PTR == entityPtr)
            {
                entityWithFocusPtr_ = ENTITY_PTR;
                foundEntityWithFocus = true;
                break;
            }
        }

        return foundEntityWithFocus;
    }


    void Stage::Draw(sf::RenderTarget & target, const sf::RenderStates & STATES)
    {
        for (auto const NEXT_ENTITY_PTR: entityPVec_)
        {
            NEXT_ENTITY_PTR->draw(target, STATES);
        }

        DrawHoverText(target, STATES);
    }


    void Stage::EntityAdd(const gui::IGuiEntityPtr_t ENTITY_PTR)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((ENTITY_PTR != nullptr),
            "sfml_util::Stage::EntityAdd() was given a null ENTITY_PTR.");

        for (auto const NEXT_ENTITY_PTR : entityPVec_)
        {
            if (NEXT_ENTITY_PTR == ENTITY_PTR)
            {
                std::ostringstream ss;
                ss << "sfml_util::Stage::EntityAdd(\"" << ENTITY_PTR->GetEntityName()
                    << "\") tried to add but it was already in the entityPVec_.";
                throw std::runtime_error(ss.str());
            }
        }

        entityPVec_.push_back(ENTITY_PTR);
    }


    bool Stage::EntityRemove(const gui::IGuiEntityPtr_t ENTITY_PTR)
    {
        if (entityWithFocusPtr_ == ENTITY_PTR)
        {
            entityWithFocusPtr_ = nullptr;

            entityPVec_.erase(std::remove(entityPVec_.begin(), entityPVec_.end(), ENTITY_PTR),
                entityPVec_.end());

            return true;
        }
        else
        {
            auto const ORIG_NUM_ENTITYS(entityPVec_.size());

            entityPVec_.erase(std::remove(entityPVec_.begin(), entityPVec_.end(), ENTITY_PTR),
                entityPVec_.end());

            const std::size_t NEW_NUM_ENTITYS(entityPVec_.size());
            return ! (ORIG_NUM_ENTITYS == NEW_NUM_ENTITYS);
        }
    }


    void Stage::SetMouseHover(const sf::Vector2f & MOUSE_POS_V, const bool IS_MOUSE_HOVERING)
    {
        if (IS_MOUSE_HOVERING)
        {
            std::string hoverText("");

            //check if focused entity is hovered first
            if ((entityWithFocusPtr_ != nullptr) &&
                (entityWithFocusPtr_->GetEntityRegion().contains(MOUSE_POS_V)))
            {
                hoverText = entityWithFocusPtr_->GetMouseHoverText();
            }

            //if focused entity is not hovered, then look for any entity the mouse is hoving over
            if (hoverText.empty())
            {
                for (auto const NEXT_ENTITY_PTR : entityPVec_)
                {
                    if (NEXT_ENTITY_PTR->GetEntityRegion().contains(MOUSE_POS_V))
                    {
                        hoverText = NEXT_ENTITY_PTR->GetMouseHoverText();

                        if (hoverText.empty() == false)
                        {
                            break;
                        }
                    }
                }
            }

            if (hoverText.empty())
            {
                if (hoverTextBoxUPtr_.get() != nullptr)
                {
                    hoverTextBoxUPtr_.reset();
                }

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
            {
                region.left = SCREEN_WIDTH - region.width;
            }

            if (region.left < 0.0f)
            {
                region.left = 0.0f;
            }

            hoverSfText_.setPosition(region.left + 10.0f, region.top + 2.0f);

            const gui::BackgroundInfo BG_INFO(
                FontManager::Color_Orange() - sf::Color(20,0,0,0));

            const gui::box::Info BOX_INFO(1, true, region, gui::ColorSet(), BG_INFO);

            hoverTextBoxUPtr_ = std::make_unique<gui::box::Box>(GetStageName() + "'sHoverText", BOX_INFO);
        }
        else
        {
            if (hoverTextBoxUPtr_.get() != nullptr)
            {
                hoverTextBoxUPtr_.reset();
            }
        }
    }


    void Stage::ClearAllEntities()
    {
        entityWithFocusPtr_ = nullptr;
        entityPVec_.clear();
    }


    void Stage::DrawHoverText(sf::RenderTarget & target, const sf::RenderStates & STATES)
    {
        if (hoverTextBoxUPtr_.get() != nullptr)
        {
            target.draw( * hoverTextBoxUPtr_, STATES);
            target.draw(hoverSfText_, STATES);
        }
    }

}
