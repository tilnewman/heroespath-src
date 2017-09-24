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
// trap-warehouse.cpp
//
#include "trap-warehouse.hpp"

#include "sfml-util/sound-effects-enum.hpp"

#include "game/log-macros.hpp"

#include "misc/assertlogandthrow.hpp"


namespace game
{
namespace trap
{

    TrapVec_t game::trap::Warehouse::traps_{};


    void Warehouse::Fill()
    {
        Empty();

        traps_.push_back(Trap(
            1, 1,
            2, 6,
            sfml_util::sound_effect::TrapPop,
            "The lock bursts in the lock picker's hands!"));

        traps_.push_back(Trap(
            1, 1,
            3, 8,
            sfml_util::sound_effect::TrapMetalBang,
            "The lock bursts in the lock picker's hands!"));

        traps_.push_back(Trap(
            1, 3,
            4, 9,
            sfml_util::sound_effect::TrapMetal,
            "Sharp metal pieces burst out and tear through the party!"));

        traps_.push_back(Trap(
            1, 3,
            6, 12,
            sfml_util::sound_effect::TrapSplutter,
            "The lock oozes with toxic black sludge!"));

        traps_.push_back(Trap(
            1, 3,
            6, 12,
            sfml_util::sound_effect::TrapSplutter,
            "The lock oozes with toxic black sludge!"));

        traps_.push_back(Trap(
            3, 6,
            8, 14,
            sfml_util::sound_effect::TrapSplatDunk,
            "Green slime spits out from holes in the ",
            "!"));

        traps_.push_back(Trap(
            2, 4,
            6, 12,
            sfml_util::sound_effect::TrapBang,
            "The lock detonates!"));

        traps_.push_back(Trap(
            3, 6,
            8, 16,
            sfml_util::sound_effect::TrapBoom,
            "The ",
            "  blows apart!"));

        traps_.push_back(Trap(
            4, 6,
            10, 20,
            sfml_util::sound_effect::TrapChungSplutter,
            "The ",
            "  explodes!"));

        traps_.push_back(Trap(
            1, 1,
            8, 14,
            sfml_util::sound_effect::TrapFire,
            "The lock becomes scalding and burns the lock picker!"));

        traps_.push_back(Trap(
            1, 3,
            10, 15,
            sfml_util::sound_effect::TrapInferno,
            "The whole ",
            "  erupts into a raging inferno!"));

        traps_.push_back(Trap(
            4, 6,
            15, 20,
            sfml_util::sound_effect::TrapFireball,
            "A fireball engulfs the party!"));

        traps_.push_back(Trap(
            6, 6,
            8, 14,
            sfml_util::sound_effect::TrapChiaowahh,
            "You hear haunting moans and a ghostly yellow light shines from the ",
            "  searing the party!"));

        traps_.push_back(Trap(
            6, 6,
            11, 18,
            sfml_util::sound_effect::TrapChiaowahh,
            "You hear cries of pain and a ghostly green light shines from the ",
            "  searing the party!"));

        traps_.push_back(Trap(
            6, 6,
            14, 24,
            sfml_util::sound_effect::TrapChiaowahh,
            "You hear frightful screams and a ghostly blue light shines from the ",
            "  searing the party!"));

        traps_.push_back(Trap(
            6, 6,
            17, 28,
            sfml_util::sound_effect::TrapChiaowahh,
            "You hear horrible wailing and a ghostly red light shines from the ",
            "  searing the party!"));

        traps_.push_back(Trap(
            1, 3,
            12, 18,
            sfml_util::sound_effect::TrapGasExhale,
            "Noxious gas hisses from vents in the ",
            "!"));

        traps_.push_back(Trap(
            2, 6,
            16, 24,
            sfml_util::sound_effect::TrapGasLeak,
            "A deadly chemical mist escapes!"));

        traps_.push_back(Trap(
            1, 2,
            8, 14,
            sfml_util::sound_effect::TrapSparksHiss,
            "Sparks spray out from the ",
            "!"));

        traps_.push_back(Trap(
            2, 4,
            12, 24,
            sfml_util::sound_effect::TrapSparksAhh,
            "A torrent of molten metal showers the party!"));

        traps_.push_back(Trap(
            1, 1,
            25, 50,
            sfml_util::sound_effect::TrapSpiritShortHiss,
            "A spectral skeleton hand reaches out and strikes at the lock picker!"));

        traps_.push_back(Trap(
            2, 4,
            30, 60,
            sfml_util::sound_effect::TrapGhost2,
            "The raging ghost of a murdered witch emerges and slashes the party with ghostly claws!"));

        traps_.push_back(Trap(
            4, 6,
            40, 80,
            sfml_util::sound_effect::TrapGhost2,
            "Spirits/Specters/Phantoms/Phantasms/Wraiths emerge and tear through the party!"));

        std::sort(
            std::begin(traps_),
            std::end(traps_),
            [](auto const & A, auto const & B)
            {
                return A.Severity() < B.Severity();
            });
    }


    void Warehouse::Empty()
    {
        traps_.clear();
    }


    const TrapVec_t Warehouse::Get()
    {
        M_ASSERT_OR_LOGANDTHROW_SS((traps_.empty() == false),
            "game::trap::Warehouse::Get() called before warehouse was Fill()ed.");

        return traps_;
    }


    const TrapVec_t Warehouse::GetWithSevertiyLessThanOrEqualTo(const int SEVERITY)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((traps_.empty() == false),
            "game::trap::Warehouse::GetWithSevertiyLessThanOrEqualTo() called before "
            << "warehouse was Fill()ed.");

        TrapVec_t trapsWithMatchingSeverity;

        std::copy_if(
            std::begin(traps_),
            std::end(traps_),
            std::back_inserter(trapsWithMatchingSeverity),
            [SEVERITY](auto const & TRAP)
            {
                return TRAP.Severity() <= SEVERITY;
            });

        return trapsWithMatchingSeverity;
    }


    const TrapVec_t Warehouse::GetWithSeverityRatioLessThanOrEqualTo(const float SEVERITY_RATIO)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((traps_.empty() == false),
            "game::trap::Warehouse::GetWithSevertiyLessThanOrEqualTo() called before "
            << "warehouse was Fill()ed.");

        auto const SEVERITY_MAX_F{ static_cast<float>(GetWithMaxSeverity().Severity()) };
        auto const SEVERITY{ static_cast<int>(SEVERITY_MAX_F * SEVERITY_RATIO) };
        return GetWithSevertiyLessThanOrEqualTo(SEVERITY);
    }


    const Trap Warehouse::GetWithMinSeverity()
    {
        M_ASSERT_OR_LOGANDTHROW_SS((traps_.empty() == false),
            "game::trap::Warehouse::GetWithMinSeverity() called before warehouse was Fill()ed.");

        return traps_[0];
    }


    const Trap Warehouse::GetWithMaxSeverity()
    {
        M_ASSERT_OR_LOGANDTHROW_SS((traps_.empty() == false),
            "game::trap::Warehouse::GetWithMinSeverity() called before warehouse was Fill()ed.");

        return traps_[traps_.size() - 1];
    }

}
}
