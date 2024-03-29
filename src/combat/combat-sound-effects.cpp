// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// combat-sound-effects.hpp
//
#include "combat-sound-effects.hpp"

#include "creature/creature.hpp"
#include "gui/sound-manager.hpp"
#include "item/item.hpp"
#include "misc/assertlogandthrow.hpp"
#include "misc/log-macros.hpp"
#include "misc/vectors.hpp"
#include "song/song.hpp"
#include "spell/spell.hpp"

namespace heroespath
{
namespace combat
{

    void CombatSoundEffects::PlayShoot(const item::ItemPtr_t WEAPON_PTR) const
    {
        const auto WEAPON_TYPE { WEAPON_PTR->WeaponType() };

        if ((WEAPON_PTR->WeaponInfo().ProjectileType() == item::weapon::projectile_type::Blowpipe)
            || (WEAPON_PTR->WeaponInfo().ProjectileType() == item::weapon::projectile_type::Sling))
        {
            gui::SoundManager::Instance()
                ->GetSoundEffectSet(gui::sound_effect_set::BlowpipeShoot)
                .PlayRandom();
        }
        else if (
            WEAPON_PTR->WeaponInfo().IsBow()
            || (WEAPON_PTR->WeaponInfo().ProjectileType()
                == item::weapon::projectile_type::Crossbow))
        {
            gui::SoundManager::Instance()
                ->GetSoundEffectSet(gui::sound_effect_set::ArrowShoot)
                .PlayRandom();
        }
        else
        {
            M_HP_LOG_ERR(
                "combat::CombatSoundEffects::PlayShoot(weapon=\""
                << WEAPON_PTR->Name() << "\", category=\""
                << item::category::ToString(WEAPON_PTR->Category())
                << "\", weapon_type=" << item::weapon_type::ToString(WEAPON_TYPE)
                << ") Unable to find a 'shoot' sound effect to fit that weapon_type.");
        }
    }

    void CombatSoundEffects::PlayHitOrMiss(
        const creature::CreaturePtr_t CREATURE_PTR, const HitInfo & HIT_INFO) const
    {
        const auto WEAPON_PTR_OPT { HIT_INFO.Weapon() };

        M_HP_ASSERT_OR_LOG_AND_THROW(
            (!!WEAPON_PTR_OPT),
            "combat::CombatSoundEffects::PlayHitOrMiss() "
                << "was given a nullptr WEAPON_PTR.  HIT_INFO=" << HIT_INFO.ToString());

        const auto WEAPON_PTR { WEAPON_PTR_OPT.value() };
        const auto WEAPON_TYPE { WEAPON_PTR->WeaponType() };

        if (HIT_INFO.WasHit())
        {
            if (WEAPON_PTR->WeaponInfo().IsBlowpipe())
            {
                gui::SoundManager::Instance()
                    ->GetSoundEffectSet(gui::sound_effect_set::BlowpipeHit)
                    .PlayRandom();
            }
            else if (
                WEAPON_PTR->WeaponInfo().ProjectileType() == item::weapon::projectile_type::Sling)
            {
                // The horn hit sfx sounded good for slings too
                return gui::SoundManager::Instance()
                    ->GetSoundEffectSet(gui::sound_effect_set::MaterialHitMisc)
                    .Play(gui::sound_effect::MaterialHitHorn);
            }
            else if (
                WEAPON_PTR->WeaponInfo().IsBow()
                || (WEAPON_PTR->WeaponInfo().ProjectileType()
                    == item::weapon::projectile_type::Crossbow))
            {
                gui::SoundManager::Instance()
                    ->GetSoundEffectSet(gui::sound_effect_set::ArrowHit)
                    .PlayRandom();
            }
            else if (WEAPON_PTR->WeaponInfo().IsFists())
            {
                gui::SoundManager::Instance()
                    ->GetSoundEffectSet(gui::sound_effect_set::FistHit)
                    .PlayRandom();
            }
            else if (WEAPON_PTR->WeaponInfo().IsTentacles())
            {
                gui::SoundManager::Instance()
                    ->GetSoundEffectSet(gui::sound_effect_set::TentacleHit)
                    .PlayRandom();
            }
            else if (
                WEAPON_PTR->WeaponInfo().IsBreath()
                && (CREATURE_PTR->Role() == creature::role::Firebrand))
            {
                gui::SoundManager::Instance()->SoundEffectPlay(
                    gui::sound_effect::BreathHitFirebrand);
            }
            else if (
                WEAPON_PTR->WeaponInfo().IsBreath()
                && (CREATURE_PTR->Role() == creature::role::Sylavin))
            {
                gui::SoundManager::Instance()->SoundEffectPlay(gui::sound_effect::BreathHitSylavin);
            }
            else if (WEAPON_PTR->WeaponInfo().IsClaws())
            {
                gui::SoundManager::Instance()
                    ->GetSoundEffectSet(gui::sound_effect_set::ClawHit)
                    .PlayRandom();
            }
            else if (WEAPON_PTR->WeaponInfo().IsWhip())
            {
                gui::SoundManager::Instance()
                    ->GetSoundEffectSet(gui::sound_effect_set::WhipHit)
                    .PlayRandom();
            }
            else
            {
                switch (WEAPON_PTR->MaterialPrimary())
                {
                    case item::material::Wood:
                    {
                        return gui::SoundManager::Instance()
                            ->GetSoundEffectSet(gui::sound_effect_set::MaterialHitMisc)
                            .Play(gui::sound_effect::MaterialHitWood);
                    }

                    case item::material::Leather:
                    {
                        return gui::SoundManager::Instance()
                            ->GetSoundEffectSet(gui::sound_effect_set::MaterialHitMisc)
                            .Play(gui::sound_effect::MaterialHitLeather);
                    }

                    case item::material::Bone:
                    {
                        return gui::SoundManager::Instance()
                            ->GetSoundEffectSet(gui::sound_effect_set::MaterialHitMisc)
                            .Play(gui::sound_effect::MaterialHitBone);
                    }

                    case item::material::Water:
                    case item::material::Acid:
                    case item::material::Blood:
                    {
                        return gui::SoundManager::Instance()
                            ->GetSoundEffectSet(gui::sound_effect_set::MaterialHitMisc)
                            .Play(gui::sound_effect::MaterialHitLiquid);
                    }

                    case item::material::Plant:
                    {
                        return gui::SoundManager::Instance()
                            ->GetSoundEffectSet(gui::sound_effect_set::MaterialHitMisc)
                            .Play(gui::sound_effect::MaterialHitPlant);
                    }

                    case item::material::Hide:
                    {
                        return gui::SoundManager::Instance()
                            ->GetSoundEffectSet(gui::sound_effect_set::MaterialHitMisc)
                            .Play(gui::sound_effect::MaterialHitFlesh);
                    }

                    case item::material::Scales:
                    {
                        return gui::SoundManager::Instance()
                            ->GetSoundEffectSet(gui::sound_effect_set::MaterialHitMisc)
                            .Play(gui::sound_effect::MaterialHitScale);
                    }

                    case item::material::DriedFlesh:
                    {
                        return gui::SoundManager::Instance()
                            ->GetSoundEffectSet(gui::sound_effect_set::MaterialHitMisc)
                            .Play(gui::sound_effect::MaterialHitHide);
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
                        return gui::SoundManager::Instance()
                            ->GetSoundEffectSet(gui::sound_effect_set::MaterialHitMisc)
                            .Play(gui::sound_effect::MaterialHitStone);
                    }

                    case item::material::Rope:
                    case item::material::Cloth:
                    case item::material::Silk:
                    {
                        return gui::SoundManager::Instance()
                            ->GetSoundEffectSet(gui::sound_effect_set::MaterialHitMisc)
                            .Play(gui::sound_effect::MaterialHitCloth);
                    }

                    case item::material::Claw:
                    case item::material::Horn:
                    {
                        return gui::SoundManager::Instance()
                            ->GetSoundEffectSet(gui::sound_effect_set::MaterialHitMisc)
                            .Play(gui::sound_effect::MaterialHitHorn);
                    }

                    case item::material::Tooth:
                    {
                        return gui::SoundManager::Instance()
                            ->GetSoundEffectSet(gui::sound_effect_set::MaterialHitMisc)
                            .Play(gui::sound_effect::MaterialHitTooth);
                    }

                    case item::material::Tin:
                    case item::material::Gold:
                    case item::material::Iron:
                    case item::material::Bronze:
                    case item::material::Silver:
                    case item::material::Steel:
                    case item::material::Platinum:
                    {
                        return gui::SoundManager::Instance()
                            ->GetSoundEffectSet(gui::sound_effect_set::MaterialHitMetal)
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
                        return gui::SoundManager::Instance()
                            ->GetSoundEffectSet(gui::sound_effect_set::MaterialHitMisc)
                            .Play(gui::sound_effect::MaterialHitMisc);
                    }
                }
            }
        }
        else
        {
            if (WEAPON_TYPE & item::weapon_type::Projectile)
            {
                gui::SoundManager::Instance()
                    ->GetSoundEffectSet(gui::sound_effect_set::ProjectileMiss)
                    .PlayRandom();
            }
            else if (WEAPON_TYPE & item::weapon_type::Whip)
            {
                gui::SoundManager::Instance()
                    ->GetSoundEffectSet(gui::sound_effect_set::WhipMiss)
                    .PlayRandom();
            }
            else if (WEAPON_TYPE & item::weapon_type::Melee)
            {
                gui::SoundManager::Instance()
                    ->GetSoundEffectSet(gui::sound_effect_set::MeleeMiss)
                    .PlayRandom();
            }
            else
            {
                M_HP_LOG_ERR(
                    "combat::CombatSoundEffects::PlayHitOrMiss("
                    << "creature=\"" << CREATURE_PTR->NameAndRaceAndRole() << "\", weapon=\""
                    << WEAPON_PTR->Name() << "\", category=\""
                    << item::category::ToString(WEAPON_PTR->Category())
                    << "\", weapon_type=" << item::weapon_type::ToString(WEAPON_TYPE)
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
                gui::SoundManager::Instance()->SoundEffectPlay(gui::sound_effect::SpellSparks);
                break;
            }
            case spell::Spells::Bandage:
            {
                gui::SoundManager::Instance()
                    ->GetSoundEffectSet(gui::sound_effect_set::SpellBandage)
                    .PlayRandom();
                break;
            }
            case spell::Spells::Sleep:
            {
                gui::SoundManager::Instance()->SoundEffectPlay(gui::sound_effect::SpellSleep);
                break;
            }
            case spell::Spells::Awaken:
            {
                gui::SoundManager::Instance()->SoundEffectPlay(gui::sound_effect::SpellAwaken);
                break;
            }
            case spell::Spells::Trip:
            {
                gui::SoundManager::Instance()->SoundEffectPlay(gui::sound_effect::SpellTrip);

                gui::SoundManager::Instance()->SoundEffectPlay(gui::sound_effect::Trip, 1.0f);
                break;
            }
            case spell::Spells::Lift:
            {
                gui::SoundManager::Instance()->SoundEffectPlay(gui::sound_effect::SpellLift);
                break;
            }
            case spell::Spells::Daze:
            {
                gui::SoundManager::Instance()->SoundEffectPlay(gui::sound_effect::SpellDaze);
                break;
            }
            case spell::Spells::Panic:
            {
                gui::SoundManager::Instance()->SoundEffectPlay(gui::sound_effect::SpellFrighten);
                break;
            }
            case spell::Spells::ClearMind:
            {
                gui::SoundManager::Instance()->SoundEffectPlay(gui::sound_effect::SpellClearMind);
                break;
            }
            case spell::Spells::Poison:
            {
                gui::SoundManager::Instance()->SoundEffectPlay(gui::sound_effect::SpellPoison);
                break;
            }
            case spell::Spells::Antidote:
            {
                gui::SoundManager::Instance()->SoundEffectPlay(gui::sound_effect::SpellAntidote);
                break;
            }
            case spell::Spells::PoisonCloud:
            {
                gui::SoundManager::Instance()->SoundEffectPlay(gui::sound_effect::SpellPoisonCloud);
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
                gui::SoundManager::Instance()->SoundEffectPlay(gui::sound_effect::SongRallyDrum);
                break;
            }
            case song::Songs::SpiritResonance:
            {
                gui::SoundManager::Instance()->SoundEffectPlay(
                    gui::sound_effect::SongSpiritResonance);
                break;
            }
            case song::Songs::RousingRhythm:
            {
                gui::SoundManager::Instance()->SoundEffectPlay(
                    gui::sound_effect::SongRousingRhythm);
                break;
            }
            case song::Songs::TripBeat:
            {
                gui::SoundManager::Instance()->SoundEffectPlay(gui::sound_effect::SongTripBeat);
                break;
            }
            case song::Songs::PanicStrings:
            {
                gui::SoundManager::Instance()->SoundEffectPlay(gui::sound_effect::SongPanicStrings);
                break;
            }
            case song::Songs::Lullaby:
            {
                gui::SoundManager::Instance()->SoundEffectPlay(gui::sound_effect::SongLullaby);
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
        const auto ROLE { CREATURE_PTR->Role() };
        if (ROLE == creature::role::Wolfen)
        {
            switch (CREATURE_PTR->WolfenClass())
            {
                case creature::wolfen_class::Pup:
                {
                    gui::SoundManager::Instance()->SoundEffectPlay(
                        gui::sound_effect::RoarWolfenPup);
                    return;
                }
                case creature::wolfen_class::Juvenile:
                {
                    gui::SoundManager::Instance()->SoundEffectPlay(
                        gui::sound_effect::RoarWolfenJuvenile);
                    return;
                }
                case creature::wolfen_class::Adult:
                {
                    gui::SoundManager::Instance()->SoundEffectPlay(
                        gui::sound_effect::RoarWolfenAdult);
                    return;
                }
                case creature::wolfen_class::Noble:
                {
                    gui::SoundManager::Instance()->SoundEffectPlay(
                        gui::sound_effect::RoarWolfenNoble);
                    return;
                }
                case creature::wolfen_class::Highborn:
                {
                    gui::SoundManager::Instance()->SoundEffectPlay(
                        gui::sound_effect::RoarWolfenHighborn);
                    return;
                }
                case creature::wolfen_class::Elder:
                {
                    gui::SoundManager::Instance()->SoundEffectPlay(
                        gui::sound_effect::RoarWolfenElder);
                    return;
                }
                case creature::wolfen_class::Count:
                default:
                {
                }
            }
        }
        else if (ROLE == creature::role::Firebrand)
        {
            switch (CREATURE_PTR->DragonClass())
            {
                case creature::dragon_class::Hatchling:
                {
                    gui::SoundManager::Instance()->SoundEffectPlay(
                        gui::sound_effect::RoarDragonFirebrandHatchling);
                    return;
                }
                case creature::dragon_class::Whelp:
                {
                    gui::SoundManager::Instance()->SoundEffectPlay(
                        gui::sound_effect::RoarDragonFirebrandWhelp);
                    return;
                }
                case creature::dragon_class::Fledgling:
                {
                    gui::SoundManager::Instance()->SoundEffectPlay(
                        gui::sound_effect::RoarDragonFirebrandFledgling);
                    return;
                }
                case creature::dragon_class::Juvenile:
                {
                    gui::SoundManager::Instance()->SoundEffectPlay(
                        gui::sound_effect::RoarDragonFirebrandJuvenile);
                    return;
                }
                case creature::dragon_class::Adult:
                {
                    gui::SoundManager::Instance()->SoundEffectPlay(
                        gui::sound_effect::RoarDragonFirebrandAdult);
                    return;
                }
                case creature::dragon_class::Wyrm:
                {
                    gui::SoundManager::Instance()->SoundEffectPlay(
                        gui::sound_effect::RoarDragonFirebrandWyrm);
                    return;
                }
                case creature::dragon_class::Skycaster:
                {
                    gui::SoundManager::Instance()->SoundEffectPlay(
                        gui::sound_effect::RoarDragonFirebrandSkycaster);
                    return;
                }
                case creature::dragon_class::Elder:
                {
                    gui::SoundManager::Instance()->SoundEffectPlay(
                        gui::sound_effect::RoarDragonFirebrandElder);
                    return;
                }
                case creature::dragon_class::Count:
                default:
                {
                }
            }
        }
        else if (ROLE == creature::role::Sylavin)
        {
            switch (CREATURE_PTR->DragonClass())
            {
                case creature::dragon_class::Hatchling:
                {
                    gui::SoundManager::Instance()->SoundEffectPlay(
                        gui::sound_effect::RoarDragonSylavinHatchling);
                    return;
                }
                case creature::dragon_class::Whelp:
                {
                    gui::SoundManager::Instance()->SoundEffectPlay(
                        gui::sound_effect::RoarDragonSylavinWhelp);
                    return;
                }
                case creature::dragon_class::Fledgling:
                {
                    gui::SoundManager::Instance()->SoundEffectPlay(
                        gui::sound_effect::RoarDragonSylavinFledgling);
                    return;
                }
                case creature::dragon_class::Juvenile:
                {
                    gui::SoundManager::Instance()->SoundEffectPlay(
                        gui::sound_effect::RoarDragonSylavinJuvenile);
                    return;
                }
                case creature::dragon_class::Adult:
                {
                    gui::SoundManager::Instance()->SoundEffectPlay(
                        gui::sound_effect::RoarDragonSylavinAdult);
                    return;
                }
                case creature::dragon_class::Wyrm:
                {
                    gui::SoundManager::Instance()->SoundEffectPlay(
                        gui::sound_effect::RoarDragonSylavinWyrm);
                    return;
                }
                case creature::dragon_class::Skycaster:
                {
                    gui::SoundManager::Instance()->SoundEffectPlay(
                        gui::sound_effect::RoarDragonSylavinSkycaster);
                    return;
                }
                case creature::dragon_class::Elder:
                {
                    gui::SoundManager::Instance()->SoundEffectPlay(
                        gui::sound_effect::RoarDragonSylavinElder);
                    return;
                }
                case creature::dragon_class::Count:
                default:
                {
                }
            }

            M_HP_LOG_ERR(
                "combat::CombatSoundEffects::PlayRoar("
                << "creature=\"" << CREATURE_PTR->NameAndRaceAndRole()
                << ") failed to find an sfx to play.");
        }
    }

    void CombatSoundEffects::PlayDeath(const creature::CreaturePtr_t CREATURE_PTR) const
    {
        gui::SfxEnumVec_t sfxVec;
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
            gui::SoundManager::Instance()->SoundEffectPlay(misc::RandomSelect(sfxVec));
        }
    }

    void CombatSoundEffects::GetDeathSfx(
        const creature::CreaturePtr_t CREATURE_PTR, gui::SfxEnumVec_t & sfxVec_OutParam) const
    {
        switch (CREATURE_PTR->Race())
        {
            case creature::race::Human:
            case creature::race::Gnome:
            {
                sfxVec_OutParam.emplace_back(gui::sound_effect::DeathHuman1);
                sfxVec_OutParam.emplace_back(gui::sound_effect::DeathHuman2);
                sfxVec_OutParam.emplace_back(gui::sound_effect::DeathHuman3);
                sfxVec_OutParam.emplace_back(gui::sound_effect::DeathHuman4);
                sfxVec_OutParam.emplace_back(gui::sound_effect::DeathHuman5);

                if (CREATURE_PTR->Sex() == creature::sex::Male)
                {
                    sfxVec_OutParam.emplace_back(gui::sound_effect::DeathHumanMale1);
                    sfxVec_OutParam.emplace_back(gui::sound_effect::DeathHumanMale2);
                    sfxVec_OutParam.emplace_back(gui::sound_effect::DeathHumanMale3);
                    sfxVec_OutParam.emplace_back(gui::sound_effect::DeathHumanMale4);
                    sfxVec_OutParam.emplace_back(gui::sound_effect::DeathHumanMale5);
                    sfxVec_OutParam.emplace_back(gui::sound_effect::DeathHumanMale6);
                    sfxVec_OutParam.emplace_back(gui::sound_effect::DeathHumanMale7);
                }
                else if (CREATURE_PTR->Sex() == creature::sex::Female)
                {
                    sfxVec_OutParam.emplace_back(gui::sound_effect::DeathHumanFemale1);
                    sfxVec_OutParam.emplace_back(gui::sound_effect::DeathHumanFemale3);
                }
                return;
            }
            case creature::race::Halfling:
            {
                sfxVec_OutParam.emplace_back(gui::sound_effect::DeathHuman2);
                sfxVec_OutParam.emplace_back(gui::sound_effect::DeathHuman4);

                if (CREATURE_PTR->Sex() == creature::sex::Male)
                {
                    sfxVec_OutParam.emplace_back(gui::sound_effect::DeathHumanMale3);
                    sfxVec_OutParam.emplace_back(gui::sound_effect::DeathHumanMale5);
                    sfxVec_OutParam.emplace_back(gui::sound_effect::DeathHumanMale6);
                }
                else if (CREATURE_PTR->Sex() == creature::sex::Female)
                {
                    sfxVec_OutParam.emplace_back(gui::sound_effect::DeathHumanFemale2);
                }
                return;
            }
            case creature::race::Pixie:
            {
                if (CREATURE_PTR->Sex() == creature::sex::Male)
                {
                    sfxVec_OutParam.emplace_back(gui::sound_effect::DeathHumanMale2);
                    sfxVec_OutParam.emplace_back(gui::sound_effect::DeathHumanMale4);
                }
                else
                {
                    sfxVec_OutParam.emplace_back(gui::sound_effect::DeathHumanFemale1);
                    sfxVec_OutParam.emplace_back(gui::sound_effect::DeathHumanFemale2);
                    sfxVec_OutParam.emplace_back(gui::sound_effect::DeathHumanFemale3);
                }
                return;
            }
            case creature::race::Witch:
            case creature::race::Harpy:
            {
                sfxVec_OutParam.emplace_back(gui::sound_effect::DeathHumanFemale1);
                sfxVec_OutParam.emplace_back(gui::sound_effect::DeathHumanFemale2);
                sfxVec_OutParam.emplace_back(gui::sound_effect::DeathHumanFemale3);
                return;
            }
            case creature::race::Golem:
            {
                sfxVec_OutParam.emplace_back(gui::sound_effect::DeathBeastCaveTroll1);
                return;
            }
            case creature::race::ThreeHeadedHound:
            case creature::race::Werewolf:
            case creature::race::Wolfen:
            {
                sfxVec_OutParam.emplace_back(gui::sound_effect::DeathWolf);
                return;
            }
            case creature::race::Hydra:
            case creature::race::Dragon:
            case creature::race::Wyvern:
            {
                sfxVec_OutParam.emplace_back(gui::sound_effect::DeathDragon1a);
                sfxVec_OutParam.emplace_back(gui::sound_effect::DeathDragon1b);
                return;
            }
            case creature::race::Newt:
            case creature::race::Goblin:
            case creature::race::Pug:
            {
                sfxVec_OutParam.emplace_back(gui::sound_effect::DeathBeastSmallRaspy1);
                sfxVec_OutParam.emplace_back(gui::sound_effect::DeathBeastSmallRaspy2);
                sfxVec_OutParam.emplace_back(gui::sound_effect::DeathBeastSmallRaspy3);
                sfxVec_OutParam.emplace_back(gui::sound_effect::DeathBeastSmallRaspy4);
                return;
            }
            case creature::race::Troll:
            {
                sfxVec_OutParam.emplace_back(gui::sound_effect::DeathBeastRough1);
                sfxVec_OutParam.emplace_back(gui::sound_effect::DeathBeastRough2);
                sfxVec_OutParam.emplace_back(gui::sound_effect::DeathBeastRough3);
                return;
            }
            case creature::race::Orc:
            {
                sfxVec_OutParam.emplace_back(gui::sound_effect::DeathBeast5);
                return;
            }
            case creature::race::Beetle:
            case creature::race::Spider:
            {
                sfxVec_OutParam.emplace_back(gui::sound_effect::DeathSpider1);
                sfxVec_OutParam.emplace_back(gui::sound_effect::DeathSpider2);
                sfxVec_OutParam.emplace_back(gui::sound_effect::DeathSpider3);
                return;
            }
            case creature::race::Bog:
            {
                if (CREATURE_PTR->Role() == creature::role::Smasher)
                {
                    sfxVec_OutParam.emplace_back(gui::sound_effect::DeathBeast3);
                }
                else
                {
                    sfxVec_OutParam.emplace_back(gui::sound_effect::DeathBeastSmallRaspy4);
                }
                return;
            }
            case creature::race::CaveCrawler:
            {
                sfxVec_OutParam.emplace_back(gui::sound_effect::DeathThing1);
                return;
            }
            case creature::race::LizardWalker:
            {
                sfxVec_OutParam.emplace_back(gui::sound_effect::DeathBeastTalker1);
                return;
            }
            case creature::race::Minotaur:
            {
                sfxVec_OutParam.emplace_back(gui::sound_effect::DeathBeast1);
                return;
            }
            case creature::race::Ogre:
            {
                sfxVec_OutParam.emplace_back(gui::sound_effect::DeathBeastBig1);
                sfxVec_OutParam.emplace_back(gui::sound_effect::DeathBeastBig2);
                sfxVec_OutParam.emplace_back(gui::sound_effect::DeathBeastBig3);
                return;
            }
            case creature::race::Plant:
            {
                sfxVec_OutParam.emplace_back(gui::sound_effect::DeathBeastWet);
                return;
            }
            case creature::race::Shade:
            {
                sfxVec_OutParam.emplace_back(gui::sound_effect::DeathGhostly1a);
                sfxVec_OutParam.emplace_back(gui::sound_effect::DeathGhostly1b);
                sfxVec_OutParam.emplace_back(gui::sound_effect::DeathGhostly1c);
                return;
            }
            case creature::race::Skeleton:
            {
                sfxVec_OutParam.emplace_back(gui::sound_effect::DeathCreepy1);
                sfxVec_OutParam.emplace_back(gui::sound_effect::DeathCreepy2);
                sfxVec_OutParam.emplace_back(gui::sound_effect::DeathCreepy3);
                sfxVec_OutParam.emplace_back(gui::sound_effect::DeathCreepy4);
                sfxVec_OutParam.emplace_back(gui::sound_effect::DeathCreepy5);
                sfxVec_OutParam.emplace_back(gui::sound_effect::DeathCreepy6);
                return;
            }
            case creature::race::Boar:
            {
                sfxVec_OutParam.emplace_back(gui::sound_effect::DeathBoar);
                return;
            }
            case creature::race::Demon:
            {
                sfxVec_OutParam.emplace_back(gui::sound_effect::DeathHowlDemon);
                return;
            }
            case creature::race::Griffin:
            {
                sfxVec_OutParam.emplace_back(gui::sound_effect::DeathGriffin);
                return;
            }
            case creature::race::Lion:
            {
                sfxVec_OutParam.emplace_back(gui::sound_effect::DeathLion);
                return;
            }
            case creature::race::Wereboar:
            case creature::race::LionBoar:
            {
                sfxVec_OutParam.emplace_back(gui::sound_effect::DeathLionboar);
                return;
            }
            case creature::race::Naga:
            {
                sfxVec_OutParam.emplace_back(gui::sound_effect::DeathBeastTalker2Roar);
                return;
            }
            case creature::race::Ramonaut:
            {
                sfxVec_OutParam.emplace_back(gui::sound_effect::DeathBeast3);
                return;
            }
            case creature::race::Serpent:
            case creature::race::Cobra:
            {
                sfxVec_OutParam.emplace_back(gui::sound_effect::DeathSnake1);
                sfxVec_OutParam.emplace_back(gui::sound_effect::DeathSnake2);
                sfxVec_OutParam.emplace_back(gui::sound_effect::DeathSnake3);
                sfxVec_OutParam.emplace_back(gui::sound_effect::DeathSnake4);
                sfxVec_OutParam.emplace_back(gui::sound_effect::DeathSnake5);
                sfxVec_OutParam.emplace_back(gui::sound_effect::DeathSnake6);
                return;
            }
            case creature::race::Werebear:
            {
                sfxVec_OutParam.emplace_back(gui::sound_effect::DeathBeast4);
                return;
            }
            case creature::race::Werecat:
            {
                sfxVec_OutParam.emplace_back(gui::sound_effect::DeathThing3);
                return;
            }
            case creature::race::Bat:
            case creature::race::Werebat:
            {
                sfxVec_OutParam.emplace_back(gui::sound_effect::DeathThing2);
                return;
            }
            case creature::race::Giant:
            {
                sfxVec_OutParam.emplace_back(gui::sound_effect::DeathBeastHuge1a);
                sfxVec_OutParam.emplace_back(gui::sound_effect::DeathBeastHuge1b);
                return;
            }
            case creature::race::Ghoul:
            {
                sfxVec_OutParam.emplace_back(gui::sound_effect::DeathBeast6);
                return;
            }
            case creature::race::Count:
            default:
            {
            }
        }
    }

} // namespace combat
} // namespace heroespath
