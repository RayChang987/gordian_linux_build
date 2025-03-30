cmake_minimum_required(VERSION 3.10)
project(gordian_linux_build)

set(CMAKE_CXX_STANDARD 17)

# 加入原始碼
add_executable(gordian_linux_build src/main.cpp src/Gordian.cpp src/EasyBMP.cpp src/EasyBMP_Geometry)

# 加入標頭文件路徑
include_directories(${PROJECT_SOURCE_DIR}/include)

# 連結 BLAS 和 LAPACK
find_package(BLAS REQUIRED)
find_package(LAPACK REQUIRED)

target_link_libraries(gordian_linux_build ${BLAS_LIBRARIES} ${LAPACK_LIBRARIES})
