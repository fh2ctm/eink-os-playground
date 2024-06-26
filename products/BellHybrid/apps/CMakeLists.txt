add_library(app STATIC)

target_sources(app
    PRIVATE
        Application.cpp
    PUBLIC
        include/Application.hpp
)

target_include_directories(app
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(app
    PRIVATE
        apps-common
        bell::appmgr
        bell::alarms
        bell::audio
    PUBLIC 
        bell::app-common
)

add_subdirectory(application-bell-main)
add_subdirectory(application-bell-onboarding)
add_subdirectory(application-bell-relaxation)
add_subdirectory(application-bell-alarm)
add_subdirectory(application-bell-bedtime)
add_subdirectory(application-bell-meditation-timer)
add_subdirectory(application-bell-settings)
add_subdirectory(application-bell-powernap)
add_subdirectory(application-bell-focus-timer)
add_subdirectory(common)
