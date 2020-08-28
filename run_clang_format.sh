find src/parser -iname *.h -o -iname *.cpp | xargs clang-format -i
find src/ -iname *.h -o -iname *.cpp | xargs clang-format -i
find unittests/ -iname *.h -o -iname *.cpp | xargs clang-format -i
