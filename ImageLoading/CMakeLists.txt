cmake_minimum_required (VERSION 3.8)

add_library (ImageLoading
	"private/ImageLoader.h"
	"private/ImageLoader.cpp"
	"private/ImageLoaderMeta.h"
	"private/ImageLoaderMeta.cpp"
	"public/ImageLoading.h"
	"private/ImageLoading.cpp"
	"private/ImageTextureMeta.h"
	"private/ImageTextureMeta.cpp"
	"private/ImageLoadingSettingsMeta.cpp"
	"private/ImageLoadingSettingsMeta.h"
	"private/ImageLoadingSettings.cpp"
	"private/ImageLoadingSettings.h"
	"private/utils.h"
	"private/utils.cpp"
	"public/ImageLoadedNotificationMeta.h"
	"private/ImageLoadedNotificationMeta.cpp"
	"public/FontAssetMeta.h"
	"private/ForntAssetMeta.cpp"
	"public/FontAsset.h"
	"private/FontAsset.cpp"
	"public/FontAssetLoadedNotificationMeta.h"
	"private/FontAssetLoadedNotificationMeta.cpp"
)

target_include_directories(ImageLoading INTERFACE PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/public")
target_include_directories(ImageLoading PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/private")

target_link_libraries(ImageLoading PUBLIC Notifications RenderingCore AppSettings)
target_link_libraries(ImageLoading PRIVATE "windowscodecs.lib")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ImageLoading PROPERTY CXX_STANDARD 20)
endif()
