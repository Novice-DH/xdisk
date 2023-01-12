TARGET=libxcom.so
OBJS=xserver_task.o xthread.o xthread_pool.o
CPPFLAGS= -fPIC # 编译选项
LIBS=-levent -lpthread
$(TARGET):$(OBJS)
	g++ -shared $^ -o $@ $(LIBS)

clean:
	rm -rf $(OBJS) $(TARGET)