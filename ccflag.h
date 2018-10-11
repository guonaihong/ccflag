#ifndef __CCFLAG_H

#include <string>

using string = std::string;

class CCFlagSet {
    string name;
    public:
        void bool_var(bool *p, string name, string value, string usage);

        void int_var(int *p, string name, string value, string usage);

        void double_var(double *p, string name, string value, string usage);

        void string_var(string *p, string name, string value, string usage);
    private:
        void var(string name, string value, string usage);
};


void CCFlagSet::bool_var(bool *p, string name, string value, string usage) {
    [this]{
        printf("%s\n", this->name.c_str());
    }();
}

void CCFlagSet::int_var(int *p, string name, string value, string usage) {
}

void CCFlagSet::double_var(double *p, string name, string value, string usage) {
}

void CCFlagSet::string_var(string *p, string name, string value, string usage) {
}
#endif
