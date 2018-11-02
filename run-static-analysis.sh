#!/bin/bash
echo Running Static Analysis on source files at $1 and on binary at $2
cd $1
echo
echo


#Cppcheck -A static analysis tool for C/C++ code.
#brew
echo ---------------------------
echo CppCheck v1.85, 2018-11-1
echo ---------------------------
cppcheck --enable=all --suppress=unusedFunction .
echo ---------------------------
echo
echo
echo


#FlawFinder -A C/C++ linter for possible security weaknesses
#brew
echo ---------------------------
echo FlawFinder v2.0.6, 2018-11-1
echo ---------------------------
flawfinder .
echo ---------------------------
echo
echo
echo


#Flint++ -Facebook's C++ Linter
#github
echo ---------------------------
echo Flint++ v?, 2018-11-1, "longer under development"
echo ---------------------------
flint++ -r .
echo ---------------------------
echo
echo
echo


#cpplint -A static code checker for C++ following Google's C++ style guide
#github
echo ---------------------------
echo CppLint v1.3.0, 2018-11-1
echo ---------------------------
cpplint --linelength=100 --recursive --filter=-whitespace,-readability/namespace,-readability/todo,-runtime/indentation_namespace,-build/include_order,-build/header_guard,-runtime/references,-legal/copyright,-readability/braces,-build/namespaces,-runtime/int,-readability/multiline_string,-build/c++17 --quiet .
echo ---------------------------
echo
echo
echo


#oclint -A static code analysis tool for improving quality and reducing defects in C/C++
#2018-11-1
#brew, See http://docs.oclint.org/en/stable/intro/homebrew.html for instructions on how to use brew to install
echo ---------------------------
echo OCLint TODO
echo ---------------------------
echo
echo
echo


#Metrix++
#(downloaded the source from: http://metrixplusplus.sourceforge.net/home.html)
echo ---------------------------
echo Metrix++ v1.3, last commit in my clone was 2013-8, 2018-11-1
echo ---------------------------
python "/Users/zieschetilnewman/src/Metrix++/metrix++.py" collect . --std.code.lines.code --std.code.complexity.cyclomatic --std.code.lines.code --std.code.todo.comments
echo ---------------------------
python "/Users/zieschetilnewman/src/Metrix++/metrix++.py" view
echo ---------------------------
python "/Users/zieschetilnewman/src/Metrix++/metrix++.py" limit --db-file=metrixpp.db --max-limit=std.code.complexity:cyclomatic:20 --hotspots=1000 | grep -E '(for region|Metric value)' | grep -v "-enum" | grep -v "ToString"
rm metrixpp.db
echo ---------------------------
echo
echo
echo


#Lizard -A simple code complexity analyser without caring about the C/C++ header files
#github
echo ---------------------------
echo Lizard v1.15.6, last commit in my clone was 2018-10-26, 2018-11-1
echo ---------------------------
python ~/src/lizard/lizard.py -s cyclomatic_complexity -w . | grep -v "-enum" | grep -v "ToString"
echo ---------------------------
echo
echo
echo


#CppClean
echo ---------------------------
echo CppClean v0.12, 2018-11-1
echo ---------------------------
cppclean . --include-path=.
echo ---------------------------
echo
echo RUNNING AGAIN FOR SUBDIRECTORIES
echo ---------------------------
echo CppClean v0.12, 2018-11-1
echo ---------------------------
for D in *; do [ -d "${D}" ] && cppclean "${D}" --include-path=.; done
echo ---------------------------
echo
echo
echo


#clang-tidy
#
#abseil- 	Checks related to Abseil library.
#android- 	Checks related to Android.
#boost- 	Checks related to Boost library.
#bugprone- 	Checks that target bugprone code constructs.
#cert- 	Checks related to CERT Secure Coding Guidelines.
#cppcoreguidelines- 	Checks related to C++ Core Guidelines.
#clang-analyzer- 	Clang Static Analyzer checks.
#fuchsia- 	Checks related to Fuchsia coding conventions.
#google- 	Checks related to Google coding conventions.
#hicpp- 	Checks related to High Integrity C++ Coding Standard.
#llvm- 	Checks related to the LLVM coding conventions.
#misc- 	Checks that we didn’t have a better category for.
#modernize- 	Checks that advocate usage of modern (currently “modern” means “C++11”) language constructs.
#mpi- 	Checks related to MPI (Message Passing Interface).
#objc- 	Checks related to Objective-C coding conventions.
#performance- 	Checks that target performance-related issues.
#portability- 	Checks that target portability-related issues that don’t relate to any particular coding style.
#readability- 	Checks that target readability-related issues that don’t relate to any particular coding style.
#zircon- 	Checks related to Zircon kernel coding conventions.
#
echo ---------------------------
echo clang-tidy  modernize-*, LLVMv7.0.0, 2018-11-1
echo ---------------------------
#clang-tidy * -checks=modernize-* -- -std=c++17 -DBOOST_NO_AUTO_PTR
echo DISABLED
echo ---------------------------
echo
echo RUNNING AGAIN FOR SUBDIRECTORIES
echo
echo ---------------------------
echo clang-tidy  modernize-*, LLVMv7.0.0, 2018-11-1
echo ---------------------------
#for D in *; do [ -d "${D}" ] && clang-tidy "${D}"/* -checks=modernize-* -- -std=c++17 -DBOOST_NO_AUTO_PTR -I$1; done
echo DISABLED
echo ---------------------------
echo
echo
echo


#Bloaty McBloatface -A command line tool that shows the binary composition both as a file and in memory
#github
echo ---------------------------
echo Bloaty McBloatface v1.0, last commit in my clone was 2018-8-21, 2018-11-1
echo ---------------------------
~/src/bloaty/bloaty $2
echo ---------------------------
echo
echo
echo

