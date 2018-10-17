#ifndef __CCFLAG_H

#include <functional>
#include <map>
#include <string>
#include <string.h>
#include <iostream>
#include <stdlib.h>
#include <limits.h>
#include <stdarg.h>
#include <stdlib.h>

using string = std::string;

enum on_error_type{continue_on_error, exit_on_error, panic_on_error};

class CCFlag{
    public:
        void  *val;
        string name;
        string defvalue;
        string usage;

        std::function<bool (string value)> set;

        bool isbool;

        string type_name;
};

class CCFlagSet {

    string name;
    std::map<string, CCFlag> formal;
    int error_handling;

    public:

        CCFlagSet(string name, on_error_type error_handling);

        string parse(char **argv);

        void usage();

        void bool_var(bool *p, string name, string defvalue, string usage);

        void int_var(int *p, string name, string defvalue, string usage);

        void double_var(double *p, string name, string defvalue, string usage);

        void string_var(string *p, string name, string defvalue, string usage);
    private:

        void var(void *p, string name, string value, string usage,
                std::function<bool (string value)> set,
                bool isbool, string type_name);

        bool parse_one();

        void failf(const char *fmt, ...);

        char **argv;

        char err_buf[512];

        void default_usage();
};

CCFlagSet::CCFlagSet(string name, on_error_type error_handling){
    this->name          = name;
    this->error_handling = error_handling;
    memset(this->err_buf, 0, sizeof(this->err_buf));
}

void CCFlagSet::default_usage() {
    if (this->name == "") {
		fprintf(stderr, "Usage:\n");
	} else {
		fprintf(stderr, "Usage of %s:\n", this->name.c_str());
	}

    auto replace_all = [](std::string str, const std::string& from, const std::string& to) -> string {
        if(from.empty())
            return "";
        size_t start_pos = 0;

        auto retstr = str;
        while((start_pos = retstr.find(from, start_pos)) != std::string::npos) {
            retstr.replace(start_pos, from.length(), to);
            start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
        }
        return retstr;
    };

    for (auto kv : this->formal) {
        auto name  = kv.second.name;
        auto usage = kv.second.usage;
        auto s     = "    -" + name;

        if (s.size() > 0) {
            s += " " + name;
        }

        if (s.size() <= 4) {
            s += "\t";
        } else {
            s += "\n    \t";
        }

        s += replace_all(usage, "\n", "\n    \t");

        auto defvalue = kv.second.defvalue;
        if (kv.second.isbool && defvalue.size() == 0) {
            defvalue = "false";
        }

        if (defvalue.size() > 0) {
            s += " (default " + defvalue + ")";
        }

        fprintf(stderr, "%s\n", s.c_str());
    }
}

void CCFlagSet::usage() {
    return default_usage();
}

void CCFlagSet::failf(const char *fmt, ...) {
    va_list ap;

    va_start(ap, fmt);
    vsnprintf(err_buf, sizeof(err_buf), fmt, ap);
    va_end(ap);

    fprintf(stdout, "%s\n", err_buf);

    usage();
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
            ++this->argv;
            return false;
        }
    }

    auto name = string(s.c_str() + num_minuses);

    if (name.size() == 0 || name.c_str()[0] == '-' || name.c_str()[0] == '=') {
        return failf("bad ccflag syntax: %s", s.c_str()), false;
    }

    ++this->argv;

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
            snprintf(this->err_buf, sizeof(this->err_buf), "ccflag: help requested");
            return false;
        }
        return failf("flag provided but not defined: -%s", name.c_str()), false;
    }

    auto flag = formal[name];

    if (flag.isbool) {
        if (has_value) {
            if(!flag.set(value)) {
                return failf("invalid boolean value %s for -%s", value.c_str(), name.c_str()), false;
            }
        } else {
            if (flag.set("true") == false) {
                return failf("invalid boolean ccflag %s", name.c_str()), false;
            }
        }

    } else {
        if (!has_value && argv[0] != nullptr) {
            has_value = true;
            value = argv[0], ++this->argv;
        }

        if (!has_value) {
            return failf("ccflag needs an argument: -%s", name.c_str()), false;
        }

        if (!flag.set(value)) {
            return failf("invalid value %s for flag -%s", value.c_str(), name.c_str()), false;
        }
    }

    return true;
}

string CCFlagSet::parse(char **argv) {
    this->argv = argv;

    for (;;) {

        if(parse_one()) {
            continue;
        }

        if (this->err_buf[0] == '\0') {
            break;
        }

        switch (this->error_handling) {
            case continue_on_error:
                return string(this->err_buf);
            case exit_on_error:
                exit(2);
            case panic_on_error:
                abort();
        }
    }

    return "";
}

void CCFlagSet::var(void *p, string name, string defvalue, string usage, std::function<bool (string value)> set,
        bool isbool, string type_name) {

    CCFlag flag = {p, name, defvalue, usage, set, isbool, type_name};

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

    flag.set(defvalue);
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

            if (str =="false" || str == "FALSE" ||str == "False") {
                return 0;
            }

            return -1;
        };

        auto b = parse_bool(value);
        if (b == -1) {
            return false;
        }

        *p = (b == 1) ? true : false;
        return true;
    };

    var(p, name, defvalue, usage, f, true, "bool");
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
        return true;
    };

    var(p, name, defvalue, usage, f, false, "int");
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
        return true;
    };

    var(p, name, defvalue, usage, f, false, "double");
}

void CCFlagSet::string_var(string *p, string name, string defvalue, string usage) {
    auto f = [=](string value) -> bool {
        *p = value;
        return true;
    };

    var(p, name, defvalue, usage, f, false, "string");
}
#endif
