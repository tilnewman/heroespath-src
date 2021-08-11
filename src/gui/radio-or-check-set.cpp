// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// radio-or-check-set.cpp
//
#include "radio-or-check-set.hpp"

#include "gui/box-entity.hpp"
#include "gui/image-entity.hpp"
#include "gui/radio-or-check-entity.hpp"
#include "gui/text-entity.hpp"
#include "sfutil/primitives.hpp"
#include "sfutil/size-and-scale.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

namespace heroespath
{
namespace gui
{

    RadioOrCheckSet::EventPacket::EventPacket(
        const misc::NotNull<RadioOrCheckSet *> SET_PTR, const std::size_t CHANGED_INDEX)
        : set_ptr(SET_PTR)
        , has_changed_index(CHANGED_INDEX < SET_PTR->Size())
        , changed_index(((CHANGED_INDEX < SET_PTR->Size()) ? CHANGED_INDEX : SET_PTR->Size()))
        , is_changed_index_selected(SET_PTR->IsSelected(CHANGED_INDEX))
    {}

    RadioOrCheckSet::RadioOrCheckSet(
        const std::string & NAME,
        const Callback_t::IHandlerPtr_t & CALLBACK_HANDLER_PTR,
        RadioOrCheckEntityUVec_t ENTITY_UVEC,
        const BoxEntityInfo & BOX_INFO,
        const float TOP_AND_BOTTOM_SPACER,
        const float LEFT_AND_RIGHT_SPACER,
        const float BETWEEN_IMAGE_AND_TEXT_HORIZ_SPACER,
        const float BETWEEN_ELEMENTS_VERT_SPACER)
        : Entity(NAME + "_RadioOrCheckSet_", 0.0f, 0.0f)
        , entityUVec_(std::move(ENTITY_UVEC))
        , callbackHandlerPtr_(CALLBACK_HANDLER_PTR)
        , boxUPtr_()
    {
        Setup(
            BOX_INFO,
            TOP_AND_BOTTOM_SPACER,
            LEFT_AND_RIGHT_SPACER,
            BETWEEN_IMAGE_AND_TEXT_HORIZ_SPACER,
            BETWEEN_ELEMENTS_VERT_SPACER);
    }

    RadioOrCheckSet::RadioOrCheckSet(
        const std::string & NAME,
        const Callback_t::IHandlerPtr_t & CALLBACK_HANDLER_PTR,
        const bool IS_RADIO_BUTTON,
        const Brightness::Enum BRIGHTNESS,
        const MouseTextInfo & MOUSE_TEXT_INFO,
        const std::vector<std::string> & TITLE_VEC,
        const BoxEntityInfo & BOX_INFO,
        const float TOP_AND_BOTTOM_SPACER,
        const float LEFT_AND_RIGHT_SPACER,
        const float BETWEEN_IMAGE_AND_TEXT_HORIZ_SPACER,
        const float BETWEEN_ELEMENTS_VERT_SPACER)
        : Entity(NAME + "_RadioOrCheckSet_", 0.0f, 0.0f)
        , entityUVec_()
        , callbackHandlerPtr_(CALLBACK_HANDLER_PTR)
        , boxUPtr_()
    {
        for (const auto & TITLE_STR : TITLE_VEC)
        {
            auto mouseTextInfo(MOUSE_TEXT_INFO);
            mouseTextInfo.up.text = TITLE_STR;
            mouseTextInfo.down.text = TITLE_STR;
            mouseTextInfo.over.text = TITLE_STR;
            mouseTextInfo.disabled.text = TITLE_STR;

            entityUVec_.emplace_back(std::make_unique<gui::RadioOrCheckEntity>(
                "RadioOrCheckEntity_For_" + GetEntityName(),
                true,
                IS_RADIO_BUTTON,
                BRIGHTNESS,
                mouseTextInfo));
        }

        Setup(
            BOX_INFO,
            TOP_AND_BOTTOM_SPACER,
            LEFT_AND_RIGHT_SPACER,
            BETWEEN_IMAGE_AND_TEXT_HORIZ_SPACER,
            BETWEEN_ELEMENTS_VERT_SPACER);
    }

    RadioOrCheckSet::~RadioOrCheckSet() = default;

    void RadioOrCheckSet::draw(sf::RenderTarget & target, sf::RenderStates states) const
    {
        if (boxUPtr_)
        {
            target.draw(*boxUPtr_, states);
        }

        for (auto & entityUPtr : entityUVec_)
        {
            target.draw(*entityUPtr, states);
        }
    }

    void RadioOrCheckSet::SetEntityPos(const float POS_LEFT, const float POS_TOP)
    {
        MoveEntityPos((POS_LEFT - entityRegion_.left), (POS_TOP - entityRegion_.top));
    }

    void RadioOrCheckSet::MoveEntityPos(const float HORIZ, const float VERT)
    {
        Entity::MoveEntityPos(HORIZ, VERT);

        if (boxUPtr_)
        {
            boxUPtr_->MoveEntityPos(HORIZ, VERT);
        }

        for (auto & entityUPtr : entityUVec_)
        {
            entityUPtr->MoveEntityPos(HORIZ, VERT);
        }
    }

    bool RadioOrCheckSet::MouseUp(const sf::Vector2f & MOUSE_POS_V)
    {
        if (entityUVec_.empty())
        {
            return false;
        }

        const auto ENTITY_COUNT { entityUVec_.size() };

        std::size_t indexOfEntityThatChangedState { ENTITY_COUNT };

        for (std::size_t i(0); i < ENTITY_COUNT; ++i)
        {
            auto & entityUPtr { entityUVec_.at(i) };

            if (entityUPtr->MouseUp(MOUSE_POS_V))
            {
                indexOfEntityThatChangedState = i;
            }
        }

        if (indexOfEntityThatChangedState >= ENTITY_COUNT
            || !entityUVec_.at(indexOfEntityThatChangedState)->IsRadioButton()
            || !entityUVec_.at(indexOfEntityThatChangedState)->IsSelected())
        {
            return false;
        }

        UnSelectAllExcept(indexOfEntityThatChangedState);
        TriggerCallbackForChangedIndex(indexOfEntityThatChangedState);
        return true;
    }

    bool RadioOrCheckSet::MouseDown(const sf::Vector2f & MOUSE_POS_V)
    {
        bool wasHandled { false };
        for (auto & entityUPtr : entityUVec_)
        {
            if (entityUPtr->MouseDown(MOUSE_POS_V))
            {
                wasHandled = true;
            }
        }

        return wasHandled;
    }

    bool RadioOrCheckSet::UpdateMousePos(const sf::Vector2f & MOUSE_POS_V)
    {
        bool didMouseEnterOrExitAny { false };
        for (auto & entityUPtr : entityUVec_)
        {
            if (entityUPtr->UpdateMousePos(MOUSE_POS_V))
            {
                didMouseEnterOrExitAny = true;
            }
        }

        return didMouseEnterOrExitAny;
    }

    const std::vector<std::size_t> RadioOrCheckSet::SelectedIndexes() const
    {
        std::vector<std::size_t> selectedIndexes;

        const auto ENTITY_COUNT { entityUVec_.size() };
        for (std::size_t i(0); i < ENTITY_COUNT; ++i)
        {
            if (entityUVec_.at(i)->IsSelected())
            {
                selectedIndexes.emplace_back(i);
            }
        }

        return selectedIndexes;
    }

    const SizetOpt_t RadioOrCheckSet::FirstSelectedIndex() const
    {
        const auto ENTITY_COUNT { entityUVec_.size() };
        for (std::size_t i(0); i < ENTITY_COUNT; ++i)
        {
            if (entityUVec_.at(i)->IsSelected())
            {
                return i;
            }
        }

        return boost::none;
    }

    bool RadioOrCheckSet::IsSelected(const std::size_t INDEX) const
    {
        return ((INDEX < entityUVec_.size()) && entityUVec_.at(INDEX)->IsSelected());
    }

    bool RadioOrCheckSet::SelectIndex(const std::size_t INDEX)
    {
        if ((INDEX < entityUVec_.size()) && !entityUVec_.at(INDEX)->IsSelected())
        {
            entityUVec_.at(INDEX)->Select(true);

            if (entityUVec_.at(INDEX)->IsRadioButton())
            {
                UnSelectAllExcept(INDEX);
            }

            TriggerCallbackForChangedIndex(INDEX);
            return true;
        }
        else
        {
            return false;
        }
    }

    bool RadioOrCheckSet::UnSelectIndex(const std::size_t INDEX)
    {
        if ((INDEX < entityUVec_.size()) && entityUVec_.at(INDEX)->IsSelected()
            && !entityUVec_.at(INDEX)->IsRadioButton())
        {
            entityUVec_.at(INDEX)->Select(false);
            TriggerCallbackForChangedIndex(INDEX);
            return true;
        }
        else
        {
            return false;
        }
    }

    bool RadioOrCheckSet::ChangeIndex(const std::size_t INDEX)
    {
        if (INDEX < entityUVec_.size())
        {
            if (entityUVec_.at(INDEX)->IsSelected())
            {
                return UnSelectIndex(INDEX);
            }
            else
            {
                return SelectIndex(INDEX);
            }
        }

        return false;
    }

    bool RadioOrCheckSet::Invalid(const std::size_t INDEX) const
    {
        return ((INDEX < entityUVec_.size()) && entityUVec_.at(INDEX)->IsInvalid());
    }

    void RadioOrCheckSet::Invalid(const std::size_t INDEX, const bool IS_INVALID)
    {
        if (INDEX < entityUVec_.size())
        {
            entityUVec_.at(INDEX)->Invalid(IS_INVALID);
        }
    }

    void RadioOrCheckSet::TriggerCallbackForChangedIndex(const std::size_t INDEX)
    {
        if (entityUVec_.empty())
        {
            return;
        }

        const Callback_t::Packet_t EVENT_PACKET(misc::MakeNotNull(this), INDEX);

        std::ostringstream ss;
        ss << "RadioOrCheckSet(" << GetEntityName() << "\", index-changed=" << INDEX << ")";

        Callback_t::HandleAndLog(*callbackHandlerPtr_, EVENT_PACKET, ss.str());
    }

    void RadioOrCheckSet::Setup(
        const BoxEntityInfo & BOX_INFO,
        const float TOP_AND_BOTTOM_SPACER,
        const float LEFT_AND_RIGHT_SPACER,
        const float BETWEEN_IMAGE_AND_TEXT_HORIZ_SPACER,
        const float BETWEEN_ELEMENTS_VERT_SPACER)
    {
        if (entityUVec_.empty())
        {
            M_HP_LOG_WRN(
                "RadioOrCheckSet constructed with an empty ENTITY_UVEC or an empty TITLE_VEC.");

            return;
        }

        PositionElements(
            TOP_AND_BOTTOM_SPACER,
            LEFT_AND_RIGHT_SPACER,
            BETWEEN_IMAGE_AND_TEXT_HORIZ_SPACER,
            BETWEEN_ELEMENTS_VERT_SPACER);

        if (BOX_INFO != BoxEntityInfo())
        {
            boxUPtr_
                = std::make_unique<BoxEntity>(GetEntityName() + "'s_", GetEntityRegion(), BOX_INFO);
        }

        if (entityUVec_.front()->IsRadioButton())
        {
            entityUVec_.front()->Select(true);
        }
    }

    void RadioOrCheckSet::PositionElements(
        const float TOP_AND_BOTTOM_SPACER,
        const float LEFT_AND_RIGHT_SPACER,
        const float BETWEEN_IMAGE_AND_TEXT_HORIZ_SPACER,
        const float BETWEEN_ELEMENTS_VERT_SPACER)
    {
        const auto IMAGE_POS_LEFT { GetEntityPos().x + LEFT_AND_RIGHT_SPACER };
        auto posY { GetEntityPos().y + TOP_AND_BOTTOM_SPACER };
        auto maxPosX { 0.0f };
        for (auto & entityUPtr : entityUVec_)
        {
            const auto TEXT_POS_TOP { posY + (BETWEEN_ELEMENTS_VERT_SPACER * 0.5f) };

            const auto TEXT_SIZE_V { sfutil::Size(entityUPtr->TextEntity()->GetEntityRegion()) };

            const auto IMAGE_SIZE_V { sfutil::Size(entityUPtr->ImageEntity()->GetEntityRegion()) };

            const auto IMAGE_POS_TOP { (TEXT_POS_TOP + (TEXT_SIZE_V.y * 0.5f))
                                       - (IMAGE_SIZE_V.y * 0.5f) };

            entityUPtr->ImageEntity()->SetEntityPos(IMAGE_POS_LEFT, IMAGE_POS_TOP);

            const auto TEXT_POS_LEFT { IMAGE_POS_LEFT + IMAGE_SIZE_V.x
                                       + BETWEEN_IMAGE_AND_TEXT_HORIZ_SPACER };

            entityUPtr->TextEntity()->SetEntityPos(TEXT_POS_LEFT, TEXT_POS_TOP);

            const auto POS_RIGHT { TEXT_POS_LEFT + TEXT_SIZE_V.x + LEFT_AND_RIGHT_SPACER };
            if (POS_RIGHT > maxPosX)
            {
                maxPosX = POS_RIGHT;
            }

            posY += TEXT_SIZE_V.y + BETWEEN_ELEMENTS_VERT_SPACER;
        }

        posY -= BETWEEN_ELEMENTS_VERT_SPACER;
        posY += TOP_AND_BOTTOM_SPACER;

        const sf::Vector2f EXTENT_V(maxPosX, posY);

        SetEntityRegion(sf::FloatRect(GetEntityPos(), EXTENT_V - GetEntityPos()));
    }

    void RadioOrCheckSet::UnSelectAllExcept(const std::size_t INDEX)
    {
        const auto ENTITY_COUNT { entityUVec_.size() };
        for (std::size_t i(0); i < ENTITY_COUNT; ++i)
        {
            if ((i != INDEX) && (entityUVec_.at(i)->IsSelected()))
            {
                entityUVec_.at(i)->Select(false);
            }
        }
    }

} // namespace gui
} // namespace heroespath
