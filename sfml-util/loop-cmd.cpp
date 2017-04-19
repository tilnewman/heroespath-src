//
// loop-cmd.cpp
//
#include "loop-cmd.hpp"


namespace sfml_util
{

    LoopCmd::LoopCmd(const std::string & NAME, ILoopSPtr_t & loopSPtr)
    :
        NAME_    (std::string(NAME).append("_LoopCommand")),
        iLoopSPtr_(loopSPtr)
    {}


    LoopCmd::~LoopCmd()
    {}

}
