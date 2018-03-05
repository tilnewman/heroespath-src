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
#ifndef HEROESPATH_INTERACT_INTERACTION_BASE_HPP_INCLUDED
#define HEROESPATH_INTERACT_INTERACTION_BASE_HPP_INCLUDED
//
// interaction-base.hpp
//
#include "interact/i-interaction.hpp"
#include "interact/interaction-button.hpp"
#include "interact/interaction-text-enum.hpp"
#include "sfml-util/sound-effects-enum.hpp"

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

        inline virtual Interact::Enum Type() const final { return interactionType_; }

        inline virtual const sfml_util::gui::TextInfo & Text() const final { return text_; }

        virtual ButtonVec_t & Buttons() final { return buttons_; }

        inline virtual const sf::Texture & SubjectTexture() const final { return subjectTexture_; }

        inline virtual const sf::Texture & ContextTexture() const final { return contextTexture_; }

        virtual void PlayEnterSfx() const final;

        virtual void PlayExitSfx() const final;

        static const sfml_util::gui::TextInfo
            MakeTextInfo(const std::string & TEXT, const Text::Enum TYPE);

        virtual bool OnButtonClick(
            stage::InteractStage * const, const sfml_util::gui::TextButton * const) final;

        virtual bool OnKeyRelease(stage::InteractStage * const, const sf::Keyboard::Key) final;

        virtual void Lock() final { isLocked_ = true; }
        virtual void Unlock() final { isLocked_ = false; }
        virtual bool IsLocked() const final { return isLocked_; }

        virtual bool OnSuccess(stage::InteractStage * const) override { return false; }
        virtual bool OnFailure(stage::InteractStage * const) override { return false; }

    private:
        virtual bool OnInteraction(stage::InteractStage * const, const Button &) = 0;

        void HandleIgnore(stage::InteractStage * const);

    public:
        static const std::string BUTTON_NAME_IGNORE_;

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
