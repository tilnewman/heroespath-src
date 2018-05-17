cppcheck --enable=all --suppress=unusedFunction src/
flint++ -r src/
cpplint --linelength=100 --recursive --filter=-whitespace,-readability/namespace,-readability/todo,-runtime/indentation_namespace,-build/include_order,-build/header_guard,-runtime/references,-legal/copyright,-readability/braces,-build/namespaces,-runtime/int,-readability/multiline_string,-build/c++14 --quiet src/*
clang-tidy ~/src/heroespath-src/src/avatar/*.[ch]pp -- -std=c++14 -I/Users/zieschetilnewman/src/heroespath-src/src/
clang-tidy ~/src/heroespath-src/src/combat/*.[ch]pp -- -std=c++14 -I/Users/zieschetilnewman/src/heroespath-src/src/
clang-tidy ~/src/heroespath-src/src/config/*.[ch]pp -- -std=c++14 -I/Users/zieschetilnewman/src/heroespath-src/src/
clang-tidy ~/src/heroespath-src/src/creature/*.[ch]pp -- -std=c++14 -I/Users/zieschetilnewman/src/heroespath-src/src/
clang-tidy ~/src/heroespath-src/src/game/*.[ch]pp -- -std=c++14 -I/Users/zieschetilnewman/src/heroespath-src/src/
clang-tidy ~/src/heroespath-src/src/interact/*.[ch]pp -- -std=c++14 -I/Users/zieschetilnewman/src/heroespath-src/src/
clang-tidy ~/src/heroespath-src/src/item/*.[ch]pp -- -std=c++14 -I/Users/zieschetilnewman/src/heroespath-src/src/
clang-tidy ~/src/heroespath-src/src/log/*.[ch]pp -- -std=c++14 -I/Users/zieschetilnewman/src/heroespath-src/src/
clang-tidy ~/src/heroespath-src/src/map/*.[ch]pp -- -std=c++14 -I/Users/zieschetilnewman/src/heroespath-src/src/
clang-tidy ~/src/heroespath-src/src/misc/*.[ch]pp -- -std=c++14 -I/Users/zieschetilnewman/src/heroespath-src/src/
clang-tidy ~/src/heroespath-src/src/non-player/*.[ch]pp -- -std=c++14 -I/Users/zieschetilnewman/src/heroespath-src/src/
clang-tidy ~/src/heroespath-src/src/player/*.[ch]pp -- -std=c++14 -I/Users/zieschetilnewman/src/heroespath-src/src/
clang-tidy ~/src/heroespath-src/src/popup/*.[ch]pp -- -std=c++14 -I/Users/zieschetilnewman/src/heroespath-src/src/
clang-tidy ~/src/heroespath-src/src/sfml-util/*.[ch]pp -- -std=c++14 -I/Users/zieschetilnewman/src/heroespath-src/src/
clang-tidy ~/src/heroespath-src/src/sfml-util/gui/*.[ch]pp -- -std=c++14 -I/Users/zieschetilnewman/src/heroespath-src/src/
clang-tidy ~/src/heroespath-src/src/song/*.[ch]pp -- -std=c++14 -I/Users/zieschetilnewman/src/heroespath-src/src/
clang-tidy ~/src/heroespath-src/src/spell/*.[ch]pp -- -std=c++14 -I/Users/zieschetilnewman/src/heroespath-src/src/
clang-tidy ~/src/heroespath-src/src/stage/*.[ch]pp -- -std=c++14 -I/Users/zieschetilnewman/src/heroespath-src/src/
clang-tidy ~/src/heroespath-src/src/state/*.[ch]pp -- -std=c++14 -I/Users/zieschetilnewman/src/heroespath-src/src/
clang-tidy ~/src/heroespath-src/src/stats/*.[ch]pp -- -std=c++14 -I/Users/zieschetilnewman/src/heroespath-src/src/
clang-tidy ~/src/heroespath-src/src/stringutil/*.[ch]pp -- -std=c++14 -I/Users/zieschetilnewman/src/heroespath-src/src/
