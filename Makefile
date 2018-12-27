all: buildgbaemu4ds

#Make
buildgbaemu4ds:
	$(MAKE)	-R	-C	hbmenu/
	$(MAKE)	-R	-C	hyperspeedup/

clean:
	$(MAKE)	clean	-C	hbmenu/
	$(MAKE)	clean	-C	hyperspeedup/