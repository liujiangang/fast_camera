LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES:= display.c \
									fastcamera.c \
									lcd_show_bmp.c \
									fastcamera_main.c \

LOCAL_STATIC_LIBRARIES := \
	libregistermsext

	
LOCAL_MODULE:= fastcamera

include $(BUILD_EXECUTABLE)



#include $(CLEAR_VARS)

#LOCAL_SRC_FILES:= \
#	ISettingsInfoChangedListener.cpp \
#	SettingsInfoChangedListener.cpp \
#	IBTInfoChangedListener.cpp \
#	BTInfoChangedListener.cpp \
#	IRadioInfoChangedListener.cpp \
#	RadioInfoChangedListener.cpp \
#	IDataChangedListener.cpp \
#	DataChangedListener.cpp \
#	IMcuService.cpp \
#	McuServiceClient.cpp \
#	JNIEnterance.cpp \

#LOCAL_SHARED_LIBRARIES := \
#	libcutils \
#	libnbaio \
#	libutils \
#	liblog \
#	libbinder
	
#LOCAL_MODULE:= libmcuserverclient_jni

#include $(BUILD_SHARED_LIBRARY)
