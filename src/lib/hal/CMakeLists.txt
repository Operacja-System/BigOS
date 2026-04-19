SETUP_LIBRARY(hal)

target_link_libraries(hal
    PUBLIC stdbigos
    PRIVATE Debug
    PRIVATE device_tree_access
)
