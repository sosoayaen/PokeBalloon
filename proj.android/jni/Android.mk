LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

LOCAL_MODULE := cocos2dcpp_shared

LOCAL_MODULE_FILENAME := libcocos2dcpp

#LOCAL_SRC_FILES := hellocpp/main.cpp \
                   ../../Classes/AppDelegate.cpp \
                   ../../Classes/HelloWorldScene.cpp
                   
#LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../Classes

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../../UMCocos2dxAnalytics/include

# �����Լ���Դ�ļ�Ŀ¼��Դ�ļ���׺��
MY_FILES_PATH  :=  $(LOCAL_PATH) \
				$(LOCAL_PATH)/../../Classes

MY_FILES_SUFFIX := %.cpp %.c

# �ݹ����Ŀ¼�µ����е��ļ�
rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))

# ��ȡ��Ӧ��Դ�ļ�
MY_ALL_FILES := $(foreach src_path,$(MY_FILES_PATH), $(call rwildcard,$(src_path),*.*) ) 
MY_ALL_FILES := $(MY_ALL_FILES:$(MY_CPP_PATH)/./%=$(MY_CPP_PATH)%)
MY_SRC_LIST  := $(filter $(MY_FILES_SUFFIX),$(MY_ALL_FILES)) 
MY_SRC_LIST  := $(MY_SRC_LIST:$(LOCAL_PATH)/%=%)

# ȥ���ִ����ظ�����
define uniq =
  $(eval seen :=)
  $(foreach _,$1,$(if $(filter $_,${seen}),,$(eval seen += $_)))
  ${seen}
endef

# �ݹ������ȡ����Ŀ¼
MY_ALL_DIRS := $(dir $(foreach src_path,$(MY_FILES_PATH), $(call rwildcard,$(src_path),*/) ) )
#MY_ALL_DIRS := $(call uniq,$(MY_ALL_DIRS))

# ��ֵ��NDK����ϵͳ
LOCAL_SRC_FILES  := $(MY_SRC_LIST)
LOCAL_C_INCLUDES += $(MY_ALL_DIRS)

# ��������ͳ�ƿ�
LOCAL_LDLIBS += libs/cocos2dx2_libMobClickCpp.a

LOCAL_WHOLE_STATIC_LIBRARIES += cocos2dx_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocosdenshion_static
LOCAL_WHOLE_STATIC_LIBRARIES += box2d_static
LOCAL_WHOLE_STATIC_LIBRARIES += chipmunk_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocos_extension_static

include $(BUILD_SHARED_LIBRARY)

$(call import-module,cocos2dx)
$(call import-module,cocos2dx/platform/third_party/android/prebuilt/libcurl)
$(call import-module,CocosDenshion/android)
$(call import-module,extensions)
$(call import-module,external/Box2D)
$(call import-module,external/chipmunk)
