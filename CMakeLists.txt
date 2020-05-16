cmake_minimum_required(VERSION 3.5)

project(YACFrontendCMake LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_BUILD_TYPE Debug)

include_directories(include)

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check http://doc.qt.io/qt-5/deployment-android.html for more information.
# They need to be set before the find_package(Qt5 ...) call.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()

find_package(Qt5 COMPONENTS Core Quick LinguistTools REQUIRED)
set(TS_FILES yac_uk_UA.ts yac_en_US.ts)

if(ANDROID)
    add_library(YACFrontendCMake SHARED
      main.cpp
      qml.qrc
      ${TS_FILES}
    )
else()
    add_executable(YACFrontendCMake
        GuiInteractor.cpp
        GuiInteractor.hpp
      ArchivedFileModel.hpp
      ArchivedFileModel.cpp
      main.cpp
      qml.qrc
		include/YAC/yac.hpp
		include/YAC/compressor.hpp
		include/YAC/extractor.hpp
		include/YAC/byte.hpp
		include/YAC/byte_source.hpp
		include/YAC/istream_byte_source.hpp
		src/main.cpp
		src/yac.cpp
		src/compressor.cpp
		src/extractor.cpp
		src/istream_byte_source.cpp
      ${TS_FILES}
    )
endif()

set_target_properties(YACFrontendCMake PROPERTIES CXX_EXTENSIONS OFF)
if(MSVC)
	target_compile_definitions(YACFrontendCMake PUBLIC -DNOMINMAX)
	if(NOT(MSVC_VERSION LESS 1910))
		message("Detected not-so-old msvc, enabling permissive mode and a sane __cplusplus marco")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /permissive- /Zc:__cplusplus")
	endif()
endif()

target_compile_definitions(YACFrontendCMake
  PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_link_libraries(YACFrontendCMake
  PRIVATE Qt5::Core Qt5::Quick)

qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
