set(BspCommonSources
    ${CMAKE_CURRENT_SOURCE_DIR}/BspStatus.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/BspConcepts.hpp
)

set(BspStm32Sources
    ${CMAKE_CURRENT_SOURCE_DIR}/stm32/Uart.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/stm32/USBCDC.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/stm32/Spi.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/stm32/Gpio.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/stm32/Exti.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/stm32/Delay.cpp
)

set(BspSimulateSources
    ${CMAKE_CURRENT_SOURCE_DIR}/simulate/Uart.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/simulate/USBCDC.cpp
)

set(BspSources ${BspCommonSources})
if(USE_HAL_DRIVER AND CMAKE_CROSSCOMPILING)
    list(APPEND BspSources ${BspStm32Sources})
else()
    list(APPEND BspSources ${BspSimulateSources})
endif()

set(BspSources ${BspSources} PARENT_SCOPE)
