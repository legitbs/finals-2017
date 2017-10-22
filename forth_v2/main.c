#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define MAXLINE 256


typedef void (*dict_func)(void);

typedef struct compiled_word {
    /// the name of this word being executed.
    char *wordn;

    /// Data if needed for the function
    int data;

    int flags;

    /// Dictionary function to execute. This will only be used for the basic functionality
    dict_func w;

    /// Sub function to execute after the first. If this is a user
    ////    created word the first is probably just a ret
    struct compiled_word *subs;

    struct compiled_word *execnext;

    /// Next thing to execute
    struct compiled_word *next;
} compiled_word_t;

typedef struct stack {
    int data;
    struct stack *next;
} stack_t;

typedef struct retstack {
    compiled_word_t *return_pc;

    struct retstack *next_return;
} retstack;

/**
 This is the root for the global stack
 **/
stack_t *data_stack_g = NULL;

/**
 Start of the global dictionary
**/
compiled_word_t *dict_g = NULL;

/// Last token pulled from the line
char *last_tok = NULL;

/// Current pc
compiled_word_t *pc = NULL;

/// return stack
retstack *rs = NULL;

/// line read in for compilation
char *comp_line = NULL;

int user_array[256];

int error = 0;

/**
 Reads until a new line or MAXLINE is reached
**/
char *readline( )
{
    char *line = malloc(MAXLINE+1);
    int i = 0;

    if ( line == NULL ) {
        puts("[-] malloc failed.\n");
        return NULL;
    }

    memset( line, 0, MAXLINE);

    printf("> ");
    fflush(stdout);

    while( 1 ) {
        wait();

        while ( (i < MAXLINE) && (read(line+i, 1) > 0) ) {

            if ( line[i] == '\n') {
                line[i] = '\x00';
                return line;
            }

            i += 1;
        }
    }

    return line;
}

/**
 Pop data from the stack
 No args
 Returns the next value from the stack or NULL on failure
**/
stack_t *pop( )
{
        stack_t *pold = data_stack_g;

        if ( data_stack_g != NULL ) {
            data_stack_g = data_stack_g->next;
        }

        return pold;
}

void pushint( int i )
{
        stack_t *ns = malloc( sizeof(stack_t));

        if ( ns == NULL ) {
            puts("[-] malloc fail");
            return;
        }

        memset(ns, 0, sizeof(stack_t));

        ns->data = i;
        ns->next = data_stack_g;

        data_stack_g = ns;

        return;
}

/// Pop two values from the stack. add them then push the result
void plus( )
{
    int one = 0;
    int two = 0;
    int *rez = NULL;

    stack_t *pone = NULL;
    stack_t *ptwo = NULL;

    pone = pop();

    if (pone == NULL ) {
        puts("[-] stack pop failed.\n");
        error = 1;
        goto end;
    }

    ptwo = pop();

    if ( ptwo == NULL ) {
        pushint( pone->data);
        free(pone);
        puts("[-] stack pop failed\n");
        error = 1;
        goto end;
    }

    one = pone->data;
    two = ptwo->data;

    free(pone);
    free(ptwo);
    pushint( one + two );

end:
    return;
}

void sub( )
{
    int one = 0;
    int two = 0;
    int *rez = NULL;

    stack_t *pone = NULL;
    stack_t *ptwo = NULL;

    pone = pop();

    if (pone == NULL ) {
        puts("[-] stack pop failed.\n");
        error = 1;
        goto end;
    }

    ptwo = pop();

    if ( ptwo == NULL ) {
        pushint( pone->data);
        free(pone);
        puts("[-] stack pop failed\n");
        error = 1;
        goto end;
    }

    one = pone->data;
    two = ptwo->data;

    free(pone);
    free(ptwo);
    pushint( one - two );

end:
    return;
}

void mul( )
{
    int one = 0;
    int two = 0;
    int *rez = NULL;

    stack_t *pone = NULL;
    stack_t *ptwo = NULL;

    pone = pop();

    if (pone == NULL ) {
        puts("[-] stack pop failed.\n");
        error = 1;
        goto end;
    }

    ptwo = pop();

    if ( ptwo == NULL ) {
        pushint( pone->data);
        free(pone);
        puts("[-] stack pop failed\n");
        error = 1;
        goto end;
    }

    one = pone->data;
    two = ptwo->data;

    free(pone);
    free(ptwo);
    pushint( one * two );

end:
    return;
}

void divi( )
{
    int one = 0;
    int two = 0;
    int *rez = NULL;

    stack_t *pone = NULL;
    stack_t *ptwo = NULL;

    pone = pop();

    if (pone == NULL ) {
        puts("[-] stack pop failed.\n");
        error = 1;
        goto end;
    }

    ptwo = pop();

    if ( ptwo == NULL ) {
        pushint( pone->data);
        free(pone);
        puts("[-] stack pop failed\n");
        error = 1;
        goto end;
    }

    one = pone->data;
    two = ptwo->data;

    free(pone);
    free(ptwo);

    if ( two == 0 ) {
        puts("[-] invalid div\n");
        goto end;
    }
    pushint( one / two );

end:
    return;
}

void less( )
{
    int one = 0;
    int two = 0;
    int *rez = NULL;

    stack_t *pone = NULL;
    stack_t *ptwo = NULL;

    pone = pop();

    if (pone == NULL ) {
        puts("[-] stack pop failed.\n");
        error = 1;
        goto end;
    }

    ptwo = pop();

    if ( ptwo == NULL ) {
        pushint( pone->data);
        free(pone);
        puts("[-] stack pop failed\n");
        error = 1;
        goto end;
    }

    one = pone->data;
    two = ptwo->data;

    free(pone);
    free(ptwo);
    pushint( one < two );

end:
    return;
}

void lessequal( )
{
    int one = 0;
    int two = 0;
    int *rez = NULL;

    stack_t *pone = NULL;
    stack_t *ptwo = NULL;

    pone = pop();

    if (pone == NULL ) {
        puts("[-] stack pop failed.\n");
        error = 1;
        goto end;
    }

    ptwo = pop();

    if ( ptwo == NULL ) {
        pushint( pone->data);
        free(pone);
        puts("[-] stack pop failed\n");
        error = 1;
        goto end;
    }

    one = pone->data;
    two = ptwo->data;

    free(pone);
    free(ptwo);
    pushint( one <= two );

end:
    return;
}

void greater( )
{
    int one = 0;
    int two = 0;
    int *rez = NULL;

    stack_t *pone = NULL;
    stack_t *ptwo = NULL;

    pone = pop();

    if (pone == NULL ) {
        puts("[-] stack pop failed.\n");
        error = 1;
        goto end;
    }

    ptwo = pop();

    if ( ptwo == NULL ) {
        pushint( pone->data);
        free(pone);
        puts("[-] stack pop failed\n");
        error = 1;
        goto end;
    }

    one = pone->data;
    two = ptwo->data;

    free(pone);
    free(ptwo);
    pushint( one > two );

end:
    return;
}

void greaterequal( )
{
    int one = 0;
    int two = 0;
    int *rez = NULL;

    stack_t *pone = NULL;
    stack_t *ptwo = NULL;

    pone = pop();

    if (pone == NULL ) {
        puts("[-] stack pop failed.\n");
        error = 1;
        goto end;
    }

    ptwo = pop();

    if ( ptwo == NULL ) {
        pushint( pone->data);
        free(pone);
        puts("[-] stack pop failed\n");
        error = 1;
        goto end;
    }

    one = pone->data;
    two = ptwo->data;

    free(pone);
    free(ptwo);
    pushint( one >= two );

end:
    return;
}

void equal( )
{
    int one = 0;
    int two = 0;
    int *rez = NULL;

    stack_t *pone = NULL;
    stack_t *ptwo = NULL;

    pone = pop();

    if (pone == NULL ) {
        puts("[-] stack pop failed.\n");
        error = 1;
        goto end;
    }

    ptwo = pop();

    if ( ptwo == NULL ) {
        pushint( pone->data);
        free(pone);
        puts("[-] stack pop failed\n");
        error = 1;
        goto end;
    }

    one = pone->data;
    two = ptwo->data;

    free(pone);
    free(ptwo);
    pushint( one == two );

end:
    return;
}

void mod( )
{
    int one = 0;
    int two = 0;
    int *rez = NULL;

    stack_t *pone = NULL;
    stack_t *ptwo = NULL;

    pone = pop();

    if (pone == NULL ) {
        puts("[-] stack pop failed.\n");
        error = 1;
        goto end;
    }

    ptwo = pop();

    if ( ptwo == NULL ) {
        pushint( pone->data);
        free(pone);
        puts("[-] stack pop failed\n");
        error = 1;
        goto end;
    }

    one = pone->data;
    two = ptwo->data;

    free(pone);
    free(ptwo);

    if ( two == 0 ) {
        printf("[-] invalid ops\n");
        error = 1;
        goto end;
    }

    pushint( one % two );

end:
    return;
}

/// Duplicate the top entry on the stack
void dups( )
{
    stack_t *p = NULL;

    p = pop();

    if ( p == NULL ) {
        puts("[-] stack empty.");
        error = 1;
        goto end;
    }

    pushint(p->data);
    pushint(p->data);

    free(p);

end:
    return;
}

/// pops an address then reads an int from that location
/// The value is written to the stack
void read_value( )
{
    stack_t *addr = NULL;
    int value = NULL;
    int *loc = NULL;

    addr = pop();

    if ( addr == NULL ) {
        puts("[-] empty stack.");
        return;
    }

    loc = addr->data;

    if ( loc < &user_array || loc > (&user_array + (256*sizeof(int*) )) ) {
        puts("[-] invalid");
        free(addr);
        free(value);
        return;
    }

    value = loc[0];

    pushint(value);

    free(addr);

    return;
}

void duptwo( )
{
    stack_t *p = NULL;
    stack_t *q = NULL;

    p = pop();

    if ( p == NULL ) {
        puts("[-] stack empty.");
        error = 1;
        goto end;
    }

    q = pop();

    if ( q == NULL ) {
        puts("[-] not enough elements.");
        error = 1;
        pushint(p->data);
        free(p);
        goto end;
    }

    pushint(q->data);
    pushint(p->data);
    pushint(q->data);
    pushint(p->data);

    free(p);
    free(q);

end:
    return;
}

void rot()
{
    stack_t *p = NULL;
    stack_t *q = NULL;
    stack_t *r = NULL;

    p = pop();

    if ( p == NULL ) {
        puts("[-] stack empty.");
        error = 1;
        goto end;
    }

    q = pop();

    if ( q == NULL ) {
        puts("[-] not enough elements.");
        error = 1;
        pushint(p->data);
        free(p);
        goto end;
    }

    r = pop();

    if ( r == NULL ) {
        puts("[-] not enough elements.");
        error = 1;
        pushint(q->data);
        pushint(p->data);
        free(p);
        free(q);
        goto end;
    }


    pushint(q->data);
    pushint(p->data);
    pushint(r->data);

    free(p);
    free(q);
    free(r);

end:
    return;
}

/// discards the top value of the stack
void drop()
{
    stack_t * t = pop();

    if ( t ) {
        free(t);
    }

    t = NULL;

    return;
}

/// swap the top two values on the stack
void swap()
{
    stack_t *one = NULL;
    stack_t *two = NULL;

    one = pop();

    if ( one == NULL ) {
        puts("[-] empty stack.");
        error = 1;
        goto end;
    }

    two = pop();

    if ( two == NULL ) {
        puts("[-] not enough elements.");
        error = 1;

        pushint(one->data);
        free(one);
        goto end;
    }

    pushint(one->data);
    pushint(two->data);

    free(one);
    free(two);

end:
    return;
}

/**
 Stack function to push data
 Takes a pointer to any data. This is stored in the struct
 Returns a pointer to the new stack structure. Use this for errors
**/
void push(  )
{
    stack_t *pnew = NULL;

    if ( pc == NULL ) {
        puts("[-] Global error.");
        exit(1);
    }

    pnew = malloc( sizeof(stack_t));

    if ( pnew == NULL ) {
        puts("[-] malloc fail.\n");
        exit(1);
    }

    memset(pnew, 0, sizeof(stack_t));

    pnew->data = pc->data;
    pnew->next = data_stack_g;
    data_stack_g = pnew;

    return;
}

void ret()
{
    return;
}

void printua()
{
    unsigned int a = &user_array;

    printf("0x%x\n", a);

    return;
}

void pushua()
{
    unsigned int a = &user_array;

    pushint(a);

    return;
}

void incua()
{
    stack_t *addr = NULL;
    stack_t *index = NULL;

    addr = pop();

    if ( !addr ) {
        puts("[-] empty stack\n");
        error = 1;
        return;
    }

    index = pop();

    if ( !index ) {
        puts("[-] not enough stack values\n");
        error = 1;
        pushint(addr->data);
        free(addr);
        return;
    }

    addr->data += index->data * sizeof(int*);

    pushint( addr->data);

    free(addr);
    free(index);

    return;
}

void incuabyone()
{
    stack_t *addr = NULL;

    addr = pop();

    if ( !addr ) {
        puts("[-] empty stack\n");
        error = 1;
        return;
    }

    addr->data += sizeof(int*);

    pushint( addr->data);

    free(addr);

    return;
}

/// pops an address then a value off of the stack
/// The value is written to the address
void write_value( )
{
    stack_t *addr = NULL;
    stack_t *value = NULL;
    int *a = NULL;

    addr = pop();

    if ( addr == NULL ) {
        puts("[-] empty stack.");
        return;
    }

    value = pop();

    if ( value == NULL ) {
        puts("[-] not enough stack values");
        pushint(addr->data);
        free(addr);
        return;
    }

    a = addr->data;

    /* Here is the first really easy bug.
    if ( a < &user_array || a > (&user_array + (256*sizeof(int*) )) ) {
        puts("[-] invalid location");
        free(addr);
        free(value);
        return;
    }
    */
    a[0] = value->data;

    free(value);
    free(addr);

    return;
}

int ishex( char *data )
{
    int index = 0;
    int len = 0;

    if ( data == NULL ) {
        return -1;
    }

    len = strlen(data);

    if ( len == 0 ) {
        return -1;
    }

    if ( data[0] == '-' ) {
        index += 1;
    }

    if ( len-index >= 3 && data[index] == '0' && tolower(data[index+1]) == 'x') {
        index += 2;
    }

    while ( index < len ) {
        if ( isdigit(data[index]) || ( tolower(data[index]) >= 'a' && tolower(data[index]) <= 'f') ) {
            index += 1;
            continue;
        } else {
            return 0;
        }
    }

    return 1;
}

int isdecimal( char *data)
{
    int len = 0;
    int index = 0;

    if ( data == NULL ) {
        return -1;
    }

    len = strlen(data);

    if ( len == 0 ) {
        return -1;
    }

    if ( data[0] == '-' && len >= 2 ) {
        index += 1;
    }

    while ( index < len ) {
        if ( !isdigit( data[index] ) ) {
            return 0;
        }

        index += 1;
    }

    return 1;
}

int strtol( const char *s, int base )
{
    int value = 0;
    int length = 0;
    int i = 0;
    int multiplier = 1;
    int start = 0;

    if ( s == NULL ) {
        return 0;
    }

    if ( base != 10 && base != 16 ) {
        return 0;
    }

    if ( base == 16 ) {
        if ( s[0] == '0' && (s[1] == 'x'||s[1] == 'X') ) {
            length += 2;
            start = 2;
        }

        while ( s[length] != '\x00' && (isdigit(s[length]) || (tolower(s[length]) >= 'a' && tolower(s[length]) <= 'f')))
        {
            length++;
        }
    } else {
        if ( s[0] == '-') {
            length++;
        }

        while ( s[length] != '\x00' && (isdigit(s[length])))
        {
            length++;
        }
    }
    
    i = length - 1;

    if ( base == 10 ) {
        while ( i > 0 ) {

            value += (s[i] - 0x30) * multiplier;

            multiplier *= 10;
            i -= 1;
        }

        if ( s[i] == '-') {
            value = value * -1;
        } else {
            value += (s[i] - 0x30) * multiplier;
        }
    } else {
        while ( i >= start ) {
            if ( isdigit( s[i] ) ) {
                value += (s[i] - 0x30) * multiplier;
            } else {
                value += (tolower(s[i]) - 87) * multiplier;
            }

            i -= 1;
            multiplier *= 16;
        }
    }

    return value;
}

void handlethen()
{

    return;
}

compiled_word_t *check_for_then( compiled_word_t *root)
{
    compiled_word_t *walker = root;
    int depth = 1;

    while ( walker ) {
        if ( strcmp( walker->wordn, "if") == 0 ) {
            printf("[-] error\n");
            error = 1;
            return NULL;
        } else if ( strcmp(walker->wordn, "then") == 0 ) {
            depth -= 1;

            if ( !depth ) {
                return walker;
            }
        } else if ( strcmp( walker->wordn, "endif") == 0 ) {
            depth -= 1;

            if ( !depth ) {
                return NULL;
            }
        }

        walker = walker->execnext;
    }

    return NULL;
}

compiled_word_t *check_for_endif( compiled_word_t *root )
{
    compiled_word_t *walker = root;
    int depth = 1;

    while ( walker ) {
        if ( strcmp( walker->wordn, "if") == 0 ) {
            printf("[-] error\n");
            error = 1;
            return NULL;
        } else if ( strcmp(walker->wordn, "then") == 0 ) {
            depth -= 1;

            if ( !depth ) {
                return NULL;
            }
        } else if ( strcmp( walker->wordn, "endif") == 0 ) {
            depth -= 1;

            if ( !depth ) {
                return walker;
            }
        }

        walker = walker->execnext;
    }

    return NULL;
}

void handleif()
{
    int depth = 1;
    compiled_word_t *eli = NULL;
    compiled_word_t *endi = NULL;
    stack_t *selection = NULL;

    selection = pop();

    if ( selection == NULL ) {
        puts("[-] empty stack\n");
        error = 1;
        return;
    }

    /// if the value is 0 then skip to the elseif or endif
    if ( selection->data == 0 ) {
        eli = check_for_then(pc->execnext);

        if ( error ) {
            return;
        }

        if ( eli != NULL ) {
            endi = check_for_endif(eli->execnext);

            if ( error ) {
                return;
            }

            if ( endi == NULL ) {
                printf("[-] wrong\n");
                error = 1;
                return;
            }

            pc->execnext = eli;
            return;
        } else {
            endi = check_for_endif( pc->execnext );

            if ( error ) {
                return;
            }

            if ( endi != NULL ) {
                pc = endi;
                return;
            }
        }

    } else {
        eli = check_for_then( pc->execnext);

        if ( error ) {
            return;
        }

        if ( eli != NULL ) {
            endi = check_for_endif( eli->execnext );

            if ( error ) {
                return;
            }

            if ( endi != NULL ) {
                eli->execnext = endi->execnext;
                return;
            }

            printf("[-] nopes\n");
            error = 1;
            return;
        } else {
            endi = check_for_endif( pc->execnext );

            if ( error ) {
                return;
            }

            if ( endi == NULL ) {
                printf("[-] failure\n");
                error = 1;
                return;
            }
        }
    }

    return;
}

void dd()
{
    compiled_word_t *yo = pc->execnext;
    compiled_word_t *walker = NULL;
    compiled_word_t *check = NULL;

    if ( yo == NULL ) {
        //printf("no next thing\n");
        error = 1;
        return;
    }

    //printf("in dd: %s\n", yo->wordn);

    /// Base case
    if ( strcmp( dict_g->wordn, yo->wordn) == 0 ) {
        walker = dict_g;
        dict_g = dict_g->next;
        //printf("base Freeing: walker %x : word %x : %s\n", walker, walker->wordn, walker->wordn);

        free(walker->wordn);
        free(walker);
        /// I don't want execution to continue after a delete so this works.
        error = 1;
        return;
    }

    check = dict_g;
    walker = check->next;

    while ( walker ) {
        if ( strcmp( walker->wordn, yo->wordn) == 0 ) {
            //printf("base Freeing: walker %x : word %x : %s\n", walker, walker->wordn, walker->wordn);
            check->next = walker->next;
            //printf("Freeing: %x\n", walker);
            free(walker);
            error = 1;
            return;
        }

        check = walker;
        walker = check->next;
    }

    return;
}

char *realloc( char *s, int newlength )
{
    char *ns = NULL;

    if ( s == NULL ) {
        return NULL;
    }

    ns = malloc( newlength );

    if ( ns == NULL ) {
        return NULL;
    }

    /// Memory leak here
    memcpy( ns, s, newlength );
    free(s);

    return ns;
}

compiled_word_t *copy_word( compiled_word_t *orig)
{
    compiled_word_t *nt = NULL;

    if ( orig == NULL ) {
        return NULL;
    }

    nt = malloc( sizeof( compiled_word_t) );

    if ( nt == NULL) {
        printf("[-] malloc fail.\n");
        return NULL;
    }

    memset( nt, 0, sizeof( compiled_word_t) );

    nt->w = orig->w;
    nt->data = orig->data;
    nt->subs = orig->subs;
    nt->wordn = orig->wordn;

    /// Free this when the time comes.
    nt->flags = 1;

    /// we don't want to copy the link.
    nt->next = NULL;

    return nt;
}

void printstr( )
{
    int i = 0;
    char * z = pc->data;

    if (pc->data != 0 ) {
        printf("%s\n", pc->data);
    }

    return;
}

compiled_word_t *handle_string()
{
    char *newstr = NULL;
    int index = 0;
    int toklen = 0;
    int endfound = 0;
    compiled_word_t ncw;
    int length = sizeof(compiled_word_t);
    newstr = malloc(length);

    if ( newstr == NULL ) {
        return;
    }

    //printf("New string: %x\n", newstr);
    memset(newstr, 0, sizeof(compiled_word_t));

    last_tok = strtok( NULL, " ");

    while ( last_tok ) {
        if ( strcmp( last_tok, ".\"") == 0 ) {
            endfound = 1;
            break;
        }

        if ( (strlen( last_tok ) == 4 || strlen(last_tok) == 2) && ishex(last_tok) ) {
            toklen = strtol( last_tok, 16);

            if ( index < length ) {
                newstr[index] = toklen;
                index += 1;
            }

        } else {
            toklen = strlen(last_tok);

            if ( index + toklen + 1> length ) {
                newstr = realloc( newstr, (index + toklen) * 2);
                length = index + toklen * 2;
            }

            memcpy( newstr + index, last_tok, toklen);

            index += toklen;
            newstr[index] = 0x20;
            index = index + 1;
        }

        last_tok = strtok( NULL, " ");
    }

    if ( !endfound ) {
        printf("[-] gen failure\n");
        free(newstr);
        return NULL;
    }

    ncw.wordn = "phr";
    ncw.data = newstr;
    ncw.subs = NULL;
    ncw.next = NULL;
    ncw.flags = 0;
    ncw.w = printstr;

    return copy_word( &ncw );
}

/// Returns the first instance of an entry in the dictionary or NULL if not found
compiled_word_t *get_dict( char *name )
{
    compiled_word_t *walker = dict_g;

    while ( walker ) {
        if ( walker->wordn == NULL ) {
            puts("[-] some error\n");
            exit(1);
        }

        if ( strncmp( walker->wordn, name, MAXLINE-1) == 0 ) {
            return walker;
        }

        walker = walker->next;
    }

    return walker;
}


compiled_word_t *compile_literal( int value )
{
    compiled_word_t *new_lit = NULL;

    /// Allocate the literal word
    new_lit = malloc( sizeof( compiled_word_t ) );

    if ( !new_lit ) {
        puts("[-] malloc fail.");
        return NULL;
    }

    memset( new_lit, 0, sizeof(compiled_word_t) );

    /// Set the data for the literal
    new_lit->data = value;

    /// Set the execute function.
    new_lit->w = &push;

    /// Set to 1 so that it will free the dict entry
    new_lit->flags = 1;

    new_lit->wordn = "literal";

    return new_lit;
}

void add_exec_link( compiled_word_t **el, compiled_word_t *new_link)
{
    compiled_word_t *walker = NULL;

    if ( !new_link || !el ) {
        return;
    }

    if ( *el == NULL ) {
        *el = new_link;
        return;
    }

    walker = *el;

    while ( walker->execnext != NULL ) {
        walker = walker->execnext;
    }

    walker->execnext = new_link;

    return;
}

// Display the values on the stack
void print_stack()
{
    stack_t *walker = data_stack_g;
    int i = 0;

    while ( walker ) {
        printf("%d ", walker->data);
        walker = walker->next;
    }


    printf("\n");
}

void pushret( )
{
    retstack *nrs = malloc( sizeof(retstack) );

    if ( nrs == NULL ) {
        printf("[-] malloc fail\n");
        return;
    }

    memset( nrs, 0, sizeof(retstack));

    nrs->return_pc = pc;
    nrs->next_return = rs;
    rs = nrs;

    return;
}

void popret( )
{
    retstack *t = NULL;

    if ( rs != NULL ) {
        pc = rs->return_pc;

        t = rs;
        rs = rs->next_return;

        free(t);

        t = NULL;
    }

    return;
}

int execute( compiled_word_t *ex )
{
    pc = ex;

    error = 0;

    while ( pc ) {
        //printf("executing: %s\n", pc->wordn);

        if ( pc->w != NULL ) {
            //printf("Execing dict func\n");
            pc->w();

            if ( error == 1 ) {
                //printf("stopping execution\n");
                return 0;
            }
        }

        if ( pc->subs != NULL ) {
            //printf("\tHas subs so executing those\n");
            pushret();
            if ( execute( pc->subs ) == 0 ) {
                printf("sub failure\n");
                popret();
                return 0;
            }

            popret();
        }

        pc = pc->execnext;
    }

    pc = NULL;

    return 1;
}

void freeexec( compiled_word_t *exec_list)
{
    /// Free the execution list
    while ( exec_list ) {
        pc = exec_list;
        exec_list = exec_list->next;

        if ( pc->flags && 1 ) {
            free(pc);
        }
    }

    pc = NULL;
    return;
}

void add_word_link( compiled_word_t *root, compiled_word_t *link)
{
    compiled_word_t *walker = NULL;

    if ( !root || !link){
        return;
    }

    if ( root->subs == NULL ) {
        root->subs = link;
        return;
    }

    walker = root->subs;

    while ( walker->execnext ) {
        walker = walker->execnext;
    }

    walker->execnext = link;

    return;
}

compiled_word_t *compile_dict( )
{
    char *wordname = NULL;
    int length = 0;
    compiled_word_t *new_word = NULL;
    compiled_word_t *search_dict = NULL;
    compiled_word_t *temp = NULL;
    int semi_colon_found = 0;
    int rv = 0;
    int value = 0;

    last_tok = strtok( NULL, " ");

    if ( last_tok == NULL ) {
        return NULL;
    }

    length = strlen( last_tok );

    if ( length <= 0) {
        return NULL;
    }

    wordname = malloc( length + 1 );

    if ( wordname == NULL ) {
        printf("[-] malloc fail\n");
        return NULL;
    }

    memset( wordname, 0, length + 1);

    memcpy( wordname, last_tok, length );

    new_word = malloc( sizeof(compiled_word_t) );

    if ( new_word == NULL ) {
        printf("[-] malloc fail\n");
        free(wordname);
        return NULL;
    }

    //printf("new dict Word buf: %x\n", new_word);
    memset( new_word, 0, sizeof(compiled_word_t) );
    new_word->wordn = wordname;

    last_tok = strtok( NULL, " ");

    while ( last_tok != NULL ) {

        if ( strcmp( last_tok, ";") == 0 ) {
            semi_colon_found = 1;
            break;
        }

        search_dict = get_dict( last_tok );

        if ( search_dict != NULL ) {
            //temp = copy_word( search_dict );
            add_word_link( new_word, search_dict);
        } else {
            rv = isdecimal( last_tok );

            if ( rv == -1 ) {
                printf("[-] invalid\n");
                /// TODO: free the things
                return NULL;
            }

            if ( rv ) {
                value = strtol( last_tok, 10);
                temp = compile_literal( value );
            } else {
                rv = ishex( last_tok );

                if ( rv == -1 ) {
                    printf("[-] invalid\n");
                    return NULL;
                }

                if ( !rv ) {
                    printf("[-] invalid: %s\n", last_tok);
                    return NULL;
                }

                value = strtol( last_tok, 16);

                temp = compile_literal ( value );
            }

            if ( temp == NULL ) {
                /// TODO: free the things.
                return NULL;
            }

            add_word_link( new_word, temp);
        }

        last_tok = strtok( NULL, " ");
    }


    if ( semi_colon_found == 0 ) {
        /// free the things
        return NULL;
    }

    return new_word;
}

int compile(  )
{
    compiled_word_t *lit = NULL;
    int rv = 0;
    int value = 0;
    compiled_word_t *exec_list = NULL;

    last_tok = strtok( comp_line, " " );

    /// Check for creating a new word
    if ( strcmp( last_tok, ":") == 0 ) {

        lit = compile_dict();

        /// Add to dictionary
        if ( lit != NULL ) {
            lit->next = dict_g;
            dict_g = lit;
            return 1;
        }

        printf("[-] Failed\n");

        return 0;
    }

    while ( last_tok != NULL ) {
        lit = get_dict(last_tok);

        /// If the compile function does not exist determine if it is an integer
        //// If so then add it to the stack. Otherwise error out.
        if ( lit == NULL ) {

            if ( strcmp( last_tok, ".\"") == 0 ) {
                lit = handle_string();

                if ( lit == NULL ) {
                    freeexec(&exec_list);
                    return 0;
                }

                add_exec_link( &exec_list, lit );

                last_tok = strtok( NULL, " ");
                continue;
            }

            rv = isdecimal( last_tok );

            if ( rv == -1 ) {
                printf("[-] invalid\n");
                return 0;
            }

            if ( rv ) {
                value = strtol( last_tok, 10);
                lit = compile_literal( value );
            } else {
                rv = ishex( last_tok );

                if ( rv == -1 ) {
                    printf("[-] invalid\n");
                    return 0;
                }

                if ( !rv ) {
                    printf("[-] invalid: %s\n", last_tok);
                    return 0;
                }

                value = strtol( last_tok, 16);

                lit = compile_literal ( value );
            }

            if ( lit == NULL ) {
                freeexec(&exec_list);
                return 0;
            }

            add_exec_link( &exec_list, lit );
        } else {

            lit = copy_word( lit );

            if ( lit == NULL ) {
                printf("[-] some error\n");
                freeexec(&exec_list);
                return 0;
            }

            add_exec_link( &exec_list, lit );
        }

        last_tok = strtok( NULL, " ");
    }

    execute(exec_list);

    return 1;
}

/// Takes the name of the dictionary value and the function to execute.
void add_dict( char *name, dict_func f)
{
    compiled_word_t *newd = NULL;
    int length = 0;
    int fcount = 0;

    if ( name == NULL || f == NULL ) {
        return;
    }

    length = strlen(name);

    if (length == 0 ) {
        puts("[-] Zero length values are not permitted.\n");
        return;
    }

    newd = malloc(sizeof(compiled_word_t));

    if ( newd == NULL ) {
        puts("[-] malloc failure.\n");
        return;
    }

    memset( newd, 0, sizeof(compiled_word_t));

    newd->wordn = malloc( length+1);

    if ( newd->wordn == NULL ) {
        puts("[-] malloc failed.\n");
        free(newd);
        return;
    }

    /// set the words name
    memset( newd->wordn, 0, length+1);
    memcpy( newd->wordn, name, length);

    /// Set the compiled word function
    newd->w = f;

    /// Add the word to the global dictionary list.
    newd->next = dict_g;
    dict_g = newd;

    return;
}

/// Pops a value off the stack and prints it as a char
void putch( )
{
    stack_t *p = pop();

    if ( p == NULL ) {
        printf("[-] empty stack\n");
        goto end;
    }

    printf("%c", p->data);
    free(p);

end:
    return;
}

void quit()
{
    exit(1);

    return;
}


/// Initialize all of the default dictionary entries
void init ( )
{

    add_dict( "+", &plus);//

    add_dict( "-", &sub);//

    add_dict( "*", &mul);//

    add_dict( "%", &mod);//

    add_dict( "/", &divi);//

    add_dict( "<", &less);//

    add_dict( "qq", &write_value); //

    add_dict( "abs", &read_value); //

    add_dict( "<=", &lessequal);//

    add_dict( ">", &greater);//

    add_dict( ">=", &greaterequal);//

    add_dict( "==", &equal);//

    add_dict("dup", &dups);//

    add_dict("ad", &incua); //

    add_dict("dio", &incuabyone); //

    add_dict("2dup", &duptwo);//

    add_dict("quit", &quit);//

    add_dict("drop", &drop);//

    add_dict("rot", &rot);//

    add_dict("null", &ret);//

    add_dict("swap", &swap);//

    add_dict("dp", &pushua);//

    add_dict("dd", &dd);//

    add_dict("if", &handleif); //

    add_dict("then", &handlethen); //

    add_dict("endif", &ret); //

    add_dict("db", &printua);//

    add_dict(".", &putch);//

    add_dict("ps", &print_stack);//

    return;
}

int main( int argc, char **argv)
{
    init();

    while ( 1 ) {
        pc = NULL;
        comp_line = readline();

        if ( comp_line == NULL ) {
            exit(1);
        }

        /// TODO: this needs to throw an error
        compile();

        if ( comp_line) {
            free(comp_line);
            comp_line = NULL;
        }

    }

	return 0;
}
