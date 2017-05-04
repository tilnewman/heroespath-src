#ifndef GAME_MARGINS_INCLUDED
#define GAME_MARGINS_INCLUDED
//
// margins.hpp
//  Margins code
//


namespace sfml_util
{

    struct Margins
    {
        explicit Margins(const float LEFT   = 0.0f,
                         const float RIGHT  = 0.0f,
                         const float TOP    = 0.0f,
                         const float BOTTOM = 0.0f)
        :
            left(LEFT),
            right(RIGHT),
            top(TOP),
            bottom(BOTTOM)
        {}

        virtual ~Margins() {}

        float left;
        float right;
        float top;
        float bottom;
    };

}
#endif //GAME_MARGINS_INCLUDED
