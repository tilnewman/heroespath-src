#ifndef GAME_CREATURE_TITLEWAREHOUSE_HPP_INCLUDED
#define GAME_CREATURE_TITLEWAREHOUSE_HPP_INCLUDED
//
// title-warehouse.hpp
//
#include "game/creature/title-enum.hpp"

#include <memory>
#include <vector>


namespace game
{
namespace creature
{
    //forward declarations
    class Title;
    using TitlePtr_t = Title *;
    using TitleSPtr_t = std::shared_ptr<Title>;
    using TitleSVec_t = std::vector<TitleSPtr_t>;

namespace title
{

    class Warehouse
    {
    public:
        static void Setup();
        static bool Test();
        static TitlePtr_t Get(const Titles::Enum);

    private:
        static TitleSVec_t titleSVec_;
    };

}
}
}

#endif //GAME_CREATURE_TITLEWAREHOUSE_HPP_INCLUDED
