#include <fstream>
#include <iostream>
#include <sstream>
#include<cmath>
#include <string>
#include <sys/wait.h>
#include <unistd.h>
#include <pthread.h>
#include <vector>

using namespace std;
pthread_mutex_t mtx; // Mutex for synchronizing writes to the pipe
vector<float> inputs;
vector<float> weights;

struct ThreadData {
int pipe_fd;
int prev_layer;
float input;
float weight;
};
/*class Layer {
public:
    Layer(vector<float>& inputs, vector<float>& weights, int read_fd = -1) : read_fd(read_fd) {
        int pipe_fd[2];
        if (pipe(pipe_fd) == -1) {
            cerr << "Error: Unable to create pipe" << endl;
            exit(1);
        }

        int pid = fork();
        if (pid == -1) {
            cerr << "Error: Unable to fork process" << endl;
            exit(1);
        }

        if (pid == 0) {
            close(pipe_fd[1]);
            if (read_fd != -1) {
                dup2(read_fd, STDIN_FILENO);
                close(read_fd);
            }
            dup2(pipe_fd[0], STDIN_FILENO);
            execl("./p2", "p1.cpp", (char *)NULL);
            cerr << "Error: execl failed" << endl;
            exit(1);
        } else {
            close(pipe_fd[0]);
            this->pipe_fd = pipe_fd[1];

            threads.resize(inputs.size() * weights.size());
            thread_data.resize(inputs.size() * weights.size());

            size_t i = 0;
            for (const float& input : inputs) {
                for (const float& weight : weights) {
                    thread_data[i].pipe_fd = this->pipe_fd;
                    thread_data[i].input = input;
                    thread_data[i].weight = weight;
                    pthread_create(&threads[i], NULL, thread_function, (void*)&thread_data[i]);
                    i++;
                }
            }
        }
    }

   int get_pipe_fd() {
        return pipe_fd;
    }*/


void read_file(const string &file_name, vector<float> &data) {
ifstream file(file_name);
float value;

string line;

if (!file) {
cout << "Error: Unable to open file " << file_name << endl;
exit(1);
}

while (file >> value) {
data.push_back(value);
}

file.close();
}

void* send_value(void* arg) {
ThreadData *newarr = reinterpret_cast<ThreadData *>(arg);
pthread_mutex_lock (&mtx);
float x = newarr->input * newarr->weight;
float x1 = (pow(x, 2.0) + x + 1) / 2;
float x2 = (pow(x, 2.0) - x) / 2;    
write(newarr->pipe_fd, &x1, sizeof(x1));
write(newarr->pipe_fd, &x2, sizeof(x2));

cout<<"X1: "<<x1<<endl;
cout<<"x2: "<<x2<<endl;
//write(newarr->pipe_fd, &result, sizeof(result));
pthread_mutex_unlock(&mtx);
pthread_exit (NULL);
}

void create_process(int *pipe_fd, int layer, int prev_layer) {
int pid = fork();
if (pid == -1) {
cout << "Error: Unable to fork process" << endl;
exit(1);
}
if (pid == 0) {      // Child process
    close(pipe_fd[1]); // Close the write end of the pipe
    dup2(pipe_fd[0], STDIN_FILENO); // Replace standard input with the read end of the pipe
    stringstream ss;
    ss << layer;
    string layer_str = ss.str();
    execl("./p2", ("p1_" + layer_str + ".cpp").c_str(), to_string(prev_layer).c_str(), (char *)NULL); // Execute the next layer (layer2_receive)
    cout << "Error: execl failed" << endl;
    exit(1);
}
}


int main() {
cout<<"we are in the first layer"<<endl;
read_file("input.txt", inputs);
read_file("weights.txt", weights);

pthread_mutex_init (&mtx ,NULL);

int numberOfLayers;
cout << "Enter the number of layers: ";
cin >> numberOfLayers;

for (int i = 1; i <= numberOfLayers; i++) {
int pipe_fd[2];
if (pipe(pipe_fd) == -1) {
    cout << "Error: Unable to create pipe" << endl;
    exit(1);
}

create_process(pipe_fd, i, i-1);

close(pipe_fd[0]); // Close the read end of the pipe

vector<pthread_t> threads;

// Create a thread for each input-weight combination
for (const float &input : inputs) {
    for (const float &weight : weights) {
        pthread_t thread;
        ThreadData *data = new ThreadData;
        data->pipe_fd = pipe_fd[1];
        data->input = input;
        data->weight = weight;
        pthread_create(&thread, NULL, send_value, (void *)data);
        threads.push_back(thread);
    }
}

sleep(5);

close(pipe_fd[1]); // Close the write end of the pipe
wait(NULL);        // Wait for the child process to finish
}
pthread_mutex_destroy(&mtx);
exit(0);

return 0;
}

