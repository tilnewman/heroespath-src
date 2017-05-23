cppcheck --enable=all .
flint++ -r .
cpplint --linelength=999 --recursive --filter=-whitespace,-readability/namespace,-readability/todo,-runtime/indentation_namespace,-build/include_order,-build/header_guard,-runtime/references,-legal/copyright,-readability/braces,-build/namespaces --quiet *
