TARGET=xdisk_server
OBJS=xserver_server.o
CPPFLAS= -I../xcom # 编译选项
LIBS=-L../xcom -lxcom
$(TARGET):$(OBJS)
	g++ -shared $^ -o $@ $(LIBS)

clean:
	rm -rf $(OBJS) $(TARGET)