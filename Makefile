DESTDIR?=dist/
VARIANT?=debug

build:
	# building ${VARIANT}...
	bjam build ${VARIANT} --user-config=./build-config.jam

clean:
	# clean...
	bjam --clean --user-config=./build-config.jam

install:
	# install...
	bjam install ${VARIANT} --user-config=./build-config.jam --prefix=${DESTDIR}

distclean: clean
