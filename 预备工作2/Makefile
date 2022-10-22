exe:
	arm-linux-gnueabihf-gcc -S main.s main.c
	arm-linux-gnueabihf-gcc -o main main.s sylib.c
	qemu-arm -L /usr/arm-linux-gnueabihf ./main

clean:
	rm main.s main
