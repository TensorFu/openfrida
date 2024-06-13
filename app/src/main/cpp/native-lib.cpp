#include <jni.h>
#include <string>


// jobject 就是针对 MainActivity 的对象

// JNI 调用 java 层的一个方法
// jclass cls = (*env)->GetObjectClass(env, obj);
// jmethodID mid = (*env)->GetMethodID(env, cls, "toString", "()Ljava/lang/String;");
// jstring result = (jstring)(*env)->CallObjectMethod(env, obj, mid);
extern "C" JNIEXPORT jstring
JNICALL
stringFromJNI(JNIEnv* env,jobject jobj){
    std::string hello = "hello,JNI_OnLoad";

    // NewStringUTF 返回 java 的 String 的对象
    // NewStringUTF 接受 char* 的字符串的数组
    // hello.c_str() 就是获取该对象的数组
    // char* cString = "ok";
    return env->NewStringUTF(hello.c_str()); // NewStringUTF 接受 char* 类型的数据
}


// 函数的结构体
JNINativeMethod methods[] = {
        // 注册的函数
        // 函数的名字，函数的签名，函数的指针
        {"stringFromJNI","()Ljava/lang/String;",(void*)stringFromJNI}
};

// 动态注册
// JavaVM *vm 就是指向 java 虚拟机的一个指针，主要就是一个生成一个 env
JNIEXPORT jint JNICALL
JNI_OnLoad(JavaVM* vm,void* reserved){
    // 指针是一个变量，存储另一个变量的地址
    // int* P 能够申请一个指针的变量
    // &x 能够取到一个变量的地址
    // *p 能够指针拿到对应的值
    int x = 10;
    int* p = &x;

    // env 就是 NDK 的环境，所有的NDK的函数的使用都是使用的这个调用
    // env
    JNIEnv* env;
    // 获取 env
    if (vm->GetEnv(reinterpret_cast<void**>(&env),JNI_VERSION_1_6) != JNI_OK){
        return JNI_ERR;
    }
    jclass clazz = env->FindClass("com/go/openfrida/MainActivity");
    if (clazz == nullptr){
        return JNI_ERR;
    }
    if(env->RegisterNatives(clazz,methods,sizeof (methods) / sizeof (methods[0])) < 0){
        return JNI_ERR;
    }
    return JNI_VERSION_1_6;
}


