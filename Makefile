LIB_PATH := ./lib
OUT_PATH := ./run
CC := g++
SHARED = -fPIC -dynamiclib -Wl -undefined dynamic_lookup

$(LIB_PATH) :
	mkdir $(LIB_PATH)

$(OUT_PATH) :
	mkdir $(OUT_PATH)

# server 
SERVER_SRC := ./GameServer/Player/*.cpp ./GameServer/Service/*.cpp ./GameServer/*.cpp
$(OUT_PATH)/server : $(SERVER_SRC) $(LIB_PATH)/gamebasic.so $(LIB_PATH)/protocol.so 	
	$(CC) -o $@ -w --std=c++11 -I . -I ./GameServer $(SERVER_SRC) -L $(LIB_PATH) -l gamebasic -l protocol
 
#ALL : $(LIB_PATH)/gamebasic.so $(LIB_PATH)/protocol.so
#.PHONY : ALL

# gamebasic
GAMEBASIC_SRC := gamebasic/*.cpp
BOOST_LIB := libboost_system-mt.a libboost_thread-mt.a libboost_exception.a
BOOST_PATH := /usr/local/lib/boost/
$(LIB_PATH)/gamebasic.so : $(GAMEBASIC_SRC)
	$(CC) -o $@ $^ -w --std=c++11 $(SHARED) $(BOOST_PATH)libboost_system-mt.a $(BOOST_PATH)libboost_thread-mt.a $(BOOST_PATH)libboost_exception.a

# protocol
PROTOCOL_SRC := protocol/*.cpp
$(LIB_PATH)/protocol.so : $(PROTOCOL_SRC)
	$(CC) -o $@ $^ -w --std=c++11 $(SHARED)


clean :
	rm $(LIB_PATH)/gamebasic.so $(LIB_PATH)/protocol.so $(OUT_PATH)/server


