#ifndef GAME_STATS_STATMULTSET_HPP_INCLUDED
#define GAME_STATS_STATMULTSET_HPP_INCLUDED
//
// stat-mult-set.hpp
//
#include "game/stats/stat-enum.hpp"

#include <string>


namespace game
{
namespace stats
{

    //Stores float versions of stats.
    class StatMultSet
    {
    public:
        StatMultSet(const float STR = 1.0f,
                    const float ACC = 1.0f,
                    const float CHA = 1.0f,
                    const float LCK = 1.0f,
                    const float SPD = 1.0f,
                    const float INT = 1.0f);

        inline float Str() const { return str_; }
        inline float Acc() const { return acc_; }
        inline float Cha() const { return cha_; }
        inline float Lck() const { return lck_; }
        inline float Spd() const { return spd_; }
        inline float Int() const { return int_; }

        float Get(const stat::Enum) const;

        void Invert();

        const std::string ToString(const bool WILL_WRAP = false) const;

        friend bool operator==(const StatMultSet & L, const StatMultSet & R);
        friend bool operator<(const StatMultSet & L, const StatMultSet & R);

    private:
        float str_;
        float acc_;
        float cha_;
        float lck_;
        float spd_;
        float int_;
    };


    bool operator==(const StatMultSet & L, const StatMultSet & R);


    inline bool operator!=(const StatMultSet & L, const StatMultSet & R)
    {
        return ! (L == R);
    }


    bool operator<(const StatMultSet & L, const StatMultSet & R);

}
}

#endif //GAME_STATS_STATMULTSET_HPP_INCLUDED
