%: %.cpp
	g++ $@.cpp -o $@ -lm -lpthread -std=c++11
