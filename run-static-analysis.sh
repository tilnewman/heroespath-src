cppcheck --enable=all src/
flint++ -r src/
cpplint --linelength=999 --recursive --filter=-whitespace,-readability/namespace,-readability/todo,-runtime/indentation_namespace,-build/include_order,-build/header_guard,-runtime/references,-legal/copyright,-readability/braces,-build/namespaces,-runtime/int,-readability/multiline_string,-build/c++11 --quiet src/*
