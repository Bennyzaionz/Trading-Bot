# external/CMakeLists.txt

# Create the actual target
add_library(eigen_interface INTERFACE)

# Set include directories relative to this CMake file
target_include_directories(eigen_interface INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/eigen-3.4.0)

# Create an alias with the expected Eigen3::Eigen name
add_library(Eigen3::Eigen ALIAS eigen_interface)
