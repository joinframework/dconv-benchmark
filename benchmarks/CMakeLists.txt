set(DCONV_BENCH_LIBS
    dconv::dconv
    ryu::ryu
    dragonbox::dragonbox
    fmt::fmt
    double-conversion
    FastFloat::fast_float
    drachennest::drachennest
    teju::teju
    milo::milo
    gay::gay
)

add_subdirectory(conformance)
add_subdirectory(performance)
