//
// stat.cpp
//
#include "stat.hpp"

#include <iomanip>
#include <sstream>


namespace game
{
namespace stats
{

    const Stat_t Stat::VAL_MIN_           {  0 };
    const Stat_t Stat::VAL_MAX_INITIAL_   { 20 };
    const Stat_t Stat::VAL_INVALID_       { -1 };
    const float  Stat::REDUCE_DIVISOR_    { 5.0f };
    const float  Stat::REDUCE_MULTIPLIER_ { 2.0f };


    Stat::Stat(const stat::Enum WHICH, const Stat_t VALUE)
    :
        normal_ (VALUE),
        current_(VALUE),
        actual_ (VALUE),
        which_  (WHICH)
    {}


    Stat::~Stat()
    {}


    bool Stat::ModifyNormal(const Stat_t MODIFIER)
    {
        normal_ += MODIFIER;
        if (normal_ < 0)
        {
            normal_ = 0;
            return true;
        }
        else
        {
            return false;
        }
    }


    bool Stat::ModifyCurrent(const Stat_t MODIFIER)
    {
        actual_ += MODIFIER;
        current_ += MODIFIER;

        if (actual_ >= 0)
        {
            current_ = actual_;
        }

        if (current_ < 0)
        {
            current_ = 0;
            return true;
        }
        else
        {
            return false;
        }
    }


    const std::string Stat::ToStringShort(const bool WILL_SHOW_ACTUAL) const
    {
        std::ostringstream ss;
        ss << stat::Abbr(which_) << "=" << current_;
        if (WILL_SHOW_ACTUAL && (IsActual() == false))
        {
            ss << "{" << actual_ << "}";
        }
        return ss.str();
    }


    const std::string Stat::ToStringLong(const bool WILL_SHOW_ACTUAL) const
    {
        std::ostringstream ss;
        ss << ToStringShort(WILL_SHOW_ACTUAL);

        if (IsNormal() == false)
            ss << " of " << normal_;

        return ss.str();
    }


    const std::string Stat::ToStringTesting(const bool WILL_SHOW_ACTUAL) const
    {
        std::ostringstream ss;
        ss << "(" << std::setw(2) << current_ << ")[" << std::setw(2) << normal_ << "]";

        if (WILL_SHOW_ACTUAL && (IsActual() == false))
        {
            ss << "{" << std::setw(2) << actual_ << "}";
        }
        return ss.str();
    }


    const std::string Stat::ModifiedCurrentStr(const bool WILL_WRAP) const
    {
        if (current_ == normal_)
        {
            return "";
        }
        else
        {
            std::ostringstream ss;

            if (WILL_WRAP)
                ss << "(";

            if (current_ > normal_)
            {
                ss << "+";
            }

            ss << current_ - normal_;

            if (WILL_WRAP)
                ss << ")";

            return ss.str();
        }
    }


    const std::string Stat::ModifiedActualStr(const bool WILL_WRAP) const
    {
        if (actual_ == normal_)
        {
            return "";
        }
        else
        {
            std::ostringstream ss;

            if (WILL_WRAP)
                ss << "(";

            if (actual_ > normal_)
            {
                ss << "+";
            }

            ss << actual_ - normal_;

            if (WILL_WRAP)
                ss << ")";

            return ss.str();
        }
    }


    void Stat::Invert()
    {
        actual_ *= -1;
        current_ *= -1;
        normal_ *= -1;
    }


    Stat_t Stat::Reduce(const Stat_t VAL)
    {
        return static_cast<Stat_t>((static_cast<float>(VAL) * REDUCE_MULTIPLIER_) / REDUCE_DIVISOR_);
    }

}
}
