#ifndef GAME_ITEM_IENCHANTMENT_INCLUDED
#define GAME_ITEM_IENCHANTMENT_INCLUDED
//
// i-enchantment.hpp
//  Pure virtual interface for all Enchantment types.
//
#include "utilz/boost-serialize-includes.hpp"
#include "game/creature/condition-enum.hpp"

#include <memory>
#include <string>
#include <vector>


namespace game
{
namespace item
{

    class Item;
    class Creature;


    //pure virtual interface for all Enchantments
    class IEnchantment
    {
    public:
        virtual ~IEnchantment() {}
        virtual const std::string Name() const = 0;
        virtual const std::string Desc() const = 0;
        virtual void ChangeItem(Item * const itemPtr) = 0;
        virtual const creature::ConditionEnumVec_t Conditions() const = 0;

    private:
        friend class boost::serialization::access;
        template<typename Archive>
        void serialize(Archive &, const unsigned int) {}
    };

    using IEnchantmentSPtr_t = std::shared_ptr<IEnchantment>;
    using IEnchantmentSVec_t = std::vector<IEnchantmentSPtr_t>;


    bool operator==(const IEnchantment & L, const IEnchantment & R);

    bool operator!=(const IEnchantment & L, const IEnchantment & R);

}
}
#endif //GAME_ITEM_IENCHANTMENT_INCLUDED
