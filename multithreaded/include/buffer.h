#define FILE_SIZE 256

#define DYNAMIC 1
#define STATIC 2


typedef struct buffer_node {
  int socket;
  int type;
  char file_name[FILE_SIZE];
  struct buffer_node* next;
} buffer_node;

typedef struct buffer {
  int size;
  int current_size;
  struct buffer_node* first_node;
} buffer;

buffer* buffer_create(int size);
void buffer_add_node(buffer* buf, int socket, int type, char *file_name);
