OUT_PATH := run
BOOST_PATH := ../3rd
LIB_PATH := lib
CC := g++
BOOST_LIB := libboost_system-mt libboost_thread-mt libboost_exception-mt

all : $(OUT_PATH) \
	$(OUT_PATH)/test
.PHONY : all

$(OUT_PATH)/test : src/*.cpp
	cp ../lib/libprotocol.so lib/
	$(CC) -o $@ $^ -w -g --std=c++11 -I. $(foreach v, $(BOOST_LIB), $(BOOST_PATH)/$(v).a) -L $(LIB_PATH) -l protocol

clean:
	rm -r $(OUT_PATH)/test*
