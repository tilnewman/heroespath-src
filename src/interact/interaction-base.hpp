// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_INTERACT_INTERACTION_BASE_HPP_INCLUDED
#define HEROESPATH_INTERACT_INTERACTION_BASE_HPP_INCLUDED
//
// interaction-base.hpp
//
#include "gui/cached-texture.hpp"
#include "gui/sound-effects-enum.hpp"
#include "interact/i-interaction.hpp"
#include "interact/interaction-button.hpp"
#include "interact/interaction-text-enum.hpp"

#include <SFML/Graphics/Sprite.hpp>

#include <string>

namespace heroespath
{
namespace stage
{
    class InteractStage;
}
namespace interact
{

    // Responsible for wrapping the common state and operation of all Interactions.
    class InteractionBase : public IInteraction
    {
    public:
        InteractionBase(
            const Interact::Enum,
            const gui::TextInfo & TEXT,
            const ButtonVec_t & BUTTONS,
            const std::string & SUBJECT_IMAGE_KEY,
            const gui::sound_effect::Enum SFX_ENTER = gui::sound_effect::Count,
            const gui::sound_effect::Enum SFX_EXIT = gui::sound_effect::Count,
            const sf::Sprite & NPC_SPRITE = sf::Sprite());

        virtual ~InteractionBase() = default;

        Interact::Enum Type() const final { return interactionType_; }

        const gui::TextInfo & Text() const final { return text_; }

        ButtonVec_t & Buttons() final { return buttons_; }

        const sf::Texture & SubjectTexture() const final { return subjectCachedTexture_.Get(); }

        const sf::Texture & ContextTexture() const final { return contextCachedTexture_.Get(); }

        void PlayEnterSfx() const final;

        void PlayExitSfx() const final;

        static const gui::TextInfo MakeTextInfo(const std::string & TEXT, const Text::Enum TYPE);

        bool OnButtonClick(const stage::InteractStagePtr_t, const gui::TextButtonPtr_t) final;

        bool OnKeyRelease(const stage::InteractStagePtr_t, const sf::Keyboard::Key) final;

        void Lock() final { isLocked_ = true; }
        void Unlock() final { isLocked_ = false; }
        bool IsLocked() const final { return isLocked_; }

        bool OnSuccess(const stage::InteractStagePtr_t) override { return false; }
        bool OnFailure(const stage::InteractStagePtr_t) override { return false; }

        const sf::Sprite & NpcSprite() const final { return npcSprite_; }

    protected:
        void HandleIgnore(const stage::InteractStagePtr_t);
        const ButtonVec_t MakeButtonVecFromButtonEnumVec(const ButtonEnumVec_t &) const;

    private:
        virtual bool OnInteraction(const stage::InteractStagePtr_t, const Button &) = 0;

    private:
        Interact::Enum interactionType_;
        gui::TextInfo text_;
        ButtonVec_t buttons_;
        gui::CachedTexture subjectCachedTexture_;
        gui::CachedTexture contextCachedTexture_;
        gui::sound_effect::Enum sfxEnter_;
        gui::sound_effect::Enum sfxExit_;
        bool isLocked_;
        sf::Sprite npcSprite_;
    };

} // namespace interact
} // namespace heroespath

#endif // HEROESPATH_INTERACT_INTERACTION_HPP_INCLUDED
