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

    commandl_line.int_var(&cnf.nt,            "nt",         "0",     "(must)Maximum number of threads");
    commandl_line.bool_var(&cnf.debug,        "debug",      "false", "Open the server debug mode");
    commandl_line.string_var(&cnf.model_path, "model_path", "./",    "xxx engine directory");
    commandl_line.double_var(&cnf.pi,         "pi",         "0",     "pi");

    commandl_line.parse(argv + 1);

    if (cnf.nt == 0) {
        commandl_line.usage();
        return 1;
    }

    std::cout << "nt: " << cnf.nt << std::endl;
    std::cout << "debug: " << cnf.debug << std::endl ;
    std::cout << "model_path: " << cnf.model_path << std::endl ;
    std::cout << "pi: " << cnf.pi << std::endl ;

    return 0;
}
