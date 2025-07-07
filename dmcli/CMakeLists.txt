# SPDX-FileCopyrightText: 2017-2018 Christian Sailer
# SPDX-FileCopyrightText: 2017-2024 Petros Koutsolampros
#
# SPDX-License-Identifier: GPL-3.0-or-later

set(target_NAME dmcli)

set(dminterface ${target_NAME})

set(target_HDRS
    consts.hpp
    agentparser.hpp
    exportparser.hpp
    interfaceversion.hpp
    modeparserregistry.hpp
    printcommunicator.hpp
    segmentshortestpathparser.hpp
    visprepparser.hpp
    axialparser.hpp
    imodeparserfactory.hpp
    isovistparser.hpp
    parsingutils.hpp
    radiusconverter.hpp
    simpletimer.hpp
    commandlineparser.hpp
    imodeparser.hpp
    linkparser.hpp
    performancesink.hpp
    runmethods.hpp
    stepdepthparser.hpp
    exceptions.hpp
    importparser.hpp
    mapconvertparser.hpp
    performancewriter.hpp
    segmentparser.hpp
    vgaparser.hpp
)
set(target_SRCS
    main.cpp
    interfaceversion.hpp
    printcommunicator.cpp
    commandlineparser.cpp
    runmethods.cpp
    radiusconverter.cpp
    vgaparser.cpp
    linkparser.cpp
    performancewriter.cpp
    modeparserregistry.cpp
    visprepparser.cpp
    axialparser.cpp
    parsingutils.cpp
    agentparser.cpp
    isovistparser.cpp
    exportparser.cpp
    importparser.cpp
    stepdepthparser.cpp
    segmentparser.cpp
    mapconvertparser.cpp
    segmentshortestpathparser.cpp)

set(LINK_LIBS salalib)

set(modules_cli "" CACHE INTERNAL "modules_cli" FORCE)
set(MODULES_GUI FALSE)
set(MODULES_CLI TRUE)
set(MODULES_CLI_TEST FALSE)
set(MODULES_CORE FALSE)
set(MODULES_CORE_TEST FALSE)
add_subdirectory(../modules modules)

# The headers are added to the target primarily so that they appear as part
# of this particular project to IDEs that open it
add_executable(${target_NAME} ${target_HDRS} ${target_SRCS})

target_compile_options(${target_NAME} PRIVATE ${COMPILE_WARNINGS})

target_link_libraries(${target_NAME} ${LINK_LIBS} ${modules_cli} ${modules_core})

add_subdirectory(dminterface)
