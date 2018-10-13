#ifndef __CCFLAG_H

#include <functional>
#include <map>
#include <string>
#include <string.h>
#include <iostream>
#include <stdlib.h>
#include <limits.h>
#include <stdarg.h>

using string = std::string;

class CCFlag{
    public:
    void  *val;
    string name;
    string defvalue;
    string usage;

    std::function<bool (string value)> set;

    bool isbool;
};

class CCFlagSet {

    string name;
    std::map<string, CCFlag> formal;

    public:
        void parse(char **argv);

        void usage();

        void bool_var(bool *p, string name, string defvalue, string usage);

        void int_var(int *p, string name, string defvalue, string usage);

        void double_var(double *p, string name, string defvalue, string usage);

        void string_var(string *p, string name, string defvalue, string usage);
    private:

        void var(void *p, string name, string value, string usage, std::function<bool (string value)> set, bool isbool);

        bool parse_one();

        void failf(const char *fmt, ...);

        char **argv;

        char err_buf[512];
};

void CCFlagSet::usage() {
}

void CCFlagSet::failf(const char *fmt, ...) {
    va_list ap;

    va_start(ap, fmt);
    vsnprintf(err_buf, sizeof(err_buf), fmt, ap);
    va_end(ap);

    fprintf(stdout, "%s\n", err_buf);

    //cflag_usage(c);
}

bool CCFlagSet::parse_one() {
    if (argv[0] == nullptr) {
        return false;
    }


    string s = argv[0];

    if (s.size() < 2 || s[0] != '-') {
        return false;
    }

    auto num_minuses = 1;

    if (s.c_str()[1] == '-' ){
        num_minuses++;
        if (s.size() == 2) {
            argv = ++argv;
            return false;
        }
    }

    auto name = string(s.c_str() + num_minuses);

    if (name.size() == 0 || name.c_str()[0] == '-' || name.c_str()[0] == '=') {
        return failf("bad ccflag syntax: %s", s.c_str()), false;
    }

    argv = ++argv;

    auto has_value = false;
    string value = "";

    char *pos;
    if ((pos = strchr((char *)s.c_str(), '=')) != nullptr) {
        value = string(pos + 1);
        has_value = true;
        *pos = '\0';
        name = string(name.c_str());
    }

    auto v = formal.count(name);
    if (v <= 0) {
        if (name == "help" || name == "h") {
            usage();
            return failf("%s", "ccflag: help requested"), false;
        }
    }

    auto flag = formal[name];

    if (flag.isbool) {
        if (has_value) {
            if(!flag.set(value)) {
                return failf("invalid boolean value %s for -%s", value, name), false;
            }
        } else {
            if (flag.set("true") == false) {
                failf("invalid boolean ccflag %s", name), false;
            }
        }

    } else {
        if (!has_value && argv[0] != nullptr) {
            has_value = true;
            value = argv[0], argv = ++argv;
        }

        if (!has_value) {
            return failf("ccflag needs an argument: -%s", name), false;
        }

        if (!flag.set(value)) {
            return failf("invalid value %s for flag -%s", value, name), false;
        }
    }

    return true;
}

void CCFlagSet::parse(char **argv) {
    argv = argv;

    for (;;) {

        if(parse_one()) {
            continue;
        }
    }
}

void CCFlagSet::var(void *p, string name, string defvalue, string usage, std::function<bool (string value)> set, bool isbool) {
    CCFlag flag = {p, name, defvalue, usage, set, isbool};

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
    auto f = [=](string value) ->bool {
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

    var(p, name, defvalue, usage, f, true);
}

void CCFlagSet::int_var(int *p, string name, string defvalue, string usage) {
    auto f = [=](string value) ->bool {
        char *endptr = NULL;

        int   val = strtol(value.c_str(), &endptr, 0);

        if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN))
                || (errno != 0 && val == 0)) {
            return false;
        }

        if (endptr == value.c_str()) {
            return false;
        }

        *p = val;
    };

    var(p, name, defvalue, usage, f, false);
}

void CCFlagSet::double_var(double *p, string name, string defvalue, string usage) {
    auto f = [=](string value) -> bool {

        char  *endptr = NULL;
        double val   = 0;

        val = strtod(value.c_str(), &endptr);

        if (endptr == value.c_str()) {
            return false;
        }

        *p = val;
    };

    var(p, name, defvalue, usage, f, false);
}

void CCFlagSet::string_var(string *p, string name, string defvalue, string usage) {
    auto f = [=](string value) -> bool {
        *p = value;
    };

    var(p, name, defvalue, usage, f, false);
}
#endif
