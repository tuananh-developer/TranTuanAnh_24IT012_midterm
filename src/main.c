#include <stdio.h>
#include <stdlib.h>
#include "options.h"
#include "traverse.h"

int main(int argc, char *argv[]) {
    Options opts;
    options_init(&opts);

    int opt_idx = options_parse(&opts, argc, argv);
    if (opt_idx < 0) {
        return 1;
    }

    int operand_count = argc - opt_idx;
    char **operands = argv + opt_idx;

    int status = traverse_operands(operand_count, operands, &opts);

    return status;
}
