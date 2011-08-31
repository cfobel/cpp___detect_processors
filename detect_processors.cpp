#include <iostream>
#include <string>
#include "ProcessorInfo.hpp"

using namespace std;

int main(int argc, char *argv[]) {
    ProcessorInfo pi;

    cout << " logical cpus: " << pi.get_logical() << endl;
    cout << "    cpu cores: " << pi.get_cores() << endl;
    cout << "hyper-threads: " << (pi.get_hyper_threads() ? "true" : "false") << endl;

    return 0;
}
