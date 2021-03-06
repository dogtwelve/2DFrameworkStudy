APP_PROJECT_PATH		:= $(call my-dir)/..

APP_SHORT_COMMANDS := true

EE_SDL_VERSION			:= EE_SDL_VERSION_2

EE_GLES_VERSION			:= -DEE_GLES2 -DSOIL_GLES2 -DSDL_GLES2 -DEE_GLES1 -DSOIL_GLES1 -DSDL_GLES1
EE_GLES_LINK			:= -lGLESv2 -lGLESv1_CM

APP_STL					:= c++_shared

APP_LDLIBS				:= -llog $(EE_GLES_LINK) -lm -lz -lOpenSLES -lEGL -landroid

#Debug Build
#APP_CFLAGS				:= -g -DDEBUG -DEE_DEBUG
#APP_CPPFLAGS			:= -std=c++11 -frtti -fexceptions
#APP_OPTIM 				:= debug

#Release Build
APP_CFLAGS				:= -fno-strict-aliasing -O3 -s -DNDEBUG -ffast-math
APP_CPPFLAGS			:= -std=c++11 -frtti -fexceptions

APP_PLATFORM			:= android-14
APP_MODULES				:= main
APP_ABI					:= armeabi-v7a

NDK_TOOLCHAIN_VERSION	:= clang
