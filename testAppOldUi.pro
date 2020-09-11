QT       += core gui sql androidextras network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use                 qt +=location
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

###############################
# Change commented dirs for PititLib and Logger Lib for api 7 and 8!!!!!!!!!!!!!!!!!!!! No curl for v8 api!!!!
###############################

#DEFINES += BOOST_ALL_NO_LIB
#Boost_USE_STATIC_LIBS = ON
#BOOSTHOME =  C:/Android/projects/FisGo_Qt/testAppOldUi/Boost_Prebuilt/boost_1_53_0

#INCLUDEPATH += ../
message("===========================================")
#INCLUDEPATH += C:/Android/projects/FisGo_Qt/testAppOldUi/Boost_Prebuilt/boost_1_53_0/include
#DEPENDPATH  += C:/Android/projects/FisGo_Qt/testAppOldUi/Boost_Prebuilt/boost_1_53_0/armeabi-v7a/lib
#INCLUDEPATH  += C:/Android/projects/FisGo_Qt/testAppOldUi/Boost_Prebuilt/boost_1_53_0/include/boost
#INCLUDEPATH  += C:/Android/projects/FisGo_Qt/testAppOldUi/Boost_Prebuilt/boost_1_53_0/include
#DEPENDPATH   += C:/Android/projects/FisGo_Qt/testAppOldUi/Boost_Prebuilt/boost_1_53_0/include/boost

#INCLUDEPATH  += $$PWD/libs/armeabi-v7a/qFisGoPirit \
#                $$PWD/libs/armeabi-v7a/qFisGoLogDb

message("INCLUDEPATH " $$INCLUDEPATH)
message("===========================================")

SOURCES += \
    kf_files/EncodeConvertor.cpp \
    kf_files/FisGo_BIOS.cpp \
    kf_files/Md5Sum.cpp \
    kf_files/MenuTree.cpp \
    kf_files/config.cpp \
    kf_files/convert.cpp \
    kf_files/filesfg.cpp \
    kf_files/https_exchange.cpp \
    kf_files/json.cpp \
    kf_files/requisites.cpp \
    loggerdaemon.cpp \
    main.cpp \
    mainwindow.cpp
message("SOURCES " $$SOURCES)
message("===========================================")


LIBS += -LC:\Android\projects\FisGo_Qt\testAppOldUi\libs\curl29sdk\curl-armeabi-v7a\lib -lcurl \
        -LC:\Android\projects\FisGo_Qt\testAppOldUi\libs\curl29sdk\nghttp2-armeabi-v7a\lib -lnghttp2 \
        -LC:\Android\projects\FisGo_Qt\testAppOldUi\libs\curl29sdk\curl-armeabi-v7a\lib -lcrypto.1.1 \
        -LC:\Android\projects\FisGo_Qt\testAppOldUi\libs\curl29sdk\openssl-armeabi-v7a\lib -lssl.1.1 \
        -LC:\Android\projects\FisGo_Qt\testAppOldUi\libs\armeabi-v7a\qFisGoPirit -lqFisGoPirit \
        -LC:\Android\projects\FisGo_Qt\testAppOldUi\libs\armeabi-v7a\qFisGoLogDb -lQFisGoLogDb

message("LIBS " $$LIBS)
message("===========================================")

HEADERS +=  kf_files/EncodeConvertor.h \
    kf_files/FisGo_BIOS.h \
    kf_files/Md5Sum.h \
    kf_files/MenuTree.h \
    kf_files/config.h \
    kf_files/convert.h \
    kf_files/dynamic_mask.h \
    kf_files/filesfg.h \
    kf_files/https_exchange.h \
    kf_files/json.h \
    kf_files/jsonParser.h \
    kf_files/license.h \
    kf_files/requisites.h \
    libs/armeabi-v7a/qFisGoLogDb/logdb.h \
    libs/armeabi-v7a/qFisGoLogDb/logdb_c_cpp.h \
    libs/armeabi-v7a/qFisGoLogDb/sqlite3.h \
    libs/armeabi-v7a/qFisGoPirit/PiritLib.h \
    libs/armeabi-v7a/qFisGoPirit/biosCounters.h \
    libs/armeabi-v7a/qFisGoPirit/pirit_low_func.h \
    libs/armeabi-v7a/qFisGoPirit/pirit_structs.h \
    libs/curl29sdk/curl-armeabi-v7a/include/curl/curl.h \
    libs/curl29sdk/curl-armeabi-v7a/include/curl/curlver.h \
    libs/curl29sdk/curl-armeabi-v7a/include/curl/easy.h \
    libs/curl29sdk/curl-armeabi-v7a/include/curl/mprintf.h \
    libs/curl29sdk/curl-armeabi-v7a/include/curl/multi.h \
    libs/curl29sdk/curl-armeabi-v7a/include/curl/stdcheaders.h \
    libs/curl29sdk/curl-armeabi-v7a/include/curl/system.h \
    libs/curl29sdk/curl-armeabi-v7a/include/curl/typecheck-gcc.h \
    libs/curl29sdk/curl-armeabi-v7a/include/curl/urlapi.h \
    loggerdaemon.h \
    mainwindow.h
message("HEADERS " $$HEADERS)

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

message("===========================================")

contains(ANDROID_TARGET_ARCH,armeabi-v7a) {
    ANDROID_EXTRA_LIBS = \
        C:/Android/projects/FisGo_Qt/testAppOldUi/libs/armeabi-v7a/qFisGoPirit/libqFisGoPirit.so \
        C:/Android/projects/FisGo_Qt/testAppOldUi/libs/armeabi-v7a/qFisGoLogDb/libQFisGoLogDb.so \
        C:/Android/projects/FisGo_Qt/testAppOldUi/libs/curl29sdk/curl-armeabi-v7a/lib/libcurl.so \
        C:/Android/projects/FisGo_Qt/testAppOldUi/libs/curl29sdk/nghttp2-armeabi-v7a/lib/libnghttp2.so \
        C:/Android/projects/FisGo_Qt/testAppOldUi/libs/curl29sdk/openssl-armeabi-v7a/lib/libcrypto.1.1.so  \
        C:/Android/projects/FisGo_Qt/testAppOldUi/libs/curl29sdk/openssl-armeabi-v7a/lib/libssl.1.1.so
}
