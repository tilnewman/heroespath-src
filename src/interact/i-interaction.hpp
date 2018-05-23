// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_INTERACT_I_INTERACTION_HPP_INCLUDED
#define HEROESPATH_INTERACT_I_INTERACTION_HPP_INCLUDED
//
// i-interaction.hpp
//
#include "interact/interact-enum.hpp"
#include "interact/interaction-button.hpp"
#include "misc/not-null.hpp"
#include "sfml-util/gui/text-button.hpp"
#include "sfml-util/gui/text-info.hpp"
#include "sfml-util/sfml-graphics.hpp"

#include <memory>

namespace heroespath
{
namespace stage
{
    class InteractStage;
    using InteractStagePtr_t = misc::NotNull<InteractStage *>;
} // namespace stage
namespace interact
{

    // Pure virtual interface for all Interactions objects.
    struct IInteraction
    {
        virtual ~IInteraction() = default;

        virtual Interact::Enum Type() const = 0;
        virtual const sfml_util::gui::TextInfo & Text() const = 0;
        virtual ButtonVec_t & Buttons() = 0;
        virtual const sf::Texture & SubjectTexture() const = 0;
        virtual const sf::Texture & ContextTexture() const = 0;
        virtual void PlayEnterSfx() const = 0;
        virtual void PlayExitSfx() const = 0;

        virtual bool
            OnButtonClick(const stage::InteractStagePtr_t, const sfml_util::gui::TextButtonPtr_t)
            = 0;

        virtual bool OnKeyRelease(const stage::InteractStagePtr_t, const sf::Keyboard::Key) = 0;

        // a locked interaction is one the player cannot simply walk away from
        virtual void Lock() = 0;
        virtual void Unlock() = 0;
        virtual bool IsLocked() const = 0;

        // these functions don't make sense for all interactions.  For example, Conversations don't
        // need these but Locked Doors do.
        virtual bool OnSuccess(const stage::InteractStagePtr_t) = 0;
        virtual bool OnFailure(const stage::InteractStagePtr_t) = 0;

        virtual const sf::Sprite & NpcSprite() const = 0;
    };

    using InteractionUPtr_t = std::unique_ptr<IInteraction>;

} // namespace interact
} // namespace heroespath

#endif // HEROESPATH_INTERACT_I_INTERACTION_HPP_INCLUDED
