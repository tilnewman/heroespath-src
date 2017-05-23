//
// stat-set.cpp
//
#include "stat-set.hpp"

#include "game/log-macros.hpp"

#include <sstream>
#include <string>
#include <exception>
#include <iomanip>


namespace game
{
namespace stats
{

    const StatSet StatSet::INVALID_STATSET_{ Stat(stat::Strength, Stat::VAL_INVALID_),
                                             Stat(stat::Accuracy, Stat::VAL_INVALID_),
                                             Stat(stat::Charm, Stat::VAL_INVALID_),
                                             Stat(stat::Luck, Stat::VAL_INVALID_),
                                             Stat(stat::Speed, Stat::VAL_INVALID_),
                                             Stat(stat::Intelligence, Stat::VAL_INVALID_) };


    StatSet::StatSet()
    :
        statVec_()
    {
        statVec_.resize(stat::Count);
        statVec_[stat::Strength]        = Stat(stat::Strength, Stat::VAL_MIN_);
        statVec_[stat::Accuracy]        = Stat(stat::Accuracy, Stat::VAL_MIN_);
        statVec_[stat::Charm]           = Stat(stat::Charm, Stat::VAL_MIN_);
        statVec_[stat::Luck]            = Stat(stat::Luck, Stat::VAL_MIN_);
        statVec_[stat::Speed]           = Stat(stat::Speed, Stat::VAL_MIN_);
        statVec_[stat::Intelligence]    = Stat(stat::Intelligence, Stat::VAL_MIN_);
    }


    StatSet::StatSet(const Stat_t STR_VAL,
                     const Stat_t ACC_VAL,
                     const Stat_t CHA_VAL,
                     const Stat_t LCK_VAL,
                     const Stat_t SPD_VAL,
                     const Stat_t INT_VAL)
    :
        statVec_()
    {
        statVec_.resize(stat::Count);
        statVec_[stat::Strength]        = Stat(stat::Strength, STR_VAL);
        statVec_[stat::Accuracy]        = Stat(stat::Accuracy, ACC_VAL);
        statVec_[stat::Charm]           = Stat(stat::Charm, CHA_VAL);
        statVec_[stat::Luck]            = Stat(stat::Luck, LCK_VAL);
        statVec_[stat::Speed]           = Stat(stat::Speed, SPD_VAL);
        statVec_[stat::Intelligence]    = Stat(stat::Intelligence, INT_VAL);
    }


    StatSet::StatSet(const Stat & STR,
                     const Stat & ACC,
                     const Stat & CHA,
                     const Stat & LCK,
                     const Stat & SPD,
                     const Stat & INT)
    :
        statVec_()
    {
        statVec_.resize(stat::Count);
        statVec_[stat::Strength]        = STR;
        statVec_[stat::Accuracy]        = ACC;
        statVec_[stat::Charm]           = CHA;
        statVec_[stat::Luck]            = LCK;
        statVec_[stat::Speed]           = SPD;
        statVec_[stat::Intelligence]    = INT;
    }


    StatSet::~StatSet()
    {}


    void StatSet::ModifyNormal(const StatSet & SET)
    {
        for (auto & nextStat : statVec_)
            nextStat.ModifyNormal(SET.GetCopy(nextStat.Which()));
    }


    void StatSet::ModifyCurrent(const StatSet & SET)
    {
        for (auto & nextStat : statVec_)
            nextStat.ModifyCurrent(SET.GetCopy(nextStat.Which()));
    }


    const std::string StatSet::ToStringNormal(const bool WILL_WRAP,
                                              const bool WILL_ABBR,
                                              const bool WILL_SKIP_ZEROS,
                                              const bool WILL_ADD_PLUS,
                                              const bool WILL_SKIP_INVALID) const
    {
        std::ostringstream ss;

        for (int i(0); i < stat::Count; ++i)
        {
            auto const NEXT_STAT{ statVec_[static_cast<std::size_t>(i)] };

            if (((WILL_SKIP_ZEROS == false) || (WILL_SKIP_ZEROS && (NEXT_STAT.Normal() != 0))) && ((WILL_SKIP_INVALID == false) || (WILL_SKIP_INVALID && (NEXT_STAT.Normal() != Stat::VAL_INVALID_))))
            {
                if (0 == i)
                {
                    if (WILL_ABBR)
                        ss << NEXT_STAT.Abbr() << (((NEXT_STAT.Normal() > 0) && WILL_ADD_PLUS) ? "=+" : "=") << NEXT_STAT.Normal();
                    else
                        ss << NEXT_STAT.Name() << (((NEXT_STAT.Normal() > 0) && WILL_ADD_PLUS) ? "=+" : "=") << NEXT_STAT.Normal();
                }
                else
                {
                    if (WILL_ABBR)
                        ss << ((ss.str().empty()) ? "" : ", ") << NEXT_STAT.Abbr() << (((NEXT_STAT.Normal() > 0) && WILL_ADD_PLUS) ? "=+" : "=") << NEXT_STAT.Normal();
                    else
                        ss << ((ss.str().empty()) ? "" : ", ") << NEXT_STAT.Name() << (((NEXT_STAT.Normal() > 0) && WILL_ADD_PLUS) ? "=+" : "=") << NEXT_STAT.Normal();
                }
            }
        }

        //don't wrap if empty
        std::ostringstream finalSS;
        if (WILL_WRAP)
        {
            if (ss.str().empty() == false)
                finalSS << "(" << ss.str() << ")";
        }
        else
            finalSS << ss.str();

        return finalSS.str();
    }


    const std::string StatSet::ToStringCurrent(const bool WILL_WRAP,
                                               const bool WILL_ABBR,
                                               const bool WILL_SKIP_ZEROS,
                                               const bool WILL_ADD_PLUS,
                                               const bool WILL_SKIP_INVALID) const
    {
        std::ostringstream ss;

        for (int i(0); i < stat::Count; ++i)
        {
            auto const NEXT_STAT{ statVec_[static_cast<std::size_t>(i)] };

            if (((WILL_SKIP_ZEROS == false) || (WILL_SKIP_ZEROS && (NEXT_STAT.Current() != 0))) && ((WILL_SKIP_INVALID == false) || (WILL_SKIP_INVALID && (NEXT_STAT.Current() != Stat::VAL_INVALID_))))
            {
                if (0 == i)
                {
                    if (WILL_ABBR)
                        ss << NEXT_STAT.Abbr() << (((NEXT_STAT.Current() > 0) && WILL_ADD_PLUS) ? "=+" : "=") << NEXT_STAT.Current();
                    else
                        ss << NEXT_STAT.Name() << (((NEXT_STAT.Current() > 0) && WILL_ADD_PLUS) ? "=+" : "=") << NEXT_STAT.Current();
                }
                else
                {
                    if (WILL_ABBR)
                        ss << ((ss.str().empty()) ? "" : ", ") << NEXT_STAT.Abbr() << (((NEXT_STAT.Current() > 0) && WILL_ADD_PLUS) ? "=+" : "=") << NEXT_STAT.Current();
                    else
                        ss << ((ss.str().empty()) ? "" : ", ") << NEXT_STAT.Name() << (((NEXT_STAT.Current() > 0) && WILL_ADD_PLUS) ? "=+" : "=") << NEXT_STAT.Current();
                }
            }
        }

        //don't wrap if empty
        std::ostringstream finalSS;
        if (WILL_WRAP)
        {
            if (ss.str().empty() == false)
                finalSS << "(" << ss.str() << ")";
        }
        else
            finalSS << ss.str();

        return finalSS.str();
    }


    const std::string StatSet::ToStringTesting(const bool WILL_SHOW_ACTUAL) const
    {
        std::ostringstream ss;
        for (auto const & NEXT_STAT : statVec_)
            ss << "\n" << NEXT_STAT.ToStringTesting(WILL_SHOW_ACTUAL);

        return ss.str();
    }


    void StatSet::Invert()
    {
        for (auto & nextStat : statVec_)
            nextStat.Invert();
    }


    const StatSet StatSet::CreateInvertCopy() const
    {
        StatSet statSetCopy( * this);
        statSetCopy.Invert();
        return statSetCopy;
    }


    bool StatSet::ChallengeCurrent(const StatSet & SET) const
    {
        return ((Str().Current() >= SET.Str().Current()) &&
                (Acc().Current() >= SET.Acc().Current()) &&
                (Cha().Current() >= SET.Cha().Current()) &&
                (Lck().Current() >= SET.Lck().Current()) &&
                (Spd().Current() >= SET.Spd().Current()) &&
                (Int().Current() >= SET.Int().Current()));
    }


    bool StatSet::ChallengeNormal(const StatSet & SET) const
    {
        return ((Str().Normal() >= SET.Str().Normal()) &&
                (Acc().Normal() >= SET.Acc().Normal()) &&
                (Cha().Normal() >= SET.Cha().Normal()) &&
                (Lck().Normal() >= SET.Lck().Normal()) &&
                (Spd().Normal() >= SET.Spd().Normal()) &&
                (Int().Normal() >= SET.Int().Normal()));
    }


    Stat & StatSet::Get(const stat::Enum E)
    {
        switch (E)
        {
            case stat::Strength:
            case stat::Accuracy:
            case stat::Charm:
            case stat::Luck:
            case stat::Speed:
            case stat::Intelligence: { return statVec_[E]; }
            case stat::Count:
            default:
            {
                std::ostringstream ss;
                ss << "game::stats::StatSet::Get(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }


    const Stat StatSet::GetCopy(const stat::Enum E) const
    {
        switch (E)
        {
            case stat::Strength:
            case stat::Accuracy:
            case stat::Charm:
            case stat::Luck:
            case stat::Speed:
            case stat::Intelligence: { return statVec_[E]; }
            case stat::Count:
            default:
            {
                std::ostringstream ss;
                ss << "game::stats::StatSet::GetCopy(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }


    void StatSet::ResetCurrentToNormal()
    {
        for (auto & nextStat : statVec_)
            nextStat.ResetCurrentToNormal();
    }


    void StatSet::ResetCurrentAndActualToNormal()
    {
        for (auto & nextStat : statVec_)
            nextStat.ResetCurrentAndActualToNormal();
    }


    void StatSet::ResetCurrent(const StatSet & NEW_CURRENT_SET)
    {
        for (auto & nextStat : statVec_)
            nextStat.ResetCurrent(NEW_CURRENT_SET.GetCopy(nextStat.Which()).Current());
    }


    void StatSet::ResetNormal(const StatSet & NEW_NORMAL_SET)
    {
        for (auto & nextStat : statVec_)
            nextStat.ResetNormal(NEW_NORMAL_SET.GetCopy(nextStat.Which()).Current());
    }


    void StatSet::ResetActual(const StatSet & NEW_ACTUAL_SET)
    {
        for (auto & nextStat : statVec_)
            nextStat.ResetActual(NEW_ACTUAL_SET.GetCopy(nextStat.Which()).Current());
    }


    void StatSet::ModifyCurrentAndActual(const StatMultSet & STAT_MULT_SET)
    {
        for (auto & nextStat : statVec_)
            nextStat.ResetCurrentAndActual( static_cast<stats::Stat_t>(static_cast<float>(nextStat.Current()) * STAT_MULT_SET.Get(nextStat.Which())) );
    }


    void StatSet::ModifyCurrentToNormal()
    {
        for (auto & nextStat : statVec_)
        {
            auto const NEXT_NORMAL_VALUE{ Get(nextStat.Which()).Normal() };
            auto const NEXT_CURRENT_VALUE{ Get(nextStat.Which()).Current() };
            nextStat.ModifyCurrent(NEXT_NORMAL_VALUE - NEXT_CURRENT_VALUE);
        }
    }


    const StatSet StatSet::ModifyCurrentToValid(const StatSet & SET)
    {
        StatSet inverseSet;

        for (auto & nextStat : statVec_)
        {
            auto const NEXT_TARGET_VALUE{ SET.GetCopy(nextStat.Which()).Current() };
            if (NEXT_TARGET_VALUE != Stat::VAL_INVALID_)
            {
                auto const NEXT_CURRENT_VALUE{ nextStat.Current() };
                nextStat.ModifyCurrent(NEXT_TARGET_VALUE - NEXT_CURRENT_VALUE);
                inverseSet.Get(nextStat.Which()).ResetAll(NEXT_CURRENT_VALUE - NEXT_TARGET_VALUE);
            }
            else
            {
                inverseSet.Get(nextStat.Which()).ResetAll(0);
            }
        }

        return inverseSet;
    }


    bool StatSet::ForceValidNormal()
    {
        auto anyValuesChanged{ false };
        for (auto & nextStat : statVec_)
            if (nextStat.ForceValidNormal())
                anyValuesChanged = true;

        return anyValuesChanged;
    }


    bool StatSet::ForceValidCurrent()
    {
        auto anyValuesChanged{ false };
        for (auto & nextStat : statVec_)
            if (nextStat.ForceValidCurrent())
                anyValuesChanged = true;

        return anyValuesChanged;
    }


    bool StatSet::ForceValidAll()
    {
        auto anyValuesChanged{ false };
        for (auto & nextStat : statVec_)
            if (nextStat.ForceValidAll())
                anyValuesChanged = true;

        return anyValuesChanged;
    }

}
}
