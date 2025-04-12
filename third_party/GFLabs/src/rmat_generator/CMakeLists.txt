file(GLOB rmat_srcs *.c)

add_library(rmat_generator SHARED ${rmat_srcs})