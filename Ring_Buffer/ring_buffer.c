#include "ring_buffer.h"
#include <string.h>

void init_buffer(struct buffer_type* b, unsigned char* buffer){
    b->head = buffer;
    b->tail = buffer;
    b->buffer = buffer;
}

enum error_type empty_buffer(struct buffer_type *b){
    b->head = b->buffer;
    b->tail = b->buffer;
    enum error_type result = OK;
    return result;
}

int get_buffer_state(struct buffer_type *b, enum error_type *err){
    // Returns amount of entries within the given buffer, when it gets to the end of the reserved space, without finding the head of the buffer, the index loops to the start
    // of the reserved storage
    unsigned char* index = b->tail;
    int counter = 0;

    while(1){
        if(index == b->buffer + sizeof(unsigned char) * MAX_BUFFER){ // If index points to end of reserved space, set index to beginning of reserved space
            index = b->buffer;
        } else if(index == b->head){ // If index points to the end of the buffer (the location where the head will store new values)
            break;
        } else if(index > b->buffer + sizeof(unsigned char) * MAX_BUFFER || index < b->buffer){ // If index is outside of buffer
			*err = POINTER_ERROR;
			return -1;
		} else { 
            counter++;
            index += sizeof(unsigned char);
        }
    }

    return counter;
}

int add_char_to_buffer(struct buffer_type *b, unsigned char c, enum error_type *err){
    //TODO: Assignment said not to, but might be useful to implement ways to fix a pointer being out of bounds
    // Enter char on location that the head points towards, and update the head to new empty space after the character
    // If the head points to the end of the reserved storage, loop back to the start of said storage
	if(b->head + sizeof(unsigned char) == b->tail || (b->head == b->buffer + sizeof(unsigned char) * MAX_BUFFER && b->tail == b->buffer)){ // If buffer is full
		*err = BUFFER_FULL;
		return -1;
	} else if(b->head >= b->buffer + sizeof(unsigned char) * MAX_BUFFER){ // If head points to end of, or after, reserved space
		*b->buffer = c;
		b->head = b->buffer + sizeof(unsigned char);
	} else {
		*b->head = c;
		b->head += sizeof(unsigned char);
	}
	return 1;
}

char get_char_from_buffer(struct buffer_type *b, enum error_type *err){
    //TODO: Assignment said not to, but might be useful to implement ways to fix a pointer being out of bounds
    // Return char on location that the tail points towards, and update the tail to next character after the current one
    // If the tail points towards the end of the reserved storage, loop back to the start of said storage
    if(b->head == b->tail){ // If buffer is empty
        *err = EMPTY_BUFFER;
        return -1;
    } else if(b->tail >= b->buffer + sizeof(unsigned char) * MAX_BUFFER){ // If the tail is located outside (after) the reserved storage, loop to the start of the storage
        b->tail = b->buffer + sizeof(unsigned char);
        return *b->buffer;
    } else {
        b->tail += sizeof(unsigned char);
        return *(b->tail - sizeof(unsigned char));
    }
}

int add_string_to_buffer(struct buffer_type *b, unsigned char *s, enum error_type *err){
    //TODO: Assignment said not to, but might be useful to implement ways to fix a pointer being out of bounds
    // Add an entire char array to the buffer, if possible
    // Get leftover size in buffer
    int size = 0;

    if(b->head > b->tail){
        size = (((b->buffer + MAX_BUFFER * sizeof(unsigned char)) - b->head) + (b->tail - b->buffer))/sizeof(unsigned char);
    } else {
        size = (b->tail - b->head)/sizeof(unsigned char);
    }

    if(size < strlen((char*)s)){ // If size is not enough
        *err = BUFFER_FULL;
        return -1;
    } else {
        for(int i = 0; i < strlen((char*)s); i++){ // Add the given array char by char into the buffer
            add_char_to_buffer(b, *(s + sizeof(char) * i), err);
        }
        return strlen((char*)s);
    }
}

int  get_string_from_buffer(struct buffer_type *b, unsigned char *dest, int len, enum error_type *err){
    int counter = 0;

    while(counter < len){
        unsigned char temp_char = get_char_from_buffer(b, err);
        if(*err == EMPTY_BUFFER){ // If there's no more characters left to get
            *err = BUFFER_FULL;
            return counter;
        } else {
            dest[counter] = temp_char;
            counter++;
        }
    }
    return counter;
}
