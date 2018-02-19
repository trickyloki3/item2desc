#include "i2d_logic.h"

int i2d_logic_init(i2d_logic ** result, i2d_str * name, i2d_range_list * range) {
    int status = I2D_OK;
    i2d_logic * object;

    if(i2d_is_invalid(result)) {
        status = i2d_panic("invalid paramater");
    } else {
        object = calloc(1, sizeof(*object));
        if(!object) {
            status = i2d_panic("out of memory");
        } else {
            if(i2d_str_init(&object->name, name->string, name->length)) {
                status = i2d_panic("failed to create string object");
            } else if(i2d_range_list_copy(&object->range, range)) {
                status = i2d_panic("failed to create range list object");
            } else {
                object->type = var;
            }

            if(status)
                i2d_logic_deit(&object);
            else
                *result = object;
        }
    }

    return status;
}

void i2d_logic_deit(i2d_logic ** result) {
    i2d_logic * object;

    object = *result;
    i2d_deit(object->right, i2d_logic_deit);
    i2d_deit(object->left, i2d_logic_deit);
    i2d_deit(object->range, i2d_range_list_deit);
    i2d_deit(object->name, i2d_str_deit);
    i2d_free(object);
    *result = NULL;
}

void i2d_logic_print(i2d_logic * logic, int level) {
    int i;

    if(logic) {
        for(i = 0; i < level; i++)
            putc('\t', stdout);
        switch(logic->type) {
            case var: fprintf(stdout, "[%s] ", logic->name->string); break;
            case and: fprintf(stdout, "[and] "); break;
            case or:  fprintf(stdout, "[or] ");  break;
            case not: fprintf(stdout, "[not] "); break;
        }
        if(logic->range)
            i2d_range_list_print(logic->range, NULL);
        else
            fprintf(stdout, "\n");

        i2d_logic_print(logic->left, level + 1);
        i2d_logic_print(logic->right, level + 1);
    }
}

int i2d_logic_link(i2d_logic ** result, i2d_logic * left, i2d_logic * right, int type) {
    int status = I2D_OK;
    i2d_logic * object;

    if(i2d_is_invalid(result)) {
        status = i2d_panic("invalid paramater");
    } else {
        object = calloc(1, sizeof(*object));
        if(!object) {
            status = i2d_panic("out of memory");
        } else {
            object->type = type;
            object->left = left;
            object->right = right;
            object->left->parent = object;
            object->right->parent = object;

            if(status)
                i2d_logic_deit(&object);
            else
                *result = object;
        }
    }

    return status;
}

int i2d_logic_var_copy(i2d_logic ** result, i2d_logic * logic) {
    int status = I2D_OK;

    if(var != logic->type) {
        status = i2d_panic("invalid paramater");
    } else {
        status = i2d_logic_init(result, logic->name, logic->range);
    }

    return status;
}

int i2d_logic_and_copy(i2d_logic ** result, i2d_logic * logic) {
    int status = I2D_OK;
    i2d_logic * left = NULL;
    i2d_logic * right = NULL;

    if(or != logic->type) {
        status = i2d_panic("invalid paramater");
    } else {
        switch(logic->left->type) {
            case var: status = i2d_logic_var_copy(&left, logic->left); break;
            case and: status = i2d_logic_and_copy(&left, logic->left); break;
            case or:  status = i2d_logic_or_copy(&left, logic->left);  break;
        }
        if(status) {
            status = i2d_panic("failed to copy left logic object");
        } else {
            switch(logic->right->type) {
                case var: status = i2d_logic_var_copy(&right, logic->right); break;
                case and: status = i2d_logic_and_copy(&right, logic->right); break;
                case or:  status = i2d_logic_or_copy(&right, logic->right);  break;
            }
            if(status) {
                status = i2d_panic("failed to copy right logic object");
            } else {
                if(i2d_logic_link(result, left, right, and))
                    status = i2d_panic("failed to link logic object");
                if(status)
                    i2d_logic_deit(&right);
            }
            if(status)
                i2d_logic_deit(&left);
        }
    }

    return status;
}

int i2d_logic_or_copy(i2d_logic ** result, i2d_logic * logic) {
    int status = I2D_OK;
    i2d_logic * left = NULL;
    i2d_logic * right = NULL;

    if(or != logic->type) {
        status = i2d_panic("invalid paramater");
    } else {
        switch(logic->left->type) {
            case var: status = i2d_logic_var_copy(&left, logic->left); break;
            case and: status = i2d_logic_and_copy(&left, logic->left); break;
            case or:  status = i2d_logic_or_copy(&left, logic->left);  break;
        }
        if(status) {
            status = i2d_panic("failed to copy left logic object");
        } else {
            switch(logic->right->type) {
                case var: status = i2d_logic_var_copy(&right, logic->right); break;
                case and: status = i2d_logic_and_copy(&right, logic->right); break;
                case or:  status = i2d_logic_or_copy(&right, logic->right);  break;
            }
            if(status) {
                status = i2d_panic("failed to copy right logic object");
            } else {
                if(i2d_logic_link(result, left, right, or))
                    status = i2d_panic("failed to link logic object");
                if(status)
                    i2d_logic_deit(&right);
            }
            if(status)
                i2d_logic_deit(&left);
        }
    }

    return status;
}

int i2d_logic_copy(i2d_logic ** result, i2d_logic * logic) {
    int status = I2D_OK;

    switch(logic->type) {
        case var: status = i2d_logic_var_copy(result, logic); break;
        case and: status = i2d_logic_and_copy(result, logic); break;
        case or:  status = i2d_logic_or_copy(result, logic);  break;
    }

    return status;
}

int i2d_logic_var(i2d_logic ** result, i2d_logic * left, i2d_logic * right, int type) {
    int status = I2D_OK;
    i2d_range_list * range = NULL;
    i2d_logic * left_copy = NULL;
    i2d_logic * right_copy = NULL;

    if(var != left->type || var != right->type) {
        status = i2d_panic("invalid paramater");
    } else if(!strcmp(left->name->string, right->name->string)) {
        switch(type) {
            case and: status = i2d_range_list_and(&range, left->range, right->range); break;
            case or:  status = i2d_range_list_or(&range, left->range, right->range);  break;
        }
        if(status) {
            status = i2d_panic("failed to merge range list");
        } else {
            if(i2d_logic_init(result, left->name, range))
                status = i2d_panic("failed to create logic object");
            i2d_range_list_deit(&range);
        }
    } else {
        if(i2d_logic_var_copy(&left_copy, left)) {
            status = i2d_panic("failed to create logic object");
        } else {
            if(i2d_logic_var_copy(&right_copy, right)) {
                status = i2d_panic("failed to create logic object");
            } else {
                if(i2d_logic_link(result, left_copy, right_copy, type))
                    status = i2d_panic("failed to link logic object");
                if(status)
                    i2d_logic_deit(&right_copy);
            }
            if(status)
                i2d_logic_deit(&left_copy);
        }
    }

    return status;
}
