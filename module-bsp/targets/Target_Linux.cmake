
set(BOARD_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/board/linux/board.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/linux/eink/ED028TC1.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/linux/keyboard/keyboard.cpp"
        
        "${CMAKE_CURRENT_SOURCE_DIR}/board/linux/headset/headset.cpp"

        "${CMAKE_CURRENT_SOURCE_DIR}/board/linux/battery-charger/battery_charger.cpp"

        "${CMAKE_CURRENT_SOURCE_DIR}/board/linux/cellular/linux_cellular.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/linux/rtc/rtc.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/linux/bluetooth/test/bsp_bt.cpp"

        "${CMAKE_CURRENT_SOURCE_DIR}/board/linux/lpm/LinuxLPM.cpp"

        "${CMAKE_CURRENT_SOURCE_DIR}/board/linux/audio/linux_audiocodec.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/board/linux/audio/LinuxCellularAudio.cpp"
        #needed because of PlaybackOperation::SetBluetoothStreamData
        "${CMAKE_CURRENT_SOURCE_DIR}/board/rt1051/bsp/audio/RT1051BluetoothAudio.cpp"

        "${CMAKE_CURRENT_SOURCE_DIR}/board/linux/bluetooth/Bluetooth.cpp"

        "${CMAKE_CURRENT_SOURCE_DIR}/board/linux/usb_cdc/usb_cdc.cpp"

        "${CMAKE_CURRENT_SOURCE_DIR}/board/linux/vibrator/vibrator.cpp"

        "${CMAKE_CURRENT_SOURCE_DIR}/board/linux/magnetometer/magnetometer.cpp"

        "${CMAKE_CURRENT_SOURCE_DIR}/board/linux/torch/torch.cpp"

        "${CMAKE_CURRENT_SOURCE_DIR}/board/linux/watchdog/watchdog.cpp"

        CACHE INTERNAL "")

set(BOARD_DIR_INCLUDES
        ${CMAKE_CURRENT_SOURCE_DIR}/board/linux
        ${CMAKE_CURRENT_SOURCE_DIR}/board/linux/eink
        ${CMAKE_CURRENT_SOURCE_DIR}/board/linux/pwr
        ${CMAKE_CURRENT_SOURCE_DIR}/board/linux/audio
        ${CMAKE_CURRENT_SOURCE_DIR}/bsp/headset

        ${CMAKE_SOURCE_DIR}/module-bluetooth/Bluetooth
        ${CMAKE_SOURCE_DIR}/module-sys/

        CACHE INTERNAL "")

set(BOARD_SOURCE_FILES_PROPERTIES ${BOARD_SOURCE_FILES_PROPERTIES} ${CMAKE_CURRENT_SOURCE_DIR}/board/linux/board.cpp PROPERTIES COMPILE_FLAGS -Wno-unused-function CACHE INTERNAL " ")

