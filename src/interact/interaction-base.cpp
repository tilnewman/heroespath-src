// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// interaction-base.cpp
//
#include "interaction-base.hpp"

#include "gui/font-manager.hpp"
#include "gui/sound-manager.hpp"
#include "misc/config-file.hpp"
#include "sfutil/color.hpp"
#include "stage/adventure-stage-interact-stage.hpp"

namespace heroespath
{
namespace interact
{

    InteractionBase::InteractionBase(
        const Interact::Enum INTERACTION_TYPE,
        const gui::TextInfo & TEXT,
        const ButtonVec_t & BUTTONS,
        const std::string & SUBJECT_IMAGE_KEY,
        const gui::sound_effect::Enum SFX_ENTER,
        const gui::sound_effect::Enum SFX_EXIT,
        const sf::Sprite & NPC_SPRITE)
        : interactionType_(INTERACTION_TYPE)
        , text_(TEXT)
        , buttons_(BUTTONS)
        , subjectCachedTexture_(SUBJECT_IMAGE_KEY)
        , contextCachedTexture_(Interact::ImageKey(INTERACTION_TYPE))
        , sfxEnter_(SFX_ENTER)
        , sfxExit_(SFX_EXIT)
        , isLocked_(false)
        , npcSprite_(NPC_SPRITE)
    {}

    void InteractionBase::PlayEnterSfx() const
    {
        if (gui::sound_effect::Count != sfxEnter_)
        {
            gui::SoundManager::Instance()->SoundEffectPlay(sfxEnter_);
        }
    }

    void InteractionBase::PlayExitSfx() const
    {
        if (gui::sound_effect::Count != sfxExit_)
        {
            gui::SoundManager::Instance()->SoundEffectPlay(sfxExit_);
        }
    }

    const gui::TextInfo
        InteractionBase::MakeTextInfo(const std::string & TEXT, const Text::Enum TYPE)
    {
        return gui::TextInfo(
            ((TYPE == Text::Dialog) ? ("\"" + TEXT + "\"") : TEXT),
            Text::Font(TYPE),
            gui::FontManager::Instance()->Size_Large(),
            sfutil::color::GrayDark,
            gui::Justified::Left);
    }

    const std::string InteractionBase::OnButtonClick(
        const stage::InteractStagePtr_t INTERACTION_STAGE_PTR,
        const gui::TextButton & TEXT_BUTTON_INVOKING_THIS_CALLBACK,
        const std::string & PACKET_DESCRIPTION)
    {
        for (const auto & BUTTON_WRAPPER_OWNED_BY_THIS_STAGE : buttons_)
        {
            if (BUTTON_WRAPPER_OWNED_BY_THIS_STAGE.IsSameButton(
                    &TEXT_BUTTON_INVOKING_THIS_CALLBACK))
            {
                return OnInteraction(INTERACTION_STAGE_PTR, BUTTON_WRAPPER_OWNED_BY_THIS_STAGE);
            }
        }

        return "not handled by any of the buttons owned by this stage, packet_desription="
            + PACKET_DESCRIPTION;
    }

    bool InteractionBase::OnKeyRelease(
        const stage::InteractStagePtr_t INTERACTION_STAGE_PTR, const sf::Keyboard::Key KEY)
    {
        for (const auto & BUTTON_WRAPPER_OWNED_BY_THIS_STAGE : buttons_)
        {
            if (BUTTON_WRAPPER_OWNED_BY_THIS_STAGE.Key() == KEY)
            {
                return OnInteraction(INTERACTION_STAGE_PTR, BUTTON_WRAPPER_OWNED_BY_THIS_STAGE)
                    .empty();
            }
        }

        return false;
    }

    void InteractionBase::HandleIgnore(const stage::InteractStagePtr_t INTERACTION_STAGE_PTR)
    {
        INTERACTION_STAGE_PTR->InteractionManager().RemoveCurrent();
    }

    const ButtonVec_t InteractionBase::MakeButtonVecFromButtonEnumVec(
        const ButtonEnumVec_t & BUTTON_ENUM_VEC) const
    {
        ButtonVec_t buttons;

        for (const auto BUTTON_ENUM : BUTTON_ENUM_VEC)
        {
            buttons.emplace_back(Button(BUTTON_ENUM));
        }

        return buttons;
    }

} // namespace interact
} // namespace heroespath
