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
	echo "$*" 2>&1
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

#[ ! -d "$SCRIPT_DIR"/boost ] && fatal "You should update submodules first"
#
#### Boost ---------------------------------------------------------------------
#debug "Updating Boost"
#BOOST_COMPONENTS="
#any
#array
#asio
#atomic
#algorithm
#bind
#chrono
#config
#concept_check
#container
#conversion
#date_time
#detail
#exception
#filesystem
#foreach
#format
#function
#functional
#graph
#integer
#io
#iterator
#math
#mpl
#move
#multi_index
#numeric
#optional
#parameter
#predef
#preprocessor
#property_map
#python
#range
#ratio
#signals2
#serialization
#smart_ptr
#static_assert
#system
#thread
#timer
#tuple
#typeof
#type_traits
#units
#unordered
#utility
#variant
#"
#
#cd "$SCRIPT_DIR"/boost
#for component in $BOOST_COMPONENTS
#do
#	git submodule update --init --recursive "libs/$component" || \
#		fatal "Cannot update boost component $component"
#done
#cd "$SCRIPT_DIR"

debug "Updating boost"
if [ ! -d boost ]
then
	svn co http://svn.boost.org/svn/boost/trunk boost
else
	( cd boost && svn update )
fi

### SDL -----------------------------------------------------------------------
debug "Updating SDL"
[ ! -d SDL ] && hg clone http://hg.libsdl.org/SDL
[ ! -d SDL_image ] && hg clone http://hg.libsdl.org/SDL_image
