//
// enchantment.cpp
//
#include "enchantment.hpp"
#include <tuple>


namespace heroespath
{
namespace item
{

    Enchantment::Enchantment(const std::string &               NAME,
                             const std::string &               DESC,
                             const creature::ConditionSVec_t & CONDITIONS_SVEC)
    :
        name_(NAME),
        desc_(DESC),
        conditionsSVec_(CONDITIONS_SVEC)
    {}


    Enchantment::~Enchantment()
    {}


    bool operator==(const Enchantment & L, const Enchantment & R)
    {
        return std::tie(L.name_, L.desc_, L.conditionsSVec_)
               ==
               std::tie(R.name_, R.desc_, R.conditionsSVec_);
    }

}
}
