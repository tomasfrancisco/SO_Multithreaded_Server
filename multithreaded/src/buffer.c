#include <stdlib.h>
#include <stdio.h>

#include "buffer.h"

buffer* buffer_create(int size) {
  struct buffer* new = (buffer*) malloc(sizeof(buffer*));

  new->size = size;
  new->first_node = NULL;

  return new;
}

void buffer_add_node(buffer* buf, int socket, int type, char *file_name) {
  struct buffer_node* node;

  struct buffer_node* new = (buffer_node*) malloc(sizeof(buffer_node*));

  new->socket = socket;
  new->type = type;
  sprintf(new->file_name, "%s", file_name);
  new->next = NULL;

  node = buf->first_node;
  while(node->next != NULL) {
    node = node->next;
  }

  node->next = new;

  printf("Added new item to buffer\n");
}
