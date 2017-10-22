#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define MAXLINE 256


typedef void (*dict_func)(void);

typedef struct dict {
    /// The name of the word to be executed
    char *wordn;

    /// Function to execute
    dict_func w;

    /// Next in the dictionary list
    struct dict *next;
} dict_t;

typedef struct compiled_word {
    /// Data if needed for the function
    int data;

    int flags;

    /// word to execute
    dict_t *e;

    /// Next thing to execute
    struct compiled_word *next;
} compiled_word_t;

typedef struct stack {
    int data;
    struct stack *next;
} stack_t;

/**
 Start of the global dictionary
**/
dict_t *dict_g = NULL;

/// Last token pulled from the line
char *last_tok = NULL;

/// Compiled line to execute.
compiled_word_t *exec_list = NULL;

/// Current pc
compiled_word_t *pc = NULL;

/// line read in for compilation
char *comp_line = NULL;


#if defined(V0)
char a = 0;
char b = 0;
int data_stack_g[100] = {0};
int stack_index_g = 0;

#elif defined(V1)
/**
 This is the root for the global stack
 **/
stack_t *data_stack_g = NULL;


#endif

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
        if( i > MAXLINE )
            break;        

        if ( read(line+i, 1) == 0 ) {
            wait();
            continue;
       }        

       if ( line[i] == '\n' ) {
           line[i] = '\x00';
           return line;
       }

       i += 1;
    }

    return line;
}

/**
 Pop data from the stack
 No args
 Returns the next value from the stack or NULL on failure
**/
#if defined(V1)
stack_t *pop( )
#elif defined(V0)
int pop()
#endif
{
    #if defined(V1)
        stack_t *pold = data_stack_g;

        if ( data_stack_g != NULL ) {
            data_stack_g = data_stack_g->next;
        }

        return pold;
    #elif defined(V0)
        int value = data_stack_g[--stack_index_g];

        return value;
    #endif
}

void pushint( int i )
{
    int *t = data_stack_g;
        data_stack_g[stack_index_g] = i;
        stack_index_g += 1;
        return;
}

/// Pop two values from the stack. add them then push the result
void plus( )
{
    int one = 0;
    int two = 0;
    int *rez = NULL;

    #if defined(V1)
        stack_t *pone = NULL;
        stack_t *ptwo = NULL;
    #elif defined(V0)
        int pone = 0;
        int ptwo = 0;
    #endif

    pone = pop();

    #if defined(V1)
        if (pone == NULL ) {
            puts("[-] stack pop failed.\n");
            goto end;
        }
    #endif

    ptwo = pop();

    pushint( pone + ptwo );

end:
    pc = pc->next;
    return;
}

void sub( )
{
    int one = 0;
    int two = 0;
    int *rez = NULL;

    #if defined(V1)
        stack_t *pone = NULL;
        stack_t *ptwo = NULL;
    #elif defined(V0)
        int pone = 0;
        int ptwo = 0;
    #endif

    pone = pop();

    #if defined(V1)
        if (pone == NULL ) {
            puts("[-] stack pop failed.\n");
            goto end;
        }
    #endif

    ptwo = pop();

    pushint( pone - ptwo );

end:
    pc = pc->next;
    return;
}

void mul( )
{
    int one = 0;
    int two = 0;
    int *rez = NULL;

    #if defined(V1)
        stack_t *pone = NULL;
        stack_t *ptwo = NULL;
    #elif defined(V0)
        int pone = 0;
        int ptwo = 0;
    #endif

    pone = pop();

    #if defined(V1)
        if (pone == NULL ) {
            puts("[-] stack pop failed.\n");
            goto end;
        }
    #endif

    ptwo = pop();

    pushint( pone * ptwo );

end:
    pc = pc->next;
    return;
}

void divi( )
{
    int one = 0;
    int two = 0;
    int *rez = NULL;

    #if defined(V1)
        stack_t *pone = NULL;
        stack_t *ptwo = NULL;
    #elif defined(V0)
        int pone = 0;
        int ptwo = 0;
    #endif

    pone = pop();

    #if defined(V1)
        if (pone == NULL ) {
            puts("[-] stack pop failed.\n");
            goto end;
        }
    #endif

    ptwo = pop();

        if ( ptwo == 0 ) {
            puts("[-] invalid div\n");
            goto end;
        }
        pushint( pone / ptwo );

end:
    pc = pc->next;
    return;
}

void less( )
{
    int one = 0;
    int two = 0;
    int *rez = NULL;

    #if defined(V1)
        stack_t *pone = NULL;
        stack_t *ptwo = NULL;
    #elif defined(V0)
        int pone = 0;
        int ptwo = 0;
    #endif

    pone = pop();

    #if defined(V1)
        if (pone == NULL ) {
            puts("[-] stack pop failed.\n");
            goto end;
        }
    #endif

    ptwo = pop();

    pushint( pone < ptwo );

end:
    pc = pc->next;
    return;
}

void lessequal( )
{
    int one = 0;
    int two = 0;
    int *rez = NULL;

    #if defined(V1)
        stack_t *pone = NULL;
        stack_t *ptwo = NULL;
    #elif defined(V0)
        int pone = 0;
        int ptwo = 0;
    #endif

    pone = pop();

    #if defined(V1)
        if (pone == NULL ) {
            puts("[-] stack pop failed.\n");
            goto end;
        }
    #endif

    ptwo = pop();

    pushint( pone <= ptwo );

end:
    pc = pc->next;
    return;
}

void greater( )
{
    int one = 0;
    int two = 0;
    int *rez = NULL;

    #if defined(V1)
        stack_t *pone = NULL;
        stack_t *ptwo = NULL;
    #elif defined(V0)
        int pone = 0;
        int ptwo = 0;
    #endif

    pone = pop();

    #if defined(V1)
        if (pone == NULL ) {
            puts("[-] stack pop failed.\n");
            goto end;
        }
    #endif

    ptwo = pop();

    pushint( pone > ptwo );

end:
    pc = pc->next;
    return;
}

void greaterequal( )
{
    int one = 0;
    int two = 0;
    int *rez = NULL;

    #if defined(V1)
        stack_t *pone = NULL;
        stack_t *ptwo = NULL;
    #elif defined(V0)
        int pone = 0;
        int ptwo = 0;
    #endif

    pone = pop();

    #if defined(V1)
        if (pone == NULL ) {
            puts("[-] stack pop failed.\n");
            goto end;
        }
    #endif

    ptwo = pop();

    pushint( pone >= ptwo );

end:
    pc = pc->next;
    return;
}

void equal( )
{
    int one = 0;
    int two = 0;
    int *rez = NULL;

    #if defined(V1)
        stack_t *pone = NULL;
        stack_t *ptwo = NULL;
    #elif defined(V0)
        int pone = 0;
        int ptwo = 0;
    #endif

    pone = pop();

    #if defined(V1)
        if (pone == NULL ) {
            puts("[-] stack pop failed.\n");
            goto end;
        }
    #endif

    ptwo = pop();

    pushint( pone == ptwo );

end:
    pc = pc->next;
    return;
}


/// Duplicate the top entry on the stack
void dups( )
{
        int p = 0;

        p = pop();

        pushint(p);
        pushint(p);

end:
    pc = pc->next;
    return;
}


/// discards the top value of the stack
void drop()
{
    pop();

    pc = pc->next;
    return;
}

/// swap the top two values on the stack
void swap()
{
        int one = pop();
        int two = pop();

        pushint(one);
        pushint(two);

end:
    pc = pc->next;
    return;
}

/**
 Stack function to push data
 Takes a pointer to any data. This is stored in the struct
 Returns a pointer to the new stack structure. Use this for errors
**/
void push(  )
{
        pushint(pc->data);
    
    pc = pc->next;

    return;
}

/// Returns the first instance of an entry in the dictionary or NULL if not found
dict_t *get_dict( char *name )
{
    dict_t *walker = dict_g;

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
    dict_t *push_lit = NULL;

    /// Allocate the literal word
    new_lit = malloc( sizeof( compiled_word_t ) );

    if ( !new_lit ) {
        puts("[-] malloc fail.");
        return NULL;
    }

    push_lit = malloc( sizeof( dict_t ) );

    if ( !push_lit ) {
        puts("[-] malloc fail.");
        return NULL;
    }

    memset( push_lit, 0, sizeof(dict_t) );
    memset( new_lit, 0, sizeof(compiled_word_t) );

    new_lit->data = value;
    new_lit->e = push_lit;

    /// Set to 1 so that it will free the dict entry
    new_lit->flags = 1;

    push_lit->wordn = "literal";
    push_lit->w = &push;

    return new_lit;
}

void add_exec_link( compiled_word_t *new_link)
{
    compiled_word_t *walker = NULL;

    if ( !new_link ) {
        return;
    }

    if ( exec_list == NULL ) {
        exec_list = new_link;
        return;
    }

    walker = exec_list;

    while ( walker->next != NULL ) {
        walker = walker->next;
    }

    walker->next = new_link;

    return;
}

// Display the values on the stack
void print_stack()
{
    #if defined(V1)
        stack_t *walker = data_stack_g;
        int i = 0;

        while ( walker ) {
            printf("%d ", walker->data);
            walker = walker->next;
        }
    #elif defined(V0)
        int i = stack_index_g - 1;

        while ( i >= 0 ) {
            printf("%d ", data_stack_g[i]);
            i -= 1;
        }
    #endif

    printf("\n");

    pc = pc->next;
}

void execute( )
{
    pc = exec_list;
    int i = 0;

    while ( pc ) {
        pc->e->w();
    }

    /// Free the execution list
    while ( exec_list ) {
        pc = exec_list;
        exec_list = exec_list->next;

        if ( pc->flags && 1 ) {
            free(pc->e);
        }

        free(pc);
    }

    pc = NULL;

    return;
}

compiled_word_t *make_exec( dict_t *e, int data )
{
    compiled_word_t *ct = NULL;

    if ( e == NULL ) {
        return NULL;
    }

    ct = malloc( sizeof(compiled_word_t) );

    if ( ct == NULL ) {
        printf("[-] malloc fail\n");
        return NULL;
    }

    memset(ct, 0, sizeof(compiled_word_t));

    ct->e = e;
    ct->data = data;

    return ct;
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

void freeexec()
{
    pc = exec_list;


    /// Free the execution list
    while ( exec_list ) {
        pc = exec_list;
        exec_list = exec_list->next;

        if ( pc->flags && 1 ) {
            free(pc->e);
        }

        free(pc);
    }

    pc = NULL;
    return;
}

int compile(  )
{
    dict_t *di = NULL;
    compiled_word_t *lit = NULL;
    int rv = 0;
    int value = 0;

    last_tok = strtok( comp_line, " " );

    while ( last_tok != NULL ) {
        di = get_dict(last_tok);

        /// If the compile function does not exist determine if it is an integer
        //// If so then add it to the stack. Otherwise error out.
        if ( di == NULL ) {
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
                freeexec();
                return 0;
            }

            add_exec_link( lit );
        } else {
            lit = make_exec( di, 0);

            if ( lit == NULL ) {
                freeexec();
                return 0;
            }

            add_exec_link( lit );
        }

        last_tok = strtok( NULL, " ");
    }

    execute();
    return 1;
}

/// Takes the name of the dictionary value and the function to execute.
void add_dict( char *name, dict_func f)
{
    dict_t *newd = NULL;
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

    newd = malloc(sizeof(dict_t));

    if ( newd == NULL ) {
        puts("[-] malloc failure.\n");
        return;
    }

    memset( newd, 0, sizeof(dict_t));


    newd->wordn = malloc( length+1);

    if ( newd->wordn == NULL ) {
        puts("[-] malloc failed.\n");
        free(newd);
        return;
    }

    memset( newd->wordn, 0, length+1);

    memcpy( newd->wordn, name, length);

    newd->w = f;

    newd->next = dict_g;
    dict_g = newd;

    return;
}

/// Pops a value off the stack and prints it as a char
void putch( )
{
    #if defined(V1)
        stack_t *p = pop();

        if ( p == NULL ) {
            printf("[-] empty stack\n");
            goto end;
        }

        printf("%c", p->data);
        free(p);
    #elif defined(V0)
        int i = pop();
        printf("%c", i);
    #endif

end:
    pc = pc->next;
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

    add_dict( "+", &plus);

    add_dict( "-", &sub);

    add_dict( "*", &mul);

    add_dict( "/", &divi);

    add_dict( "<", &less);

    add_dict( "<=", &lessequal);

    add_dict( ">", &greater);

    add_dict( ">=", &greaterequal);

    add_dict( "==", &equal);

    add_dict("dup", &dups);

    add_dict("quit", &quit);

    add_dict("drop", &drop);

    add_dict("swap", &swap);

    add_dict(".", &putch);

    add_dict("ps", &print_stack);

    return;
}

int main( int argc, char **argv)
{
    init();

    while ( 1 ) {
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
