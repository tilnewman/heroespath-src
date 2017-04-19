//
// node-type.hpp
//
#include "node-type.hpp"

#include <sstream>
#include <exception>


namespace heroespath
{
namespace combat
{

    const std::string NodeType::ToString(const NodeType::Enum E)
    {
        switch(E)
        {
            case NodeType::Position:      { return "Position"; }
            case NodeType::Obstruction:   { return "Obstruction"; }
            case NodeType::All:
            default:
            {
                std::ostringstream ss;
                ss << "CombatTree::NodeType::ToString(" << E << ")_InvalidValueError.";
                throw std::range_error(ss.str());
            }
        }
    }

}
}
