obj-m+=gpio_test.o

.PHONY: all
all: gpio_test.ko bin/buttonled

# This rule tells make how to build modules
.PHONY: gpio_test.ko
gpio_test.ko: gpio_test.c
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) modules

# This rule tells make how to build application "buttonled"
bin/buttonled: src/main.c src/buttonled.c
	g++ -o bin/buttonled src/buttonled.c src/main.c -l pthread

# This rule tells make to delete application "buttonled" and modules
.PHONY: clean
clean: 
	rm -f bin/buttonled
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) clean