#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

struct node {
    int data;
    struct node *children[2];
};

struct lock {
    pthread_mutex_t mutex;
};

void compress(struct node *root, FILE *out, struct lock *lock) {
    if (root->children[0] == NULL && root->children[1] == NULL) {
        // Leaf node
        pthread_mutex_lock(&lock->mutex);
        fprintf(out, "%d\n", root->data);
        pthread_mutex_unlock(&lock->mutex);
    } else {
        // Internal node
        pthread_mutex_lock(&lock->mutex);
        fprintf(out, "%d ", root->data);
        compress(root->children[0], out, lock);
        compress(root->children[1], out, lock);
        pthread_mutex_unlock(&lock->mutex);
    }
}

struct node *decompress(FILE *in, struct lock *lock) {
    int data;
    pthread_mutex_lock(&lock->mutex);
    fscanf(in, "%d", &data);
    pthread_mutex_unlock(&lock->mutex);

    if (data == -1) {
        // Leaf node
        return NULL;
    } else {
        // Internal node
        struct node *root = malloc(sizeof(struct node));
        root->data = data;
        root->children[0] = decompress(in, lock);
        root->children[1] = decompress(in, lock);
        return root;
    }
}

int main() {
    struct node *root = malloc(sizeof(struct node));
    root->data = 1;
    root->children[0] = malloc(sizeof(struct node));
    root->children[0]->data = 2;
    root->children[1] = malloc(sizeof(struct node));
    root->children[1]->data = 3;

    FILE *out = fopen("compressed.txt", "w");
    struct lock lock;
    pthread_mutex_init(&lock.mutex, NULL);
    compress(root, out, &lock);
    fclose(out);

    FILE *in = fopen("compressed.txt", "r");
    struct node *decompressed_root = decompress(in, &lock);
    fclose(in);

    printf("Data: %d\n", decompressed_root->data);
    printf("Children: %d, %d\n", decompressed_root->children[0]->data, decompressed_root->children[1]->data);

    return 0;
}
