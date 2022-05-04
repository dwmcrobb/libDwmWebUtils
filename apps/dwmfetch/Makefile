include ../../Makefile.vars

LTLINK          = ${LIBTOOL} --tag=CXX --mode=link ${CXX}
MYINCS		= -I../../classes/include
OBJFILES 	= dwmfetch.o
OBJDEPS	 	= $(OBJFILES:%.o=deps/%_deps)
APPS	 	= $(OBJFILES:%.o=%)
ALLINCS		= ${DWMINCS} ${MYINCS}
ALLLIBS  	= ../../classes/lib/libDwmWebUtils.la ${DWMLIBS} -lssl -lcrypto
ALLLIBS        += ${OSLIBS}
LDFLAGS		= -Wl,-rpath ${DWMDIR}/lib
TARTARGETS      = ${TARDIR}/bin/dwmfetch

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

%: %.o ../../classes/lib/libDwmWebUtils.la
	${LTLINK} ${LDFLAGS} -o $@ $< ${ALLLIBS}

../../classes/lib/libDwmWebUtils.la::
	${MAKE} -C ../../classes/src

tarprep: ${TARTARGETS}

${TARDIR}/bin/dwmfetch: ${APPS}
	${LIBTOOL} --mode=install ../../install-sh -s -c -m 555 $< $@

clean::
	rm -f ${APPS} ${OBJFILES}
	rm -f ${TARTARGETS}

distclean:: clean
	rm -f deps/*_deps
