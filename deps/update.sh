#!/bin/sh
# @file update.sh
# @author <raphael.londeix@gmail.com> Raphaël Londeix

SCRIPTCMD="$0"
SCRIPT=`basename "$0"`
SCRIPT_DIR=`python -c "import os;print(os.path.abspath(os.path.dirname('$0')))"`
cd "${SCRIPT_DIR}"

[ -n "$VERBOSE" ] && set -x

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

try()
{
	"$@" || fatal "Coudn't execute $*"
}

[ -z "`which hg 2> /dev/null`" ] && fatal "Mercurial not installed (hg command not found)"
[ -z "`which svn 2> /dev/null`" ] && fatal "Subversion not installed (svn command not found)"
if [ ! -z "`which wget 2> /dev/null`" ]
then
	fetch()
	{
		debug "wget $1 -> $2"
		wget --no-check-certificate "$1" -O "$2"
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
	try hg clone http://hg.python.org/cpython ${PYTHON_DIR}
fi
( try cd ${PYTHON_DIR} && hg pull && hg update && hg checkout ${PYTHON_VERSION} )

################################ Boost ########################################
BOOST_DIR=boost_1_57_0
BOOST_TARBALL="${BOOST_DIR}.tar.gz"
BOOST_URL="http://downloads.sourceforge.net/project/boost/boost/1.57.0/boost_1_57_0.tar.gz?r=http%3A%2F%2Fsourceforge.net%2Fprojects%2Fboost%2Ffiles%2Fboost%2F1.57.0%2F&ts=1418808100&use_mirror=freefr"
debug "Updating ${BOOST_DIR}"
if [ ! -d ${BOOST_DIR} ]
then
	[ ! -f "${BOOST_TARBALL}" ] && try fetch "${BOOST_URL}" "${BOOST_TARBALL}"
	try tar xf "${BOOST_TARBALL}"
fi

################################ SDL ##########################################
debug "Updating SDL"
#SDL_VERSION=release-2.0.3
SDL_VERSION=default
[ ! -d SDL ] && try hg clone http://hg.libsdl.org/SDL
( try cd SDL && hg pull && hg update && hg checkout ${SDL_VERSION} )
[ ! -d SDL_image ] && try hg clone http://hg.libsdl.org/SDL_image
( try cd SDL_image && hg pull && hg update )

############################ Bullet physics ###################################
debug "Update BulletPhysics"
BULLET_VERSION="2.82-r2704"
BULLET_NAME="bullet-${BULLET_VERSION}"
BULLET_TARBALL="${BULLET_NAME}.tgz"
BULLET_URL="https://bullet.googlecode.com/files/${BULLET_TARBALL}"

[ ! -d  "${BULLET_NAME}" ] && (
	[ ! -f "${BULLET_TARBALL}" ] && try fetch "${BULLET_URL}" "${BULLET_TARBALL}"
	try tar xf ${BULLET_TARBALL}
)

################################ CURL #########################################
debug "Update curl"
CURL_VERSION=7.35.0
CURL_NAME="curl-${CURL_VERSION}"
CURL_TARBALL="${CURL_NAME}.tar.gz"
CURL_URL="http://curl.haxx.se/download/${CURL_TARBALL}"

[ ! -d "${CURL_NAME}" ] && (
	[ ! -f "${CURL_TARBALL}" ] && try fetch "${CURL_URL}" "${CURL_TARBALL}"
	try tar xf "${CURL_TARBALL}"
)

############################### LIBIDN ########################################
debug "Update libidn"
LIBIDN_VERSION=1.28
LIBIDN_NAME=libidn-${LIBIDN_VERSION}
LIBIDN_TARBALL=${LIBIDN_NAME}.tar.gz
LIBIDN_URL=http://ftp.gnu.org/gnu/libidn/${LIBIDN_TARBALL}

[ ! -d "${LIBIDN_NAME}" ] && (
	[ ! -f "${LIBIDN_TARBALL}" ] && try fetch "${LIBIDN_URL}" "${LIBIDN_TARBALL}"
	try tar xf "${LIBIDN_TARBALL}"
)

############################## OpenSSL ########################################
debug "Update OpenSSL"
OPENSSL_VERSION=1.0.1f
OPENSSL_NAME=openssl-${OPENSSL_VERSION}
OPENSSL_TARBALL=${OPENSSL_NAME}.tar.gz
OPENSSL_URL=https://www.openssl.org/source/${OPENSSL_TARBALL}

[ ! -d "${OPENSSL_NAME}" ] && (
	[ ! -f "${OPENSSL_TARBALL}" ] && try fetch "${OPENSSL_URL}" "${OPENSSL_TARBALL}"
	try tar xf "${OPENSSL_TARBALL}"
)

################################ ZLIB #########################################
debug "Update zlib"
ZLIB_VERSION=1.2.8
ZLIB_NAME=zlib-${ZLIB_VERSION}
ZLIB_TARBALL=${ZLIB_NAME}.tar.gz
ZLIB_URL=http://zlib.net/${ZLIB_TARBALL}

[ ! -d "${ZLIB_NAME}" ] && (
	[ ! -f "${ZLIB_TARBALL}" ] && try fetch "${ZLIB_URL}" "${ZLIB_TARBALL}"
	try tar xf "${ZLIB_TARBALL}"
)

################################ BZIP2 ########################################
debug "Update bzip2"
BZIP2_VERSION=1.0.6
BZIP2_NAME=bzip2-${BZIP2_VERSION}
BZIP2_TARBALL=${BZIP2_NAME}.tar.gz
BZIP2_URL=http://www.bzip.org/${BZIP2_VERSION}/${BZIP2_TARBALL}

[ ! -d "${BZIP2_NAME}" ] && (
	[ ! -f "${BZIP2_TARBALL}" ] && try fetch "${BZIP2_URL}" "${BZIP2_TARBALL}"
	try tar xf "${BZIP2_TARBALL}"
)

############################## libRocket ######################################
debug "Update libRocket"

LIBROCKET_URL="https://github.com/hotgloupi/libRocket.git"
LIBROCKET_NAME="libRocket"
LIBROCKET_VERSION=master

[ ! -d "${LIBROCKET_NAME}" ] && (
	try git clone "${LIBROCKET_URL}" $LIBROCKET_NAME
)
( try cd "$LIBROCKET_NAME" && git pull origin $LIBROCKET_VERSION )


exit 0
