QT += opengl
TEMPLATE = app
QT_CONFIG -= no-pkg-config
CONFIG += c++11

# Lets assume that everyone uses (the awesome) pkg-config.
CONFIG += link_pkgconfig
PKGCONFIG += Magick++ # save animation
PKGCONFIG += boost # scoped_array, scoped_ptr and shared_ptr

DESTDIR = $$PWD/../bin

CONFIG(release, debug|release) {
	TARGET = Morphing
} else {
	TARGET = Morphing_d
}

SOURCES = $$PWD/../src/*.cpp
HEADERS = $$PWD/../include/*.hpp $$PWD/../include/cgl/*.hpp
INCLUDEPATH += $$PWD/../include
RESOURCES += resources.qrc

macx {
	ICON = Icon.icns
	QMAKE_INFO_PLIST = Info.plist
# Same as the macports target or else it won't link "x86_64 not found..."
	QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.11
}
