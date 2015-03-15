CURDIR=`pwd`

#cd boot
make install

MAKERET=$?
cd $CURDIR
case $MAKERET in
	0)
	mkisofs -R -b boot/grub/stage2_eltorito  -no-emul-boot -boot-load-size 4 -boot-info-table -o praos.iso iso
	;;
	*)
	echo "make install failed!!!"
	;;
esac	
