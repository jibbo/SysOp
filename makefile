SUBDIRS = equal mkbkp plive

all: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@

install:
	$(MAKE) install -C $@

uninstall:
	cd equal && make clean ; cd ../
	cd mkbkp && make uninstall; cd ../
	cd plive && make uninstall; cd ../
