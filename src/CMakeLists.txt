# create empty library which has source files added to it in each of the subdirectories using target_source()
add_library(${LIBRARY_NAME})

message(STATUS "LIBRARY NAME: ${LIBRARY_NAME}.lib")  # debug output

add_subdirectory(actor)
add_subdirectory(core)
add_subdirectory(market)
add_subdirectory(strategy)
add_subdirectory(trading)
add_subdirectory(risk)

target_include_directories(${LIBRARY_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)