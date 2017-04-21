#ifndef APPBASE_HEROESPATH_TESTINGSTAGE_INCLUDED
#define APPBASE_HEROESPATH_TESTINGSTAGE_INCLUDED
//
// testing-stage.hpp
//  A Stage class that allows visualization of testing routines.
//
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/stage.hpp"

#include "heroespath/ouroboros.hpp"
#include "heroespath/stats/stat-set.hpp"

#include <memory>
#include <string>
#include <vector>
#include <utility>


namespace heroespath
{
namespace stage
{

    using StrSizePair_t     = std::pair<std::string, std::size_t>;
    using StrSizePairVec_t  = std::vector<StrSizePair_t>;


    //A Stage class that allows visualizing testing info
    class TestingStage : public sfml_util::Stage
    {
        //prevent copy construction
        TestingStage(const TestingStage &);

        //prevent copy assignment
        TestingStage & operator=(const TestingStage &);

    public:
        TestingStage();
        virtual ~TestingStage();

        virtual void Setup();
        virtual void Draw(sf::RenderTarget & target, sf::RenderStates states);
        virtual void UpdateTime(const float ELAPSED_TIME_SECONDS);
        virtual bool KeyPress(const sf::Event::KeyEvent &);

        void TestingStrAppend(const std::string &);
        void TestingStrIncrement(const std::string &);
        void TestingImageSet(const sfml_util::TextureSPtr_t &);

        void PerformNextTest();
        void PerformStatsTests();

    private:
        void TestStatSetsCurrentAndNormal(const std::string &    TEST_NAME,
                                          const stats::StatSet & ACTUAL,
                                          const stats::StatSet & EXPECTED);

        bool TestImageSet();

    public:
        static const std::size_t TEXT_LINES_COUNT_MAX_;

    private:
        const float SCREEN_WIDTH_;
        const float SCREEN_HEIGHT_;
        sfml_util::TextureSVec_t textureSVec_;
        OuroborosSPtr_t ouroborosSPtr_;
        StrSizePairVec_t testingBlurbsVec_;
        int sleepMilliseconds_;
    };

}
}
#endif //APPBASE_HEROESPATH_TESTINGSTAGE_INCLUDED

