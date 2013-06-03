compile-equal:
	cd equal; make; cd ../
compile-mkbkp:
	cd mkbkp; make; cd ../
compile-plive:
	cd plive; make; cd ../

all:
	compile-equal compile-mkbkp compile-plive

install:
	cd equal; make install; cd ../
	cd mkbkp; make install; cd ../
	cd plive; make install; cd ../

uninstall:
	cd equal; make clean; cd ../
	cd mkbkp; make uninstall; cd ../
	cd plive; make uninstall; cd ../
