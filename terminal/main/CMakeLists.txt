idf_component_register(
                SRCS  
                    "app_screen.cpp" 
                    "app_main.cpp"
                    "app_lcd.cpp"
                INCLUDE_DIRS
                    include
                    opencv
                )

add_prebuilt_library(opencv_imgcodecs "opencv/libopencv_imgcodecs.a")
add_prebuilt_library(libpng "opencv/3rdparty/liblibpng.a")
add_prebuilt_library(libzlib "opencv/3rdparty/libzlib.a")
add_prebuilt_library(opencv_imgproc "opencv/libopencv_imgproc.a")
add_prebuilt_library(opencv_core "opencv/libopencv_core.a")
add_prebuilt_library(ade "opencv/libade.a")

set(IMGCODEC_DEP libpng libzlib)
target_link_libraries(opencv_imgcodecs INTERFACE ${IMGCODEC_DEP})

set(CORE_DEP libzlib)
target_link_libraries(opencv_core INTERFACE ${CORE_DEP})

set(OPENCV_DEP opencv_imgcodecs opencv_imgproc opencv_core)
target_link_libraries(${COMPONENT_LIB} ${OPENCV_DEP})

set(lib libhuman_face_detect.a libdl.a)

if(${IDF_TARGET} STREQUAL "esp32")
    set(links   "-L ${CMAKE_CURRENT_SOURCE_DIR}/../components/esp-dl/lib/esp32")

elseif(${IDF_TARGET} STREQUAL "esp32s2")
    set(links   "-L ${CMAKE_CURRENT_SOURCE_DIR}/../components/esp-dl/lib/esp32s2")

elseif(${IDF_TARGET} STREQUAL "esp32s3")
    set(links   "-L ${CMAKE_CURRENT_SOURCE_DIR}/../components/esp-dl/lib/esp32s3")

elseif(${IDF_TARGET} STREQUAL "esp32c3")    
    set(links   "-L ${CMAKE_CURRENT_SOURCE_DIR}/../components/esp-dl/lib/esp32c3")

endif()

target_link_libraries(${COMPONENT_TARGET} ${links})
target_link_libraries(${COMPONENT_TARGET} ${lib})