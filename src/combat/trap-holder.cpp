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
// trap-holder.cpp
//
#include "trap-holder.hpp"

#include "log/log-macros.hpp"
#include "misc/assertlogandthrow.hpp"
#include "misc/vectors.hpp"
#include "sfml-util/sound-effects-enum.hpp"

#include <algorithm>

namespace heroespath
{
namespace combat
{
    namespace trap
    {

        TrapVec_t combat::trap::Holder::traps_{};

        void Holder::Fill()
        {
            Empty();

            traps_.emplace_back(Trap(
                1,
                1,
                2_health,
                6_health,
                "blasted",
                sfml_util::sound_effect::TrapPop,
                "The lock bursts in the lock picker's hands!"));

            traps_.emplace_back(Trap(
                1,
                1,
                3_health,
                8_health,
                "stabbed",
                sfml_util::sound_effect::TrapMetalBang,
                "A metal spike pops out from the",
                "!"));

            traps_.emplace_back(Trap(
                1,
                3,
                4_health,
                9_health,
                "sliced",
                sfml_util::sound_effect::TrapMetal,
                "Sharp metal pieces burst out and tear through the party!"));

            traps_.emplace_back(Trap(
                1,
                3,
                6_health,
                12_health,
                "poisoned",
                sfml_util::sound_effect::TrapSplutter,
                "The lock oozes with toxic black sludge!"));

            traps_.emplace_back(Trap(
                3,
                6,
                8_health,
                14_health,
                "sickened",
                sfml_util::sound_effect::TrapSplatDunk,
                "Green slime spits out from holes in the ",
                "!"));

            traps_.emplace_back(Trap(
                2,
                4,
                6_health,
                12_health,
                "blasted",
                sfml_util::sound_effect::TrapBang,
                "The lock detonates!"));

            traps_.emplace_back(Trap(
                3,
                6,
                8_health,
                16_health,
                "blasted",
                sfml_util::sound_effect::TrapBoom,
                "The ",
                "  blows apart!"));

            traps_.emplace_back(Trap(
                4,
                6,
                10_health,
                20_health,
                "blasted",
                sfml_util::sound_effect::TrapChungSplutter,
                "The ",
                "  explodes!"));

            traps_.emplace_back(Trap(
                1,
                1,
                8_health,
                14_health,
                "burned",
                sfml_util::sound_effect::TrapFire,
                "The lock becomes scalding and burns the lock picker!"));

            traps_.emplace_back(Trap(
                1,
                3,
                10_health,
                15_health,
                "burned",
                sfml_util::sound_effect::TrapInferno,
                "The whole ",
                "  erupts into a raging inferno!"));

            traps_.emplace_back(Trap(
                4,
                6,
                15_health,
                20_health,
                "burned",
                sfml_util::sound_effect::TrapFireball,
                "A fireball engulfs the party!"));

            traps_.emplace_back(Trap(
                6,
                6,
                8_health,
                14_health,
                "burned",
                sfml_util::sound_effect::TrapChiaowahh,
                "You hear haunting moans and a ghostly yellow light shines from the ",
                "  searing the party!"));

            traps_.emplace_back(Trap(
                6,
                6,
                11_health,
                18_health,
                "burned",
                sfml_util::sound_effect::TrapChiaowahh,
                "You hear cries of pain and a ghostly green light shines from the ",
                "  searing the party!"));

            traps_.emplace_back(Trap(
                6,
                6,
                14_health,
                24_health,
                "burned",
                sfml_util::sound_effect::TrapChiaowahh,
                "You hear frightful screams and a ghostly blue light shines from the ",
                "  searing the party!"));

            traps_.emplace_back(Trap(
                6,
                6,
                17_health,
                28_health,
                "burned",
                sfml_util::sound_effect::TrapChiaowahh,
                "You hear horrible wailing and a ghostly red light shines from the ",
                "  searing the party!"));

            traps_.emplace_back(Trap(
                1,
                4,
                12_health,
                18_health,
                "poisoned",
                sfml_util::sound_effect::TrapGasExhale,
                "Noxious gas hisses from vents in the ",
                "!"));

            traps_.emplace_back(Trap(
                2,
                6,
                16_health,
                24_health,
                "poisoned",
                sfml_util::sound_effect::TrapGasLeak,
                "A deadly chemical mist escapes!"));

            traps_.emplace_back(Trap(
                2,
                4,
                8_health,
                14_health,
                "burned",
                sfml_util::sound_effect::TrapSparksHiss,
                "Sparks spray out from the ",
                "!"));

            traps_.emplace_back(Trap(
                3,
                6,
                12_health,
                24_health,
                "sliced",
                sfml_util::sound_effect::TrapSparksAhh,
                "A torrent of molten metal showers the party!"));

            traps_.emplace_back(Trap(
                1,
                1,
                25_health,
                50_health,
                "sliced",
                sfml_util::sound_effect::TrapSpiritShortHiss,
                "A spectral skeleton hand reaches out and strikes at the lock picker!"));

            traps_.emplace_back(Trap(
                2,
                4,
                30_health,
                50_health,
                "sliced",
                sfml_util::sound_effect::TrapGhost2,
                "The raging ghost of a murdered witch emerges and slashes the party with ghostly "
                "claws!"));

            traps_.emplace_back(Trap(
                4,
                6,
                20_health,
                40_health,
                "sliced",
                sfml_util::sound_effect::TrapGhost2,
                "Evil spirits emerge and tear through the party!"));

            std::sort(std::begin(traps_), std::end(traps_), [](auto const & A, auto const & B) {
                return A.Severity() < B.Severity();
            });
        }

        void Holder::Empty() { traps_.clear(); }

        int Holder::GetMinSeverity()
        {
            M_ASSERT_OR_LOGANDTHROW_SS(
                (traps_.empty() == false),
                "combat::trap::Holder::GetWithMinSeverity() called when the holder was "
                "empty.");

            return traps_[0].Severity();
        }

        int Holder::GetMaxSeverity()
        {
            M_ASSERT_OR_LOGANDTHROW_SS(
                (traps_.empty() == false),
                "combat::trap::Holder::GetWithMinSeverity() called when the holder was "
                "empty.");

            return traps_[traps_.size() - 1].Severity();
        }

        const TrapVec_t
            Holder::GetWithSeverityRatioBetween(const float THE_MIN, const float THE_MAX)
        {
            M_ASSERT_OR_LOGANDTHROW_SS(
                (traps_.empty() == false),
                "combat::trap::Holder::GetWithSeverityRatio() called when the holder was "
                "empty.");

            auto const SEVERITY_MAX_F{ static_cast<float>(GetMaxSeverity()) };

            TrapVec_t trapsWithSeverityWithinRange;

            for (auto const & TRAP : traps_)
            {
                auto const SEVERITY_RATIO{ static_cast<float>(TRAP.Severity()) / SEVERITY_MAX_F };

                if (((SEVERITY_RATIO > THE_MIN) && (SEVERITY_RATIO < THE_MAX))
                    || misc::IsRealClose(SEVERITY_RATIO, THE_MIN)
                    || misc::IsRealClose(SEVERITY_RATIO, THE_MAX))
                {
                    trapsWithSeverityWithinRange.emplace_back(TRAP);
                }
            }

            if (trapsWithSeverityWithinRange.empty())
            {
                M_HP_LOG_ERR(
                    "combat::trap::Holder::GetWithSeverityRatioBetween(min="
                    << THE_MIN << ", max=" << THE_MAX << ") "
                    << "resulted in no traps, so the default weakest trap is being used.");

                trapsWithSeverityWithinRange.emplace_back(traps_[0]);
            }

            return trapsWithSeverityWithinRange;
        }

        const Trap Holder::SelectRandomWithSeverityRatioNear(const float SEVERITY_RATIO)
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

    } // namespace trap
} // namespace combat
} // namespace heroespath
