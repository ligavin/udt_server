WBL_INC=-I /home/gavinlli/mc_server/lib/publiclib/wbl_oss/include
WBL_LIB=-L /home/gavinlli/mc_server/lib/publiclib/wbl_oss/lib -lwbl

UDT_INC=-I/home/gavinlli/mypro/udt4/src
UDT_LIB=-L/home/gavinlli/mypro/udt4/src -ludt

CFLAGS += -Wall
CXX = g++
AROPT=-scurv
RANLIB=ranlib

INCLUDE = $(UDT_INC) $(WBL_INC)

%.o: %.cpp
	$(CXX) $(CFLAGS) $(INCLUDE) -c $<