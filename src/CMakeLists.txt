file(GLOB PUBLIC_HEADER_LIST CONFIGURE_DEPENDS "public-include/*.h")
file(GLOB HEADER_LIST CONFIGURE_DEPENDS "*.h")

add_library(property_system INTERFACE ${HEADER_LIST} ${PUBLIC_HEADER_LIST})

target_include_directories(property_system INTERFACE public-include)
target_link_libraries(property_system INTERFACE GSL)
target_compile_features(property_system INTERFACE cxx_std_17)
