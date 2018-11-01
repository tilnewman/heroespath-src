// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// trap-holder.cpp
//
#include "trap-holder.hpp"

#include "misc/assertlogandthrow.hpp"
#include "misc/log-macros.hpp"
#include "misc/vectors.hpp"
#include "sfml-util/sound-effects-enum.hpp"

#include <algorithm>

namespace heroespath
{
namespace combat
{
    namespace trap
    {

        TrapVec_t combat::trap::Holder::traps_ {};

        void Holder::Fill()
        {
            Empty();

            traps_.emplace_back(Trap(
                1,
                1,
                2_health,
                6_health,
                "blasted",
                gui::sound_effect::TrapPop,
                "The lock bursts in the lock picker's hands!"));

            traps_.emplace_back(Trap(
                1,
                1,
                3_health,
                8_health,
                "stabbed",
                gui::sound_effect::TrapMetalBang,
                "A metal spike pops out from the",
                "!"));

            traps_.emplace_back(Trap(
                1,
                3,
                4_health,
                9_health,
                "sliced",
                gui::sound_effect::TrapMetal,
                "Sharp metal pieces burst out and tear through the party!"));

            traps_.emplace_back(Trap(
                1,
                3,
                6_health,
                12_health,
                "poisoned",
                gui::sound_effect::TrapSplutter,
                "The lock oozes with toxic black sludge!"));

            traps_.emplace_back(Trap(
                3,
                6,
                8_health,
                14_health,
                "sickened",
                gui::sound_effect::TrapSplatDunk,
                "Green slime spits out from holes in the ",
                "!"));

            traps_.emplace_back(Trap(
                2,
                4,
                6_health,
                12_health,
                "blasted",
                gui::sound_effect::TrapBang,
                "The lock detonates!"));

            traps_.emplace_back(Trap(
                3,
                6,
                8_health,
                16_health,
                "blasted",
                gui::sound_effect::TrapBoom,
                "The ",
                "  blows apart!"));

            traps_.emplace_back(Trap(
                4,
                6,
                10_health,
                20_health,
                "blasted",
                gui::sound_effect::TrapChungSplutter,
                "The ",
                "  explodes!"));

            traps_.emplace_back(Trap(
                1,
                1,
                8_health,
                14_health,
                "burned",
                gui::sound_effect::TrapFire,
                "The lock becomes scalding and burns the lock picker!"));

            traps_.emplace_back(Trap(
                1,
                3,
                10_health,
                15_health,
                "burned",
                gui::sound_effect::TrapInferno,
                "The whole ",
                "  erupts into a raging inferno!"));

            traps_.emplace_back(Trap(
                4,
                6,
                15_health,
                20_health,
                "burned",
                gui::sound_effect::TrapFireball,
                "A fireball engulfs the party!"));

            traps_.emplace_back(Trap(
                6,
                6,
                8_health,
                14_health,
                "burned",
                gui::sound_effect::TrapChiaowahh,
                "You hear haunting moans and a ghostly yellow light shines from the ",
                "  searing the party!"));

            traps_.emplace_back(Trap(
                6,
                6,
                11_health,
                18_health,
                "burned",
                gui::sound_effect::TrapChiaowahh,
                "You hear cries of pain and a ghostly green light shines from the ",
                "  searing the party!"));

            traps_.emplace_back(Trap(
                6,
                6,
                14_health,
                24_health,
                "burned",
                gui::sound_effect::TrapChiaowahh,
                "You hear frightful screams and a ghostly blue light shines from the ",
                "  searing the party!"));

            traps_.emplace_back(Trap(
                6,
                6,
                17_health,
                28_health,
                "burned",
                gui::sound_effect::TrapChiaowahh,
                "You hear horrible wailing and a ghostly red light shines from the ",
                "  searing the party!"));

            traps_.emplace_back(Trap(
                1,
                4,
                12_health,
                18_health,
                "poisoned",
                gui::sound_effect::TrapGasExhale,
                "Noxious gas hisses from vents in the ",
                "!"));

            traps_.emplace_back(Trap(
                2,
                6,
                16_health,
                24_health,
                "poisoned",
                gui::sound_effect::TrapGasLeak,
                "A deadly chemical mist escapes!"));

            traps_.emplace_back(Trap(
                2,
                4,
                8_health,
                14_health,
                "burned",
                gui::sound_effect::TrapSparksHiss,
                "Sparks spray out from the ",
                "!"));

            traps_.emplace_back(Trap(
                3,
                6,
                12_health,
                24_health,
                "sliced",
                gui::sound_effect::TrapSparksAhh,
                "A torrent of molten metal showers the party!"));

            traps_.emplace_back(Trap(
                1,
                1,
                25_health,
                50_health,
                "sliced",
                gui::sound_effect::TrapSpiritShortHiss,
                "A spectral skeleton hand reaches out and strikes at the lock picker!"));

            traps_.emplace_back(Trap(
                2,
                4,
                30_health,
                50_health,
                "sliced",
                gui::sound_effect::TrapGhost2,
                "The raging ghost of a murdered witch emerges and slashes the party with ghostly "
                "claws!"));

            traps_.emplace_back(Trap(
                4,
                6,
                20_health,
                40_health,
                "sliced",
                gui::sound_effect::TrapGhost2,
                "Evil spirits emerge and tear through the party!"));

            std::sort(std::begin(traps_), std::end(traps_), [](const auto & A, const auto & B) {
                return A.Severity() < B.Severity();
            });
        }

        void Holder::Empty() { traps_.clear(); }

        int Holder::GetMinSeverity()
        {
            M_HP_ASSERT_OR_LOG_AND_THROW(
                (traps_.empty() == false),
                "combat::trap::Holder::GetWithMinSeverity() called when the holder was "
                "empty.");

            return traps_[0].Severity();
        }

        int Holder::GetMaxSeverity()
        {
            M_HP_ASSERT_OR_LOG_AND_THROW(
                (traps_.empty() == false),
                "combat::trap::Holder::GetWithMinSeverity() called when the holder was "
                "empty.");

            return traps_[traps_.size() - 1].Severity();
        }

        const TrapVec_t
            Holder::GetWithSeverityRatioBetween(const float THE_MIN, const float THE_MAX)
        {
            M_HP_ASSERT_OR_LOG_AND_THROW(
                (traps_.empty() == false),
                "combat::trap::Holder::GetWithSeverityRatio() called when the holder was "
                "empty.");

            const auto SEVERITY_MAX_F { static_cast<float>(GetMaxSeverity()) };

            TrapVec_t trapsWithSeverityWithinRange;

            for (const auto & TRAP : traps_)
            {
                const auto SEVERITY_RATIO { static_cast<float>(TRAP.Severity()) / SEVERITY_MAX_F };

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
            auto severityRatioMin { SEVERITY_RATIO - 0.333f };
            if (severityRatioMin < 0.0f)
            {
                severityRatioMin = 0.0f;
            }

            auto severityRatioMax { SEVERITY_RATIO + 0.1f };
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
