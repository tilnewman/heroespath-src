//
// i-enchantment.cpp
//
#include "i-enchantment.hpp"


namespace game
{
namespace item
{

    bool operator==(const IEnchantment & L, const IEnchantment & R)
    {
        return ((L.Name() == R.Name()) &&
                (L.Desc() == R.Desc()) &&
                (L.Conditions() == R.Conditions()));
    }


    bool operator!=(const IEnchantment & L, const IEnchantment & R)
    {
        return ! (L == R);
    }

}
}
