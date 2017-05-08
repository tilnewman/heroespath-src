//
// stat-mult-set.cpp
//
#include "stat-mult-set.hpp"

#include <sstream>


namespace game
{
namespace stats
{

    StatMultSet::StatMultSet(const float STR,
                             const float ACC,
                             const float CHA,
                             const float LCK,
                             const float SPD,
                             const float INT)
    :
        str_(STR),
        acc_(ACC),
        cha_(CHA),
        lck_(LCK),
        spd_(SPD),
        int_(INT)
    {}

    
    void StatMultSet::Invert()
    {
        if (utilz::IsRealZero(str_)) str_ = 1.0f; else str_ = 1.0f / str_;
        if (utilz::IsRealZero(acc_)) acc_ = 1.0f; else acc_ = 1.0f / acc_;
        if (utilz::IsRealZero(cha_)) cha_ = 1.0f; else cha_ = 1.0f / cha_;
        if (utilz::IsRealZero(lck_)) lck_ = 1.0f; else lck_ = 1.0f / lck_;
        if (utilz::IsRealZero(spd_)) spd_ = 1.0f; else spd_ = 1.0f / spd_;
        if (utilz::IsRealZero(int_)) int_ = 1.0f; else int_ = 1.0f / int_;
    }


    const std::string StatMultSet::ToString(const bool WILL_WRAP) const
    {
        std::ostringstream ss;

        ss << "Str=" << str_ << ", "
           << "Acc=" << acc_ << ", "
           << "Cha=" << cha_ << ", "
           << "Lck=" << lck_ << ", "
           << "Spd=" << spd_ << ", "
           << "Int=" << int_;

        if (WILL_WRAP)
        {
            return "(" + ss.str() + ")";
        }
        else
        {
            return ss.str();
        }
    }

}
}
