include ../../Makefile.vars

MYINCS		= -I../include
OBJFILES 	= TestGetJson.o \
                  TestUrl.o
OBJDEPS	 	= $(OBJFILES:%.o=deps/%_deps)
TESTS	 	= $(OBJFILES:%.o=%)
ALLINCS		= ${DWMINCS} ${MYINCS}
ALLLIBS  	= ../lib/libDwmWebUtils.a ${DWMLIBS} -lssl -lcrypto

all: ${TESTS}

runtests: all
	@ for tp in ${TESTS} ; do \
		printf "%-36s " $$tp ; \
		out=`./$$tp` ; \
		if [ $$? -eq 0 ]; then \
		  printf "%25s\n" "$$out" ; \
		else \
		  printf "\n%s" "$$out" ; \
		fi ; \
	done

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

%: %.o ../lib/libDwmWebUtils.a
	${CXX} ${LDFLAGS} -o $@ $< ${ALLLIBS}

../lib/libDwmWebUtils.a::
	${MAKE} -C ../src

clean:
	rm -f ${TESTS} ${OBJFILES}
