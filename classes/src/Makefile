include ../../Makefile.vars

LTCOMPILE    = ${LIBTOOL} --mode=compile --tag=CXX ${CXX}
LTLINK       = ${LIBTOOL} --tag=CXX --mode=link ${CXX}

DWMLIBS		= $(shell pkg-config --libs libDwm)
INCS            = -I../include ${DWMINCS}
OBJFILESNP	= DwmWebUtils.o \
                  DwmWebUtilsGetFailure.o \
                  DwmWebUtilsUrl.o \
                  DwmWebUtilsVersion.o
OBJFILES        = $(OBJFILESNP:%=../obj/%)
OBJDEPS         = $(OBJFILESNP:%.o=deps/%_deps)
SHARED_OBJFILES = $(OBJFILES:%.o=%.lo)
shlib_version = $(shell ../../getvers.sh -s)

../lib/libDwmWebUtils.la: ${SHARED_OBJFILES} ${OBJFILES}
	${LTLINK} -o $@ ${SHARED_OBJFILES} -rpath ${INSTALLPREFIX}/lib -version-info ${shlib_version} ${DWMLIBS}

#  dependency rule
deps/%_deps: %.cc
	@echo "making dependencies for $<"
	@set -e; \
	${CXX} -MM ${CXXFLAGS} ${INCS} -c $< | \
	 sed 's/\($*\)\.o[ :]*/\1.o \1.lo $(@D)\/$(@F) : /g' > $@ ; [ -s $@ ] || \
	 rm -f $@

#  only include dependency makefiles if target is not 'clean' or 'distclean'
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),distclean)
-include ${OBJDEPS}
endif
endif

../obj/%.lo ../obj/%.o: %.cc deps/%_deps
	${LTCOMPILE} ${CXXFLAGS} -o $@ -c $< ${INCS}

DwmWebUtilsUrl.cc: DwmWebUtilsUrl.lex
	flex -d -o$@ $<

tarprep: ${TARDIR}/lib/libDwmWebUtils.la

${TARDIR}/lib/libDwmWebUtils.la: ../lib/libDwmWebUtils.la
	${LIBTOOL} --mode=install ../../install-sh -c $< $@

clean:: clean-tarprep
	${LIBTOOL} --mode=clean rm -f ../lib/libDwmWebUtils.la ${SHARED_OBJFILES} DwmWebUtilsUrl.cc

distclean:: clean
	rm -f ${OBJDEPS}

clean-tarprep::
	${LIBTOOL} --mode=uninstall rm -f ${TARDIR}/lib/libDwmWebUtils.la
