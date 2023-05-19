set(priv_requires "driver")
idf_component_register(SRCS "motor.c"
                    INCLUDE_DIRS "include"
                    PRIV_REQUIRES ${priv_requires})
