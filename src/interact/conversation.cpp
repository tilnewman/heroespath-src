// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// conversation.cpp
//
#include "conversation.hpp"

#include "game/npc.hpp"
#include "interact/interaction-factory.hpp"
#include "stage/adventure-stage-interact-stage.hpp"

namespace heroespath
{
namespace interact
{

    Conversation::Conversation(const game::NpcPtr_t NPC_PTR, const sf::Sprite & NPC_SPRITE)
        : InteractionBase(
            Interact::Conversation,
            InteractionBase::MakeTextInfo(NPC_PTR->ConversationPoint().Text(), Text::Dialog),
            MakeButtonVecFromButtonEnumVec(NPC_PTR->ConversationPoint().Buttons()),
            "media-image-misc-talk",
            gui::sound_effect::Count,
            gui::sound_effect::Count,
            NPC_SPRITE)
        , npcPtr_(NPC_PTR)
    {}

    const std::string Conversation::OnInteraction(
        const stage::InteractStagePtr_t INTERACTION_STAGE_PTR, const Button & BUTTON)
    {
        INTERACTION_STAGE_PTR->InteractionManager().RemoveCurrent();

        if (npcPtr_->ApplyConversationResponse(BUTTON.Which()) == false)
        {
            INTERACTION_STAGE_PTR->InteractionManager().SetNext(
                interact::InteractionFactory::MakeConversation(npcPtr_, NpcSprite()));
        }

        return "";
    }

} // namespace interact
} // namespace heroespath
