# Makefile for building the kernel module

# Tên mô-đun
obj-m += WQsched.o

# Thư mục chứa header của kernel
KDIR := /lib/modules/$(shell uname -r)/build

# Thư mục làm việc hiện tại
PWD := $(shell pwd)

# Lệnh để biên dịch mô-đun
all:
	make -C $(KDIR) M=$(PWD) modules

# Lệnh để dọn dẹp khi biên dịch xong
clean:
	make -C $(KDIR) M=$(PWD) clean

