#!/bin/sh
# @file update.sh
# @author <raphael.londeix@gmail.com> Raphaël Londeix

SCRIPTCMD="$0"
SCRIPT=`basename "$0"`
SCRIPT_DIR=`python -c "import os;print(os.path.abspath(os.path.dirname('$0')))"`
cd "${SCRIPT_DIR}"

abspath()
{
	python -c 'import os, sys; print(os.path.abspath(os.path.join(*((len(sys.argv) > 1) and sys.argv[1:] or ["."]))))' "$@"
}

fatal()
{
	echo "$*" 1>&2
	exit 1
}

DEBUG=1
debug()
{
	[ $DEBUG = 1 ] && echo "--[ $*"
}

usage()
{
	fatal "Usage: $SCRIPTCMD"
}

[ -z "`which hg 2> /dev/null`" ] && fatal "Mercurial not installed (hg command not found)"
[ -z "`which svn 2> /dev/null`" ] && fatal "Subversion not installed (svn command not found)"
if [ ! -z "`which wget 2> /dev/null`" ]
then
	fetch()
	{
			debug "wget $1 -> $2"
		wget "$1" -O "$2"
	}
else
	if [ ! -z "`which curl 2> /dev/null`" ]
	then
		fetch()
		{
			debug "curl $1 -> $2"
			curl "$1" -o "$2"
		}
	else
		fatal "Cannot fetch from internet (wget and curl commands not found)"
	fi
fi

################################ Python #######################################
PYTHON_VERSION=v3.4.0
PYTHON_DIR=Python-${PYTHON_VERSION}
debug "Updating ${PYTHON_DIR}"
if [ ! -d ${PYTHON_DIR} ]
then
	hg clone http://hg.python.org/cpython ${PYTHON_DIR}
fi
( cd ${PYTHON_DIR} && hg pull && hg update && hg checkout ${PYTHON_VERSION})

################################ Boost ########################################
BOOST_VERSION=1_55_0
BOOST_DIR=Boost_${BOOST_VERSION}
debug "Updating ${BOOST_DIR}"
if [ ! -d ${BOOST_DIR} ]
then
	svn co http://svn.boost.org/svn/boost/tags/release/${BOOST_DIR} ${BOOST_DIR}
else
	( cd ${BOOST_DIR} && svn update )
fi

################################ SDL ##########################################
debug "Updating SDL"
[ ! -d SDL ] && hg clone http://hg.libsdl.org/SDL
( cd SDL && hg pull && hg update )
[ ! -d SDL_image ] && hg clone http://hg.libsdl.org/SDL_image
( cd SDL_image && hg pull && hg update )

############################ Bullet physics ###################################
debug "Update BulletPhysics"
BULLET_VERSION="2.82-r2704"
BULLET_NAME="bullet-${BULLET_VERSION}"
BULLET_TARBALL="${BULLET_NAME}.tgz"
BULLET_URL="https://bullet.googlecode.com/files/${BULLET_TARBALL}"

[ ! -d  "${BULLET_NAME}" ] && (
	[ ! -f "${BULLET_TARBALL}" ] && fetch "${BULLET_URL}" "${BULLET_TARBALL}"
	tar xf ${BULLET_TARBALL}
)

################################ CURL #########################################
debug "Update curl"
CURL_VERSION=7.35.0
CURL_NAME="curl-${CURL_VERSION}"
CURL_TARBALL="${CURL_NAME}.tar.gz"
CURL_URL="http://curl.haxx.se/download/${CURL_TARBALL}"

[ ! -d "${CURL_NAME}" ] && (
	[ ! -f "${CURL_TARBALL}" ] && fetch "${CURL_URL}" "${CURL_TARBALL}"
	tar xf "${CURL_TARBALL}"
)

############################### LIBIDN ########################################
debug "Update libidn"
LIBIDN_VERSION=1.9
LIBIDN_NAME=libidn-${LIBIDN_VERSION}
LIBIDN_TARBALL=${LIBIDN_NAME}.tar.gz
LIBIDN_URL=ftp://alpha.gnu.org/pub/gnu/libidn/${LIBIDN_TARBALL}

[ ! -d "${LIBIDN_NAME}" ] && (
	[ ! -f "${LIBIDN_TARBALL}" ] && fetch "${LIBIDN_URL}" "${LIBIDN_TARBALL}"
	tar xf "${LIBIDN_TARBALL}"
)

############################## OpenSSL ########################################
debug "Update OpenSSL"
OPENSSL_VERSION=1.0.1f
OPENSSL_NAME=openssl-${OPENSSL_VERSION}
OPENSSL_TARBALL=${OPENSSL_NAME}.tar.gz
OPENSSL_URL=https://www.openssl.org/source/${OPENSSL_TARBALL}

[ ! -d "${OPENSSL_NAME}" ] && (
	[ ! -f "${OPENSSL_TARBALL}" ] && fetch "${OPENSSL_URL}" "${OPENSSL_TARBALL}"
	tar xf "${OPENSSL_TARBALL}"
)

################################ ZLIB #########################################
debug "Update zlib"
ZLIB_VERSION=1.2.8
ZLIB_NAME=zlib-${ZLIB_VERSION}
ZLIB_TARBALL=${ZLIB_NAME}.tar.gz
ZLIB_URL=http://zlib.net/${ZLIB_TARBALL}

[ ! -d "${ZLIB_NAME}" ] && (
	[ ! -f "${ZLIB_TARBALL}" ] && fetch "${ZLIB_URL}" "${ZLIB_TARBALL}"
	tar xf "${ZLIB_TARBALL}"
)

################################ BZIP2 ########################################
debug "Update bzip2"
BZIP2_VERSION=1.0.6
BZIP2_NAME=bzip2-${BZIP2_VERSION}
BZIP2_TARBALL=${BZIP2_NAME}.tar.gz
BZIP2_URL=http://www.bzip.org/${BZIP2_VERSION}/${BZIP2_TARBALL}

[ ! -d "${BZIP2_NAME}" ] && (
	[ ! -f "${BZIP2_TARBALL}" ] && fetch "${BZIP2_URL}" "${BZIP2_TARBALL}"
	tar xf "${BZIP2_TARBALL}"
)
