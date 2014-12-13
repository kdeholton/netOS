default : all;

run : os;

os : all
	make -C kernel kernel.img
	qemu-system-x86_64 -curses -hdc kernel/kernel.img -hdd fat439/user.img

% :
	(make -C kernel $@)
	(make -C user $@)
	(make -C fat439 $@)
