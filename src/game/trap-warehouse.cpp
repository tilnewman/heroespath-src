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

#include "misc/vectors.hpp"
#include "misc/assertlogandthrow.hpp"

#include <algorithm>


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
            "blasted",
            sfml_util::sound_effect::TrapPop,
            "The lock bursts in the lock picker's hands!"));

        traps_.push_back(Trap(
            1, 1,
            3, 8,
            "blasted",
            sfml_util::sound_effect::TrapMetalBang,
            "A metal spike pops out from the",
            "!"));

        traps_.push_back(Trap(
            1, 3,
            4, 9,
            "sliced",
            sfml_util::sound_effect::TrapMetal,
            "Sharp metal pieces burst out and tear through the party!"));

        traps_.push_back(Trap(
            1, 3,
            6, 12,
            "poisoned",
            sfml_util::sound_effect::TrapSplutter,
            "The lock oozes with toxic black sludge!"));

        traps_.push_back(Trap(
            3, 6,
            8, 14,
            "sickened",
            sfml_util::sound_effect::TrapSplatDunk,
            "Green slime spits out from holes in the ",
            "!"));

        traps_.push_back(Trap(
            2, 4,
            6, 12,
            "blasted",
            sfml_util::sound_effect::TrapBang,
            "The lock detonates!"));

        traps_.push_back(Trap(
            3, 6,
            8, 16,
            "blasted",
            sfml_util::sound_effect::TrapBoom,
            "The ",
            "  blows apart!"));

        traps_.push_back(Trap(
            4, 6,
            10, 20,
            "blasted",
            sfml_util::sound_effect::TrapChungSplutter,
            "The ",
            "  explodes!"));

        traps_.push_back(Trap(
            1, 1,
            8, 14,
            "burned",
            sfml_util::sound_effect::TrapFire,
            "The lock becomes scalding and burns the lock picker!"));

        traps_.push_back(Trap(
            1, 3,
            10, 15,
            "burned",
            sfml_util::sound_effect::TrapInferno,
            "The whole ",
            "  erupts into a raging inferno!"));

        traps_.push_back(Trap(
            4, 6,
            15, 20,
            "burned",
            sfml_util::sound_effect::TrapFireball,
            "A fireball engulfs the party!"));

        traps_.push_back(Trap(
            6, 6,
            8, 14,
            "burned",
            sfml_util::sound_effect::TrapChiaowahh,
            "You hear haunting moans and a ghostly yellow light shines from the ",
            "  searing the party!"));

        traps_.push_back(Trap(
            6, 6,
            11, 18,
            "burned",
            sfml_util::sound_effect::TrapChiaowahh,
            "You hear cries of pain and a ghostly green light shines from the ",
            "  searing the party!"));

        traps_.push_back(Trap(
            6, 6,
            14, 24,
            "burned",
            sfml_util::sound_effect::TrapChiaowahh,
            "You hear frightful screams and a ghostly blue light shines from the ",
            "  searing the party!"));

        traps_.push_back(Trap(
            6, 6,
            17, 28,
            "burned",
            sfml_util::sound_effect::TrapChiaowahh,
            "You hear horrible wailing and a ghostly red light shines from the ",
            "  searing the party!"));
        
        traps_.push_back(Trap(
            1, 4,
            12, 18,
            "poisoned",
            sfml_util::sound_effect::TrapGasExhale,
            "Noxious gas hisses from vents in the ",
            "!"));
        
        traps_.push_back(Trap(
            2, 6,
            16, 24,
            "poisoned",
            sfml_util::sound_effect::TrapGasLeak,
            "A deadly chemical mist escapes!"));
        
        traps_.push_back(Trap(
            2, 4,
            8, 14,
            "burned",
            sfml_util::sound_effect::TrapSparksHiss,
            "Sparks spray out from the ",
            "!"));
        
        traps_.push_back(Trap(
            3, 6,
            12, 24,
            "sliced",
            sfml_util::sound_effect::TrapSparksAhh,
            "A torrent of molten metal showers the party!"));
        
        traps_.push_back(Trap(
            1, 1,
            25, 50,
            "sliced",
            sfml_util::sound_effect::TrapSpiritShortHiss,
            "A spectral skeleton hand reaches out and strikes at the lock picker!"));
        
        traps_.push_back(Trap(
            2, 4,
            30, 50,
            "sliced",
            sfml_util::sound_effect::TrapGhost2,
            "The raging ghost of a murdered witch emerges and slashes the party with ghostly claws!"));
        
        traps_.push_back(Trap(
            4, 6,
            20, 40,
            "sliced",
            sfml_util::sound_effect::TrapGhost2,
            "Evil spirits emerge and tear through the party!"));
        
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


    int Warehouse::GetMinSeverity()
    {
        M_ASSERT_OR_LOGANDTHROW_SS((traps_.empty() == false),
            "game::trap::Warehouse::GetWithMinSeverity() called before warehouse was Fill()ed.");

        return traps_[0].Severity();
    }


    int Warehouse::GetMaxSeverity()
    {
        M_ASSERT_OR_LOGANDTHROW_SS((traps_.empty() == false),
            "game::trap::Warehouse::GetWithMinSeverity() called before warehouse was Fill()ed.");

        return traps_[traps_.size() - 1].Severity();
    }


    const TrapVec_t Warehouse::GetWithSeverityRatioBetween(
        const float THE_MIN,
        const float THE_MAX)
    {
        M_ASSERT_OR_LOGANDTHROW_SS((traps_.empty() == false),
            "game::trap::Warehouse::GetWithSeverityRatio() called before "
            << "warehouse was Fill()ed.");

        auto const SEVERITY_MAX_F{ static_cast<float>(GetMaxSeverity()) };

        TrapVec_t trapsWithSeverityWithinRange;

        for (auto const & TRAP : traps_)
        {
            auto const SEVERITY_RATIO{ static_cast<float>(TRAP.Severity()) / SEVERITY_MAX_F };

            if (((SEVERITY_RATIO > THE_MIN) &&
                 (SEVERITY_RATIO < THE_MAX)) ||
                misc::IsRealClose(SEVERITY_RATIO, THE_MIN) ||
                misc::IsRealClose(SEVERITY_RATIO, THE_MAX))
            {
                trapsWithSeverityWithinRange.push_back(TRAP);
            }
        }

        if (trapsWithSeverityWithinRange.empty())
        {
            M_HP_LOG_ERR("game::trap::Warehouse::GetWithSeverityRatioBetween(min=" << THE_MIN 
                << ", max=" << THE_MAX << ") "
                << "resulted in no traps, so the default weakest trap is being used.");

            trapsWithSeverityWithinRange.push_back(traps_[0]);
        }

        return trapsWithSeverityWithinRange;
    }


    const Trap Warehouse::SelectRandomWithSeverityRatioNear(const float SEVERITY_RATIO)
    {
        auto severityRatioMin{ SEVERITY_RATIO - 0.333f };
        if (severityRatioMin < 0.0f)
        {
            severityRatioMin = 0.0f;
        }

        auto severityRatioMax{ SEVERITY_RATIO + 0.1f };
        if (severityRatioMax > 1.0f)
        {
            severityRatioMax = 1.0f;
        }

        return misc::Vector::SelectRandom(
            GetWithSeverityRatioBetween(severityRatioMin, severityRatioMax));
    }

}
}
