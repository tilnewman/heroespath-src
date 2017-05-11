#ifndef GAME_ITEM_MISCITEMFACTORY_INCLUDED
#define GAME_ITEM_MISCITEMFACTORY_INCLUDED
//
// misc-item-factory.hpp
//  Responsible for making individual item objects.
//
#include "game/item/item-type-enum.hpp"
#include "game/item/item-factory-base.hpp"

#include <memory>
#include <string>


namespace game
{
namespace item
{
    class Item;
    using ItemSPtr_t = std::shared_ptr<Item>;

namespace misc
{

    namespace instrument_type
    {
        enum Enum
        {
            Lute = 0,
            Sitar,
            Flute,
            Panflute,
            Count
        };
    }


    //types required by singleton implementation
    class ItemFactory;
    using ItemFactorySPtr_t = std::shared_ptr<ItemFactory>;


    //A singleton class responsible for making misc item objects.
    class ItemFactory : public FactoryBase
    {
        //prevent copy construction
        ItemFactory(const ItemFactory &) =delete;

        //prevent copy assignment
        ItemFactory & operator=(const ItemFactory &) =delete;

        //prevent non-singleton construction
        ItemFactory();

    public:
        virtual ~ItemFactory();

        static ItemFactorySPtr_t Instance();

        static ItemSPtr_t Make_Ring(const material::Enum MATERIAL_PRI,
                                    const material::Enum MATERIAL_SEC,
                                    const bool           IS_PIXIE_ITEM = false);

        static ItemSPtr_t Make_Wand(const material::Enum MATERIAL_PRI,
                                    const material::Enum MATERIAL_SEC,
                                    const bool           IS_PIXIE_ITEM = false);

        static ItemSPtr_t Make_Instrument(const instrument_type::Enum TYPE,
                                          const bool                  IS_PIXIE_ITEM = false);

    private:
        static ItemFactorySPtr_t instance_;
    };

}
}
}
#endif //GAME_ITEM_MISCITEMFACTORY_INCLUDED
