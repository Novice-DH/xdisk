TARGET=xdisk_server
OBJS=xdisk_server.o xfile_server_task.o
CPPFLAGS= -I../xcom # 编译选项
LIBS=-L../xcom -lxcom
$(TARGET):$(OBJS)
	g++ $^ -o $@ $(LIBS)

clean:
	rm -rf $(OBJS) $(TARGET)