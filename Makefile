# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt

DESTDIR?=dist/
VARIANT?=debug
TOOLSET?=gcc
UPDATE?=yes

build: update-tools
	# building ${VARIANT}...
	bjam build ${VARIANT} --user-config=./build-config.jam --toolset=${TOOLSET} -j2

clean: update-tools
	# clean...
	bjam --clean build --user-config=./build-config.jam --toolset=${TOOLSET} -j2

install: update-tools
	# install...
	bjam install ${VARIANT} --user-config=./build-config.jam --prefix=${DESTDIR} --toolset=${TOOLSET} -j2

uninstall: update-tools
	# uninstall...
	bjam --clean install --user-config=./build-config.jam --prefix=${DESTDIR} --toolset=${TOOLSET} -j2

distclean: update-tools clean

update-tools:
	# updating tools...
	@[ "$(UPDATE)" = "yes" ] && git remote add build-tools http://github.com/berenm/boost-build-tools.git 2>/dev/null ||:
	@[ "$(UPDATE)" = "yes" ] && git fetch build-tools master:build-tools 2>/dev/null                                  ||:
	@[ "$(UPDATE)" = "yes" ] && git checkout build-tools Makefile multiarch.jam build-config.jam                      ||:
	@[ "$(UPDATE)" = "yes" ] && git reset fhs.jam build-config.jam 2>/dev/null                                        ||:
	@[ "$(UPDATE)" = "yes" ] && git branch -D build-tools 2>/dev/null 1>/dev/null                                     ||:
	@[ "$(UPDATE)" = "yes" ] && git remote rm build-tools                                                             ||:
