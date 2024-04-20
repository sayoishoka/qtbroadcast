QT       += core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
UI/user/add/adduser.cpp \
UI/user/rights_management.cpp \
UI/user/role_management.cpp \
broadcastmain.cpp \
UI/databaseconfig.cpp \
UI/device/addupdatedev.cpp \
UI/device/billing_manager.cpp \
UI/device/billingdetail.cpp \
UI/device/device_manager.cpp \
UI/device/deviceprice_manager.cpp \
UI/device/devicerent_manager.cpp \
UI/device/group_manager.cpp \
UI/menu.cpp \
UI/user/user_manage.cpp \
connect/http/httpserver.cpp \
connect/http/httpsession.cpp \
connect/tcp_ip/tcplink.cpp \
function/dbinteraction/db_dev.cpp \
    function/dbinteraction/db_device_rent.cpp \
function/dbinteraction/db_file.cpp \
function/dbinteraction/db_group.cpp \
function/dbinteraction/db_pms_mg.cpp \
function/dbinteraction/db_program.cpp \
    function/dbinteraction/db_program_2.cpp \
    function/dbinteraction/dbselectupdate.cpp \
function/device.cpp \
function/group.cpp \
function/userlogin.cpp \
main.cpp

HEADERS += \
UI/MySlider.h \
UI/Mydata.h \
    UI/device/mytreewidget.h \
    UI/user/add/adduser.h \
    UI/user/rights_management.h \
    UI/user/role_management.h \
    UI/user/userdata.h \
broadcastmain.h \
UI/databaseconfig.h \
UI/device/addupdatedev.h \
UI/device/billing_manager.h \
UI/device/billingdetail.h \
UI/device/device_manager.h \
UI/device/deviceprice_manager.h \
UI/device/devicerent_manager.h \
UI/device/group_manager.h \
UI/menu.h \
UI/user/user_manage.h \
connect/dispatcher.h \
connect/http/httpserver.h \
connect/http/httpsession.h \
connect/tcp_ip/tcplink.h \
function/daotype.h \
function/dbinteraction.h \
    function/dbinteraction/db_device_rent.h \
    function/dbinteraction/db_program_2.h \
    function/dbinteraction/dbselectupdate.h \
    function/device.h \
function/group.h \
function/userlogin.h \
lib/NAudioServerLib.h

FORMS += \
    UI/user/add/adduser.ui \
    UI/user/rights_management.ui \
    UI/user/role_management.ui \
broadcastmain.ui \
UI/databaseconfig.ui \
UI/device/addupdatedev.ui \
UI/device/billing_manager.ui \
UI/device/billingdetail.ui \
UI/device/device_manager.ui \
UI/device/deviceprice_manager.ui \
UI/device/devicerent_manager.ui \
UI/device/group_manager.ui \
UI/menu.ui \
UI/user/user_manage.ui

RESOURCES += \
main.qrc

LIBS +="D:\qtproject1\build-BroadcastServer-Desktop_Qt_5_9_9_MinGW_32bit-Debug\debug\NAudioServerDll.dll"

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

