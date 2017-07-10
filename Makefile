obj-m:=print_pid.o 
kernel_path=/lib/modules/$(shell uname -r)/build
  
all:  
	make -C $(kernel_path) M=$(PWD) modules
	gcc test_word_count.c  -o test_word_count 
clean:  
	make -C $(kernel_path) M=$(PWD)  
