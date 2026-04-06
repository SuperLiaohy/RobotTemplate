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
)

set(BspSources ${BspCommonSources})
list(APPEND BspSources ${BspStm32Sources})

set(BspSources ${BspSources} PARENT_SCOPE)
