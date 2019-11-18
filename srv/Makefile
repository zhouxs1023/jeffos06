all: namer console keyboard null test1 test2 ne2000 io

ide::
	@echo "----  srv/ide  ----"
	@cd namer ; $(MAKE)

namer::
	@echo "---- srv/namer  ----"
	@cd namer ; $(MAKE)

console::
	@echo "----  srv/console  ----"
	@cd console ; $(MAKE)

keyboard::
	@echo "----  srv/keyboard  ----"
	@cd keyboard ; $(MAKE)

test1::
	@echo "----  srv/test1  ----"
	@cd test1 ; $(MAKE)

test2::
	@echo "----  srv/test2  ----"
	@cd test2 ; $(MAKE)

test3::
	@echo "----  srv/test3  ----"
	@cd test3 ; $(MAKE)

null::
	@echo "----  srv/null  ----"
	@cd null ; $(MAKE)

ne2000::
	@echo "---- srv/ne2000 ----"
	@cd ne2000 ; $(MAKE)

io::
	@echo "---- srv/io ----"
	@cd io ; $(MAKE)

clean:
	@cd namer ; $(MAKE) clean
	@cd console ; $(MAKE) clean
	@cd keyboard ; $(MAKE) clean
	@cd test1 ; $(MAKE) clean
	@cd test2 ; $(MAKE) clean
	@cd null ; $(MAKE) clean
	@cd ne2000 ; $(MAKE) clean
	@cd io ; $(MAKE) clean
