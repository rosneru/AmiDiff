# README #

A graphical file compare / diff viewer for AmigaOS3.0+.

The diff engine uses some code from Matthias Hertel. See the file 
LICENSE-3RD-PARTY for more information.

## Development environment ##

After cloning the needed submodules should be initialized:

    git submodule init
    git submodule update
    
ADiffView is developed using these tools:

* Amiga with OS 3.1.4
* NDK for AmigaOS 3.1
* StormC 4 on Amiga
* VisualStudio Code using bebbos gcc 6.5 toolchain on Windows 10 / 
Debian WSL

The project file src/ADiffView.¶ can be loaded with StormC4. 

In StormC4/gcc mode it should compile without errors and just a few 
warnings..