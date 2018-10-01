#QT += sql xml network
TEMPLATE = app
CONFIG += qt debug_and_release

mac {
  CONFIG -= app_bundle
}

INCLUDEPATH +=  oscottlibs

# don't embed the manifest for now (doesn't work :( )
#CONFIG -= embed_manifest_exe 

FORMS =

HEADERS = diff_match_patch.h diff_match_patch_test.h \
    oscottlibs/LinkedList.h \
    oscottlibs/LinkedListNode.h \
    oscottlibs/SimpleString.h

SOURCES = diff_match_patch.cpp diff_match_patch_test.cpp \
    oscottlibs/LinkedList.cpp \
    oscottlibs/LinkedListNode.cpp \
    oscottlibs/SimpleString.cpp

RESOURCES = 

