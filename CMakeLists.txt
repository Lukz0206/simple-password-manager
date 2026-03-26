cmake_minimum_required(VERSION 4.0)
project(simple_password_manager)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)

find_library(CRYPTOPP_LIB cryptopp)

find_package(SQLite3 REQUIRED)


add_executable(${PROJECT_NAME} 
        main.cpp 
        src/model/loginhandler.cpp
        src/model/dbhandler.cpp
        src/model/passwordentry.cpp
        src/model/user.cpp
        src/errors/dbexception.cpp
        src/controller/appmanager.cpp
        src/gui/loginmenu.cpp
        include/gui/loginmenu.hpp
        include/controller/appmanager.hpp
        include/gui/usermenu.hpp
        include/gui/insertdialog.hpp
        src/gui/insertdialog.cpp
        src/gui/usermenu.cpp
        src/model/appmodel.cpp
        include/gui/confirmdialog.hpp
        src/gui/confirmdialog.cpp
        )

target_link_libraries(${PROJECT_NAME}
        Qt::Core
        Qt::Gui
        Qt::Widgets
        ${CRYPTOPP_LIB}
        ${SQLite3_LIBRARIES}
)

target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${SQLite3_INCLUDE_DIRS}
    )

