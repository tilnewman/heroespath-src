// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
//  random.cpp
//
#include "misc/nameof.hpp"
#include "random.hpp"

#include <array>
#include <cstdlib>

namespace heroespath
{
namespace misc
{
    namespace helpers
    {

        MersenneTwister19937::Engine_t MersenneTwister19937::engine_;

        const std::string MersenneTwister19937::SeedPoorQualityButLoggable()
        {
            std::random_device randomDevice;
            const auto SEED { randomDevice() };
            std::seed_seq seedSequence { SEED };
            engine_.seed(seedSequence);

            return "poor quality seed of size "
                + std::to_string(sizeof(std::random_device::result_type) * engine_.state_size)
                + "bytes, but it's loggable: " + std::to_string(SEED);
        }

        const std::string MersenneTwister19937::SeedHighQualityButNotLoggable()
        {
            std::random_device randomDevice;
            std::array<std::random_device::result_type, engine_.state_size> initialSeed;
            std::generate_n(initialSeed.data(), initialSeed.size(), std::ref(randomDevice));
            std::seed_seq seedSequence(initialSeed.begin(), initialSeed.end());
            engine_.seed(seedSequence);

            return "high quality seed of size "
                + std::to_string(sizeof(std::random_device::result_type) * engine_.state_size)
                + "bytes, but it's NOT loggable";
        }

        void MersenneTwister19937::Setup()
        {
            // always seed the old one just in case something somewhere uses it
            srand(static_cast<unsigned>(time(nullptr)));

            const auto SEED_DESCRIPTION_STR = SeedPoorQualityButLoggable();

            engine_.discard(WARM_UP_COUNT_);

            M_HP_LOG(
                "Random Number Generator is "
                << NAMEOF_TYPE_T(Engine_t) << ", " << SEED_DESCRIPTION_STR
                << ", warm_up_count=" << std::to_string(WARM_UP_COUNT_) << ", first_random_number<"
                << NAMEOF_TYPE_T(Engine_t::result_type) << ">=" << engine_());
        }

    } // namespace helpers

} // namespace misc
} // namespace heroespath
