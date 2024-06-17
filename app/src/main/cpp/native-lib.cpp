#include <jni.h>
#include <string>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>



// JNI 调用 java 层的一个方法
// jclass cls = (*env)->GetObjectClass(env, obj);
// jmethodID mid = (*env)->GetMethodID(env, cls, "toString", "()Ljava/lang/String;");
// jstring result = (jstring)(*env)->CallObjectMethod(env, obj, mid);
extern "C" JNIEXPORT jstring
JNICALL
stringFromJNI(JNIEnv* env,jobject obj){
    // jobject 就是针对 MainActivity 的对象
    std::string hello = "hello,JNI_OnLoad";

    // NewStringUTF 返回 java 的 String 的对象
    // NewStringUTF 接受 char* 的字符串的数组
    // hello.c_str() 就是获取该对象的数组
    // char* cString = "ok";
    return env->NewStringUTF(hello.c_str()); // NewStringUTF 接受 char* 类型的数据
}

// 创建一个 socket 并且访问 27042 的接口
extern "C" JNIEXPORT jobject
JNICALL
followFridaByPort(JNIEnv* env,jobject obj){
    // jboolean 并不是 Boolean jboolean 返回的是原生的 boolean，但是 Boolean 是一个封装的类
    // public native Boolean followFridaByPort(); 要得到这个返回值需要转换一下
    struct sockaddr_in  sa{};
    sa.sin_family = AF_INET;
    sa.sin_port = htons(27042);
    inet_aton("127.0.0.1",&sa.sin_addr);
    int sock = socket(AF_INET,SOCK_STREAM,0);

    jboolean result = JNI_FALSE;
    if(connect(sock,(struct sockaddr*) &sa,sizeof (sa)) == 0){
        close(sock);
        result = JNI_TRUE;
    }
    else{
        result = JNI_FALSE;
    }

    // 准备
    // new 一个
    jclass booleanClass = env->FindClass("java/lang/Boolean");
    jmethodID booleanConstructor = env->GetMethodID(booleanClass,"<init>", "(Z)V");
    //构造
    // jclass,方法的签名,传参
    jobject booleanObject = env->NewObject(booleanClass,booleanConstructor,result);
    return booleanObject;
}





// 函数的结构体
JNINativeMethod MainActivityMethods[] = {
        // 注册的函数
        // 函数的名字，函数的签名，函数的指针
        {"stringFromJNI","()Ljava/lang/String;",(void*)stringFromJNI}
};

JNINativeMethod coreMethods[] = {
        {"followFridaByPort","()Ljava/lang/Boolean;",(void*)followFridaByPort}
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

    // 注册 MainActivity class
    jclass MainActivityClazz = env->FindClass("com/go/openfrida/MainActivity");
    if (MainActivityClazz == nullptr){
        return JNI_ERR;
    }
    if(env->RegisterNatives(MainActivityClazz,MainActivityMethods,sizeof (MainActivityMethods) / sizeof (MainActivityMethods[0])) < 0){
        return JNI_ERR;
    }

    // 注册 core class
    jclass coreClazz = env->FindClass("com/go/openfrida/core/followFrida");
    if(coreClazz == nullptr){
        return JNI_ERR;
    }
    if(env->RegisterNatives(coreClazz,coreMethods,sizeof (coreMethods)/sizeof (coreMethods[0]))<0){
        return JNI_ERR;
    }


    return JNI_VERSION_1_6;
}


