cmake_minimum_required(VERSION 3.19)
project(wht_backend LANGUAGES CXX)

# 设置C++17标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找Qt6组件
find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Network SerialPort)

# QXlsx需要Qt的私有API，尝试查找GuiPrivate组件
# 对于Qt6.9.1，尝试直接查找GuiPrivate
find_package(Qt6GuiPrivate QUIET)
if(NOT Qt6GuiPrivate_FOUND)
    # 如果找不到GuiPrivate包，尝试通过Gui组件查找
    find_package(Qt6 REQUIRED COMPONENTS Gui)
endif()

qt_standard_project_setup()

# 启用AUTOMOC和AUTOUIC以确保Qt元对象系统和UI文件被正确处理
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

# 收集protocol目录下的头文件
file(GLOB PROTOCOL_HEADERS "protocol/*.h")

# 收集QXlsx源文件和头文件
file(GLOB QXLSX_SOURCES "QXlsx/source/*.cpp")
file(GLOB QXLSX_HEADERS "QXlsx/header/*.h")

# 创建可执行文件
qt_add_executable(wht_backend
    WIN32 MACOSX_BUNDLE
    # 主源文件
    main.cpp
    mainwindow.cpp
    mainwindow.h
    ${CMAKE_CURRENT_SOURCE_DIR}/mainwindow.ui
    
    # protocol源文件
    protocol/protocol.cpp
    protocol/slave2backend.cpp
    protocol/packetbase.cpp
    
    # protocol头文件
    ${PROTOCOL_HEADERS}
    
    # 其他源文件
    newdevice.cpp
    newdevice.h
    ${CMAKE_CURRENT_SOURCE_DIR}/newdevice.ui
    slavedevice.cpp
    slavedevice.h
    slavepage.cpp
    slavepage.h
    udpmanager.cpp
    udpmanager.h
    logger.cpp
    logger.h
    
    # QXlsx源文件和头文件（头文件需要MOC处理）
    ${QXLSX_SOURCES}
    ${QXLSX_HEADERS}
    
    # Windows资源文件（包含应用程序图标）
    app.rc
)

# 添加QXlsx头文件路径
target_include_directories(wht_backend PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}  # 根目录，使所有文件都能找到logger.h
    ${CMAKE_CURRENT_SOURCE_DIR}/QXlsx
    ${CMAKE_CURRENT_SOURCE_DIR}/QXlsx/header
    ${CMAKE_CURRENT_SOURCE_DIR}/protocol
)

# QXlsx需要的编译定义（只应用于QXlsx源文件）
# 使用源文件属性来只对QXlsx文件应用这些定义
set(QXLSX_COMPILE_DEFINITIONS
    QT_NO_KEYWORDS
    QT_NO_CAST_TO_ASCII
    QT_NO_CAST_FROM_ASCII
    QT_NO_URL_CAST_FROM_STRING
    QT_NO_CAST_FROM_BYTEARRAY
    QT_USE_QSTRINGBUILDER
    QT_NO_SIGNALS_SLOTS_KEYWORDS
    QT_USE_FAST_OPERATOR_PLUS
    QT_DISABLE_DEPRECATED_BEFORE=0x060600
)

# 为每个QXlsx源文件设置编译定义
foreach(QXLSX_SOURCE ${QXLSX_SOURCES})
    set_source_files_properties(${QXLSX_SOURCE} PROPERTIES
        COMPILE_DEFINITIONS "${QXLSX_COMPILE_DEFINITIONS}"
    )
endforeach()

# 链接Qt6库
target_link_libraries(wht_backend
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt::Network
        Qt::SerialPort
)

# 链接GuiPrivate（如果可用）
if(TARGET Qt::GuiPrivate)
    target_link_libraries(wht_backend PRIVATE Qt::GuiPrivate)
elseif(TARGET Qt6::GuiPrivate)
    target_link_libraries(wht_backend PRIVATE Qt6::GuiPrivate)
else()
    # 如果GuiPrivate不可用，尝试链接Gui并手动添加私有头文件路径
    target_link_libraries(wht_backend PRIVATE Qt::Gui)
    # 获取Qt安装路径
    get_target_property(QT_QMAKE_EXECUTABLE Qt6::qmake IMPORTED_LOCATION)
    if(QT_QMAKE_EXECUTABLE)
        execute_process(
            COMMAND ${QT_QMAKE_EXECUTABLE} -query QT_INSTALL_HEADERS
            OUTPUT_VARIABLE QT_INSTALL_HEADERS
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        if(QT_INSTALL_HEADERS)
            # Qt6私有头文件通常在 include/QtGui/6.x.x/QtGui/private
            target_include_directories(wht_backend PRIVATE
                "${QT_INSTALL_HEADERS}/QtGui/${Qt6Gui_VERSION}/QtGui/private"
                "${QT_INSTALL_HEADERS}/QtGui/private"
            )
        endif()
    endif()
endif()

include(GNUInstallDirs)

install(TARGETS wht_backend
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET wht_backend
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
