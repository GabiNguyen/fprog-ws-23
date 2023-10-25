all: main.cpp

.outputFolder:
	mkdir -p out

main.cpp: .outputFolder
	g++-10 -std=c++20 -lstdc++ -lm main.cpp -Wall -Wextra -o out/main
	./out/main
