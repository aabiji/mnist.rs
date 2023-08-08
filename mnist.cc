#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

typedef unsigned char u8;

class MNIST {
public:
    MNIST();
    ~MNIST();
private:
    const int img_row  = 28;
    const int img_col  = 28;
    const int train_count = 60000;
    const int test_count  = 10000;
    const int img_size = img_row * img_col;

    u8 *test_images;
    u8 *train_images;
    int *test_labels;
    int *train_labels;

    void load_labels(int *labels, int count, const char *file);
    void load_images(unsigned char *images, int count, const char *file);
};

MNIST::MNIST() {
    test_labels = (int*) malloc(test_count * sizeof(int));
    test_images = (u8*) malloc(test_count * img_size * sizeof(u8));
    load_images(test_images, test_count, "data/testing.idx3-ubyte");
    load_labels(test_labels, test_count, "data/testing-labels.idx1-ubyte");

    train_labels = (int*) malloc(train_count * sizeof(int));
    train_images = (u8*) malloc(train_count * img_size * sizeof(u8));
    load_images(train_images, train_count, "data/training.idx3-ubyte");
    load_labels(train_labels, train_count, "data/training-labels.idx1-ubyte");
}

MNIST::~MNIST() {
    free(test_labels);
    free(test_images);
    free(train_labels);
    free(train_images);
}

void MNIST::load_labels(int *labels, int count, const char *file) {
    FILE *fp = fopen(file, "rb");

    u8 size[4];
    fread(size, 4, 1, fp);
    int magic_number = (size[0] << 24) | (size[1] << 16) | (size[2] << 8) | size[3];
    assert(magic_number == 2049);
    fread(size, 4, 1, fp);
    int items = (size[0] << 24) | (size[1] << 16) | (size[2] << 8) | size[3];
    assert(items == count);

    for (int i = 0; i < count; ++i) {
        unsigned char byte;
        fread(&byte, 1, 1, fp);
        labels[i] = byte;
    }

    fclose(fp);
}

void MNIST::load_images(unsigned char *images, int count, const char *file) {
    FILE *fp = fopen(file, "rb");

    u8 size[4];
    fread(size, 4, 1, fp);
    int magic_number = (size[0] << 24) | (size[1] << 16) | (size[2] << 8) | size[3];
    assert(magic_number == 2051);
    fread(size, 4, 1, fp);
    int items = (size[0] << 24) | (size[1] << 16) | (size[2] << 8) | size[3];
    assert(items == count);
    fread(size, 4, 1, fp);
    int rows = (size[0] << 24) | (size[1] << 16) | (size[2] << 8) | size[3];
    assert(rows == img_row);
    fread(size, 4, 1, fp);
    int columns = (size[0] << 24) | (size[1] << 16) | (size[2] << 8) | size[3];
    assert(columns == img_col);

    for (int j = 0; j < count; ++j) {
        for (int i = 0; i < img_size; ++i) {
            int idx = j * img_size + i;
            unsigned char byte;
            fread(&byte, 1, 1, fp);
            images[idx] = byte;
        }
    }

    fclose(fp);
}

int main() {
    MNIST nn;
}

/*
Neural network:
Input layer: 784 neurons
Hidden layer: 100 neurons
Output layer: 10 neurons

Forward propagation:
hidden_layer           = hidden_weights * input_layer + hidden_bias
activated_hidden_layer = sigmoid(hidden_layer)
output_layer           = output_weights * activated_hidden_layer + output_bias
activated_output_layer = softmax(output_layer)

Backward propagation:
loss = mean_squared_error(target_output, activated_output_layer)

output_gradient         = activated_output_layer - target_output
output_weights_gradient = output_gradient * activated_hidden_layer

hidden_gradient         = (transpose(output_weights) * output_gradient) * (activated_hidden_layer * (1 - activated_hidden_layer))
hidden_weights_gradient = hidden_gradient * input_layer

output_weights -= learning_rate * output_weights_gradient
output_bias    -= learning_rate * output_gradient

hidden_weights -= learning_rate * hidden_weights_gradient
hidden_bias    -= learning_rate * hidden_gradient
*/
