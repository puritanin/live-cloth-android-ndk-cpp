
Demo project showing concept of the Verlet integration.

Based on Android NDK (C++) and OpenGL Mathematics (GLM) library.


How build (Windows):

- install JRE and JDK 8

- install Android SDK

- download and extract Android NDK

- set the correct values for system variables: JAVA_HOME, ANDROID_HOME, ANDROID_NDK_HOME

- download GLM source folder "glm" (https://github.com/g-truc/glm/tree/0.9.6.0) and put it to sub-folder of project /jni/.

- run in place:

  gradlew.bat build


Android apk-file will be in /build/outputs/apk/.

Debug version recommended for test use on real devices.
