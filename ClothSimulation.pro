QT       += core gui
QT       += opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ClothSimulation
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11


SOURCES += \
    src/ClothModel.cpp \
    src/RenderWidget.cpp \
    src/MainWindow.cpp \
    src/Vec3.cpp \
    src/main.cpp \

HEADERS += \
    src/ClothModel.h \
    src/RenderWidget.h \
    src/MainWindow.h \ \
    src/Vec3.h \
    src/tiny_obj_loader.h \
    src/stb_image.h \

win32 {
    LIBS += -lopengl32
    LIBS += -lglu32
}

INCLUDEPATH += /usr/local/include/opencv4

LIBS += -F"/Library/Frameworks/" -framework GLUT

LIBS += -L/usr/local/lib -lopencv_imgproc -lopencv_highgui -lopencv_core -lopencv_video -lopencv_videoio -lopencv_imgcodecs -lopencv_img_hash

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
