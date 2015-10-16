include ../Inc.mk
 
LIB_TOOLS = ../lib/libcomm.a

OBJS_TOOLS = tools.o   task.o lockmutex.o\
			lockcond.o timer.o task_client.o
	#PwdProtected.o ptlogin2_api.o CBalanceCredit.o tools.o \ 
	#CGameCoinTools_v2.o 
	#channel_pay_base.o channel_pay_tools.o


LIBS = $(LIB_TOOLS)
OBJS = $(OBJS_TOOLS)

INCLUDE += -I ../

all: $(LIBS)

$(LIB_TOOLS): $(OBJS_TOOLS) 
	$(AR) $(AROPT) $@ $? 
	$(RANLIB) $@
		
clean:
	rm -f $(OBJS) $(LIBS)
