cmake_minimum_required(VERSION 3.0.0)

set(USING_MODULES "FALSE")

project(Compiler)

set(CMAKE_BUILD_TYPE "DEBUG")
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#set(CMAKE_CXX_FLAGS_RELEASE "-O3")
#set(CMAKE_CXX_FLAGS "-Ofast")
#set(CMAKE_CXX_FLAGS "-fmodules-ts")

set(CMAKE_CXX_FLAGS "-fpermissive")
#set(CMAKE_CXX_FLAGS "-Wwrite-strings")


LINK_DIRECTORIES(${CMAKE_SOURCE_DIR}/lib)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

link_directories("${OPENMP_LIBRARIES}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fopenmp")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")

set(COMMON_LIBS ${COMMON_LIBS} ${EXTRA_LIBS})

include_directories(glm)
include_directories(include)

include_directories(
	"../../Boost/*"
	"./Common/Platform"
	"./Common/MathUtils"
	"./Common/Include"
	"./Common/Logger"
	"./Common/FileUtils"
	"./Common/StringUtils"
	"./Common/DateTimeUtils"
	"./Common/DoublyLinkedList"
	"./Common/TerminalColorsUtils")

add_executable(
	Compiler
	./Common/Include/ExceptionsMacro.h
	./Common/Platform/DestinationPlatform.h
	./Common/Logger/Logger.h
	./Common/Logger/Logger.cpp
	./Common/MathUtils/Combinatorics.cpp
	./Common/MathUtils/Combinatorics.h
	./Common/FileUtils/FileUtils.h
	./Common/FileUtils/FileUtils.cpp
	./Common/StringUtils/StringUtils.h
	./Common/StringUtils/StringUtils.cpp
	./Common/DateTimeUtils/DateTimeUtils.h
	./Common/DateTimeUtils/DateTimeUtils.cpp
	./Common/DoublyLinkedList/DoublyLinkedList.h
	./Common/DoublyLinkedList/DoublyLinkedList.cpp
	./Common/TerminalColorsUtils/TerminalColorsUtils.h
	./Common/Compilation/ConditionalCompilationConstants.h
	Types.h
	Constants.h
	LexicalAnalysisTokensGenerator.h
	LexicalAnalysisTokensGenerator.cpp
	ParserToVirtualCodeGenerator.h
	ParserToVirtualCodeGenerator.cpp
	InlineFunctionsGenerator.h
	InlineFunctionsGenerator.cpp
	VirtualCodePrinter.cpp
	VirtualCodePrinter.h
	IntelAssemblerCodeGenerator.h
	IntelAssemblerCodeGenerator.cpp
	VirtualMachine.cpp
	VirtualMachine.h
	Compiler.cpp)

#if(USING_MODULES STREQUAL "TRUE")
#target_sources(Compiler
#		PUBLIC
#		FILE_SET all_my_modules TYPE CXX_MODULES FILES
#		Colors.cpp)
#endif()

target_link_libraries(${PROJECT_NAME} -static-libgcc -static-libstdc++ -lpthread)

set(CMAKE_THREAD_LIBS_INIT "-lpthread")
set(CMAKE_HAVE_THREADS_LIBRARY 1)
set(CMAKE_USE_WIN32_THREADS_INIT 0)
set(CMAKE_USE_PTHREADS_INIT 1)
set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(Threads REQUIRED)
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

include(CTest)
enable_testing()

set (GMOCK_DIR "/usr/src/googletest/googlemock")
add_subdirectory(${GMOCK_DIR} ${CMAKE_BINARY_DIR}/gmock)
include_directories("/usr/src/googletest/googlemock/include")

add_executable(
	CompilerRunTests
	../Common/Logger/Logger.h
	../Common/Logger/Logger.cpp
	../Common/MathUtils/ArcBall.h
	../Common/MathUtils/ArcBall.cpp
	../Common/FileUtils/FileUtils.h
	../Common/FileUtils/FileUtils.cpp
	../Common/StringUtils/StringUtils.h
	../Common/StringUtils/StringUtils.cpp
	../Common/DateTimeUtils/DateTimeUtils.h
	../Common/DateTimeUtils/DateTimeUtils.cpp
	../Common/DoublyLinkedList/DoublyLinkedList.h
	../Common/DoublyLinkedList/DoublyLinkedList.cpp
	tests/CompilerRunTests.cpp)

target_link_libraries(CompilerRunTests -static-libgcc -static-libstdc++ -lpthread gtest gmock)

add_test(CompilerRunTests CompilerRunTests)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)