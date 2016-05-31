#!/bin/bash

GXX_VERSION=4.8
BOOST_VERSION=1.54

if [ "x$SIMULATOR_HOME" == "x" ] ; then
    echo "[INSTALL DEPENDENCIES] Environmental variables not set. Please run shenv" ;
    exit ;
fi;

#Functions:
#----------

# Checking package dependencies
function package_check() {

DEPENDENCIES="build-essential libglib2.0-dev\
 libboost$BOOST_VERSION-dev libboost-thread$BOOST_VERSION-dev\
 libboost-system$BOOST_VERSION-dev\
 libboost-locale$BOOST_VERSION-dev libboost-regex$BOOST_VERSION-dev \
 libboost-filesystem$BOOST_VERSION-dev libboost-random$BOOST_VERSION-dev\
 python-software-properties libssl-dev cmake g++-$GXX_VERSION"
 
    deps_ok=YES
    # Dependency check!!
    echo "Checking for dependencies" ;
    INSTALLPKG=
    for dep in $DEPENDENCIES ; do
        PKG_OK=`dpkg-query -W --showformat='${Status}\n' $dep | grep "install ok installed"`
        if [ "$PKG_OK" == "" ]; then
            echo " -- Adding $dep to installation list";
            #sudo apt-get --force-yes --yes install $dep ;
            INSTALLPKG="$INSTALLPKG $dep"
        else 
            echo " -- Package $dep already installed... Skipping";
        fi;
    done;
    if [ "x$INSTALLPKG" != "x" ] ; then
        echo " -- Installing missing packages... Will need sudo privileges.";
        sudo apt-get install $INSTALLPKG ;
    fi;
}


# MAIN:
#-------

TEMPDIR=$SIMULATOR_HOME/common/libs/
mkdir -p $SIMULATOR_HOME/common/libs

echo "-----------------------------------------";
echo " Checking whether needed system packages are installed ";
echo "-----------------------------------------";
package_check ;
echo "Done";

echo "Now listing library dependencies..." ;
echo "1)  gflags: Google Flag processing for C++" ;
echo "2)  google-log: Google logging library for C++" ;
echo "3)  casablanca: C++ REST SDK";
echo "4)  tclap: Templatized C++ Command Line Parser";
echo "-----------------------------------------";

echo "Downloading and installing library dependencies...";

echo "-----------------------------------------";
echo "1)  gflags: Google Flag processing for C++" ;
echo "-----------------------------------------";
if [ -f /usr/local/lib/libgflags.so ] || [ -f $SIMULATOR_HOME/build/libs/libgflags.so ] ; then
    echo " -- Already installed... Skipping";
else
    echo "INFO: Installing gflags...";
    echo "INFO: glags is a submodule, no need to download"; 
    echo "INFO: Pulling module changes..."; 
    cd $SIMULATOR_HOME/../../
    git submodule init
    git submodule update
    cd $TEMPDIR/gflags/
    echo "INFO: Installing...";
    ./configure && make 
    echo "INFO: gflags: I'll need sudo for make install.." ;
    sudo make install
fi;

echo "-----------------------------------------";
echo "2)  google-log: Google logging library for C++" ;
echo "-----------------------------------------";
GLOGDIR=$TEMPDIR/google-glog
if [ -f $GLOGDIR/*/libglog.la ] || [ -f $SIMULATOR_HOME/build/libs/libglog.la ] ; then 
    echo " -- Already installed... Skipping";
else 
    mkdir -p $GLOGDIR
    cd $GLOGDIR 
    echo "INFO: Downloading and decompressing...";
    wget http://google-glog.googlecode.com/files/glog-0.3.3.tar.gz
    tar xvzf glog-0.3.3.tar.gz
    cd glog-0.3.3/
    
    echo "INFO: Installing...";
    ./configure && make 
    echo "INFO: google-log: I'll need sudo for make install.." ;
    sudo make install
fi;

echo "-----------------------------------------";
echo "3)  casablanca: C++ REST SDK" ;
echo "-----------------------------------------";
CASABLANCA=$TEMPDIR/casablanca

CASABLANCA_DEPS="libboost_locale-mt.so libboost_system-mt.so libboost_thread-mt.so\
 libboost_regex-mt.so libboost_filesystem-mt.so"
LIBPATH="/usr/lib"
arch=`dpkg --print-architecture`
if [ "$arch" == "amd64" ]; then
   archfolder="x86_64" 
else
    archfolder="i386" 
fi;
LIBPATH_ORIG="/usr/lib/$archfolder-linux-gnu"
    
for i in $CASABLANCA_DEPS ; do
    if ls $LIBPATH | grep "$i" > /dev/null ; then
        echo "Simlink to $i found. OK";
    else
        echo "Missing simlink to $i. Creating it...";
        ifix=`echo "$i" | sed -e 's#-mt##g'`
        sudo ln -s $LIBPATH_ORIG/$ifix $LIBPATH/$i ;
        echo "... OK";
    fi;
done;

if [ -f $CASABLANCA/Release/build.release/Binaries/libcasablanca.so ] || [ -f $SIMULATOR_HOME/build/libs/libcasablanca.so ]; then
    echo " -- Already installed... Skipping";
else
    mkdir -p $CASABLANCA
    cd $CASABLANCA
    echo "INFO: Downloading git repository...";
    #echo "DISCLAIMER: If casablanca fails because it does not find libboost-*-mt.so libraries, just make\
    #symlinks to them. The suffix -mt has been removed from newer boost libraries"
    
    git clone https://git01.codeplex.com/casablanca .
    git checkout tags/v2.0.1
    cd $CASABLANCA/Release && mkdir build.release && cd build.release
    echo "INFO: Installing...";
    cmake .. -DCMAKE_BUILD_TYPE=Release && make
    echo "Testing installation..." ;
    cd $CASABLANCA/Release/build.release/Binaries && ./run_tests.sh
fi;

echo "-----------------------------------------";
echo "4)  tclap: Templatized C++ Command Line Parser" ;
echo "-----------------------------------------";
TCLAP=$TEMPDIR/tclap/
cd $TCLAP ;
wget http://sourceforge.net/projects/tclap/files/latest/download ;
tar xvzf download;
cd tclap-1.2.1 ;
./configure && make ;
echo "Installing tclap, will need sudo passwd";
sudo make install;

echo "";
echo "-----------------------------------------";
echo " IMPORTANT:" ;
echo "-----------------------------------------";
echo " -- Remember to create a new file config.json and edit it according\
to your system needs";
echo "";

