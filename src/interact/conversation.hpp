// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_INTERACT_CONVERSATION_HPP_INCLUDED
#define HEROESPATH_INTERACT_CONVERSATION_HPP_INCLUDED
//
//  conversation.hpp
//
#include "interact/interaction-base.hpp"
#include "map/transition.hpp"

namespace sf
{
class Sprite;
}

namespace heroespath
{
namespace game
{
    class Npc;
    using NpcPtr_t = misc::NotNull<Npc *>;
} // namespace game
namespace interact
{

    // Responsible for conversation interactions with NPCs.
    class Conversation : public InteractionBase
    {
    public:
        explicit Conversation(const game::NpcPtr_t, const sf::Sprite & NPC_SPRITE);

        virtual ~Conversation() = default;

    private:
        bool OnInteraction(const stage::InteractStagePtr_t, const Button &) final;

    private:
        game::NpcPtr_t npcPtr_;
    };

} // namespace interact
} // namespace heroespath

#endif // HEROESPATH_INTERACT_CONVERSATION_HPP_INCLUDED
