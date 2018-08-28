// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_EVEN_SPACER_HPP_INCLUDE
#define HEROESPATH_SFMLUTIL_EVEN_SPACER_HPP_INCLUDE
//
// even-spacer.hpp
//
#include <vector>

namespace heroespath
{
namespace sfml_util
{

    // Responsible for calculating positions that will evenly space a certain number of things
    // across some distance.
    class EvenSpacer
    {
    public:
        EvenSpacer(
            const std::size_t COUNT,
            const float TOTAL_DISTANCE,
            const float EXTRA_SPREAD_RATIO = 0.0f)
            : positions_()
            , extraSpreadRatio_(EXTRA_SPREAD_RATIO)
            , distanceTotal_(TOTAL_DISTANCE)
            , distanceBetween_(TOTAL_DISTANCE / static_cast<float>(COUNT + 1))
            , distanceExtraSpread_(EXTRA_SPREAD_RATIO * distanceBetween_)
        {
            for (std::size_t i(0); i < COUNT; ++i)
            {
                const auto I_FLOAT { static_cast<float>(i) };

                positions_.emplace_back(
                    (distanceBetween_ * (I_FLOAT + 1.0f))
                    + (((static_cast<float>(COUNT) * -0.5f) - I_FLOAT) * distanceExtraSpread_));
            }
        }

        EvenSpacer(const EvenSpacer &) = default;
        EvenSpacer(EvenSpacer &&) = default;
        EvenSpacer & operator=(const EvenSpacer &) = default;
        EvenSpacer & operator=(EvenSpacer &&) = default;

        float Position(const std::size_t INDEX, const float WIDTH = 0.0f) const
        {
            return (positions_.at(INDEX) - (WIDTH * 0.5f));
        }

        const std::vector<float> Positions() const { return positions_; }
        std::size_t Count() const { return positions_.size(); }
        float DistanceTotal() const { return distanceTotal_; }
        float ExtraSpreadRatio() const { return extraSpreadRatio_; }
        float DistanceExtraSpread() const { return distanceExtraSpread_; }
        float DistanceBetween() const { return (distanceBetween_ + distanceExtraSpread_); }

    private:
        std::vector<float> positions_;
        float extraSpreadRatio_;
        float distanceTotal_;
        float distanceBetween_;
        float distanceExtraSpread_;
    };

} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_EVEN_SPACER_HPP_INCLUDE
