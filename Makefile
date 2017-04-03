.PHONY: test
test: ./build/test
	./build/test

./build/test: ./test/shared_resource_test.cpp
	mkdir -p $(shell dirname $@)
	$(CXX) $< -std=c++14 -o $@ -lgtest_main -lgtest
