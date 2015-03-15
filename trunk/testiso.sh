case $1 in
	b*)
	bochs -q
	;;
	*)
	qemu -dummy-net -cdrom praos.iso 2> /dev/null
	;;
esac
