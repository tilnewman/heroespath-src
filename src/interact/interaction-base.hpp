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
#include "interact/i-interaction.hpp"
#include "interact/interaction-button.hpp"
#include "interact/interaction-text-enum.hpp"
#include "sfml-util/sound-effects-enum.hpp"
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
            const sfml_util::gui::TextInfo & TEXT,
            const ButtonVec_t & BUTTONS,
            const std::string & SUBJECT_IMAGE_KEY,
            const sfml_util::sound_effect::Enum SFX_ENTER = sfml_util::sound_effect::Count,
            const sfml_util::sound_effect::Enum SFX_EXIT = sfml_util::sound_effect::Count);

        virtual ~InteractionBase() = default;

        Interact::Enum Type() const final { return interactionType_; }

        const sfml_util::gui::TextInfo & Text() const final { return text_; }

        ButtonVec_t & Buttons() final { return buttons_; }

        const sf::Texture & SubjectTexture() const final { return subjectTexture_; }

        const sf::Texture & ContextTexture() const final { return contextTexture_; }

        void PlayEnterSfx() const final;

        void PlayExitSfx() const final;

        static const sfml_util::gui::TextInfo
            MakeTextInfo(const std::string & TEXT, const Text::Enum TYPE);

        bool OnButtonClick(
            const stage::InteractStagePtr_t, const sfml_util::gui::TextButtonPtr_t) final;

        bool OnKeyRelease(const stage::InteractStagePtr_t, const sf::Keyboard::Key) final;

        void Lock() final { isLocked_ = true; }
        void Unlock() final { isLocked_ = false; }
        bool IsLocked() const final { return isLocked_; }

        bool OnSuccess(const stage::InteractStagePtr_t) override { return false; }
        bool OnFailure(const stage::InteractStagePtr_t) override { return false; }

    private:
        virtual bool OnInteraction(const stage::InteractStagePtr_t, const Button &) = 0;

        void HandleIgnore(const stage::InteractStagePtr_t);

    private:
        Interact::Enum interactionType_;
        sfml_util::gui::TextInfo text_;
        ButtonVec_t buttons_;
        sf::Texture subjectTexture_;
        sf::Texture contextTexture_;
        sfml_util::sound_effect::Enum sfxEnter_;
        sfml_util::sound_effect::Enum sfxExit_;
        bool isLocked_;
    };

} // namespace interact
} // namespace heroespath

#endif // HEROESPATH_INTERACT_INTERACTION_HPP_INCLUDED
