find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

file(GLOB cpp_files ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
foreach(cpp_file IN LISTS cpp_files)
  get_filename_component(cpp_name ${cpp_file} NAME_WE)
  add_executable(${cpp_name} ${cpp_name}.cpp)
  target_link_libraries(${cpp_name} ${OpenCV_LIBS})
endforeach()