list(APPEND SOURCES
    color.c
    led_encoder.c
    led_panel_driver.c
    surface.c
)

list(APPEND INCLUDES
    include
)

list(APPEND REQ
    driver
    esp_common
)

idf_component_register(SRCS ${SOURCES}
                       INCLUDE_DIRS ${INCLUDES}
                       REQUIRES ${REQ})


# Enable anonymous structs
target_compile_options(${COMPONENT_LIB} PRIVATE -fms-extensions)
# Force all warnings to be errors
target_compile_options(${COMPONENT_LIB} PRIVATE -Werror -Wall -Wextra
                        -Werror=unused-variable)
