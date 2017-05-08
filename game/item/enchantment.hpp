#ifndef GAME_ITEM_ENCHANTMENT_INCLUDED
#define GAME_ITEM_ENCHANTMENT_INCLUDED
//
// enchantment.hpp
//  Code that encapsulates a magical enchantment on an item.
//
#include "utilz/boost-serialize-includes.hpp"

#include "game/item/i-enchantment.hpp"

#include <string>


namespace game
{
namespace item
{

    class Item;
    class Creature;


    //A base class for all Enchantments
    //Enchantments can effect the item and the creature holding or equiping them.
    //Enchantments effect the creature holding or equiping them through conditions only.
    class Enchantment : public IEnchantment
    {
    public:
        Enchantment(const std::string &                  NAME           = "no_name_error",
                    const std::string &                  DESC           = "no-desc_error",
                    const creature::ConditionEnumVec_t & CONDITIONS_VEC = creature::ConditionEnumVec_t());

        virtual ~Enchantment();

        inline virtual const std::string Name() const { return name_; }
        inline virtual const std::string Desc() const { return desc_; }

        virtual void ChangeItem(Item * const itemPtr) = 0;

        inline virtual const creature::ConditionEnumVec_t Conditions() const { return conditionsVec_; }

        friend bool operator==(const Enchantment & L, const Enchantment & R);

    protected:
        std::string name_;
        std::string desc_;
        creature::ConditionEnumVec_t conditionsVec_;

    private:
        friend class boost::serialization::access;
        template<typename Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & boost::serialization::base_object<IEnchantment>(*this);
            ar & name_;
            ar & desc_;
            ar & conditionsVec_;
        }
    };


    bool operator==(const Enchantment & L, const Enchantment & R);

    bool operator!=(const Enchantment & L, const Enchantment & R);

}
}
#endif //GAME_ITEM_ENCHANTMENT_INCLUDED
