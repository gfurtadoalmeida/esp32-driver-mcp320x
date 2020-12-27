file(GLOB srcsCOMP "*.c")

idf_component_register(
    SRCS
        "${srcsCOMP}"
    INCLUDE_DIRS
        "../include"
        "include"
    PRIV_INCLUDE_DIRS
        "../private_include"
    REQUIRES
        unity
        mcp320x
)