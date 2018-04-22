#include "i2d_range.h"

#define min(x, y) ((x < y) ? x : y)
#define max(x, y) ((x < y) ? y : x)

int i2d_range_init(i2d_range ** result, long min, long max) {
    int status = I2D_OK;
    i2d_range * object;

    if(i2d_is_invalid(result)) {
        status = i2d_panic("invalid paramater");
    } else {
        object = calloc(1, sizeof(*object));
        if(!object) {
            status = i2d_panic("out of memory");
        } else {
            object->min = min;
            object->max = max;
            object->next = object;
            object->prev = object;

            if(status)
                i2d_range_deit(&object);
            else
                *result = object;
        }
    }

    return status;
}

void i2d_range_deit(i2d_range ** result) {
    i2d_range * object;

    object = *result;
    i2d_free(object);
    *result = NULL;
}

void i2d_range_append(i2d_range * x, i2d_range * y) {
    x->next->prev = y->prev;
    y->prev->next = x->next;
    x->next = y;
    y->prev = x;
}

void i2d_range_remove(i2d_range * x) {
    x->prev->next = x->next;
    x->next->prev = x->prev;
    x->next = x;
    x->prev = x;
}

int i2d_range_list_init(i2d_range_list ** result) {
    int status = I2D_OK;
    i2d_range_list * object;

    if(i2d_is_invalid(result)) {
        status = i2d_panic("invalid paramater");
    } else {
        object = calloc(1, sizeof(*object));
        if(!object) {
            status = i2d_panic("out of memory");
        } else {

            if(status)
                i2d_range_list_deit(&object);
            else
                *result = object;
        }
    }

    return status;
}

void i2d_range_list_deit(i2d_range_list ** result) {
    i2d_range_list * object;
    i2d_range * range;

    object = *result;
    if(object->list) {
        while(object->list != object->list->next) {
            range = object->list->next;
            i2d_range_remove(range);
            i2d_range_deit(&range);
        }
        i2d_range_deit(&object->list);
    }
    i2d_free(object);
    *result = NULL;
}

void i2d_range_list_print(i2d_range_list * list, const char * tag) {
    i2d_range * walk;

    if(list->list) {
        walk = list->list;
        if(tag)
            fprintf(stdout, "(%s) ", tag);
        do {
            fprintf(stdout, "[%ld,%ld]", walk->min, walk->max);
            walk = walk->next;
        } while(walk != list->list);
        fprintf(stdout, "\n");
    }
}

int i2d_range_list_add(i2d_range_list * list, long x, long y) {
    int status = I2D_OK;
    long min;
    long max;
    i2d_range * walk;
    i2d_range * range = NULL;

    if(!list->list) {
        if(i2d_range_init(&list->list, min(x, y), max(x, y)))
            status = i2d_panic("failed to create range object");
    } else {
        min = min(x, y);
        max = max(x, y);
        walk = list->list;
        while(walk->max < min - 1 && walk->next != list->list)
            walk = walk->next;

        if(max < walk->min - 1) {
            if(i2d_range_init(&range, min, max)) {
                status = i2d_panic("failed to create range object");
            } else {
                i2d_range_append(range, walk);
                if(walk == list->list)
                    list->list = range;
            }
        } else if(walk->max < min - 1) {
            if(i2d_range_init(&range, min, max)) {
                status = i2d_panic("failed to create range object");
            } else {
                i2d_range_append(walk, range);
            }
        } else {
            walk->min = min(walk->min, min);
            walk->max = max(walk->max, max);
            while(walk != walk->next && walk->max >= walk->next->min - 1) {
                range = walk->next;
                walk->max = max(walk->max, range->max);
                i2d_range_remove(range);
                i2d_range_deit(&range);
            }
        }
    }

    return status;
}

int i2d_range_list_copy(i2d_range_list ** result, i2d_range_list * list) {
    int status = I2D_OK;
    i2d_range_list * object = NULL;
    i2d_range * walk;

    if(i2d_is_invalid(result)) {
        status = i2d_panic("invalid paramater");
    } else {
        if(i2d_range_list_init(&object)) {
            status = i2d_panic("failed to create range list object");
        } else {
            if(list->list) {
                walk = list->list->next;
                if(i2d_range_list_add(object, list->list->min, list->list->max))
                    status = i2d_panic("failed to add range object");
                while(walk != list->list && !status) {
                    if(i2d_range_list_add(object, walk->min, walk->max))
                        status = i2d_panic("failed to add range object");
                    walk = walk->next;
                }
            }

            if(status) {
                i2d_range_list_deit(&object);
            } else {
                *result = object;
            }
        }
    }

    return status;
}

int i2d_range_list_negate(i2d_range_list ** result, i2d_range_list * list) {
    int status = I2D_OK;
    i2d_range_list * object = NULL;
    i2d_range * walk;

    if(i2d_is_invalid(result)) {
        status = i2d_panic("invalid paramater");
    } else {
        if(i2d_range_list_init(&object)) {
            status = i2d_panic("failed to create range list object");
        } else {
            if(list->list) {
                walk = list->list->next;
                if(i2d_range_list_add(object, -1 * list->list->min, -1 * list->list->max))
                    status = i2d_panic("failed to add range object");
                while(walk != list->list && !status) {
                    if(i2d_range_list_add(object, -1 * walk->min, -1 * walk->max))
                        status = i2d_panic("failed to add range object");
                    walk = walk->next;
                }
            }

            if(status) {
                i2d_range_list_deit(&object);
            } else {
                *result = object;
            }
        }
    }

    return status;
}

int i2d_range_list_bitnot(i2d_range_list ** result, i2d_range_list * list) {
    int status = I2D_OK;
    i2d_range_list * object = NULL;
    i2d_range * walk;

    if(i2d_is_invalid(result)) {
        status = i2d_panic("invalid paramater");
    } else {
        if(i2d_range_list_init(&object)) {
            status = i2d_panic("failed to create range list object");
        } else {
            if(list->list) {
                walk = list->list->next;
                if(i2d_range_list_add(object, ~list->list->min, ~list->list->max))
                    status = i2d_panic("failed to add range object");
                while(walk != list->list && !status) {
                    if(i2d_range_list_add(object, ~walk->min, ~walk->max))
                        status = i2d_panic("failed to add range object");
                    walk = walk->next;
                }
            }

            if(status) {
                i2d_range_list_deit(&object);
            } else {
                *result = object;
            }
        }
    }

    return status;
}

int i2d_range_list_merge(i2d_range_list * list, i2d_range * left_root, i2d_range * right_root, i2d_range_merge_cb merge_cb) {
    int status = I2D_OK;
    i2d_range * left;
    i2d_range * right;
    i2d_range * next = NULL;
    i2d_range * last = NULL;

    left = left_root;
    right = right_root;
    while(left && right && !status) {
        if(left->min < right->min) {
            next = left;
            left = left->next;
            if(left == left_root)
                left = NULL;
        } else {
            next = right;
            right = right->next;
            if(right == right_root)
                right = NULL;
        }

        if(merge_cb(list, &last, next))
            status = i2d_panic("failed to merge range");
    }

    while(left && !status) {
        next = left;
        left = left->next;
        if(left == left_root)
            left = NULL;

        if(merge_cb(list, &last, next))
            status = i2d_panic("failed to merge range");
    }

    while(right && !status) {
        next = right;
        right = right->next;
        if(right == right_root)
            right = NULL;

        if(merge_cb(list, &last, next))
            status = i2d_panic("failed to merge range");
    }

    return status;
}

int i2d_range_list_merge_or(i2d_range_list * list, i2d_range ** last, i2d_range * next) {
    int status = I2D_OK;

    if(!(*last)) {
        if(i2d_range_list_add(list, next->min, next->max)) {
            status = i2d_panic("failed to add range object");
        } else {
            *last = list->list->prev;
        }
    } else {
        if(next->min - 1 > (*last)->max) {
            if(i2d_range_list_add(list, next->min, next->max)) {
                status = i2d_panic("failed to add range object");
            } else {
                *last = list->list->prev;
            }
        } else {
            (*last)->min = min((*last)->min, next->min);
            (*last)->max = max((*last)->max, next->max);
        }
    }

    return status;
}

int i2d_range_list_merge_and(i2d_range_list * list, i2d_range ** last, i2d_range * next) {
    int status = I2D_OK;

    if(!(*last)) {
        *last = next;
    } else {
        if((*last)->min <= next->max && (*last)->max >= next->min) {
            if(i2d_range_list_add(list, max((*last)->min, next->min), min((*last)->max, next->max)))
                status = i2d_panic("failed to add range object");
        } else if(next->min <= (*last)->max && next->max >= (*last)->min) {
            if(i2d_range_list_add(list, max((*last)->min, next->min), min((*last)->max, next->max)))
                status = i2d_panic("failed to add range object");
        }

        if((*last)->max < next->max)
            *last = next;
    }

    return status;
}

int i2d_range_list_or(i2d_range_list ** result, i2d_range_list * left, i2d_range_list * right) {
    int status = I2D_OK;
    i2d_range_list * object = NULL;

    if(i2d_is_invalid(result)) {
        status = i2d_panic("invalid paramater");
    } else {
        if(i2d_range_list_init(&object)) {
            status = i2d_panic("failed to create range list object");
        } else {
            if(i2d_range_list_merge(object, left->list, right->list, i2d_range_list_merge_or))
                status = i2d_panic("failed to merge range list");

            if(status) {
                i2d_range_list_deit(&object);
            } else {
                *result = object;
            }
        }
    }

    return status;
}

int i2d_range_list_and(i2d_range_list ** result, i2d_range_list * left, i2d_range_list * right) {
    int status = I2D_OK;
    i2d_range_list * object = NULL;

    if(i2d_is_invalid(result)) {
        status = i2d_panic("invalid paramater");
    } else {
        if(i2d_range_list_init(&object)) {
            status = i2d_panic("failed to create range list object");
        } else {
            if(i2d_range_list_merge(object, left->list, right->list, i2d_range_list_merge_and))
                status = i2d_panic("failed to merge range list");

            if(status) {
                i2d_range_list_deit(&object);
            } else {
                *result = object;
            }
        }
    }

    return status;
}

int i2d_range_list_not(i2d_range_list ** result, i2d_range_list * list) {
    int status = I2D_OK;
    i2d_range_list * object = NULL;
    i2d_range * walk = NULL;

    if(i2d_is_invalid(result)) {
        status = i2d_panic("invalid paramater");
    } else {
        if(i2d_range_list_init(&object)) {
            status = i2d_panic("failed to create range list object");
        } else {
            if(list->list && list->list != list->list->next) {
                walk = list->list;
                do {
                    if(i2d_range_list_add(object, walk->max + 1, walk->next->min - 1))
                        status = i2d_panic("failed to add range object");
                    walk = walk->next;
                } while(walk->next != list->list && !status);
            } else if(i2d_range_list_add(object, 0, 0)) {
                status = i2d_panic("failed to add range object");
            }

            if(status) {
                i2d_range_list_deit(&object);
            } else {
                *result = object;
            }
        }
    }

    return status;
}

void i2d_range_list_get_range(i2d_range_list * list, long * min, long * max) {
    if(!list->list) {
        *min = 0;
        *max = 0;
    } else {
        *min = list->list->min;
        *max = list->list->prev->max;
    }
}

int i2d_range_list_compute(i2d_range_list ** result, i2d_range_list * left, i2d_range_list * right, int operator) {
    int status = I2D_OK;
    i2d_range_list * object = NULL;
    i2d_range * walk;

    long left_min;
    long left_max;
    long right_min;
    long right_max;

    i2d_range_list_get_range(left, &left_min, &left_max);
    i2d_range_list_get_range(right, &right_min, &right_max);

    switch(operator) {
        case '|' + '|':
            if(i2d_range_list_or(result, left, right))
                status = i2d_panic("failed to or range list operand");
            break;
        case '&' + '&':
            if(i2d_range_list_and(result, left, right))
                status = i2d_panic("failed to and range list operand");
            break;
        case '=' + '=':
            if(i2d_range_list_and(result, left, right))
                status = i2d_panic("failed to and range list operand");
            break;
        case '!' + '=':
            if(i2d_range_list_and(&object, left, right)) {
                status = i2d_panic("failed to and range list operand");
            } else {
                if(i2d_range_list_not(result, object))
                    status = i2d_panic("failed to invert range list operand");
                i2d_range_list_deit(&object);
            }
            break;
        case '<':
            right_max--;
        case '<' + '=':
            if(i2d_range_list_init(&object)) {
                status = i2d_panic("failed to create range list object");
            } else {
                if( right_max < left_min ?
                        i2d_range_list_add(object, 0, 0) :
                        i2d_range_list_add(object, left_min, right_max) ) {
                    status = i2d_panic("failed to add range object");
                } else if(i2d_range_list_and(result, left, object)) {
                    status = i2d_panic("failed to and range list operand");
                }
                i2d_range_list_deit(&object);
            }
            break;
        case '>':
            right_max++;
        case '>' + '=':
            if(i2d_range_list_init(&object)) {
                status = i2d_panic("failed to create range list object");
            } else {
                if( left_max < right_max ?
                        i2d_range_list_add(object, 0, 0) :
                        i2d_range_list_add(object, right_max, left_max) ) {
                    status = i2d_panic("failed to add range object");
                } else if(i2d_range_list_and(result, left, object)) {
                    status = i2d_panic("failed to and range list operand");
                }
                i2d_range_list_deit(&object);
            }
            break;
        case '+':
        case '-':
        case '*':
        case '/':
        case '%':
        case '>' + '>' + 'b':
        case '<' + '<' + 'b':
        case '&':
        case '|':
        case '^' + 'b':
            if(i2d_range_list_init(&object)) {
                status = i2d_panic("failed to create range list object");
            } else {
                if(left->list) {
                    walk = left->list;
                    do {
                        switch(operator) {
                            case '+': status = i2d_range_list_add(object, walk->min + right_min, walk->max + right_max); break;
                            case '-': status = i2d_range_list_add(object, walk->min - right_min, walk->max - right_max); break;
                            case '*': status = i2d_range_list_add(object, walk->min * right_min, walk->max * right_max); break;
                            case '/': status = i2d_range_list_add(object, right_min ? walk->min / right_min : 0, right_max ? walk->max / right_max : 0); break;
                            case '%': status = i2d_range_list_add(object, walk->min % right_min, walk->max % right_max); break;
                            case '>' + '>' + 'b': status = i2d_range_list_add(object, walk->min >> right_min, walk->max >> right_max); break;
                            case '<' + '<' + 'b': status = i2d_range_list_add(object, walk->min << right_min, walk->max << right_max); break;
                            case '&': status = i2d_range_list_add(object, walk->min & right_min, walk->max & right_max); break;
                            case '|': status = i2d_range_list_add(object, walk->min | right_min, walk->max | right_max); break;
                            case '^' + 'b': status = i2d_range_list_add(object, walk->min ^ right_min, walk->max ^ right_max); break;
                        }
                    } while(walk != left->list && !status);
                }

                if(status)
                    i2d_range_list_deit(&object);
                else
                    *result = object;
            }
            break;
    }

    return status;
}
