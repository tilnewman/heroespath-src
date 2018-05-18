// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_CREATURE_TRAIT_HPP_INCLUDED
#define HEROESPATH_CREATURE_TRAIT_HPP_INCLUDED
//
// trait.hpp
//
//  Maintains a current and normal value.  Sometimes the numbers are interpreted as
//  percent bonus values, so ...Ratio() functions are provided.  Initially (and typically)
//  the current_ value stays at normal_, until something temorary changes it to be
//  something other than normal.  Eventually, the temporary condition will expire
//  and the current will return to 'normal'.
//
#include "creature/trait-enum.hpp"
#include "creature/trait.hpp"

#include "misc/boost-serialize-includes.hpp"
#include "misc/real.hpp"

#include <string>
#include <tuple>
#include <vector>

namespace heroespath
{
namespace creature
{

    using Trait_t = int;

    // Maintains a creature variable or 'trait' as a current and normal value.
    // This class is also used to store trait percent bonuses.
    class Trait
    {
    public:
        explicit Trait(const Traits::Enum TYPE = Traits::Count, const Trait_t NORMAL = 0);

        Traits::Enum Which() const { return type_; }
        const std::string Name() const { return Traits::Name(type_); }
        const std::string Desc() const { return Traits::Desc(type_); }
        Trait_t Normal() const { return normal_; }
        Trait_t Current() const { return current_; }

        float NormalRatio() const { return static_cast<float>(current_) / 100.0f; }
        float CurrentRatio() const { return static_cast<float>(normal_) / 100.0f; }

        bool ResetCurrentToNormal() const { return (current_ == normal_); }

        Trait_t NormalAdj(const Trait_t X) { return normal_ += X; }
        Trait_t CurrentAdj(const Trait_t X) { return current_ += X; }
        void CurrAndNormAdj(const Trait_t X)
        {
            NormalAdj(X);
            CurrentAdj(X);
        }

        void NormalSet(const Trait_t X) { normal_ = X; }
        void CurrentSet(const Trait_t X) { current_ = X; }
        void CurrAndNormSet(const Trait_t X)
        {
            NormalSet(X);
            CurrentSet(X);
        }

        void SetCurrentToNormal() { current_ = normal_; }

        // the given Trait's current value is used to modify
        void NormalAdj(const Trait & RS) { normal_ += RS.current_; }
        void CurrentAdj(const Trait & RS) { current_ += RS.current_; }

        const std::string ToString(const bool WILL_PREPEND_PLUS = true) const;

        friend bool operator==(const Trait & L, const Trait & R);
        friend bool operator<(const Trait & L, const Trait & R);

    public:
        static const Trait_t STAT_MAX_INITIAL_ = 20;
        static const Trait_t STAT_MAX_ESTIMATED_ = 100;

    private:
        Traits::Enum type_;
        Trait_t normal_;
        Trait_t current_;

    private:
        friend class boost::serialization::access;
        template <typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & type_;
            ar & normal_;
            ar & current_;
        }
    };

    using TraitVec_t = std::vector<Trait>;

    inline bool operator==(const Trait & L, const Trait & R)
    {
        return std::tie(L.type_, L.normal_, L.current_) == std::tie(R.type_, R.normal_, R.current_);
    }

    inline bool operator!=(const Trait & L, const Trait & R) { return !(L == R); }

    inline bool operator<(const Trait & L, const Trait & R)
    {
        return std::tie(L.type_, L.normal_, L.current_) < std::tie(R.type_, R.normal_, R.current_);
    }

} // namespace creature
} // namespace heroespath

#endif // HEROESPATH_CREATURE_TRAIT_HPP_INCLUDED
