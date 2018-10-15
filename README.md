##### ccflag

##### 主要功能如下
* 可以解析int, bool, str, double类型
* 编写扩展类型很简单

##### 主要函数
* 初始化函数(构造函数)
```c++
CCFlagSet::CCFlagSet(string name,                //进程名
                    on_error_type error_handlin  //出错时，可以控制是直接退出进程，还是abort，还是return返回g
);
```
* 解析函数
```c++
// argv命令行参数
string CCFlagSet::parse(char **argv);
```
##### 示例
``` c++
#include "ccflag.h"

class config {
    public:
    int                 nt;
    bool                debug;
    string              model_path;
    double              pi;
    //unsigned            ip;
    //unsigned short      port;
    //struct sockaddr_in  addr;
};

int main(int argc, char **argv) {
    CCFlagSet commandl_line{argv[0], exit_on_error};

    config cnf;

    commandl_line.int_var(&cnf.nt,            "nt",         "0",     "Maximum number of threads");
    commandl_line.bool_var(&cnf.debug,        "debug",      "false", "Open the server debug mode");
    commandl_line.string_var(&cnf.model_path, "model_path", "./",    "xxx engine directory");
    commandl_line.double_var(&cnf.pi,         "pi",         "0",     "pi");

    commandl_line.parse(argv + 1);

    std::cout << "nt: " << cnf.nt << std::endl;
    std::cout << "debug: " << cnf.debug << std::endl ;
    std::cout << "model_path: " << cnf.model_path << std::endl ;
    std::cout << "pi: " << cnf.pi << std::endl ;
    return 0;
}
```

##### 编写扩展类型
ccflag已经自带, int, bool, str, double等多个类型
但是真实世界是复杂的，很可能遇到ccflag自带类型不能满足的需求,这时候就学要自己编写扩展类型了

首先加一个新的成员函数，参考string_var 成员函数，修改lamabda表达式的逻辑
最后调用下var函数，是不是很简单
``` c++
void CCFlagSet::string_var(string *p, string name, string defvalue, string usage) {
    auto f = [=](string value) -> bool {
        *p = value;
        return true;
    };

    var(p, name, defvalue, usage, f, false, "string");
}
```
