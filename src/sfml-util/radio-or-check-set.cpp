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

#include "sfml-util/image-entity.hpp"
#include "sfml-util/radio-or-check-entity.hpp"
#include "sfml-util/text-entity.hpp"
#include "sfutil/size-and-scale.hpp"

namespace heroespath
{
namespace gui
{

    RadioOrCheckSet::RadioOrCheckSet(
        const std::string & NAME,
        const Callback_t::IHandlerPtrOpt_t & CALLBACK_HANDLER_PTR_OPT,
        RadioOrCheckEntityUVec_t ENTITY_UVEC,
        const float TOP_AND_BOTTOM_SPACER,
        const float LEFT_AND_RIGHT_SPACER,
        const float BETWEEN_IMAGE_AND_TEXT_HORIZ_SPACER,
        const float BETWEEN_ELEMENTS_VERT_SPACER)
        : Entity(NAME + "_RadioOrCheckSet_", 0.0f, 0.0f)
        , entityUVec_(std::move(ENTITY_UVEC))
        , callbackHandlerPtrOpt_(CALLBACK_HANDLER_PTR_OPT)
    {
        M_HP_ASSERT_OR_LOG_AND_THROW(
            (entityUVec_.empty() == false),
            "RadioOrCheckSet::RadioOrCheckSet() was given an ENTITY_UVEC that was empty.");

        PositionElements(
            TOP_AND_BOTTOM_SPACER,
            LEFT_AND_RIGHT_SPACER,
            BETWEEN_IMAGE_AND_TEXT_HORIZ_SPACER,
            BETWEEN_ELEMENTS_VERT_SPACER);

        if (entityUVec_.front()->IsRadioButton())
        {
            entityUVec_.front()->Select(true);
        }
    }

    RadioOrCheckSet::RadioOrCheckSet(
        const std::string & NAME,
        const Callback_t::IHandlerPtrOpt_t & CALLBACK_HANDLER_PTR_OPT,
        const bool IS_RADIO_BUTTON,
        const Brightness::Enum BRIGHTNESS,
        const MouseTextInfo & MOUSE_TEXT_INFO,
        const TitleValidPairVec_t & TITLE_VALID_PAIR_VEC,
        const float TOP_AND_BOTTOM_SPACER,
        const float LEFT_AND_RIGHT_SPACER,
        const float BETWEEN_IMAGE_AND_TEXT_HORIZ_SPACER,
        const float BETWEEN_ELEMENTS_VERT_SPACER)
        : Entity(NAME + "_RadioOrCheckSet_", 0.0f, 0.0f)
        , entityUVec_()
        , callbackHandlerPtrOpt_(CALLBACK_HANDLER_PTR_OPT)
    {
        M_HP_ASSERT_OR_LOG_AND_THROW(
            (TITLE_VALID_PAIR_VEC.empty() == false),
            "RadioOrCheckSet::RadioOrCheckSet() was given an TITLE_VALID_PAIR_VEC that was "
            "empty.");

        for (const auto & TITLE_VALID_PAIR : TITLE_VALID_PAIR_VEC)
        {
            MouseTextInfo mti(MOUSE_TEXT_INFO);
            mti.up.text = TITLE_VALID_PAIR.first;
            mti.down.text = TITLE_VALID_PAIR.first;
            mti.over.text = TITLE_VALID_PAIR.first;
            mti.disabled.text = TITLE_VALID_PAIR.first;

            entityUVec_.emplace_back(std::make_unique<gui::RadioOrCheckEntity>(
                "RadioOrCheckEntity_For_" + GetEntityName(),
                TITLE_VALID_PAIR.second,
                IS_RADIO_BUTTON,
                BRIGHTNESS,
                mti));
        }

        PositionElements(
            TOP_AND_BOTTOM_SPACER,
            LEFT_AND_RIGHT_SPACER,
            BETWEEN_IMAGE_AND_TEXT_HORIZ_SPACER,
            BETWEEN_ELEMENTS_VERT_SPACER);

        if (IS_RADIO_BUTTON)
        {
            entityUVec_.front()->Select(true);
        }
    }

    RadioOrCheckSet::~RadioOrCheckSet() = default;

    bool RadioOrCheckSet::MouseUp(const sf::Vector2f & MOUSE_POS_V)
    {
        const auto ENTITY_COUNT { entityUVec_.size() };

        std::size_t indexOfEntityThatChangedState { ENTITY_COUNT };

        for (std::size_t i(0); i < ENTITY_COUNT; ++i)
        {
            auto & entityUPtr { entityUVec_[i] };

            if (entityUPtr->MouseUp(MOUSE_POS_V))
            {
                indexOfEntityThatChangedState = i;
            }
        }

        if ((indexOfEntityThatChangedState < ENTITY_COUNT)
            && entityUVec_[indexOfEntityThatChangedState]->IsRadioButton()
            && entityUVec_[indexOfEntityThatChangedState]->IsSelected())
        {
            UnSelectAllExcept(indexOfEntityThatChangedState);
            TriggerCallbackForChangedIndex(indexOfEntityThatChangedState);
            return true;
        }
        else
        {
            return false;
        }
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
            if (entityUVec_[i]->IsSelected())
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
            if (entityUVec_[i]->IsSelected())
            {
                return i;
            }
        }

        return boost::none;
    }

    bool RadioOrCheckSet::IsSelected(const std::size_t INDEX) const
    {
        return ((INDEX < entityUVec_.size()) && entityUVec_[INDEX]->IsSelected());
    }

    bool RadioOrCheckSet::SelectIndex(const std::size_t INDEX)
    {
        if ((INDEX < entityUVec_.size()) && !entityUVec_[INDEX]->IsSelected())
        {
            entityUVec_[INDEX]->Select(true);

            if (entityUVec_[INDEX]->IsRadioButton())
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
        if ((INDEX < entityUVec_.size()) && entityUVec_[INDEX]->IsSelected()
            && !entityUVec_[INDEX]->IsRadioButton())
        {
            entityUVec_[INDEX]->Select(false);
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
            if (entityUVec_[INDEX]->IsSelected())
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

    void RadioOrCheckSet::TriggerCallbackForChangedIndex(const std::size_t INDEX)
    {
        if (callbackHandlerPtrOpt_)
        {
            Callback_t::Packet_t callbackPacket(this, INDEX);

            callbackHandlerPtrOpt_.value()->HandleCallback(
                Callback_t::PacketPtr_t(&callbackPacket));
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
            if ((i != INDEX) && (entityUVec_[i]->IsSelected()))
            {
                entityUVec_[i]->Select(false);
            }
        }
    }

} // namespace gui
} // namespace heroespath
