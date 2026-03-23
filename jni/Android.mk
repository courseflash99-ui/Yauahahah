LOCAL_PATH := $(call my-dir)

# --- ڕێکخستنی Curl ---
include $(CLEAR_VARS)
LOCAL_MODULE := libcurl
LOCAL_SRC_FILES := curl/curl-android-$(TARGET_ARCH_ABI)/lib/libcurl.a
include $(PREBUILT_STATIC_LIBRARY)

# --- ڕێکخستنی OpenSSL (SSL) ---
include $(CLEAR_VARS)
LOCAL_MODULE := libssl
LOCAL_SRC_FILES := curl/openssl-android-$(TARGET_ARCH_ABI)/lib/libssl.a
include $(PREBUILT_STATIC_LIBRARY)

# --- ڕێکخستنی OpenSSL (Crypto) ---
include $(CLEAR_VARS)
LOCAL_MODULE := libcrypto
LOCAL_SRC_FILES := curl/openssl-android-$(TARGET_ARCH_ABI)/lib/libcrypto.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := rary

# رێکخستنی ئاڵاکانی کۆمپایلەر بۆ پاراستن و خێرایی
LOCAL_CFLAGS := -Wno-error=format-security -fvisibility=hidden -ffunction-sections -fdata-sections -w
LOCAL_CFLAGS += -fno-exceptions -fpermissive
LOCAL_CFLAGS += -rdynamic -funwind-tables
LOCAL_CPPFLAGS := -std=c++17 -Wno-error=format-security -fvisibility=hidden -ffunction-sections -fdata-sections -w -Werror -s
LOCAL_CPPFLAGS += -Wno-error=c++11-narrowing -fms-extensions -fno-exceptions -fexceptions -fpermissive
LOCAL_CPPFLAGS += -rdynamic -funwind-tables -fno-omit-frame-pointer
LOCAL_LDFLAGS += -Wl,--strip-all
LOCAL_ARM_MODE := arm

# زیادکردنی فایلە سەرەکییەکان
LOCAL_SRC_FILES := main.cpp \
    include/external/dlfcn/dlfcn.cpp \
    include/external/imgui/imgui.cpp \
    include/external/Substrate/SubstrateHook.cpp \
    include/external/Substrate/SubstrateDebug.cpp \
    include/external/Substrate/SubstratePosixMemory.cpp \
    include/external/And64InlineHook/And64InlineHook.cpp \
    include/external/xhook/xhook.c \
    include/external/oxorany/oxorany.cpp

# دیاریکردنی ناونیشانی فۆڵدەرەکانی Include بۆ ئەوەی کێشەی "File not found" دروست نەبێت
LOCAL_C_INCLUDES := $(NDK_ROOT)/sources/android
LOCAL_C_INCLUDES += $(LOCAL_PATH)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include/external
LOCAL_C_INCLUDES += $(LOCAL_PATH)/curl/curl-android-$(TARGET_ARCH_ABI)/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/curl/openssl-android-$(TARGET_ARCH_ABI)/include

# بەستنەوەی لایبرەرییە دەرەکییەکان
LOCAL_LDLIBS := -landroid -lGLESv3 -lEGL -ldl -llog -lz
LOCAL_STATIC_LIBRARIES := libcurl libssl libcrypto
include $(BUILD_SHARED_LIBRARY)
