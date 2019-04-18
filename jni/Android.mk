LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := LiveCloth
LOCAL_SRC_FILES := ShaderProgram.cpp Camera.cpp Cloth.cpp Shadow.cpp ColorEffect.cpp Buttons.cpp NativeRenderer.cpp
LOCAL_LDLIBS    := -lGLESv2 -llog

include $(BUILD_SHARED_LIBRARY)
