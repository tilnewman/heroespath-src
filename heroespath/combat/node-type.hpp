#ifndef HEROSPATH_COMBAT_NODETYPE_HPP_INCLUDE
#define HEROSPATH_COMBAT_NODETYPE_HPP_INCLUDE
//
// node-type.hpp
//
#include <string>


namespace heroespath
{
namespace combat
{

    struct NodeType
    {
        enum Enum
        {
            Position = 0,
            Count,
            All = Count
        };

        static const std::string ToString(const NodeType::Enum);
    };

}
}

#endif //HEROSPATH_COMBAT_NODETYPE_HPP_INCLUDE
