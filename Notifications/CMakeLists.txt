cmake_minimum_required (VERSION 3.8)

add_library (Notifications
	"public/Notifications.h"
	"private/Notifications.cpp"
	"public/NotificationReceiverMeta.h"
	"public/NotificationReceiver.h"
	"private/NotificationReceiver.cpp"
	"private/NotificationReceiverMeta.cpp"
	"private/utils.cpp"
	"private/utils.h"
)

target_include_directories(Notifications INTERFACE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public")
target_include_directories(Notifications PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private")

target_link_libraries(Notifications PUBLIC Jobs CommonResources)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Notifications PROPERTY CXX_STANDARD 20)
endif()
