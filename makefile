all: main.cpp tests.cpp

.outputFolder:
	mkdir -p out

main.cpp: .outputFolder
	g++-10 -std=c++20 -lstdc++ -lm main.cpp -Wall -Wextra -o out/main

tests.cpp: .outputFolder
	g++-10 -std=c++20 -lstdc++ -lm tests.cpp -Wall -Wextra -o out/tests
	./out/tests

tests: tests.cpp
	./out/tests