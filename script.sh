#!/bin/sh

PrintHelp()
{
echo Enter 'build.sh action' where action is one of the following:
echo 
echo clean                Remove all binaries and generated files
echo codelite           Generate CodeLite project files
echo gmake             Generate GNU makefiles for POSIX, MinGW, and Cygwin
echo gmake2           Generate GNU makefiles for POSIX, MinGW, and Cygwin
echo vs2005            Generate Visual Studio 2005 project files
echo vs2008            Generate Visual Studio 2008 project files
echo vs2010            Generate Visual Studio 2010 project files
echo vs2012            Generate Visual Studio 2012 project files
echo vs2013            Generate Visual Studio 2013 project files
echo vs2015            Generate Visual Studio 2015 project files
echo vs2017            Generate Visual Studio 2017 project files
echo vs2019            Generate Visual Studio 2019 project files
echo xcode4            Generate Apple Xcode 4 project files
}

Compile()
{
    if [ "$1" = "mac" ]; then
        echo using mac
        # Create makefile gmake
        premake5 xcode4
    elif [ "$1" = "linux" ]; then
        echo using linux
        # Create makefile gmake
        premake5 gmake
        make
    fi
}

Run()
{
    Compile $1
    if [ "$1" = "mac" ]; then
        echo using mac
        # Create makefile gmake
        premake5 xcode4
    elif [ "$1" = "linux" ]; then
        # Run
        cd bin
        # Create a variable that will get the folder that has the word "...linux..." in its name
        var=$(find -name *linux*)
        ./$var/ProjectFolder/ProjectFolder
        cd ..
    fi
}

# If there are no arguments or if the argument is equivalent to "help"
if [ "$1" == "" ] || [ "$1" == "help" ]; then
PrintHelp
elif [ "$1" == "compile" ]; then
Compile $2
elif [ "$1" == "run" ]; then
Run $2
else
    if [ "$2" = "mac" ]; then
        echo using mac
        ./vendor/premake5mac $1
        exit
    fi
    if [ "$2" = "linux" ]; then
        echo using linux
        premake5 $1
        exit
    fi

    if [ "$(uname)" = "Darwin" ]; then
        echo using mac
        ./vendor/premake5mac $1
    elif [ "$(expr substr "$(uname -s)" 1 5)" = "Linux" ]; then
        echo using linux
        ./vendor/premake5lin $1
    fi
fi