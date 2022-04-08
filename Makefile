include Makefile.vars

all:: classes/lib/libDwmWebUtils.a

classes/lib/libDwmWebUtils.a::
	${MAKE} -C classes

tarprep:: classes/lib/libDwmWebUtils.a
	${MAKE} -C classes $@
ifeq ("${BUILD_DOCS}", "yes")
	${MAKE} -C doc $@
endif
	${MAKE} -C packaging $@

package: ${OSNAME}-pkg

freebsd-pkg: tarprep
	mkfbsdmnfst -r packaging/fbsd_manifest -s staging classes/tests > staging/+MANIFEST
	pkg create -o . -r staging -m staging

darwin-pkg: tarprep
	pkgbuild --root staging --identifier com.mcplex.libDwmWebUtils --version ${VERSION} libDwmUtils-${VERSION}.pkg

linux-pkg: tarprep
	if [ ! -d staging/DEBIAN ]; then mkdir staging/DEBIAN; fi
	mkdebcontrol -r packaging/debcontrol -s staging/usr/local > staging/DEBIAN/control
	dpkg-deb -b --root-owner-group staging
	dpkg-name -o staging.deb

clean::
	${MAKE} -C classes $@

distclean:: clean
	${MAKE} -C classes $@
	${MAKE} -C packaging $@
	rm -Rf autom4te.cache staging
	rm -f config.log config.status Makefile.vars
	rm -f libDwmWebUtils_*.deb libDwmUtils-*.pkg

