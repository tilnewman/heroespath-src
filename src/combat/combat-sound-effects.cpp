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

#include "creature/creature.hpp"
#include "item/item.hpp"
#include "log/log-macros.hpp"
#include "song/song.hpp"
#include "spell/spell-base.hpp"

#include "misc/assertlogandthrow.hpp"
#include "misc/vectors.hpp"

namespace heroespath
{
namespace combat
{

    CombatSoundEffects::CombatSoundEffects() {}

    void CombatSoundEffects::PlayShoot(const item::ItemPtr_t WEAPON_PTR) const
    {
        M_ASSERT_OR_LOGANDTHROW_SS(
            (WEAPON_PTR != nullptr),
            "combat::CombatSoundEffects::PlayShoot() was given a nullptr WEAPON_PTR.");

        auto const WEAPON_TYPE{ WEAPON_PTR->WeaponType() };

        if ((WEAPON_TYPE & item::weapon_type::Blowpipe) || (WEAPON_TYPE & item::weapon_type::Sling))
        {
            sfml_util::SoundManager::Instance()
                ->Getsound_effect_set(sfml_util::sound_effect_set::BlowpipeShoot)
                .PlayRandom();
        }
        else if (
            (WEAPON_TYPE & item::weapon_type::Bow) || (WEAPON_TYPE & item::weapon_type::Crossbow))
        {
            sfml_util::SoundManager::Instance()
                ->Getsound_effect_set(sfml_util::sound_effect_set::ArrowShoot)
                .PlayRandom();
        }
        else
        {
            M_HP_LOG_ERR(
                "combat::CombatSoundEffects::PlayShoot(weapon=\""
                << WEAPON_PTR->Name() << "\", category=\""
                << item::category::ToString(WEAPON_PTR->Category(), false)
                << "\", weapon_type=" << item::weapon_type::ToString(WEAPON_TYPE, false)
                << ") Unable to find a 'shoot' sound effect to fit that weapon_type.");
        }
    }

    void CombatSoundEffects::PlayHitOrMiss(
        const creature::CreaturePtr_t CREATURE_PTR, const HitInfo & HIT_INFO) const
    {
        auto const WEAPON_PTR{ HIT_INFO.Weapon() };
        if (WEAPON_PTR == nullptr)
        {
            M_HP_LOG_ERR(
                "combat::CombatSoundEffects::PlayHitOrMiss() "
                << "was given a nullptr WEAPON_PTR.  HIT_INFO=" << HIT_INFO.ToString());

            return;
        }

        auto const WEAPON_TYPE{ WEAPON_PTR->WeaponType() };

        if (HIT_INFO.WasHit())
        {
            if (WEAPON_TYPE & item::weapon_type::Blowpipe)
            {
                sfml_util::SoundManager::Instance()
                    ->Getsound_effect_set(sfml_util::sound_effect_set::BlowpipeHit)
                    .PlayRandom();
            }
            else if (WEAPON_TYPE & item::weapon_type::Sling)
            {
                // The horn hit sfx sounded good for slings too
                return sfml_util::SoundManager::Instance()
                    ->Getsound_effect_set(sfml_util::sound_effect_set::MaterialHitMisc)
                    .Play(sfml_util::sound_effect::MaterialHitHorn);
            }
            else if (
                (WEAPON_TYPE & item::weapon_type::Bow)
                || (WEAPON_TYPE & item::weapon_type::Crossbow))
            {
                sfml_util::SoundManager::Instance()
                    ->Getsound_effect_set(sfml_util::sound_effect_set::ArrowHit)
                    .PlayRandom();
            }
            else if (WEAPON_TYPE & item::weapon_type::Fists)
            {
                sfml_util::SoundManager::Instance()
                    ->Getsound_effect_set(sfml_util::sound_effect_set::FistHit)
                    .PlayRandom();
            }
            else if (WEAPON_TYPE & item::weapon_type::Tendrils)
            {
                sfml_util::SoundManager::Instance()
                    ->Getsound_effect_set(sfml_util::sound_effect_set::TendrilHit)
                    .PlayRandom();
            }
            else if (
                (WEAPON_TYPE & item::weapon_type::Breath)
                && (CREATURE_PTR->Role() == creature::role::Firebrand))
            {
                sfml_util::SoundManager::Instance()->SoundEffectPlay(
                    sfml_util::sound_effect::BreathHitFirebrand);
            }
            else if (
                (WEAPON_TYPE & item::weapon_type::Breath)
                && (CREATURE_PTR->Role() == creature::role::Sylavin))
            {
                sfml_util::SoundManager::Instance()->SoundEffectPlay(
                    sfml_util::sound_effect::BreathHitSylavin);
            }
            else if (WEAPON_TYPE & item::weapon_type::Claws)
            {
                sfml_util::SoundManager::Instance()
                    ->Getsound_effect_set(sfml_util::sound_effect_set::ClawHit)
                    .PlayRandom();
            }
            else if (WEAPON_TYPE & item::weapon_type::Whip)
            {
                sfml_util::SoundManager::Instance()
                    ->Getsound_effect_set(sfml_util::sound_effect_set::WhipHit)
                    .PlayRandom();
            }
            else
            {
                switch (WEAPON_PTR->MaterialPrimary())
                {

                    case item::material::Wood:
                    {
                        return sfml_util::SoundManager::Instance()
                            ->Getsound_effect_set(sfml_util::sound_effect_set::MaterialHitMisc)
                            .Play(sfml_util::sound_effect::MaterialHitWood);
                    }

                    case item::material::HardLeather:
                    case item::material::SoftLeather:
                    {
                        return sfml_util::SoundManager::Instance()
                            ->Getsound_effect_set(sfml_util::sound_effect_set::MaterialHitMisc)
                            .Play(sfml_util::sound_effect::MaterialHitLeather);
                    }

                    case item::material::Bone:
                    {
                        return sfml_util::SoundManager::Instance()
                            ->Getsound_effect_set(sfml_util::sound_effect_set::MaterialHitMisc)
                            .Play(sfml_util::sound_effect::MaterialHitBone);
                    }

                    case item::material::Water:
                    case item::material::Acid:
                    case item::material::Blood:
                    {
                        return sfml_util::SoundManager::Instance()
                            ->Getsound_effect_set(sfml_util::sound_effect_set::MaterialHitMisc)
                            .Play(sfml_util::sound_effect::MaterialHitLiquid);
                    }

                    case item::material::Plant:
                    {
                        return sfml_util::SoundManager::Instance()
                            ->Getsound_effect_set(sfml_util::sound_effect_set::MaterialHitMisc)
                            .Play(sfml_util::sound_effect::MaterialHitPlant);
                    }

                    case item::material::Flesh:
                    {
                        return sfml_util::SoundManager::Instance()
                            ->Getsound_effect_set(sfml_util::sound_effect_set::MaterialHitMisc)
                            .Play(sfml_util::sound_effect::MaterialHitFlesh);
                    }

                    case item::material::Scale:
                    {
                        return sfml_util::SoundManager::Instance()
                            ->Getsound_effect_set(sfml_util::sound_effect_set::MaterialHitMisc)
                            .Play(sfml_util::sound_effect::MaterialHitScale);
                    }

                    case item::material::Hide:
                    {
                        return sfml_util::SoundManager::Instance()
                            ->Getsound_effect_set(sfml_util::sound_effect_set::MaterialHitMisc)
                            .Play(sfml_util::sound_effect::MaterialHitHide);
                    }

                    case item::material::Stone:
                    case item::material::Jade:
                    case item::material::Obsidian:
                    case item::material::Amethyst:
                    case item::material::Emerald:
                    case item::material::Pearl:
                    case item::material::Ruby:
                    case item::material::Lazuli:
                    case item::material::Sapphire:
                    case item::material::Diamond:
                    {
                        return sfml_util::SoundManager::Instance()
                            ->Getsound_effect_set(sfml_util::sound_effect_set::MaterialHitMisc)
                            .Play(sfml_util::sound_effect::MaterialHitStone);
                    }

                    case item::material::Rope:
                    case item::material::Cloth:
                    {
                        return sfml_util::SoundManager::Instance()
                            ->Getsound_effect_set(sfml_util::sound_effect_set::MaterialHitMisc)
                            .Play(sfml_util::sound_effect::MaterialHitCloth);
                    }

                    case item::material::Claw:
                    case item::material::Horn:
                    {
                        return sfml_util::SoundManager::Instance()
                            ->Getsound_effect_set(sfml_util::sound_effect_set::MaterialHitMisc)
                            .Play(sfml_util::sound_effect::MaterialHitHorn);
                    }

                    case item::material::Tooth:
                    {
                        return sfml_util::SoundManager::Instance()
                            ->Getsound_effect_set(sfml_util::sound_effect_set::MaterialHitMisc)
                            .Play(sfml_util::sound_effect::MaterialHitTooth);
                    }

                    case item::material::Tin:
                    case item::material::Gold:
                    case item::material::Iron:
                    case item::material::Bronze:
                    case item::material::Silver:
                    case item::material::Steel:
                    case item::material::Platinum:
                    {
                        return sfml_util::SoundManager::Instance()
                            ->Getsound_effect_set(sfml_util::sound_effect_set::MaterialHitMetal)
                            .PlayRandom();
                    }

                    case item::material::Feather:
                    case item::material::Fur:
                    case item::material::Hair:
                    case item::material::Glass:
                    case item::material::Gas:
                    case item::material::Paper:
                    case item::material::Spirit:
                    case item::material::Dirt:
                    case item::material::Nothing:
                    case item::material::Count:
                    default:
                    {
                        return sfml_util::SoundManager::Instance()
                            ->Getsound_effect_set(sfml_util::sound_effect_set::MaterialHitMisc)
                            .Play(sfml_util::sound_effect::MaterialHitMisc);
                    }
                }
            }
        }
        else
        {
            if (WEAPON_TYPE & item::weapon_type::Projectile)
            {
                sfml_util::SoundManager::Instance()
                    ->Getsound_effect_set(sfml_util::sound_effect_set::ProjectileMiss)
                    .PlayRandom();
            }
            else if (WEAPON_TYPE & item::weapon_type::Whip)
            {
                sfml_util::SoundManager::Instance()
                    ->Getsound_effect_set(sfml_util::sound_effect_set::WhipMiss)
                    .PlayRandom();
            }
            else if (WEAPON_TYPE & item::weapon_type::Melee)
            {
                sfml_util::SoundManager::Instance()
                    ->Getsound_effect_set(sfml_util::sound_effect_set::MeleeMiss)
                    .PlayRandom();
            }
            else
            {
                M_HP_LOG_ERR(
                    "combat::CombatSoundEffects::PlayHitOrMiss("
                    << "creature=\"" << CREATURE_PTR->NameAndRaceAndRole() << "\", weapon=\""
                    << WEAPON_PTR->Name() << "\", category=\""
                    << item::category::ToString(WEAPON_PTR->Category(), false)
                    << "\", weapon_type=" << item::weapon_type::ToString(WEAPON_TYPE, false)
                    << ") Unable to find a 'miss' sound effect to fit that weapon_type.");
            }
        }
    }

    void CombatSoundEffects::PlaySpell(const spell::SpellPtr_t SPELL_PTR) const
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
                sfml_util::SoundManager::Instance()
                    ->Getsound_effect_set(sfml_util::sound_effect_set::SpellBandage)
                    .PlayRandom();
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
                M_HP_LOG_ERR(
                    "combat::CombatSoundEffects::PlaySpell(" << SPELL_PTR->Name()
                                                             << ") failed to play a sound effect.");
            }
        }
    }

    void CombatSoundEffects::PlaySong(const song::SongPtr_t SONG_PTR) const
    {
        switch (SONG_PTR->Which())
        {
            case song::Songs::RallyDrum:
            {
                sfml_util::SoundManager::Instance()->SoundEffectPlay(
                    sfml_util::sound_effect::SongRallyDrum);
                break;
            }
            case song::Songs::SpiritResonance:
            {
                sfml_util::SoundManager::Instance()->SoundEffectPlay(
                    sfml_util::sound_effect::SongSpiritResonance);
                break;
            }
            case song::Songs::RousingRhythm:
            {
                sfml_util::SoundManager::Instance()->SoundEffectPlay(
                    sfml_util::sound_effect::SongRousingRhythm);
                break;
            }
            case song::Songs::TripBeat:
            {
                sfml_util::SoundManager::Instance()->SoundEffectPlay(
                    sfml_util::sound_effect::SongTripBeat);
                break;
            }
            case song::Songs::PanicStrings:
            {
                sfml_util::SoundManager::Instance()->SoundEffectPlay(
                    sfml_util::sound_effect::SongPanicStrings);
                break;
            }
            case song::Songs::Lullaby:
            {
                sfml_util::SoundManager::Instance()->SoundEffectPlay(
                    sfml_util::sound_effect::SongLullaby);
                break;
            }
            case song::Songs::Count:
            default:
            {
                M_HP_LOG_ERR(
                    "combat::CombatSoundEffects::PlaySong(" << SONG_PTR->Name()
                                                            << ") failed to play a sound effect.");
            }
        }
    }

    void CombatSoundEffects::PlayRoar(const creature::CreaturePtr_t CREATURE_PTR) const
    {
        auto const ROLE{ CREATURE_PTR->Role() };
        if (ROLE == creature::role::Wolfen)
        {
            switch (CREATURE_PTR->WolfenClass())
            {
                case creature::wolfen_class::Pup:
                {
                    sfml_util::SoundManager::Instance()->SoundEffectPlay(
                        sfml_util::sound_effect::RoarWolfenPup);
                    return;
                }
                case creature::wolfen_class::Juvenile:
                {
                    sfml_util::SoundManager::Instance()->SoundEffectPlay(
                        sfml_util::sound_effect::RoarWolfenJuvenile);
                    return;
                }
                case creature::wolfen_class::Adult:
                {
                    sfml_util::SoundManager::Instance()->SoundEffectPlay(
                        sfml_util::sound_effect::RoarWolfenAdult);
                    return;
                }
                case creature::wolfen_class::Noble:
                {
                    sfml_util::SoundManager::Instance()->SoundEffectPlay(
                        sfml_util::sound_effect::RoarWolfenNoble);
                    return;
                }
                case creature::wolfen_class::Highborn:
                {
                    sfml_util::SoundManager::Instance()->SoundEffectPlay(
                        sfml_util::sound_effect::RoarWolfenHighborn);
                    return;
                }
                case creature::wolfen_class::Elder:
                {
                    sfml_util::SoundManager::Instance()->SoundEffectPlay(
                        sfml_util::sound_effect::RoarWolfenElder);
                    return;
                }
                case creature::wolfen_class::Count:
                default:
                {}
            }
        }
        else if (ROLE == creature::role::Firebrand)
        {
            switch (CREATURE_PTR->DragonClass())
            {
                case creature::dragon_class::Hatchling:
                {
                    sfml_util::SoundManager::Instance()->SoundEffectPlay(
                        sfml_util::sound_effect::RoarDragonFirebrandHatchling);
                    return;
                }
                case creature::dragon_class::Whelp:
                {
                    sfml_util::SoundManager::Instance()->SoundEffectPlay(
                        sfml_util::sound_effect::RoarDragonFirebrandWhelp);
                    return;
                }
                case creature::dragon_class::Fledgling:
                {
                    sfml_util::SoundManager::Instance()->SoundEffectPlay(
                        sfml_util::sound_effect::RoarDragonFirebrandFledgling);
                    return;
                }
                case creature::dragon_class::Juvenile:
                {
                    sfml_util::SoundManager::Instance()->SoundEffectPlay(
                        sfml_util::sound_effect::RoarDragonFirebrandJuvenile);
                    return;
                }
                case creature::dragon_class::Adult:
                {
                    sfml_util::SoundManager::Instance()->SoundEffectPlay(
                        sfml_util::sound_effect::RoarDragonFirebrandAdult);
                    return;
                }
                case creature::dragon_class::Wyrm:
                {
                    sfml_util::SoundManager::Instance()->SoundEffectPlay(
                        sfml_util::sound_effect::RoarDragonFirebrandWyrm);
                    return;
                }
                case creature::dragon_class::Skycaster:
                {
                    sfml_util::SoundManager::Instance()->SoundEffectPlay(
                        sfml_util::sound_effect::RoarDragonFirebrandSkycaster);
                    return;
                }
                case creature::dragon_class::Elder:
                {
                    sfml_util::SoundManager::Instance()->SoundEffectPlay(
                        sfml_util::sound_effect::RoarDragonFirebrandElder);
                    return;
                }
                case creature::dragon_class::Count:
                default:
                {}
            }
        }
        else if (ROLE == creature::role::Sylavin)
        {
            switch (CREATURE_PTR->DragonClass())
            {
                case creature::dragon_class::Hatchling:
                {
                    sfml_util::SoundManager::Instance()->SoundEffectPlay(
                        sfml_util::sound_effect::RoarDragonSylavinHatchling);
                    return;
                }
                case creature::dragon_class::Whelp:
                {
                    sfml_util::SoundManager::Instance()->SoundEffectPlay(
                        sfml_util::sound_effect::RoarDragonSylavinWhelp);
                    return;
                }
                case creature::dragon_class::Fledgling:
                {
                    sfml_util::SoundManager::Instance()->SoundEffectPlay(
                        sfml_util::sound_effect::RoarDragonSylavinFledgling);
                    return;
                }
                case creature::dragon_class::Juvenile:
                {
                    sfml_util::SoundManager::Instance()->SoundEffectPlay(
                        sfml_util::sound_effect::RoarDragonSylavinJuvenile);
                    return;
                }
                case creature::dragon_class::Adult:
                {
                    sfml_util::SoundManager::Instance()->SoundEffectPlay(
                        sfml_util::sound_effect::RoarDragonSylavinAdult);
                    return;
                }
                case creature::dragon_class::Wyrm:
                {
                    sfml_util::SoundManager::Instance()->SoundEffectPlay(
                        sfml_util::sound_effect::RoarDragonSylavinWyrm);
                    return;
                }
                case creature::dragon_class::Skycaster:
                {
                    sfml_util::SoundManager::Instance()->SoundEffectPlay(
                        sfml_util::sound_effect::RoarDragonSylavinSkycaster);
                    return;
                }
                case creature::dragon_class::Elder:
                {
                    sfml_util::SoundManager::Instance()->SoundEffectPlay(
                        sfml_util::sound_effect::RoarDragonSylavinElder);
                    return;
                }
                case creature::dragon_class::Count:
                default:
                {}
            }

            M_HP_LOG_ERR(
                "combat::CombatSoundEffects::PlayRoar("
                << "creature=\"" << CREATURE_PTR->NameAndRaceAndRole()
                << ") failed to find an sfx to play.");
        }
    }

    void CombatSoundEffects::PlayDeath(const creature::CreaturePtr_t CREATURE_PTR) const
    {
        sfml_util::SfxEnumVec_t sfxVec;
        GetDeathSfx(CREATURE_PTR, sfxVec);

        if (sfxVec.empty())
        {
            M_HP_LOG_ERR(
                "combat::CombatSoundEffects::PlayDeath("
                << "creature=\"" << CREATURE_PTR->NameAndRaceAndRole()
                << ") failed to find an sfx to play.");
        }
        else
        {
            sfml_util::SoundManager::Instance()->SoundEffectPlay(
                misc::Vector::SelectRandom(sfxVec));
        }
    }

    void CombatSoundEffects::GetDeathSfx(
        creature::CreaturePtr_t CREATURE_PTR, sfml_util::SfxEnumVec_t & sfxVec_OutParam) const
    {
        switch (CREATURE_PTR->Race())
        {
            case creature::race::Human:
            case creature::race::Gnome:
            {
                sfxVec_OutParam.push_back(sfml_util::sound_effect::DeathHuman1);
                sfxVec_OutParam.push_back(sfml_util::sound_effect::DeathHuman2);
                sfxVec_OutParam.push_back(sfml_util::sound_effect::DeathHuman3);
                sfxVec_OutParam.push_back(sfml_util::sound_effect::DeathHuman4);
                sfxVec_OutParam.push_back(sfml_util::sound_effect::DeathHuman5);

                if (CREATURE_PTR->Sex() == creature::sex::Male)
                {
                    sfxVec_OutParam.push_back(sfml_util::sound_effect::DeathHumanMale1);
                    sfxVec_OutParam.push_back(sfml_util::sound_effect::DeathHumanMale2);
                    sfxVec_OutParam.push_back(sfml_util::sound_effect::DeathHumanMale3);
                    sfxVec_OutParam.push_back(sfml_util::sound_effect::DeathHumanMale4);
                    sfxVec_OutParam.push_back(sfml_util::sound_effect::DeathHumanMale5);
                    sfxVec_OutParam.push_back(sfml_util::sound_effect::DeathHumanMale6);
                    sfxVec_OutParam.push_back(sfml_util::sound_effect::DeathHumanMale7);
                }
                else if (CREATURE_PTR->Sex() == creature::sex::Female)
                {
                    sfxVec_OutParam.push_back(sfml_util::sound_effect::DeathHumanFemale1);
                    sfxVec_OutParam.push_back(sfml_util::sound_effect::DeathHumanFemale3);
                }
                return;
            }
            case creature::race::Halfling:
            {
                sfxVec_OutParam.push_back(sfml_util::sound_effect::DeathHuman2);
                sfxVec_OutParam.push_back(sfml_util::sound_effect::DeathHuman4);

                if (CREATURE_PTR->Sex() == creature::sex::Male)
                {
                    sfxVec_OutParam.push_back(sfml_util::sound_effect::DeathHumanMale3);
                    sfxVec_OutParam.push_back(sfml_util::sound_effect::DeathHumanMale5);
                    sfxVec_OutParam.push_back(sfml_util::sound_effect::DeathHumanMale6);
                }
                else if (CREATURE_PTR->Sex() == creature::sex::Female)
                {
                    sfxVec_OutParam.push_back(sfml_util::sound_effect::DeathHumanFemale2);
                }
                return;
            }
            case creature::race::Pixie:
            {
                if (CREATURE_PTR->Sex() == creature::sex::Male)
                {
                    sfxVec_OutParam.push_back(sfml_util::sound_effect::DeathHumanMale2);
                    sfxVec_OutParam.push_back(sfml_util::sound_effect::DeathHumanMale4);
                }
                else
                {
                    sfxVec_OutParam.push_back(sfml_util::sound_effect::DeathHumanFemale1);
                    sfxVec_OutParam.push_back(sfml_util::sound_effect::DeathHumanFemale2);
                    sfxVec_OutParam.push_back(sfml_util::sound_effect::DeathHumanFemale3);
                }
                return;
            }
            case creature::race::Witch:
            case creature::race::Harpy:
            {
                sfxVec_OutParam.push_back(sfml_util::sound_effect::DeathHumanFemale1);
                sfxVec_OutParam.push_back(sfml_util::sound_effect::DeathHumanFemale2);
                sfxVec_OutParam.push_back(sfml_util::sound_effect::DeathHumanFemale3);
                return;
            }
            case creature::race::Golem:
            {
                sfxVec_OutParam.push_back(sfml_util::sound_effect::DeathBeastCaveTroll1);
                return;
            }
            case creature::race::ThreeHeadedHound:
            case creature::race::Werewolf:
            case creature::race::Wolfen:
            {
                sfxVec_OutParam.push_back(sfml_util::sound_effect::DeathWolf);
                return;
            }
            case creature::race::Hydra:
            case creature::race::Dragon:
            case creature::race::Wyvern:
            {
                sfxVec_OutParam.push_back(sfml_util::sound_effect::DeathDragon1a);
                sfxVec_OutParam.push_back(sfml_util::sound_effect::DeathDragon1b);
                return;
            }
            case creature::race::Newt:
            case creature::race::Goblin:
            case creature::race::Pug:
            {
                sfxVec_OutParam.push_back(sfml_util::sound_effect::DeathBeastSmallRaspy1);
                sfxVec_OutParam.push_back(sfml_util::sound_effect::DeathBeastSmallRaspy2);
                sfxVec_OutParam.push_back(sfml_util::sound_effect::DeathBeastSmallRaspy3);
                sfxVec_OutParam.push_back(sfml_util::sound_effect::DeathBeastSmallRaspy4);
                return;
            }
            case creature::race::Troll:
            {
                sfxVec_OutParam.push_back(sfml_util::sound_effect::DeathBeastRaugh1);
                sfxVec_OutParam.push_back(sfml_util::sound_effect::DeathBeastRaugh2);
                sfxVec_OutParam.push_back(sfml_util::sound_effect::DeathBeastRaugh3);
                return;
            }
            case creature::race::Orc:
            {
                sfxVec_OutParam.push_back(sfml_util::sound_effect::DeathBeast5);
                return;
            }
            case creature::race::Beetle:
            case creature::race::Spider:
            {
                sfxVec_OutParam.push_back(sfml_util::sound_effect::DeathSpider1);
                sfxVec_OutParam.push_back(sfml_util::sound_effect::DeathSpider2);
                sfxVec_OutParam.push_back(sfml_util::sound_effect::DeathSpider3);
                return;
            }
            case creature::race::Bog:
            {
                if (CREATURE_PTR->Role() == creature::role::Smasher)
                {
                    sfxVec_OutParam.push_back(sfml_util::sound_effect::DeathBeast3);
                }
                else
                {
                    sfxVec_OutParam.push_back(sfml_util::sound_effect::DeathBeastSmallRaspy4);
                }
                return;
            }
            case creature::race::CaveCrawler:
            {
                sfxVec_OutParam.push_back(sfml_util::sound_effect::DeathThing1);
                return;
            }
            case creature::race::LizardWalker:
            {
                sfxVec_OutParam.push_back(sfml_util::sound_effect::DeathBeastTalker1);
                return;
            }
            case creature::race::Minotaur:
            {
                sfxVec_OutParam.push_back(sfml_util::sound_effect::DeathBeast1);
                return;
            }
            case creature::race::Ogre:
            {
                sfxVec_OutParam.push_back(sfml_util::sound_effect::DeathBeastBig1);
                sfxVec_OutParam.push_back(sfml_util::sound_effect::DeathBeastBig2);
                sfxVec_OutParam.push_back(sfml_util::sound_effect::DeathBeastBig3);
                return;
            }
            case creature::race::Plant:
            {
                sfxVec_OutParam.push_back(sfml_util::sound_effect::DeathBeastWet);
                return;
            }
            case creature::race::Shade:
            {
                sfxVec_OutParam.push_back(sfml_util::sound_effect::DeathGhostly1a);
                sfxVec_OutParam.push_back(sfml_util::sound_effect::DeathGhostly1b);
                sfxVec_OutParam.push_back(sfml_util::sound_effect::DeathGhostly1c);
                return;
            }
            case creature::race::Skeleton:
            {
                sfxVec_OutParam.push_back(sfml_util::sound_effect::DeathCreepy1);
                sfxVec_OutParam.push_back(sfml_util::sound_effect::DeathCreepy2);
                sfxVec_OutParam.push_back(sfml_util::sound_effect::DeathCreepy3);
                sfxVec_OutParam.push_back(sfml_util::sound_effect::DeathCreepy4);
                sfxVec_OutParam.push_back(sfml_util::sound_effect::DeathCreepy5);
                sfxVec_OutParam.push_back(sfml_util::sound_effect::DeathCreepy6);
                return;
            }
            case creature::race::Boar:
            {
                sfxVec_OutParam.push_back(sfml_util::sound_effect::DeathBoar);
                return;
            }
            case creature::race::Demon:
            {
                sfxVec_OutParam.push_back(sfml_util::sound_effect::DeathHowlDemon);
                return;
            }
            case creature::race::Griffin:
            {
                sfxVec_OutParam.push_back(sfml_util::sound_effect::DeathGriffin);
                return;
            }
            case creature::race::Lion:
            {
                sfxVec_OutParam.push_back(sfml_util::sound_effect::DeathLion);
                return;
            }
            case creature::race::Wereboar:
            case creature::race::LionBoar:
            {
                sfxVec_OutParam.push_back(sfml_util::sound_effect::DeathLionboar);
                return;
            }
            case creature::race::Naga:
            {
                sfxVec_OutParam.push_back(sfml_util::sound_effect::DeathBeastTalker2Roar);
                return;
            }
            case creature::race::Ramonaut:
            {
                sfxVec_OutParam.push_back(sfml_util::sound_effect::DeathBeast3);
                return;
            }
            case creature::race::Serpent:
            case creature::race::Cobra:
            {
                sfxVec_OutParam.push_back(sfml_util::sound_effect::DeathSnake1);
                sfxVec_OutParam.push_back(sfml_util::sound_effect::DeathSnake2);
                sfxVec_OutParam.push_back(sfml_util::sound_effect::DeathSnake3);
                sfxVec_OutParam.push_back(sfml_util::sound_effect::DeathSnake4);
                sfxVec_OutParam.push_back(sfml_util::sound_effect::DeathSnake5);
                sfxVec_OutParam.push_back(sfml_util::sound_effect::DeathSnake6);
                return;
            }
            case creature::race::Werebear:
            {
                sfxVec_OutParam.push_back(sfml_util::sound_effect::DeathBeast4);
                return;
            }
            case creature::race::Werecat:
            {
                sfxVec_OutParam.push_back(sfml_util::sound_effect::DeathThing3);
                return;
            }
            case creature::race::Bat:
            case creature::race::Werebat:
            {
                sfxVec_OutParam.push_back(sfml_util::sound_effect::DeathThing2);
                return;
            }
            case creature::race::Giant:
            {
                sfxVec_OutParam.push_back(sfml_util::sound_effect::DeathBeastHuge1a);
                sfxVec_OutParam.push_back(sfml_util::sound_effect::DeathBeastHuge1b);
                return;
            }
            case creature::race::Ghoul:
            {
                sfxVec_OutParam.push_back(sfml_util::sound_effect::DeathBeast6);
                return;
            }
            case creature::race::Count:
            default:
            {}
        }
    }
} // namespace combat
} // namespace heroespath
