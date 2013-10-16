QT += opengl
TEMPLATE = app

# Lets assume that everyone uses (the awesome) pkg-config.
CONFIG += link_pkgconfig
PKGCONFIG += Magick++ boost

DESTDIR = $$PWD/../bin

CONFIG(release, debug|release) {
	TARGET = ffd
} else {
	TARGET = ffd_d
}

SOURCES = $$PWD/../src/*.cpp $$PWD/../src/cgl/*.cpp
HEADERS = $$PWD/../include/*.hpp $$PWD/../include/cgl/*.hpp
INCLUDEPATH += $$PWD/../include
RESOURCES += resources.qrc

LIBS += -lboost_filesystem-mt \
	-lboost_system-mt

macx {
	ICON = Icon.icns
	QMAKE_INFO_PLIST = Info.plist
}
