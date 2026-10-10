#ifndef TRAVERSE_H
#define TRAVERSE_H

#include "options.h"

/*
 * Traverses given file/directory operands according to options.
 * If operand_count is 0, defaults to ".".
 * Returns 0 on success, >0 if any error occurred.
 */
int traverse_operands(int operand_count, char *operands[], const Options *opts);

#endif /* TRAVERSE_H */
