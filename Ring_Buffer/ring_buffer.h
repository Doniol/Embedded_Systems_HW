#define MAX_BUFFER 100
struct buffer_type{
    unsigned char* head;
    unsigned char* tail;
    unsigned char* buffer;
};

enum error_type{OK, BUFFER_OVER_FLOW, EMPTY_BUFFER, POINTER_ERROR, BUFFER_FULL};

void init_buffer(struct buffer_type* b, unsigned char* buffer);
enum error_type empty_buffer(struct buffer_type *b);
int get_buffer_state(struct buffer_type *b, enum error_type *err);
int add_char_to_buffer(struct buffer_type *b,  unsigned char c, enum error_type *err);
char get_char_from_buffer(struct buffer_type *b, enum error_type *err);
int add_string_to_buffer(struct buffer_type *b, unsigned char *s, enum error_type *err);
int  get_string_from_buffer(struct buffer_type *b, unsigned char *dest, int len, enum error_type *err);