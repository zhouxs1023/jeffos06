# JeffOS Makefile

all: lib srv boot kernel

boot:: 
	@echo "" 
	@echo "--- boot ----------------"
	@cd boot ; $(MAKE)

kernel::
	@echo ""
	@echo "--- kernel v0.2 --------------"
	@cd kernel ; $(MAKE)

lib::
	@echo ""
	@echo "--- lib -----------------"
	@cd lib ; $(MAKE)

srv::
	@echo ""
	@echo "--- srv --------------"
	@cd srv ; $(MAKE)

floppy::
	bootmaker openblt.ini kernel.img -floppy

clean:
	@cd boot ; $(MAKE) clean
	@cd lib ; $(MAKE) clean
	@cd srv ; $(MAKE) clean
	@cd kernel ; $(MAKE) clean

