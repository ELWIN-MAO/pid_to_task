obj-m:=pid2task.o 
kernel_path=/lib/modules/$(shell uname -r)/build
  
all:  
	make -C $(kernel_path) M=$(PWD) modules
	gcc test_pid2task.c  -o  test_pid2task
clean:  
	make -C $(kernel_path) M=$(PWD)  
