include ../../Makefile.vars

# CXXFLAGS	= -O2 $(shell pkg-config --cflags libDwm)
DWMLIBS		= $(shell pkg-config --libs libDwm)
INCS            = -I../include ${DWMINCS}
OBJFILESNP	= DwmWebUtils.o \
                  DwmWebUtilsUrl.o \
                  DwmWebUtilsVersion.o
OBJFILES        = $(OBJFILESNP:%=../obj/%)
OBJDEPS         = $(OBJFILESNP:%.o=deps/%_deps)

../lib/libDwmWebUtils.a: ${OBJFILES}
	ar rv $@ $^

#  dependency rule
deps/%_deps: %.cc
	@echo "making dependencies for $<"
	@set -e; \
	${CXX} -MM ${CXXFLAGS} ${INCS} -c $< | \
	 sed 's/\($*\)\.o[ :]*/\1.o $(@D)\/$(@F) : /g' > $@ ; [ -s $@ ] || \
	 rm -f $@

#  only include dependency makefiles if target is not 'clean' or 'distclean'
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),distclean)
-include ${OBJDEPS}
endif
endif

../obj/%.o: %.cc
	${CXX} ${CXXFLAGS} -o $@ -c $< ${INCS}

DwmWebUtilsUrl.cc: DwmWebUtilsUrl.lex
	flex -d -o$@ $<

tarprep: ${TARDIR}/lib/libDwmWebUtils.a

${TARDIR}/lib/libDwmWebUtils.a: ../lib/libDwmWebUtils.a
	../../install-sh -c $< $@

clean:: clean-tarprep
	rm -f ../lib/libDwmWebUtils.a ${OBJFILES} DwmWebUtilsUrl.cc

distclean:: clean
	rm -f ${OBJDEPS}

clean-tarprep::
	${LIBTOOL} --mode=clean rm -f ${TARDIR}/lib/libDwmWebUtils.a
