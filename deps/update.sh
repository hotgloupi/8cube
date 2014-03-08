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

#### Boost ---------------------------------------------------------------------

debug "Updating boost"
if [ ! -d boost ]
then
	svn co http://svn.boost.org/svn/boost/tags/release/Boost_1_55_0 boost
else
	( cd boost && svn update )
fi

### SDL -----------------------------------------------------------------------
debug "Updating SDL"
[ ! -d SDL ] && hg clone http://hg.libsdl.org/SDL
[ ! -d SDL_image ] && hg clone http://hg.libsdl.org/SDL_image


### Bullet physics ------------------------------------------------------------
debug "Update BulletPhysics"
BULLET_VERSION="2.82-r2704"
BULLET_NAME="bullet-${BULLET_VERSION}"
BULLET_TARBALL="${BULLET_NAME}.tgz"
BULLET_URL="https://bullet.googlecode.com/files/${BULLET_TARBALL}"

[ ! -d  "${BULLET_NAME}" ] && (
	[ ! -f "${BULLET_TARBALL}" ] && wget "${BULLET_URL}" -O "${BULLET_TARBALL}"
	tar xf ${BULLET_TARBALL}
)

### CURL
debug "Update curl"
CURL_VERSION=7.35.0
CURL_NAME="curl-${CURL_VERSION}"
CURL_TARBALL="${CURL_NAME}.tar.gz"
CURL_URL="http://curl.haxx.se/download/${CURL_TARBALL}"

[ ! -d "${CURL_NAME}" ] && (
	[ ! -f "${CURL_TARBALL}" ] && wget "${CURL_URL}" -O "${CURL_TARBALL}"
	tar xf "${CURL_TARBALL}"
)

