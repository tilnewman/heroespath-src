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
// combat-sound-effects.hpp
//
#include "combat-sound-effects.hpp"

#include "sfml-util/sound-manager.hpp"

#include "game/item/item.hpp"
#include "game/log-macros.hpp"
#include "game/spell/spell-base.hpp"

#include "misc/assertlogandthrow.hpp"


namespace game
{
namespace combat
{

    CombatSoundEffects::CombatSoundEffects()
    {}


    void CombatSoundEffects::PlayShoot(const item::ItemPtr_t WEAPON_PTR)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((WEAPON_PTR != nullptr),
            "game::combat::CombatSoundEffects::PlayShoot() was given a nullptr WEAPON_PTR.");

        auto const WEAPON_TYPE{ WEAPON_PTR->WeaponType() };

        if ((WEAPON_TYPE & item::weapon_type::Blowpipe) || (WEAPON_TYPE & item::weapon_type::Sling))
        {
            sfml_util::SoundManager::Instance()->GetSfxSet(sfml_util::SfxSet::BlowpipeShoot).PlayRandom();
        }
        else if ((WEAPON_TYPE & item::weapon_type::Bow) || (WEAPON_TYPE & item::weapon_type::Crossbow))
        {
            sfml_util::SoundManager::Instance()->GetSfxSet(sfml_util::SfxSet::ArrowShoot).PlayRandom();
        }
        else
        {
            M_HP_LOG_ERR("game::combat::CombatSoundEffects::PlayShoot(weapon=\"" << WEAPON_PTR->Name()
                << "\", category=\"" << item::category::ToString(WEAPON_PTR->Category(), false)
                << "\", weapon_type=" << item::weapon_type::ToString(WEAPON_TYPE, false)
                << ") Unable to find a 'shoot' sound effect to fit that weapon_type.");
        }
    }


    void CombatSoundEffects::PlayHitOrMiss(const HitInfo & HIT_INFO)
    {
        auto const WEAPON_PTR{ HIT_INFO.Weapon() };
        if (WEAPON_PTR == nullptr)
        {
            M_HP_LOG_ERR("game::combat::CombatSoundEffects::PlayHitOrMiss() was given a nullptr WEAPON_PTR.");
            return;
        }

        auto const WEAPON_TYPE{ WEAPON_PTR->WeaponType() };

        if (HIT_INFO.WasHit())
        {
            if ((WEAPON_TYPE & item::weapon_type::Blowpipe) || (WEAPON_TYPE & item::weapon_type::Sling))
            {
                sfml_util::SoundManager::Instance()->GetSfxSet(sfml_util::SfxSet::BlowpipeHit).PlayRandom();
            }
            else if ((WEAPON_TYPE & item::weapon_type::Bow) || (WEAPON_TYPE & item::weapon_type::Crossbow))
            {
                sfml_util::SoundManager::Instance()->GetSfxSet(sfml_util::SfxSet::ArrowHit).PlayRandom();
            }
            else if (WEAPON_TYPE & item::weapon_type::Fists)
            {
                sfml_util::SoundManager::Instance()->GetSfxSet(sfml_util::SfxSet::FistHit).PlayRandom();
            }
            else if (WEAPON_TYPE & item::weapon_type::Tendrils)
            {
                sfml_util::SoundManager::Instance()->GetSfxSet(sfml_util::SfxSet::TendrilHit).PlayRandom();
            }
            else if (WEAPON_TYPE & item::weapon_type::Breath)
            {
                sfml_util::SoundManager::Instance()->GetSfxSet(sfml_util::SfxSet::BreathHit).PlayRandom();
            }
            else if (WEAPON_TYPE & item::weapon_type::Claws)
            {
                sfml_util::SoundManager::Instance()->GetSfxSet(sfml_util::SfxSet::ClawHit).PlayRandom();
            }
            else if (WEAPON_TYPE & item::weapon_type::Whip)
            {
                sfml_util::SoundManager::Instance()->GetSfxSet(sfml_util::SfxSet::WhipHit).PlayRandom();
            }
            else
            {
                switch (WEAPON_PTR->MaterialPrimary())
                {

                    case item::material::Wood:              { return sfml_util::SoundManager::Instance()->GetSfxSet(sfml_util::SfxSet::MaterialHitMisc).Play(sfml_util::sound_effect::MaterialHitWood); }
                    case item::material::HardLeather:
                    case item::material::SoftLeather:       { return sfml_util::SoundManager::Instance()->GetSfxSet(sfml_util::SfxSet::MaterialHitMisc).Play(sfml_util::sound_effect::MaterialHitLeather); }
                    case item::material::Bone:              { return sfml_util::SoundManager::Instance()->GetSfxSet(sfml_util::SfxSet::MaterialHitMisc).Play(sfml_util::sound_effect::MaterialHitBone); }

                    case item::material::Water:
                    case item::material::Acid:
                    case item::material::Blood:             { return sfml_util::SoundManager::Instance()->GetSfxSet(sfml_util::SfxSet::MaterialHitMisc).Play(sfml_util::sound_effect::MaterialHitLiquid); }

                    case item::material::Plant:             { return sfml_util::SoundManager::Instance()->GetSfxSet(sfml_util::SfxSet::MaterialHitMisc).Play(sfml_util::sound_effect::MaterialHitPlant); }

                    case item::material::Flesh:             { return sfml_util::SoundManager::Instance()->GetSfxSet(sfml_util::SfxSet::MaterialHitMisc).Play(sfml_util::sound_effect::MaterialHitFlesh); }

                    case item::material::Scale:             { return sfml_util::SoundManager::Instance()->GetSfxSet(sfml_util::SfxSet::MaterialHitMisc).Play(sfml_util::sound_effect::MaterialHitScale); }

                    case item::material::Hide:              { return sfml_util::SoundManager::Instance()->GetSfxSet(sfml_util::SfxSet::MaterialHitMisc).Play(sfml_util::sound_effect::MaterialHitHide); }

                    case item::material::Stone:
                    case item::material::Obsidian:
                    case item::material::Amethyst:
                    case item::material::Emerald:
                    case item::material::Pearl:
                    case item::material::Ruby:
                    case item::material::Lapis:
                    case item::material::Sapphire:
                    case item::material::Diamond:           { return sfml_util::SoundManager::Instance()->GetSfxSet(sfml_util::SfxSet::MaterialHitMisc).Play(sfml_util::sound_effect::MaterialHitStone); }

                    case item::material::Rope:
                    case item::material::Cloth:             { return sfml_util::SoundManager::Instance()->GetSfxSet(sfml_util::SfxSet::MaterialHitMisc).Play(sfml_util::sound_effect::MaterialHitCloth); }

                    case item::material::Claw:
                    case item::material::Horn:              { return sfml_util::SoundManager::Instance()->GetSfxSet(sfml_util::SfxSet::MaterialHitMisc).Play(sfml_util::sound_effect::MaterialHitHorn); }

                    case item::material::Tooth:             { return sfml_util::SoundManager::Instance()->GetSfxSet(sfml_util::SfxSet::MaterialHitMisc).Play(sfml_util::sound_effect::MaterialHitTooth); }

                    case item::material::Tin:
                    case item::material::Gold:
                    case item::material::Iron:
                    case item::material::Bronze:
                    case item::material::Silver:
                    case item::material::Steel:
                    case item::material::Platinum:          { return sfml_util::SoundManager::Instance()->GetSfxSet(sfml_util::SfxSet::MaterialHitMisc).PlayRandom(); }

                    case item::material::Feathers:
                    case item::material::Fur:
                    case item::material::Hair:
                    case item::material::Glass:
                    case item::material::Gas:
                    case item::material::Paper:
                    case item::material::Ether:
                    case item::material::Dirt:
                    case item::material::Nothing:
                    case item::material::Count:
                    default:
                    {
                        return sfml_util::SoundManager::Instance()->GetSfxSet(sfml_util::SfxSet::MaterialHitMisc).Play(sfml_util::sound_effect::MaterialHitMisc);
                    }
                }
            }
        }
        else
        {
            if (WEAPON_TYPE & item::weapon_type::Projectile)
            {
                sfml_util::SoundManager::Instance()->GetSfxSet(sfml_util::SfxSet::ProjectileMiss).PlayRandom();
            }
            else if (WEAPON_TYPE & item::weapon_type::Whip)
            {
                sfml_util::SoundManager::Instance()->GetSfxSet(sfml_util::SfxSet::WhipMiss).PlayRandom();
            }
            else if (WEAPON_TYPE & item::weapon_type::Melee)
            {
                sfml_util::SoundManager::Instance()->GetSfxSet(sfml_util::SfxSet::MeleeMiss).PlayRandom();
            }
            else
            {
                M_HP_LOG_ERR("game::combat::CombatSoundEffects::PlayHitOrMiss(weapon=\"" << WEAPON_PTR->Name() << "\", category=\"" << item::category::ToString(WEAPON_PTR->Category(), false) << "\", weapon_type=" << item::weapon_type::ToString(WEAPON_TYPE, false) << ") Unable to find a 'miss' sound effect to fit that weapon_type.");
            }
        }
    }


    void CombatSoundEffects::PlaySpell(const spell::SpellPtr_t SPELL_PTR)
    {
        switch (SPELL_PTR->Which())
        {
            case spell::Spells::Sparks:
            {
                sfml_util::SoundManager::Instance()->SoundEffectPlay(
                    sfml_util::sound_effect::SpellSparks);
                break;
            }
            case spell::Spells::Bandage:
            {
                sfml_util::SoundManager::Instance()->GetSfxSet(
                    sfml_util::SfxSet::SpellBandage).PlayRandom();
                break;
            }
            case spell::Spells::Sleep:
            {
                sfml_util::SoundManager::Instance()->SoundEffectPlay(
                    sfml_util::sound_effect::SpellSleep);
                break;
            }
            case spell::Spells::Awaken:
            {
                sfml_util::SoundManager::Instance()->SoundEffectPlay(
                    sfml_util::sound_effect::SpellAwaken);
                break;
            }
            case spell::Spells::Trip:
            {
                sfml_util::SoundManager::Instance()->SoundEffectPlay(
                    sfml_util::sound_effect::SpellTrip);

                sfml_util::SoundManager::Instance()->SoundEffectPlay(
                    sfml_util::sound_effect::Trip, 1.0f);
                break;
            }
            case spell::Spells::Lift:
            {
                sfml_util::SoundManager::Instance()->SoundEffectPlay(
                    sfml_util::sound_effect::SpellLift);
                break;
            }
            case spell::Spells::Daze:
            {
                sfml_util::SoundManager::Instance()->SoundEffectPlay(
                    sfml_util::sound_effect::SpellDaze);
                break;
            }
            case spell::Spells::Panic:
            {
                sfml_util::SoundManager::Instance()->SoundEffectPlay(
                    sfml_util::sound_effect::SpellFrighten);
                break;
            }
            case spell::Spells::ClearMind:
            {
                sfml_util::SoundManager::Instance()->SoundEffectPlay(
                    sfml_util::sound_effect::SpellClearMind);
                break;
            }
            case spell::Spells::Poison:
            {
                sfml_util::SoundManager::Instance()->SoundEffectPlay(
                    sfml_util::sound_effect::SpellPoison);
                break;
            }
            case spell::Spells::Antidote:
            {
                sfml_util::SoundManager::Instance()->SoundEffectPlay(
                    sfml_util::sound_effect::SpellAntidote);
                break;
            }
            case spell::Spells::PoisonCloud:
            {
                sfml_util::SoundManager::Instance()->SoundEffectPlay(
                    sfml_util::sound_effect::SpellPoisonCloud);
                break;
            }
            case spell::Spells::Count:
            default:
            {
                M_HP_LOG_ERR("game::combat::CombatSoundEffects(" << SPELL_PTR->Name() << ") failed to play a sound effect.");
            }
        }
    }

}
}
