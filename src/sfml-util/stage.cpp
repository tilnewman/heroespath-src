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

#include "sfml-util/display.hpp"
#include "sfml-util/font-manager.hpp"
#include "sfml-util/sound-manager.hpp"
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
    {
        SoundManager::Instance()->ClearSoundEffectsCache();
    }


    void Stage::UpdateTime(const float ELAPSED_TIME_SECONDS)
    {
        for (auto & entitySPtr : entitySSet_)
        {
            entitySPtr->UpdateTime(ELAPSED_TIME_SECONDS);
        }
    }


    void Stage::UpdateMousePos(const sf::Vector2f & MOUSE_POS_V)
    {
        for (auto & entitySPtr : entitySSet_)
        {
            entitySPtr->UpdateMousePos(MOUSE_POS_V);
        }
    }


    void Stage::UpdateMouseDown(const sf::Vector2f & MOUSE_POS_V)
    {
        for (auto & entitySPtr : entitySSet_)
        {
            entitySPtr->MouseDown(MOUSE_POS_V);
        }
    }


    gui::IGuiEntitySPtr_t Stage::UpdateMouseUp(const sf::Vector2f & MOUSE_POS_V)
    {
        bool foundFocus(false);

        for (auto & entitySPtr : entitySSet_)
        {
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
            entitySPtr->UpdateMouseWheel(MOUSE_POS_V, MOUSEWHEEL_DELTA);
        }
    }


    bool Stage::KeyPress(const sf::Event::KeyEvent & KE)
    {
        return ((entityWithFocusSPtr_.get() != nullptr) && (entityWithFocusSPtr_->KeyPress(KE)));
    }


    bool Stage::KeyRelease(const sf::Event::KeyEvent & KE)
    {
        return ((entityWithFocusSPtr_.get() != nullptr) && (entityWithFocusSPtr_->KeyRelease(KE)));
    }


    void Stage::RemoveFocus()
    {
        entityWithFocusSPtr_.reset();

        for (auto & entitySPtr : entitySSet_)
        {
            entitySPtr->SetHasFocus(false);
        }
    }


    bool Stage::SetFocus(const gui::IGuiEntitySPtr_t & ENTITY_SPTR)
    {
        entityWithFocusSPtr_.reset();

        bool foundEntityWithFocus(false);

        for(auto & entitySPtr : entitySSet_)
        {
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


    void Stage::Draw(sf::RenderTarget & target, const sf::RenderStates & STATES)
    {
        for (auto const & ENTITY_SPTR: entitySSet_)
        {
            ENTITY_SPTR->draw(target, STATES);
        }

        DrawHoverText(target, STATES);
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
        {
            wereAnyRemoved = !(ORIG_NUM_ENTITYS == NEW_NUM_ENTITYS);
        }

        return wereAnyRemoved;
    }


    void Stage::SetMouseHover(const sf::Vector2f & MOUSE_POS_V, const bool IS_MOUSE_HOVERING)
    {
        if (IS_MOUSE_HOVERING)
        {
            std::string hoverText("");

            //check if focused entity is hovered first
            if ((entityWithFocusSPtr_.get() != nullptr) && (entityWithFocusSPtr_->GetEntityRegion().contains(MOUSE_POS_V)))
            {
                hoverText = entityWithFocusSPtr_->GetMouseHoverText(MOUSE_POS_V);
            }

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
                {
                    hoverTextBoxSPtr_.reset();
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
                region.left = SCREEN_WIDTH - region.width;

            if (region.left < 0.0f)
            {
                region.left = 0.0f;
            }

            hoverSfText_.setPosition(region.left + 10.0f, region.top + 2.0f);
            const gui::BackgroundInfo BG_INFO(FontManager::Instance()->Color_Orange() - sf::Color(20,0,0,0));

            const gui::box::Info BOX_INFO(1, true, region, gui::ColorSet(), BG_INFO);

            hoverTextBoxSPtr_.reset( new gui::box::Box(std::string(GetStageName()).append("'sHoverText"), BOX_INFO) );
        }
        else
        {
            if (hoverTextBoxSPtr_.get() != nullptr)
            {
                hoverTextBoxSPtr_.reset();
            }
        }
    }


    void Stage::DrawHoverText(sf::RenderTarget & target, const sf::RenderStates & STATES)
    {
        if (hoverTextBoxSPtr_.get() != nullptr)
        {
            target.draw( * hoverTextBoxSPtr_, STATES);
            target.draw(hoverSfText_, STATES);
        }
    }

}
