#include "ccflag.h"

int main() {
    CCFlagSet set;

    bool b;
    set.bool_var(&b, "bool", "false", "bool value");
    return 0;
}
