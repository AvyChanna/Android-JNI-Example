//
// Created by h2g2 on 10/8/20.
//
#include <jni.h>
#include <pthread.h>
#include <string.h> // memset
#include <unistd.h> // sleep

// ********** Utils **********
#define STR(str) #str
#define ESTR(str) STR(str)

#define __PACKAGE_ID com_example_jniexample
#define PACKAGE_ID ESTR(__PACKAGE_ID)

#define FUNC(activity, function) __FUNC(__PACKAGE_ID, activity, function)
#define __FUNC(package, activity, function) __FUNC_IMPL(package, activity, function)
#define __FUNC_IMPL(package, activity, function) \
                    JNICALL Java_ ## package ## _ ## activity ## _ ## function

// ********** Logging **********
#include <android/log.h>

static const char *TAG = "JNI_LOG";
#define LOG(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)

// ********** Data Structures **********
typedef struct {
    JavaVM *javaVM; // JAVA VM
    jclass clazz; // MainActivity class
    jobject object; // mainActivity object instance
} GlobalState;

// ********** Variable Declarations **********
static GlobalState gVar;
static unsigned int time_to_sleep = 3;

// ********** Function Declarations **********
void *startCallback();

JNIEXPORT void FUNC(MainActivity, registerCallback)(JNIEnv *env, jobject instance);

JNIEXPORT void FUNC(MainActivity, deregisterCallback)(JNIEnv *env, jobject ignored);

// ********** Function Definitions **********
void *startCallback() {
    JavaVM *javaVM = gVar.javaVM;
    JNIEnv *env;
    jint res = (*javaVM)->GetEnv(javaVM, (void **) &env, JNI_VERSION_1_6);
    if (res != JNI_OK) {
        res = (*javaVM)->AttachCurrentThread(javaVM, &env, NULL);
        if (JNI_OK != res) {
            LOG("Failed to AttachCurrentThread, ErrorCode = %d", res);
            return NULL;
        }
    }
    jmethodID timerId = (*env)->GetMethodID(env, gVar.clazz, "onCallback", "()V");
    sleep(time_to_sleep);
    (*env)->CallVoidMethod(env, gVar.object, timerId);
    (*javaVM)->DetachCurrentThread(javaVM);
    return NULL;
}

void FUNC(MainActivity, registerCallback)(JNIEnv *env, jobject instance) {
    LOG("Registering callback");
    pthread_t threadInfo_;
    pthread_attr_t threadAttr_;
    pthread_attr_init(&threadAttr_);
    pthread_attr_setdetachstate(&threadAttr_, PTHREAD_CREATE_DETACHED); // IMPORTANT
    jclass clazz = (*env)->GetObjectClass(env, instance);
    gVar.clazz = (*env)->NewGlobalRef(env, clazz);
    gVar.object = (*env)->NewGlobalRef(env, instance);
    if (pthread_create(&threadInfo_, &threadAttr_, startCallback, NULL) == 0) {
        LOG("Thread creation successful");
    } else {
        LOG("Thread creation failed");
    }
    pthread_attr_destroy(&threadAttr_);
}

void FUNC(MainActivity, deregisterCallback)(JNIEnv *env, jobject ignored) {
    (*env)->DeleteGlobalRef(env, gVar.clazz);
    (*env)->DeleteGlobalRef(env, gVar.object);
    gVar.object = NULL;
    gVar.clazz = NULL;
}

jint JNI_OnLoad(JavaVM *vm, void *ignored) {
    JNIEnv *env;
    memset(&gVar, 0, sizeof(gVar));
    gVar.javaVM = vm;
    if ((*vm)->GetEnv(vm, (void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        LOG("JNI 1.6 not supported");
        return JNI_ERR; // JNI version not supported.
    }
    LOG("OnLoad successful");
    gVar.object = NULL;
    return JNI_VERSION_1_6;
}
