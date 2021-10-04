#include <gtest/gtest.h>
#include "ring_buffer.h"


TEST(ring_buffer, init_test){
    unsigned char buffer[MAX_BUFFER];
    struct buffer_type ring_buffer;

	// Check if buffer is created correctly
    init_buffer(&ring_buffer, buffer);
    EXPECT_EQ(ring_buffer.head, buffer);
    EXPECT_EQ(ring_buffer.tail, buffer);
    EXPECT_EQ(ring_buffer.buffer, buffer);
}


TEST(ring_buffer, empty_test){
    unsigned char buffer[MAX_BUFFER];
    buffer[0] = 'a';
    buffer[1] = 'b';
	
    struct buffer_type ring_buffer;
    ring_buffer.head = buffer + sizeof(unsigned char) * 2;
    ring_buffer.tail = buffer;
    ring_buffer.buffer = buffer;

    empty_buffer(&ring_buffer);

	// Check if changed buffer has actually been reset to original data
    EXPECT_EQ(ring_buffer.head, buffer);
    EXPECT_EQ(ring_buffer.tail, buffer);
    EXPECT_EQ(ring_buffer.buffer, buffer);
}


TEST(regular_buffer, data_amount){
    unsigned char buffer[MAX_BUFFER];
    buffer[0] = 'a';
    buffer[1] = 'b';
	
    struct buffer_type ring_buffer;
    ring_buffer.head = buffer + sizeof(unsigned char) * 2;
    ring_buffer.tail = buffer;
    ring_buffer.buffer = buffer;

	// Check whether function correctly counts amount of entries in regular buffer
    enum error_type err = OK;
    EXPECT_EQ(get_buffer_state(&ring_buffer, &err), 2);

	// Check it again
    buffer[2] = 'a';
    buffer[3] = 'b';
    buffer[4] = 'a';
    buffer[5] = 'b';
    ring_buffer.head = buffer + sizeof(unsigned char) * 6;
    EXPECT_EQ(get_buffer_state(&ring_buffer, &err), 6);
}

TEST(ring_buffer, data_amount_or_fail){
    unsigned char buffer[MAX_BUFFER];
    buffer[0] = 'c';
    buffer[1] = 'd';
    buffer[98] = 'a';
    buffer[99] = 'b';
	
    struct buffer_type ring_buffer;
    ring_buffer.head = buffer + sizeof(unsigned char) * 2; // Point to array[1], where new values will be stored
    ring_buffer.tail = buffer + sizeof(unsigned char) * 98; // Point to array[10], where the values are stored    
    ring_buffer.buffer = buffer;

	// Check whether function correctly counts amount of entries in ring-buffer
    enum error_type err = OK;
    EXPECT_EQ(4, get_buffer_state(&ring_buffer, &err));
	
	// Check whether error returns correctly when pointer is wrong
	// When pointer is too high
    err = OK;
	ring_buffer.tail = buffer + sizeof(unsigned char) * (MAX_BUFFER + 1);
	EXPECT_EQ(-1, get_buffer_state(&ring_buffer, &err));
	EXPECT_EQ(err, POINTER_ERROR);
	// When pointer is too low
    err = OK;
	ring_buffer.tail = buffer - sizeof(unsigned char) * 10;
	EXPECT_EQ(-1, get_buffer_state(&ring_buffer, &err));
	EXPECT_EQ(err, POINTER_ERROR);
}


TEST(regular_buffer, add_char){
	unsigned char buffer[MAX_BUFFER];
    buffer[0] = 't';
    buffer[1] = 'e';
    buffer[2] = 's';
	
	struct buffer_type ring_buffer;
    ring_buffer.head = buffer + sizeof(unsigned char) * 3;
    ring_buffer.tail = buffer;
    ring_buffer.buffer = buffer;
    enum error_type err = OK;
	
	// Check whether character is correctly stored in regular buffer
	add_char_to_buffer(&ring_buffer, 't', &err);
	EXPECT_EQ(*(ring_buffer.head - sizeof(unsigned char) * 1), 't');
}

TEST(ring_buffer, add_char_or_fail){
	unsigned char buffer[MAX_BUFFER];
    buffer[-3] = 't';
    buffer[-2] = 'e';
    buffer[-1] = 's';
	
	struct buffer_type ring_buffer;
    ring_buffer.head = buffer + sizeof(unsigned char) * MAX_BUFFER;
    ring_buffer.tail = buffer + sizeof(unsigned char) * (MAX_BUFFER - 3);
    ring_buffer.buffer = buffer;
    enum error_type err = OK;
	
	// Check whether character is correctly stored in ring-buffer
	add_char_to_buffer(&ring_buffer, 't', &err);
	EXPECT_EQ(*ring_buffer.buffer, 't');
	
	// Check whether error is correctly processed
	err = OK;
    ring_buffer.head = buffer + sizeof(unsigned char) * MAX_BUFFER;
    ring_buffer.tail = buffer;
    ring_buffer.buffer = buffer;
	EXPECT_EQ(add_char_to_buffer(&ring_buffer, 't', &err), -1);
	EXPECT_EQ(err, BUFFER_FULL);
	
	// Check other error
	err = OK;
    ring_buffer.head = buffer + sizeof(unsigned char) * 2;
    ring_buffer.tail = buffer + sizeof(unsigned char) * 3;
    ring_buffer.buffer = buffer;
	EXPECT_EQ(add_char_to_buffer(&ring_buffer, 't', &err), -1);
	EXPECT_EQ(err, BUFFER_FULL);
}


TEST(ring_buffer, get_char){
    unsigned char buffer[MAX_BUFFER];
    buffer[97] = 'a';
    buffer[98] = 'b';
    buffer[99] = 'c';

	struct buffer_type ring_buffer;
    ring_buffer.head = buffer + sizeof(unsigned char) * MAX_BUFFER;
    ring_buffer.tail = buffer + sizeof(unsigned char) * (MAX_BUFFER - 3);
    ring_buffer.buffer = buffer;
    enum error_type err = OK;

    // Check whether character is fetched correctly
    char result = get_char_from_buffer(&ring_buffer, &err);
    EXPECT_EQ(result, 'a');
}

TEST(ring_buffer, get_char_failure){
    unsigned char buffer[MAX_BUFFER];

	struct buffer_type ring_buffer;
    ring_buffer.head = buffer;
    ring_buffer.tail = buffer;
    ring_buffer.buffer = buffer;
    enum error_type err = OK;

    // Check whether error is implemented correctly
    char result = get_char_from_buffer(&ring_buffer, &err);
    EXPECT_EQ(result, -1);
    EXPECT_EQ(err, EMPTY_BUFFER);
}


TEST(regular_buffer, add_string){
    unsigned char buffer[MAX_BUFFER];
    buffer[0] = 'a';
    buffer[1] = 'b';
    buffer[2] = 'c';

	struct buffer_type ring_buffer;
    ring_buffer.head = buffer + sizeof(unsigned char) * 3;
    ring_buffer.tail = buffer;
    ring_buffer.buffer = buffer;

    enum error_type err = OK;
    unsigned char new_char[] = "word";
    unsigned char result = add_string_to_buffer(&ring_buffer, new_char, &err);

    // Check if results are as expected
    EXPECT_EQ(result, 4);
    EXPECT_EQ(ring_buffer.head, buffer + sizeof(unsigned char) * 7);
    EXPECT_EQ(*(ring_buffer.head - sizeof(unsigned char)), 'd');
    EXPECT_EQ(*ring_buffer.buffer, 'a');
}

TEST(ring_buffer, add_string){
    unsigned char buffer[MAX_BUFFER];
    buffer[97] = 'a';
    buffer[98] = 'b';
    buffer[99] = 'c';

	struct buffer_type ring_buffer;
    ring_buffer.head = buffer + sizeof(unsigned char) * MAX_BUFFER;
    ring_buffer.tail = buffer + sizeof(unsigned char) * (MAX_BUFFER - 3);
    ring_buffer.buffer = buffer;

    enum error_type err = OK;
    unsigned char new_char[] = "word";
    int result = add_string_to_buffer(&ring_buffer, new_char, &err);

    // Check if results are as expected
    EXPECT_EQ(result, 4);
    EXPECT_EQ(ring_buffer.head, buffer + sizeof(unsigned char) * 4);
    EXPECT_EQ(*(ring_buffer.head - sizeof(unsigned char)), 'd');
    EXPECT_EQ(*(ring_buffer.buffer + sizeof(unsigned char) * (MAX_BUFFER - 3)), 'a');
}

TEST(ring_buffer, add_string_fail){
    unsigned char buffer[MAX_BUFFER];
    buffer[97] = 'a';
    buffer[98] = 'b';
    buffer[99] = 'c';

	struct buffer_type ring_buffer;
    ring_buffer.head = buffer + sizeof(unsigned char) * MAX_BUFFER;
    ring_buffer.tail = buffer + sizeof(unsigned char) * (MAX_BUFFER - 3);
    ring_buffer.buffer = buffer;

    enum error_type err = OK;
    unsigned char new_char[] = "vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv"; // 150
    int result = add_string_to_buffer(&ring_buffer, new_char, &err);
    
    // Check if errors are handled correctly
    EXPECT_EQ(result, -1);
    EXPECT_EQ(ring_buffer.head, buffer + sizeof(unsigned char) * MAX_BUFFER);
    EXPECT_EQ(ring_buffer.tail, buffer + sizeof(unsigned char) * (MAX_BUFFER - 3));
}

TEST(regular_buffer, get_string){
    unsigned char buffer[MAX_BUFFER];
    buffer[0] = 'a';
    buffer[1] = 'b';
    buffer[2] = 'c';

	struct buffer_type ring_buffer;
    ring_buffer.head = buffer + sizeof(unsigned char) * 3;
    ring_buffer.tail = buffer;
    ring_buffer.buffer = buffer;

    enum error_type err = OK;
    unsigned char destination[2];

    // Check whether the string is correctly retrieved from a regular buffer
    EXPECT_EQ(get_string_from_buffer(&ring_buffer, destination, 2, &err), 2);
    EXPECT_EQ(destination[0], 'a');
    EXPECT_EQ(destination[1], 'b');
    EXPECT_EQ(ring_buffer.tail, ring_buffer.buffer + sizeof(unsigned char) * 2);
    EXPECT_EQ(*ring_buffer.tail, 'c');
}

TEST(ring_buffer, get_string_and_error){
    unsigned char buffer[MAX_BUFFER];
    buffer[97] = 'a';
    buffer[98] = 'b';
    buffer[99] = 'c';
    buffer[0] = 'd';
    buffer[1] = 'e';

	struct buffer_type ring_buffer;
    ring_buffer.head = buffer + sizeof(unsigned char) * 2;
    ring_buffer.tail = buffer + sizeof(unsigned char) * (MAX_BUFFER - 3);
    ring_buffer.buffer = buffer;

    enum error_type err = OK;
    unsigned char destination[4];

    // Check whether the string is correctly retrieved from a regular buffer
    EXPECT_EQ(get_string_from_buffer(&ring_buffer, destination, 4, &err), 4);
    EXPECT_EQ(destination[0], 'a');
    EXPECT_EQ(destination[3], 'd');
    EXPECT_EQ(ring_buffer.tail + sizeof(unsigned char), ring_buffer.head);

    // Check whether errors are handled correctly
    unsigned char other_destination[3];
    enum error_type compare = BUFFER_FULL;
    EXPECT_EQ(get_string_from_buffer(&ring_buffer, other_destination, 3, &err), 1);
    EXPECT_EQ(err, compare);
    EXPECT_EQ(other_destination[0], 'e');
    EXPECT_EQ(ring_buffer.tail, ring_buffer.head);
}
