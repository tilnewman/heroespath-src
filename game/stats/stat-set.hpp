#ifndef GAME_STATSET_INCLUDED
#define GAME_STATSET_INCLUDED
//
// stat-set.hpp
//  The collection for of stats for all creatures
//
#include "utilz/boost-serialize-includes.hpp"

#include "game/stats/stat.hpp"
#include "game/stats/stat-mult-set.hpp"

#include <tuple>
#include <string>


namespace game
{
namespace stats
{

    //a collection of stats
    class StatSet
    {
    public:
        //construct with all zeros for both normal_ and current_
        StatSet();

        //construct with initial values only
        StatSet(const Stat_t STR,
                const Stat_t ACC,
                const Stat_t CHA,
                const Stat_t LCK,
                const Stat_t SPD,
                const Stat_t INT);

        //specific constructor
        StatSet(const Stat & STR,
                const Stat & ACC,
                const Stat & CHA,
                const Stat & LCK,
                const Stat & SPD,
                const Stat & INT);

        virtual ~StatSet();

        void ModifyNormal(const StatSet & SET);
        void ModifyCurrent(const StatSet & SET);

        const std::string ToStringNormal(const bool WILL_WRAP         = true,
                                         const bool WILL_ABBR         = true,
                                         const bool WILL_SKIP_ZEROS   = false,
                                         const bool WILL_ADD_PLUS     = false,
                                         const bool WILL_SKIP_INVALID = false) const;

        const std::string ToStringCurrent(const bool WILL_WRAP         = true,
                                          const bool WILL_ABBR         = true,
                                          const bool WILL_SKIP_ZEROS   = false,
                                          const bool WILL_ADD_PLUS     = false,
                                          const bool WILL_SKIP_INVALID = false) const;

        const std::string ToStringTesting(const bool WILL_SHOW_ACTUAL = false) const;

        void Invert();
        const StatSet CreateInvertCopy() const;

        //returns true if all stats are >= SET's stats
        bool ChallengeNormal(const StatSet & SET) const;
        bool ChallengeCurrent(const StatSet & SET) const;

        Stat & Get(const stat::Enum);

        const Stat GetCopy(const stat::Enum) const;

        inline const Stat Str() const { return statVec_[stat::Strength]; }
        inline const Stat Acc() const { return statVec_[stat::Accuracy]; }
        inline const Stat Cha() const { return statVec_[stat::Charm]; }
        inline const Stat Lck() const { return statVec_[stat::Luck]; }
        inline const Stat Spd() const { return statVec_[stat::Speed]; }
        inline const Stat Int() const { return statVec_[stat::Intelligence]; }

        //no bounds checking or actual value adjustment is performed by these functions
        void ResetCurrentToNormal();
        void ResetCurrentAndActualToNormal();

        //these functions are meant to only be used in testing
        //these functions use the NEW_..._SET current values to reset values to
        //no bounds checking or actual value adjustment is performed by these functions
        void ResetCurrent(const StatSet & NEW_CURRENT_SET);
        void ResetNormal(const StatSet & NEW_NORMAL_SET);
        void ResetActual(const StatSet & NEW_ACTUAL_SET);

        //operates on Current NOT Actual
        void ModifyCurrentAndActual(const StatMultSet &);

        //actual values are adjusted by this function
        void ModifyCurrentToNormal();

        //Returns an inverse-modifier StatSet that can later be used with ModifyCurrent() to undo the changes made with this function call.
        const StatSet ModifyCurrentToValid(const StatSet &);

        //returns true if any values were changed
        bool ForceValidNormal();
        bool ForceValidCurrent();
        bool ForceValidAll();

        friend bool operator==(const StatSet & L, const StatSet & R);
        friend bool operator<(const StatSet & L, const StatSet & R);

    public:
        static const StatSet INVALID_STATSET_;

    private:
        StatVec_t statVec_;

    private:
        friend class boost::serialization::access;
        template<typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & statVec_;
        }
    };


    inline bool operator==(const StatSet & L, const StatSet & R)
    {
        return (L.statVec_ == R.statVec_);
    }


    inline bool operator!=(const StatSet & L, const StatSet & R)
    {
        return ! (L == R);
    }


    inline bool operator<(const StatSet & L, const StatSet & R)
    {
        return (L.statVec_ < R.statVec_);
    }

}
}
#endif //GAME_ISTAT_INCLUDED
