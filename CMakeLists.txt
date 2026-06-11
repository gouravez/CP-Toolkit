cmake_minimum_required(VERSION 3.20)
project(CPT VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/Zc:__cplusplus)
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS
    Core
    Widgets
    Sql
)

set(SOURCES
    src/main.cpp

    src/ui/editor/cppsyntaxhighlighter.cpp
    src/ui/mainwindow.cpp
    src/ui/sidebar.cpp
    src/ui/snippets/snippetlistview.cpp
    src/ui/snippets/snippeteditorview.cpp
    src/ui/solutions/solutionlistview.cpp
    src/ui/solutions/solutioneditorview.cpp
    src/ui/search/searchview.cpp
    src/ui/stresstest/stresstestview.cpp
    src/ui/workspace/workspaceview.cpp
    src/ui/templates/templateview.cpp

    src/core/searchindex.cpp
    src/core/stresstestrunner.cpp
    src/core/workspacegenerator.cpp
    src/core/templategenerator.cpp

    src/dal/sqlitedatabase.cpp
    src/dal/snippetrepository.cpp
    src/dal/solutionrepository.cpp
)

set(HEADERS
    src/ui/editor/cppsyntaxhighlighter.h
    src/ui/mainwindow.h
    src/ui/sidebar.h
    src/ui/snippets/snippetlistview.h
    src/ui/snippets/snippeteditorview.h
    src/ui/solutions/solutionlistview.h
    src/ui/solutions/solutioneditorview.h
    src/ui/search/searchview.h
    src/ui/stresstest/stresstestview.h
    src/ui/workspace/workspaceview.h
    src/ui/templates/templateview.h

    src/core/searchindex.h
    src/core/stresstestrunner.h
    src/core/workspacegenerator.h
    src/core/templategenerator.h

    src/dal/idatabase.h
    src/dal/sqlitedatabase.h
    src/dal/snippetrepository.h
    src/dal/solutionrepository.h
)

set(RESOURCES
    resources/app.qrc
)

qt_add_executable(CPT
    ${SOURCES}
    ${HEADERS}
    ${RESOURCES}
)

target_include_directories(CPT PRIVATE
    src
    src/ui
    src/core
    src/dal
)

target_link_libraries(CPT PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Sql
)

if(WIN32)
    set_target_properties(CPT PROPERTIES
        WIN32_EXECUTABLE $<CONFIG:Release>
    )
endif()

set_target_properties(CPT PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)