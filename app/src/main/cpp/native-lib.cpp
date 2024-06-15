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


// 创建一个 socket 并且访问这个 27042 端口
extern "C" JNIEXPORT jobject
JNICALL
followFridaByPort(JNIEnv* env,jobject obj){
    // jboolean 并不是 Boolean jboolean 返回的是原生的 boolean，但是 Boolean 是一个封装的类
    // public native Boolean followFridaByPort(); 要得到这个返回值需要转换一下

    // 设置 sockaddr_in 结构体存放 ip:port
    struct sockaddr_in  sa{};
    sa.sin_family = AF_INET; // sin 表示的是 socket internet family 表示的是地址家族
    sa.sin_port = htons(27042); // 设置端口
    inet_aton("127.0.0.1",&sa.sin_addr); // internal ASCII to network  将一个以 ASCII 表示的 IP 地址转换为网络字节序的地址

    // 表示创建一个 ipv4 的的 TCP 协议的 socket 的对象
    // AF_INET 表示 ipv4 ，后面的两的参数的解释太过于复杂，连起来记死就是一个，TCP 的协议
    int sock = socket(AF_INET,SOCK_STREAM,0);

    jboolean result = JNI_FALSE;
    // 尝试连接用 sock 连接 sa 这个地址
    // 第一个方法，第二个是地址，第三个是 sa 的大小，这样能够保证传递的是正确的地址信息
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

// D-Bus 是 frida-server 的通信的协议，遍历所有的端口，给每一个端口都发送消息，谁回复了，哪个就是 frida-server
extern "C" JNIEXPORT jobject
JNICALL
followFridaByD_Bus(JNIEnv* env,jobject obj){
    // 创建 sockaddr_in 结构体
    struct sockaddr_in as{};
    as.sin_family = AF_INET;
    inet_aton("127.0.0.1",&as.sin_addr);

    int sock = socket(AF_INET,SOCK_STREAM,0);

    jboolean result = JNI_FALSE;
    for(int i=0;i<=65535;i++){
        char* res;
        as.sin_port = htonl(i);
        if(connect(sock,(struct sockaddr*) &as,sizeof (as)) == 0){
            memset(res,0,7); // 设置指定内存地址的值，数值是 0 ，长度是 7 个字节
            // 发送消息 sock，内容，长度，无特殊意义
            send(sock,"\x00", 1,NULL);
            send(sock,"AUTH\\r\\n",6,NULL);
            int ret;
            usleep(100);
            ret = recv(sock, res, 6, MSG_DONTWAIT) != -1; // 接受消息，sock协议，存储的 res，接受长度，MSG_DONTWAIT 没有接收到消息就跳过，不干等
            if(ret){
                if(strcmp(res,"REJECT") == 0){
                    result = JNI_TRUE;
                }
            }
            close(sock);
        }
    }
    jclass clazz = env->FindClass("java/lang/Boolean");
    jmethodID booleanConstructor = env->GetMethodID(clazz,"<init>","(Z)V");
    jobject booleanObject = env->NewObject(clazz,booleanConstructor,result);
    return booleanObject;
}

// 读取 /proc/self/maps
extern "C" JNIEXPORT jobject
JNICALL
followFridaByMaps(JNIEnv* env,jobject obj){
    char line[512];
    FILE* fp;
    fp = fopen("/proc/self/maps", "r");
    jboolean result = JNI_FALSE;
    if(fp){
        while (fgets(line,512,fp)){
            if (strstr(line,"frida")){
                result = JNI_TRUE;
            }
        }
    }

    jclass clazz = env->FindClass("java/lang/Boolean");
    jmethodID booleanConstructor = env->GetMethodID(clazz,"<init>","(Z)V");
    jobject booleanObject = env->NewObject(clazz,booleanConstructor,result);
    return booleanObject;
}




// 函数的结构体
JNINativeMethod MainActivityMethods[] = {
        // 注册的函数
        // 函数的名字，函数的签名，函数的指针
        {"stringFromJNI","()Ljava/lang/String;",(void*)stringFromJNI}
};

JNINativeMethod coreMethods[] = {
        {"followFridaByPort","()Ljava/lang/Boolean;",(void*)followFridaByPort},
        {"followFridaByD_Bus","()Ljava/lang/Boolean;",(void*)followFridaByD_Bus},
        {"followFridaByMaps","()Ljava/lang/Boolean;",(void*)followFridaByMaps}
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


