//
// enchantment.cpp
//
#include "enchantment.hpp"
#include <tuple>


namespace game
{
namespace item
{

    Enchantment::Enchantment(const std::string &                  NAME,
                             const std::string &                  DESC,
                             const creature::ConditionEnumVec_t & CONDITIONS_VEC)
    :
        name_         (NAME),
        desc_         (DESC),
        conditionsVec_(CONDITIONS_VEC)
    {}


    Enchantment::~Enchantment()
    {}


    bool operator==(const Enchantment & L, const Enchantment & R)
    {
        return std::tie(L.name_, L.desc_, L.conditionsVec_)
               ==
               std::tie(R.name_, R.desc_, R.conditionsVec_);
    }

}
}
