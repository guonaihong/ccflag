#ifndef __CCFLAG_H

#include <functional>
#include <map>
#include <string>

using string = std::string;

class CCFlag{
    void  *val;
    string name;
    string defvalue;
    string usage;
};

class CCFlagSet {
    string name;
    public:
        void bool_var(bool *p, string name, string value, string usage);

        void int_var(int *p, string name, string value, string usage);

        void double_var(double *p, string name, string value, string usage);

        void string_var(string *p, string name, string value, string usage);
    private:
        std::map<string, CCFlag> formal;

        void var(string name, string value, string usage, std::function<void (string, string, string)> func);
};


void CCFlagSet::var(string name, string value, string usage, std::function<void (string, string, string)> func) {
    func(name, value, usage);
}

void CCFlagSet::bool_var(bool *p, string name, string value, string usage) {
    /*
    auto f = [this]{
        printf("%s\n", this->name.c_str());
    };
    var(name, value, usage, f);
    */
}

void CCFlagSet::int_var(int *p, string name, string value, string usage) {
}

void CCFlagSet::double_var(double *p, string name, string value, string usage) {
}

void CCFlagSet::string_var(string *p, string name, string value, string usage) {
}
#endif
