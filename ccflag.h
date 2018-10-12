#ifndef __CCFLAG_H

#include <functional>
#include <map>
#include <string>
#include <iostream>
#include <stdlib.h>

using string = std::string;

class CCFlag{
    public:
    void  *val;
    string name;
    string defvalue;
    string usage;

    std::function<void (string value)> func;
};

class CCFlagSet {

    string name;
    std::map<string, CCFlag> formal;

    public:
        void bool_var(bool *p, string name, string defvalue, string usage);

        void int_var(int *p, string name, string defvalue, string usage);

        void double_var(double *p, string name, string defvalue, string usage);

        void string_var(string *p, string name, string defvalue, string usage);
    private:

        void var(void *p, string name, string value, string usage, std::function<bool (string value)> func);
};


void CCFlagSet::var(void *p, string name, string defvalue, string usage, std::function<bool (string value)> func) {
    CCFlag flag = {p, name, defvalue, usage, func};

    auto v = formal.count(name);
    if (v > 0) {
        string msg = "";
        if (this->name.length() > 0) {
            msg = "ccflag redefined:" + name;
        } else {
            msg = this->name + "ccflag redefined:" + name;
        }

        std::cout << msg << std::endl;
        exit(1);
    }

    formal[name] = flag;
}

void CCFlagSet::bool_var(bool *p, string name, string defvalue, string usage) {
    auto f = [=](string value)->bool{
        auto parse_bool = [](string str) -> int{
            if (str == "1" || str == "t" || str == "T") {
                return 1;
            }

            if (str == "true" || str == "TRUE"||str == "True") {
                return 1;
            }

            if (str == "0" || str == "f" || str == "F") {
                return 0;
            }

            if (str=="false" || str == "FALSE" ||str == "False") {
                return 0;
            }

            return -1;
        };

        auto b = parse_bool(value);
        if (b == -1) {
            return false;
        }

        *p = (b == 1) ? true : false;
    };

    var(p, name, defvalue, usage, f);
}

void CCFlagSet::int_var(int *p, string name, string defvalue, string usage) {
}

void CCFlagSet::double_var(double *p, string name, string defvalue, string usage) {
}

void CCFlagSet::string_var(string *p, string name, string defvalue, string usage) {
}
#endif
