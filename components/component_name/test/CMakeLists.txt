file(GLOB srcsCOMP "*.c")

idf_component_register(
    SRCS
        "${srcsCOMP}"
    INCLUDE_DIRS
        "../include"
    PRIV_INCLUDE_DIRS
        "../private_include"
    REQUIRES
        unity
        # Change the component name.
        component_name
)