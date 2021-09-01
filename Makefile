obj-m+= run.o

all:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) clean
in:
	sudo insmod run.ko
out:
	sudo rmmod run
