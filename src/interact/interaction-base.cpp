// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
//
// interaction-base.cpp
//
#include "interaction-base.hpp"
#include "game/game-data-file.hpp"
#include "sfml-util/font-manager.hpp"
#include "sfml-util/loaders.hpp"
#include "sfml-util/sound-manager.hpp"
#include "stage/adventure-stage-interact-stage.hpp"

namespace heroespath
{
namespace interact
{

    InteractionBase::InteractionBase(
        const Interact::Enum INTERACTION_TYPE,
        const sfml_util::gui::TextInfo & TEXT,
        const ButtonVec_t & BUTTONS,
        const std::string & SUBJECT_IMAGE_KEY,
        const sfml_util::sound_effect::Enum SFX_ENTER,
        const sfml_util::sound_effect::Enum SFX_EXIT)
        : interactionType_(INTERACTION_TYPE)
        , text_(TEXT)
        , buttons_(BUTTONS)
        , subjectTexture_()
        , contextTexture_()
        , sfxEnter_(SFX_ENTER)
        , sfxExit_(SFX_EXIT)
        , isLocked_(false)
    {
        sfml_util::LoadTexture(
            subjectTexture_, game::GameDataFile::Instance()->GetMediaPath(SUBJECT_IMAGE_KEY));

        sfml_util::LoadTexture(
            contextTexture_,
            game::GameDataFile::Instance()->GetMediaPath(Interact::ImageKey(INTERACTION_TYPE)));
    }

    void InteractionBase::PlayEnterSfx() const
    {
        if (sfml_util::sound_effect::Count != sfxEnter_)
        {
            sfml_util::SoundManager::Instance()->SoundEffectPlay(sfxEnter_);
        }
    }

    void InteractionBase::PlayExitSfx() const
    {
        if (sfml_util::sound_effect::Count != sfxExit_)
        {
            sfml_util::SoundManager::Instance()->SoundEffectPlay(sfxExit_);
        }
    }

    const sfml_util::gui::TextInfo
        InteractionBase::MakeTextInfo(const std::string & TEXT, const Text::Enum TYPE)
    {
        return sfml_util::gui::TextInfo(
            TEXT,
            Text::Font(TYPE),
            sfml_util::FontManager::Instance()->Size_Large(),
            sfml_util::FontManager::Instance()->Color_GrayDark(),
            sfml_util::Justified::Left);
    }

    bool InteractionBase::OnButtonClick(
        stage::InteractStage * const interactStagePtr,
        const sfml_util::gui::TextButton * const TEXT_BUTTON_PTR)
    {
        for (auto const & BUTTON : buttons_)
        {
            if (BUTTON.DoPointersMatch(TEXT_BUTTON_PTR))
            {
                return OnInteraction(interactStagePtr, BUTTON);
            }
        }

        return false;
    }

    bool InteractionBase::OnKeyRelease(
        stage::InteractStage * const interactStagePtr, const sf::Keyboard::Key KEY)
    {
        for (auto const & BUTTON : buttons_)
        {
            if (BUTTON.Key() == KEY)
            {
                return OnInteraction(interactStagePtr, BUTTON);
            }
        }

        return false;
    }

    void InteractionBase::HandleIgnore(stage::InteractStage * const interactStagePtr)
    {
        if (nullptr != interactStagePtr)
        {
            interactStagePtr->InteractionManager().RemoveCurrent();
        }
    }
}
}
