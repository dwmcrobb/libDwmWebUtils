include ../../Makefile.vars

MYINCS		= -I../../classes/include
OBJFILES 	= dwmfetch.o
OBJDEPS	 	= $(OBJFILES:%.o=deps/%_deps)
APPS	 	= $(OBJFILES:%.o=%)
ALLINCS		= ${DWMINCS} ${MYINCS}
ALLLIBS  	= ../../classes/lib/libDwmWebUtils.a ${DWMLIBS} -lssl -lcrypto
ALLLIBS        += ${OSLIBS}
LDFLAGS		= -Wl,-rpath ${DWMDIR}/lib

all: ${APPS}

#  dependency rule
deps/%_deps: %.cc 
	@echo "making dependencies for $<"
	@set -e; \
	${CXX} -MM ${CXXFLAGS} ${ALLINCS} -c $< | \
	 sed 's/\($*\)\.o[ :]*/\1.o $(@D)\/$(@F) : /g' > $@ ; [ -s $@ ] || \
	 rm -f $@

#  only include dependency makefiles if target is not 'clean'
ifneq ($(MAKECMDGOALS),clean)
-include ${OBJDEPS}
endif

%.o: %.cc deps/%_deps
	${CXX} ${CXXFLAGS} ${PTHREADCXXFLAGS} ${ALLINCS} -c $<

%: %.o ../../classes/lib/libDwmWebUtils.a
	${CXX} ${LDFLAGS} -o $@ $< ${ALLLIBS}

../../classes/lib/libDwmWebUtils.a::
	${MAKE} -C ../../classes/src

clean:
	rm -f ${APPS} ${OBJFILES}

distclean:: clean
	rm -f deps/*_deps
