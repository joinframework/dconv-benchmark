function(add_conv_benches library header link)
    cmake_parse_arguments(ARG "" "" "DTOA;ATOD" ${ARGN})
    foreach(op DTOA ATOD)
        if(ARG_${op})
            string(TOLOWER ${op} name)
            foreach(adapter ${ARG_${op}})
                string(REPLACE "Adapter" "" variant ${adapter})
                string(TOLOWER ${variant} variant)
                set(target bench_${name}_${variant})
                add_executable(${target} bench_${name}.cpp)
                target_include_directories(${target} PRIVATE ${CMAKE_SOURCE_DIR}/benchmarks)
                target_compile_definitions(${target} PRIVATE
                    DATA_DIR="${CMAKE_SOURCE_DIR}/data"
                    ADAPTER_HEADER="${header}"
                    "BENCH_ADAPTERS=X(${adapter})"
                )
                target_link_libraries(${target} PRIVATE benchmark::benchmark ${link})
                target_compile_options(${target} PRIVATE -Wall -Wextra)
            endforeach()
        endif()
    endforeach()
endfunction()

add_conv_benches(dconv adapters/dconv.hpp dconv::dconv
    DTOA DconvAdapter DconvGrisuAdapter DconvGrisu2Adapter DconvGrisu3Adapter
    ATOD DconvAdapter
)
add_conv_benches(double-conversion adapters/double_conversion.hpp double-conversion
    DTOA DoubleConversionAdapter
    ATOD DoubleConversionAdapter
)
add_conv_benches(drachennest adapters/drachennest.hpp drachennest::drachennest
    DTOA DrachennestGrisu3Adapter DrachennestSchubfachAdapter
)
add_conv_benches(dragonbox adapters/dragonbox.hpp dragonbox::dragonbox
    DTOA DragonboxAdapter
)
add_conv_benches(fast_float adapters/fast_float.hpp FastFloat::fast_float
    ATOD FastFloatAdapter
)
add_conv_benches(fmt adapters/fmt.hpp fmt::fmt
    DTOA FmtAdapter
)
add_conv_benches(gay adapters/gay.hpp gay::gay
    DTOA GayAdapter
    ATOD GayAdapter
)
add_conv_benches(libc adapters/libc.hpp ""
    DTOA LibcAdapter
    ATOD LibcAdapter
)
add_conv_benches(milo adapters/milo.hpp milo::milo
    DTOA MiloAdapter
)
add_conv_benches(ryu adapters/ryu.hpp ryu::ryu
    DTOA RyuAdapter
    ATOD RyuAdapter
)
add_conv_benches(std adapters/std.hpp ""
    DTOA StdAdapter
    ATOD StdAdapter
)
add_conv_benches(teju adapters/teju.hpp teju::teju
    DTOA TejuAdapter
)
