// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// stage.cpp
//
#include "stage.hpp"

#include "game/loop-manager.hpp"
#include "sfml-util/display.hpp"
#include "sfml-util/font-manager.hpp"
#include "sfml-util/gui/box-info.hpp"
#include "sfml-util/gui/box.hpp"
#include "sfml-util/gui/gui-entity.hpp"
#include "sfml-util/gui/text-info.hpp"
#include "sfml-util/sfml-util.hpp"
#include "sfml-util/sound-manager.hpp"
#include "sfml-util/texture-cache.hpp"

#include <algorithm>
#include <exception>
#include <sstream>

namespace heroespath
{
namespace sfml_util
{

    const float Stage::MOUSE_DRAG_MIN_DISTANCE_ { 3.0f };

    Stage::Stage(
        const std::string & NAME,
        const FontEnumVec_t & FONTS_TO_PRELOAD,
        const bool WILL_CLEAR_CACHE_ON_EXIT,
        const SfxEnumVec_t & SFX_TO_PRELOAD)
        : STAGE_NAME_(std::string(NAME).append("_Stage"))
        , stageRegion_(sf::FloatRect(
              0.0f, 0.0f, Display::Instance()->GetWinWidth(), Display::Instance()->GetWinHeight()))
        , entityPVec_()
        , entityWithFocusPtrOpt_(boost::none)
        , hoverTextBoxUPtr_()
        , hoverSfText_()
        , isMouseHeldDown_(false)
        , isMouseHeldDownAndMoving_(false)
        , mouseDownPosV_(0.0f, 0.0f)
        , willClearCachesOnExit_(WILL_CLEAR_CACHE_ON_EXIT)
    {
        FontManager::Instance()->Load(FONTS_TO_PRELOAD);
        SoundManager::Instance()->PreLoadSfx(SFX_TO_PRELOAD);
    }

    Stage::Stage(
        const std::string & NAME,
        const sf::FloatRect & REGION,
        const FontEnumVec_t & FONTS_TO_PRELOAD,
        const bool WILL_CLEAR_CACHE_ON_EXIT,
        const SfxEnumVec_t & SFX_TO_PRELOAD)
        : STAGE_NAME_(std::string(NAME).append("_Stage"))
        , stageRegion_(REGION)
        , entityPVec_()
        , entityWithFocusPtrOpt_(boost::none)
        , hoverTextBoxUPtr_()
        , hoverSfText_()
        , isMouseHeldDown_(false)
        , isMouseHeldDownAndMoving_(false)
        , mouseDownPosV_(0.0f, 0.0f)
        , willClearCachesOnExit_(WILL_CLEAR_CACHE_ON_EXIT)
    {
        FontManager::Instance()->Load(FONTS_TO_PRELOAD);
        SoundManager::Instance()->PreLoadSfx(SFX_TO_PRELOAD);
    }

    Stage::Stage(
        const std::string & NAME,
        const float REGION_LEFT,
        const float REGION_TOP,
        const float REGION_WIDTH,
        const float REGION_HEIGHT,
        const FontEnumVec_t & FONTS_TO_PRELOAD,
        const bool WILL_CLEAR_CACHE_ON_EXIT,
        const SfxEnumVec_t & SFX_TO_PRELOAD)
        : STAGE_NAME_(std::string(NAME).append("_Stage"))
        , stageRegion_(sf::FloatRect(REGION_LEFT, REGION_TOP, REGION_WIDTH, REGION_HEIGHT))
        , entityPVec_()
        , entityWithFocusPtrOpt_(boost::none)
        , hoverTextBoxUPtr_()
        , hoverSfText_()
        , isMouseHeldDown_(false)
        , isMouseHeldDownAndMoving_(false)
        , mouseDownPosV_(0.0f, 0.0f)
        , willClearCachesOnExit_(WILL_CLEAR_CACHE_ON_EXIT)
    {
        FontManager::Instance()->Load(FONTS_TO_PRELOAD);
        SoundManager::Instance()->PreLoadSfx(SFX_TO_PRELOAD);
    }

    Stage::~Stage()
    {
        if (willClearCachesOnExit_)
        {
            SoundManager::Instance()->ClearSoundEffectsCache();
            TextureCache::Instance()->RemoveAll();
            FontManager::Instance()->UnloadAll();
        }
    }

    void Stage::UpdateTime(const float ELAPSED_TIME_SECONDS)
    {
        std::for_each(
            entityPVec_.begin(), entityPVec_.end(), [ELAPSED_TIME_SECONDS](auto const ENTITY_PTR) {
                ENTITY_PTR->UpdateTime(ELAPSED_TIME_SECONDS);
            });
    }

    void Stage::UpdateMousePos(const sf::Vector2i & NEW_MOUSE_POS)
    {
        const sf::Vector2f NEW_MOUSE_POS_F(NEW_MOUSE_POS);

        isMouseHeldDownAndMoving_
            = (isMouseHeldDown_
               && (sfml_util::Distance(mouseDownPosV_, NEW_MOUSE_POS_F)
                   > MOUSE_DRAG_MIN_DISTANCE_));

        std::for_each(
            entityPVec_.begin(), entityPVec_.end(), [&NEW_MOUSE_POS_F](auto const ENTITY_PTR) {
                ENTITY_PTR->UpdateMousePos(NEW_MOUSE_POS_F);
            });
    }

    void Stage::UpdateMouseDown(const sf::Vector2f & MOUSE_POS_V)
    {
        isMouseHeldDown_ = true;
        mouseDownPosV_ = MOUSE_POS_V;

        std::for_each(
            entityPVec_.begin(), entityPVec_.end(), [&MOUSE_POS_V](auto const ENTITY_PTR) {
                ENTITY_PTR->MouseDown(MOUSE_POS_V);
            });
    }

    const gui::IGuiEntityPtrOpt_t Stage::UpdateMouseUp(const sf::Vector2f & MOUSE_POS_V)
    {
        isMouseHeldDown_ = false;
        isMouseHeldDownAndMoving_ = false;

        for (auto const & ENTITY_PTR : entityPVec_)
        {
            if ((ENTITY_PTR->MouseUp(MOUSE_POS_V)) && ENTITY_PTR->WillAcceptFocus())
            {
                ENTITY_PTR->SetHasFocus(true);
                entityWithFocusPtrOpt_ = ENTITY_PTR;
                break; // can only find one entity with focus
            }
        }

        return entityWithFocusPtrOpt_;
    }

    void Stage::UpdateMouseWheel(const sf::Vector2f & MOUSE_POS_V, const float MOUSEWHEEL_DELTA)
    {
        std::for_each(
            entityPVec_.begin(),
            entityPVec_.end(),
            [&MOUSE_POS_V, MOUSEWHEEL_DELTA](auto const ENTITY_PTR) {
                ENTITY_PTR->UpdateMouseWheel(MOUSE_POS_V, MOUSEWHEEL_DELTA);
            });
    }

    bool Stage::KeyPress(const sf::Event::KeyEvent & KE)
    {
        return (entityWithFocusPtrOpt_ && (entityWithFocusPtrOpt_.value()->KeyPress(KE)));
    }

    bool Stage::KeyRelease(const sf::Event::KeyEvent & KE)
    {
        return (entityWithFocusPtrOpt_ && (entityWithFocusPtrOpt_.value()->KeyRelease(KE)));
    }

    void Stage::RemoveFocus()
    {
        entityWithFocusPtrOpt_ = boost::none;

        std::for_each(entityPVec_.begin(), entityPVec_.end(), [](auto const ENTITY_PTR) {
            ENTITY_PTR->SetHasFocus(false);
        });
    }

    void Stage::SetFocus(const gui::IGuiEntityPtr_t ENTITY_PTR)
    {
        const auto ORIG_ENTITY_WITH_FOCUS_NAME {
            ((entityWithFocusPtrOpt_) ? entityWithFocusPtrOpt_.value()->GetEntityName() : "(None)")
        };

        // TODO Should we eliminate the current focus before we know if we are setting a new focus?
        entityWithFocusPtrOpt_ = boost::none;

        auto const WAS_FOUND { std::find(std::begin(entityPVec_), std::end(entityPVec_), ENTITY_PTR)
                               != std::end(entityPVec_) };

        if (WAS_FOUND)
        {
            entityWithFocusPtrOpt_ = ENTITY_PTR;
        }
        else
        {
            M_HP_LOG_ERR(
                "sfml_util::Stage::SetFocus(entity="
                << ENTITY_PTR->GetEntityName()
                << ")  Attempt to set focus with an IGuiEntityPtr_t that was not in entityPVec_.  "
                   "orig_enity_with_focus=\""
                << ORIG_ENTITY_WITH_FOCUS_NAME << "\"");
        }
    }

    void Stage::Draw(sf::RenderTarget & target, const sf::RenderStates & STATES)
    {
        std::for_each(
            entityPVec_.begin(), entityPVec_.end(), [&target, &STATES](const auto ENTITY_PTR) {
                ENTITY_PTR->draw(target, STATES);
            });

        DrawHoverText(target, STATES);
    }

    void Stage::EntityAdd(const gui::IGuiEntityPtr_t ENTITY_PTR)
    {
        auto const WAS_FOUND { std::find(std::begin(entityPVec_), std::end(entityPVec_), ENTITY_PTR)
                               != std::end(entityPVec_) };

        M_ASSERT_OR_LOGANDTHROW_SS(
            (WAS_FOUND == false),
            "sfml_util::Stage::EntityAdd(\""
                << ENTITY_PTR->GetEntityName()
                << "\") tried to add but it was already in the entityPVec_.");

        entityPVec_.emplace_back(ENTITY_PTR);
    }

    bool Stage::EntityRemove(const gui::IGuiEntityPtr_t ENTITY_PTR)
    {
        auto const ORIG_NUM_ENTITYS { entityPVec_.size() };

        entityPVec_.erase(
            std::remove(entityPVec_.begin(), entityPVec_.end(), ENTITY_PTR), entityPVec_.end());

        if (entityWithFocusPtrOpt_ == ENTITY_PTR)
        {
            entityWithFocusPtrOpt_ = boost::none;
            return true;
        }
        else
        {
            return !(ORIG_NUM_ENTITYS == entityPVec_.size());
        }
    }

    void Stage::SetMouseHover(const sf::Vector2f & MOUSE_POS_V, const bool IS_MOUSE_HOVERING)
    {
        if (IS_MOUSE_HOVERING)
        {
            std::string hoverText("");

            // check if focused entity is hovered first
            if (entityWithFocusPtrOpt_
                && (entityWithFocusPtrOpt_.value()->GetEntityRegion().contains(MOUSE_POS_V)))
            {
                hoverText = entityWithFocusPtrOpt_.value()->GetMouseHoverText();
            }

            // if focused entity is not hovered, then look for any entity the mouse is hoving over
            if (hoverText.empty())
            {
                for (auto const & NEXT_ENTITY_PTR : entityPVec_)
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
                if (hoverTextBoxUPtr_)
                {
                    hoverTextBoxUPtr_.reset();
                }

                return;
            }

            const gui::TextInfo TEXT_INFO(
                hoverText,
                sfml_util::FontManager::Instance()->GetFont(sfml_util::Font::System),
                FontManager::Instance()->Size_Smallish(),
                sf::Color(50, 50, 50),
                Justified::Left);

            TEXT_INFO.Apply(hoverSfText_);

            sf::FloatRect region(
                MOUSE_POS_V.x - 200.0f,
                MOUSE_POS_V.y + 10.0f,
                hoverSfText_.getGlobalBounds().width + 20.0f,
                hoverSfText_.getGlobalBounds().height + 8.0f);

            auto const SCREEN_WIDTH { Display::Instance()->GetWinWidth() };
            if ((region.left + region.width) > SCREEN_WIDTH)
            {
                region.left = SCREEN_WIDTH - region.width;
            }

            if (region.left < 0.0f)
            {
                region.left = 0.0f;
            }

            hoverSfText_.setPosition(region.left + 10.0f, region.top + 2.0f);

            const gui::BackgroundInfo BG_INFO(sfml_util::Colors::Orange - sf::Color(20, 0, 0, 0));

            const gui::box::Info BOX_INFO(1, true, region, gui::ColorSet(), BG_INFO);

            hoverTextBoxUPtr_
                = std::make_unique<gui::box::Box>(GetStageName() + "'sHoverText", BOX_INFO);
        }
        else
        {
            if (hoverTextBoxUPtr_)
            {
                hoverTextBoxUPtr_.reset();
            }
        }
    }

    void Stage::ClearAllEntities()
    {
        entityWithFocusPtrOpt_ = boost::none;
        entityPVec_.clear();
    }

    void Stage::DrawHoverText(sf::RenderTarget & target, const sf::RenderStates & STATES)
    {
        if (hoverTextBoxUPtr_)
        {
            target.draw(*hoverTextBoxUPtr_, STATES);
            target.draw(hoverSfText_, STATES);
        }
    }

} // namespace sfml_util
} // namespace heroespath
