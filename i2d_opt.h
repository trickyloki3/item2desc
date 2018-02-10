#ifndef i2d_opt_h
#define i2d_opt_h

#include "i2d_util.h"

struct i2d_option {
    i2d_str * item_db_path;
};

typedef struct i2d_option i2d_option;

int i2d_option_init(i2d_option **, int, char **);
void i2d_option_deit(i2d_option **);
#endif