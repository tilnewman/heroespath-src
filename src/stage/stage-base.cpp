// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// stage-base.cpp
//
#include "stage-base.hpp"

#include "game/game-controller.hpp"
#include "gui/box-entity-info.hpp"
#include "gui/box-entity.hpp"
#include "gui/display.hpp"
#include "gui/entity.hpp"
#include "gui/font-manager.hpp"
#include "gui/i-entity.hpp"
#include "gui/sound-manager.hpp"
#include "gui/text-info.hpp"
#include "gui/texture-cache.hpp"
#include "sfutil/distance.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>

#include <algorithm>
#include <exception>
#include <sstream>

namespace heroespath
{
namespace stage
{

    const float StageBase::MOUSE_DRAG_MIN_DISTANCE_ { 3.0f };

    StageBase::StageBase(
        const std::string & NAME,
        const gui::FontEnumVec_t & FONTS_TO_PRELOAD,
        const bool WILL_CLEAR_CACHE_ON_EXIT,
        const gui::SfxEnumVec_t & SFX_TO_PRELOAD)
        : STAGE_NAME_(std::string(NAME).append("_StageBase"))
        , stageRegion_(gui::Display::Instance()->FullScreenRect())
        , entityPVec_()
        , entityWithFocusPtrOpt_()
        , hoverTextBoxUPtr_()
        , hoverText_()
        , isFading_(false)
        , isMouseHeldDown_(false)
        , isMouseHeldDownAndMoving_(false)
        , mouseDownPosV_(0.0f, 0.0f)
        , willClearCachesOnExit_(WILL_CLEAR_CACHE_ON_EXIT)
    {
        gui::FontManager::Instance()->Load(FONTS_TO_PRELOAD);
        gui::SoundManager::Instance()->PreLoadSfx(SFX_TO_PRELOAD);
    }

    StageBase::StageBase(
        const std::string & NAME,
        const sf::FloatRect & REGION,
        const gui::FontEnumVec_t & FONTS_TO_PRELOAD,
        const bool WILL_CLEAR_CACHE_ON_EXIT,
        const gui::SfxEnumVec_t & SFX_TO_PRELOAD)
        : STAGE_NAME_(std::string(NAME).append("_StageBase"))
        , stageRegion_(REGION)
        , entityPVec_()
        , entityWithFocusPtrOpt_()
        , hoverTextBoxUPtr_()
        , hoverText_()
        , isFading_(false)
        , isMouseHeldDown_(false)
        , isMouseHeldDownAndMoving_(false)
        , mouseDownPosV_(0.0f, 0.0f)
        , willClearCachesOnExit_(WILL_CLEAR_CACHE_ON_EXIT)
    {
        gui::FontManager::Instance()->Load(FONTS_TO_PRELOAD);
        gui::SoundManager::Instance()->PreLoadSfx(SFX_TO_PRELOAD);
    }

    StageBase::~StageBase()
    {
        if (willClearCachesOnExit_)
        {
            gui::SoundManager::Instance()->ClearSoundEffectsCache();
            gui::TextureCache::Instance()->RemoveAll();
            gui::FontManager::Instance()->UnloadAll();
        }
    }

    game::Phase::Enum StageBase::GetPhase() const
    {
        return game::GameController::Instance()->GetPhase();
    }

    void StageBase::UpdateTime(const float ELAPSED_TIME_SECONDS)
    {
        for (auto & entityPtr : entityPVec_)
        {
            entityPtr->UpdateTime(ELAPSED_TIME_SECONDS);
        }
    }

    void StageBase::UpdateMousePos(const sf::Vector2i & NEW_MOUSE_POS)
    {
        const sf::Vector2f NEW_MOUSE_POS_F(NEW_MOUSE_POS);

        isMouseHeldDownAndMoving_
            = (isMouseHeldDown_
               && (sfutil::Distance(mouseDownPosV_, NEW_MOUSE_POS_F) > MOUSE_DRAG_MIN_DISTANCE_));

        for (auto & entityPtr : entityPVec_)
        {
            entityPtr->UpdateMousePos(NEW_MOUSE_POS_F);
        }
    }

    void StageBase::UpdateMouseDown(const sf::Vector2f & MOUSE_POS_V)
    {
        isMouseHeldDown_ = true;
        mouseDownPosV_ = MOUSE_POS_V;

        for (auto & entityPtr : entityPVec_)
        {
            entityPtr->MouseDown(MOUSE_POS_V);
        }
    }

    const gui::IEntityPtrOpt_t StageBase::UpdateMouseUp(const sf::Vector2f & MOUSE_POS_V)
    {
        isMouseHeldDown_ = false;
        isMouseHeldDownAndMoving_ = false;

        for (const auto & ENTITY_PTR : entityPVec_)
        {
            if ((ENTITY_PTR->MouseUp(MOUSE_POS_V)) && ENTITY_PTR->WillAcceptFocus())
            {
                return ENTITY_PTR;
            }
        }

        return boost::none;
    }

    void StageBase::UpdateMouseWheel(const sf::Vector2f & MOUSE_POS_V, const float MOUSEWHEEL_DELTA)
    {
        for (auto & entityPtr : entityPVec_)
        {
            entityPtr->UpdateMouseWheel(MOUSE_POS_V, MOUSEWHEEL_DELTA);
        }
    }

    bool StageBase::KeyPress(const sf::Event::KeyEvent & KE)
    {
        return (entityWithFocusPtrOpt_ && (entityWithFocusPtrOpt_.value()->KeyPress(KE)));
    }

    bool StageBase::KeyRelease(const sf::Event::KeyEvent & KE)
    {
        return (entityWithFocusPtrOpt_ && (entityWithFocusPtrOpt_.value()->KeyRelease(KE)));
    }

    void StageBase::RemoveFocus()
    {
        if (entityWithFocusPtrOpt_)
        {
            entityWithFocusPtrOpt_.value()->SetHasFocus(false);
            entityWithFocusPtrOpt_ = boost::none;
        }
    }

    void StageBase::SetFocus(const gui::IEntityPtr_t ENTITY_PTR)
    {
        const auto ORIG_ENTITY_WITH_FOCUS_NAME {
            ((entityWithFocusPtrOpt_) ? entityWithFocusPtrOpt_.value()->GetEntityName() : "(None)")
        };

        const auto WAS_FOUND { std::find(std::begin(entityPVec_), std::end(entityPVec_), ENTITY_PTR)
                               != std::end(entityPVec_) };

        if (WAS_FOUND)
        {
            RemoveFocus();
            ENTITY_PTR->SetHasFocus(true);
            entityWithFocusPtrOpt_ = ENTITY_PTR;
        }
        else
        {
            M_HP_LOG_ERR(
                "stage::StageBase("
                << GetStageName() << ")::SetFocus(entity=" << ENTITY_PTR->GetEntityName()
                << ")  Attempt to set focus with an IEntityPtr_t that was not in entityPVec_.  "
                   "orig_enity_withfocus=\""
                << ORIG_ENTITY_WITH_FOCUS_NAME << "\"");
        }
    }

    void StageBase::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        for (auto & entityPtr : entityPVec_)
        {
            entityPtr->draw(target, states);
        }

        if (hoverTextBoxUPtr_)
        {
            target.draw(*hoverTextBoxUPtr_, states);
            target.draw(hoverText_, states);
        }
    }

    void StageBase::EntityAdd(const gui::IEntityPtr_t ENTITY_PTR, const bool WILL_INSERT_AT_FRONT)
    {
        const auto WAS_FOUND { std::find(std::begin(entityPVec_), std::end(entityPVec_), ENTITY_PTR)
                               != std::end(entityPVec_) };

        if (WAS_FOUND)
        {
            M_HP_LOG_WRN(
                "Ignoring because this entity was already in the list. (entity_name=\""
                << ENTITY_PTR->GetEntityName() << "\")" << M_HP_VAR_STR(WILL_INSERT_AT_FRONT)
                << "(stage=" << GetStageName() << ")");

            return;
        }

        if (WILL_INSERT_AT_FRONT)
        {
            entityPVec_.insert(std::begin(entityPVec_), ENTITY_PTR);
        }
        else
        {
            entityPVec_.emplace_back(ENTITY_PTR);
        }
    }

    void StageBase::EntityRemove(const gui::IEntityPtr_t ENTITY_PTR)
    {
        const auto ORIG_NUM_ENTITYS { entityPVec_.size() };

        entityPVec_.erase(
            std::remove(entityPVec_.begin(), entityPVec_.end(), ENTITY_PTR), entityPVec_.end());

        bool wasEntityFoundAndRemoved { (ORIG_NUM_ENTITYS != entityPVec_.size()) };

        if (entityWithFocusPtrOpt_ == ENTITY_PTR)
        {
            RemoveFocus();
            wasEntityFoundAndRemoved = true;
        }

        if (false == wasEntityFoundAndRemoved)
        {
            M_HP_LOG_WRN(
                "Entity to remove named \"" << ENTITY_PTR->GetEntityName() << "\" was not found.  "
                                            << "(stage=" << GetStageName() << ")");
        }
    }

    void StageBase::SetMouseHover(const sf::Vector2f & MOUSE_POS_V, const bool IS_MOUSE_HOVERING)
    {
        if (IS_MOUSE_HOVERING)
        {
            std::string text("");

            // check if focused entity is hovered first
            if (entityWithFocusPtrOpt_
                && (entityWithFocusPtrOpt_.value()->GetEntityRegion().contains(MOUSE_POS_V)))
            {
                text = entityWithFocusPtrOpt_.value()->GetMouseHoverText();
            }

            // if focused entity is not hovered, then look for any entity the mouse is hoving over
            if (text.empty())
            {
                for (const auto & NEXT_ENTITY_PTR : entityPVec_)
                {
                    if (NEXT_ENTITY_PTR->GetEntityRegion().contains(MOUSE_POS_V))
                    {
                        text = NEXT_ENTITY_PTR->GetMouseHoverText();

                        if (text.empty() == false)
                        {
                            break;
                        }
                    }
                }
            }

            if (text.empty())
            {
                if (hoverTextBoxUPtr_)
                {
                    hoverTextBoxUPtr_.reset();
                }

                return;
            }

            const gui::TextInfo TEXT_INFO(
                text,
                gui::GuiFont::System,
                gui::FontManager::Instance()->Size_Smallish(),
                sf::Color(50, 50, 50),
                gui::Justified::Left);

            hoverText_.setup(TEXT_INFO);

            sf::FloatRect region(
                MOUSE_POS_V.x - 200.0f,
                MOUSE_POS_V.y + 10.0f,
                hoverText_.getGlobalBounds().width + 20.0f,
                hoverText_.getGlobalBounds().height + 8.0f);

            const auto SCREEN_WIDTH { gui::Display::Instance()->GetWinWidth() };
            if ((region.left + region.width) > SCREEN_WIDTH)
            {
                region.left = SCREEN_WIDTH - region.width;
            }

            if (region.left < 0.0f)
            {
                region.left = 0.0f;
            }

            hoverText_.setPosition(region.left + 10.0f, region.top + 2.0f);

            gui::BoxEntityInfo boxInfo;
            boxInfo.SetupColor(sfutil::color::Orange - sf::Color(20, 0, 0, 0));
            boxInfo.SetupBorder(true, 1.0f);

            hoverTextBoxUPtr_
                = std::make_unique<gui::BoxEntity>(GetStageName() + "'sHoverText", region, boxInfo);
        }
        else
        {
            if (hoverTextBoxUPtr_)
            {
                hoverTextBoxUPtr_.reset();
            }
        }
    }

    void StageBase::TestingStrAppend(const std::string & MESSAGE)
    {
        game::GameController::Instance()->TestingStrAppend(MESSAGE);
    }

    void StageBase::TestingStrIncrement(const std::string & MESSAGE)
    {
        game::GameController::Instance()->TestingStrIncrement(MESSAGE);
    }

    void StageBase::TestingImageSet(const std::string & MESSAGE, const bool WILL_CHECK_FOR_OUTLINE)
    {
        game::GameController::Instance()->TestingImageSet(MESSAGE, WILL_CHECK_FOR_OUTLINE);
    }

    void StageBase::ClearAllEntities()
    {
        entityWithFocusPtrOpt_ = boost::none;
        entityPVec_.clear();
    }

    void StageBase::SpawnPopup(
        const gui::PopupCallback_t::IHandlerPtr_t & POPUP_HANDLER_PTR,
        const popup::PopupInfo & POPUP_INFO) const
    {
        game::GameController::Instance()->SpawnPopup(POPUP_HANDLER_PTR, POPUP_INFO);
    }

    void StageBase::RemovePopup(
        const popup::ResponseTypes::Enum TYPE, const std::size_t SELECTION) const
    {
        game::GameController::Instance()->RemovePopup(TYPE, SELECTION);
    }

    void StageBase::TransitionTo(const stage::Stage::Enum NEW_STAGE) const
    {
        game::GameController::Instance()->TransitionTo(NEW_STAGE);
    }

    void StageBase::TransitionTo(const stage::SetupPacket & SETUP_PACKET) const
    {
        game::GameController::Instance()->TransitionTo(SETUP_PACKET);
    }

    const gui::DisplayChangeResult StageBase::ChangeResolution(
        const gui::PopupCallback_t::IHandlerPtr_t & POPUP_HANDLER_PTR,
        const gui::Resolution & NEW_RES,
        const unsigned ANTIALIAS_LEVEL) const
    {
        return game::GameController::Instance()->ChangeResolution(
            POPUP_HANDLER_PTR, NEW_RES, ANTIALIAS_LEVEL);
    }

} // namespace stage
} // namespace heroespath
