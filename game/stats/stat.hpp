#ifndef GAME_STAT_INCLUDED
#define GAME_STAT_INCLUDED
//
// stat.hpp
//  A base class for all stat classes
//
#include "utilz/boost-serialize-includes.hpp"

#include "game/stats/types.hpp"
#include "game/stats/stat-enum.hpp"

#include <string>
#include <memory>
#include <tuple>
#include <vector>


namespace game
{
namespace stats
{

    //a base class for all stat objects
    class Stat
    {
    public:
        Stat(const stat::Enum WHICH = static_cast<stat::Enum>(0),
             const Stat_t     VALUE = VAL_MIN_);

        virtual ~Stat();

        inline stat::Enum Which() const                             { return which_; }
        inline const std::string Name() const                       { return stat::Name(which_); }
        inline const std::string Desc() const                       { return stat::Desc(which_); }
        inline const std::string Abbr(const bool B = false) const   { return stat::Abbr(which_, B); }

        inline Stat_t Normal() const                                { return normal_; }
        inline Stat_t Current() const                               { return current_; }
        inline Stat_t Actual() const                                { return actual_; }

        inline bool IsActual() const                                { return (current_ == actual_); }
        inline bool IsNormal() const                                { return (current_ == normal_); }
        inline bool IsActualAndNormal() const                       { return (IsActual() && IsNormal()); }

        //returns true if normal_ was prevented from going below zero
        bool ModifyNormal(const Stat_t MODIFIER);
        inline bool ModifyNormal(const Stat MODIFIER)               { return ModifyNormal(MODIFIER.Current()); }

        //returns true if current_ was prevented from going below zero
        bool ModifyCurrent(const Stat_t MODIFIER);
        inline void ModifyCurrent(const Stat MODIFIER)              { ModifyCurrent(MODIFIER.Current()); }

        //no bounds checking is performed by these functions
        inline void ResetActual(const Stat_t S)                     { actual_ = S; }
        inline void ResetCurrent(const Stat_t S)                    { current_ = S; }
        inline void ResetNormal(const Stat_t S)                     { normal_ = S; }
        inline void ResetCurrentAndNormal(const Stat_t S)           { ResetCurrent(S); ResetNormal(S); }
        inline void ResetCurrentAndActual(const Stat_t S)           { ResetCurrent(S); ResetActual(S); }
        inline void ResetAll(const Stat_t S)                        { ResetActual(S); ResetCurrentAndNormal(S); }
        //
        inline void ResetCurrentToNormal()                          { ResetCurrent(normal_); }
        inline void ResetCurrentAndActualToNormal()                 { ResetCurrentAndActual(normal_); }

        const std::string ToStringShort(const bool WILL_SHOW_ACTUAL = false) const;
        const std::string ToStringLong(const bool WILL_SHOW_ACTUAL = false) const;
        const std::string ToStringTesting(const bool WILL_SHOW_ACTUAL = false) const;

        const std::string ModifiedCurrentStr(const bool WILL_WRAP = false) const;
        const std::string ModifiedActualStr(const bool WILL_WRAP = false) const;

        inline bool IsEqualActual(const Stat & RHS) const           { return (actual_ == RHS.actual_); }
        inline bool IsEqualNormal(const Stat & RHS) const           { return (normal_ == RHS.normal_); }
        inline bool IsEqualCurrent(const Stat & RHS) const          { return (current_ == RHS.current_); }
        inline bool IsEqualAllValues(const Stat & RHS) const        { return (IsEqualActual(RHS) && IsEqualNormal(RHS) && IsEqualCurrent(RHS)); }

        void Invert();

        inline bool ForceValidNormal()                              { return ModifyNormal(0); }
        inline bool ForceValidCurrent()                             { return ModifyCurrent(0); }
        inline bool ForceValidAll()                                 { const bool A{ForceValidNormal()}; const bool B{ForceValidCurrent()}; return (A || B); }

        friend bool operator==(const Stat & L, const Stat & R);
        friend bool operator<(const Stat & L, const Stat & R);
        
        static const int X_;

    public:
        static const Stat_t VAL_MIN_;
        static const Stat_t VAL_MAX_INITIAL_;
        static const Stat_t VAL_INVALID_;

    protected:
        Stat_t normal_;
        Stat_t current_;
        Stat_t actual_;
        stat::Enum which_;

    private:
        friend class boost::serialization::access;
        template<typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & normal_;
            ar & current_;
            ar & actual_;
            ar & which_;
        }
    };

    using StatVec_t = std::vector<Stat>;


    inline bool operator==(const Stat & L, const Stat & R)
    {
        return std::tie(L.normal_, L.current_, L.actual_, L.which_)
               ==
               std::tie(R.normal_, R.current_, R.actual_, R.which_);
    }


    inline bool operator!=(const Stat & L, const Stat & R)
    {
        return ! (L == R);
    }


    inline bool operator<(const Stat & L, const Stat & R)
    {
        return std::tie(L.normal_, L.current_, L.actual_, L.which_)
               <
               std::tie(R.normal_, R.current_, R.actual_, R.which_);
    }
}
}
#endif //GAME_STAT_INCLUDED
