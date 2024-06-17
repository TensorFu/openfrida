# 收集市面上常见的 frida 检测方式

## 自定义 syscall 获取 /proc/self/maps 检测 frida
### 修改 CMakeLists.txt
set(can_use_assembler TRUE)
enable_language(ASM)

### 
