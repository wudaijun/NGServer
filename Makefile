LIB_PATH := ./lib
OUT_PATH := ./run
CC := g++
SHARED = -fPIC -dynamiclib -Wl -undefined dynamic_lookup

All : \
	$(LIB_PATH) \
	$(OUT_PATH) \
	$(OUT_PATH)/server

.PHONY : All

# server 
SERVER_SRC := ./GameServer/Player/*.cpp ./GameServer/Service/*.cpp ./GameServer/*.cpp
$(OUT_PATH)/server : $(SERVER_SRC) $(LIB_PATH)/libgamebasic.so $(LIB_PATH)/libprotocol.so 	
	$(CC) -o $@ -g -w --std=c++11 -I . -I ./GameServer $(SERVER_SRC) -L $(LIB_PATH) -l gamebasic -l protocol
 
#ALL : $(LIB_PATH)/gamebasic.so $(LIB_PATH)/protocol.so
#.PHONY : ALL

# gamebasic
GAMEBASIC_SRC := gamebasic/*.cpp
BOOST_PATH := ./3rd
BOOST_LIB := libboost_system-mt libboost_thread-mt libboost_exception-mt
$(LIB_PATH)/libgamebasic.so : $(GAMEBASIC_SRC)
	$(CC) -o $@ $^ -w --std=c++11 $(SHARED) $(foreach v, $(BOOST_LIB), $(BOOST_PATH)/$(v).a)
	
# protocol
PROTOCOL_SRC := protocol/*.cpp
$(LIB_PATH)/libprotocol.so : $(PROTOCOL_SRC)
	$(CC) -o $@ $^ -w --std=c++11 $(SHARED)

$(LIB_PATH) :
	mkdir $(LIB_PATH)

$(OUT_PATH) :
	mkdir $(OUT_PATH)


clean :
	rm $(LIB_PATH)/libgamebasic.so $(LIB_PATH)/libprotocol.so $(OUT_PATH)/server


