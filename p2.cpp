#include <fstream>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>
#include <sys/wait.h>
#include <thread>
#include <unistd.h>
#include <vector>
#include<pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include<cstring>

using namespace std;

pthread_mutex_t mtx;
vector<float> received_data;

struct ThreadData {
    int pipe_fd;
    float input;
    float weight;
};

void* send_value(void* arg) {
  ThreadData *newarr = reinterpret_cast<ThreadData *>(arg);
  pthread_mutex_lock (&mtx);
  float result = newarr->input * newarr->weight;
  write(newarr->pipe_fd, &result, sizeof(result));
  pthread_mutex_unlock(&mtx);
  pthread_exit (NULL);
}
// Function that parses a data string from argument and prints the values
void print_backpropagated_values(int argc, char* argv[]) {
    vector<double> backpropagated_values;
    if (argc < 2) {
        cerr << "Missing argument\n";
        return;
    }
    string data_string = argv[1];
    istringstream iss(data_string);
    string token;
    while (iss >> token) {
        double value = stod(token);
        backpropagated_values.push_back(value);
    }

    for (size_t i = 0; i < backpropagated_values.size(); i++) {
        cout << "Value " << i << ": " << backpropagated_values[i] << endl;
    }
}


int main(int argc, char* argv[]) {
  // Check if the current executable file is "p3"
  char exe_name[1024];
  ssize_t count = readlink("/proc/self/exe", exe_name, sizeof(exe_name));
  if (count == -1 || static_cast<size_t>(count) == sizeof(exe_name)) {
    std::cerr << "Error: Unable to get the name of the current executable file." << std::endl;
    return 1;
  }
  exe_name[count] = '\0';

  if (std::strcmp(exe_name, "./p3") == 0) {
    // Execute "p1"
    execl("./p1", "p1.cpp", nullptr);
    std::cerr << "Error: execl failed" << std::endl;
    return 1;
  }
 
// char* args[] = {"", data_string.c_str(), nullptr};
 int a;
  cout<<"We are in layer 2"<<endl;
  cout<<"are you going forwards or backwards"<<endl;
  cout<<"press 1 for forwards and 2 for backwards"<<endl; 
  cin>>a;
  float value;

  while (read(STDIN_FILENO, &value, sizeof(value)) > 0) {
    received_data.push_back(value);
  }

  cout << "after applying weights in first layer:" << endl;
  

  int array_size = received_data.size();
  float *v_values = new float[array_size];

  for (int i = 0; i < array_size; ++i) {
    float v = received_data[i];
    cout << v << endl;
    v_values[i] = v;
  }
   cout<<endl;
  // Read data.txt file and save values in dynamic array
  vector<float> data;
  ifstream data_file("weights.txt");
  float data_value;

  if (!data_file) {
    cout << "Error: Unable to open file data.txt" << endl;
    exit(1);
  }

  while (data_file >> data_value) {
    data.push_back(data_value);
    
  }
  
  cout << "Received data from data file:" << endl;

  int array_size1 = data.size();
  float *f_values = new float[array_size1];

  for (int i = 0; i < array_size1; ++i) {
    float f = data[i];
    cout << f << endl;
    f_values[i] = f;
  }
  cout<<endl;
  data_file.close();

  
int pipe_fd[2];
  if (pipe(pipe_fd) == -1) {
    cout << "Error: Unable to create pipe" << endl;
    exit(1);
  }

  int pid = fork();
  if (pid == -1) {
    cerr << "Error: Unable to fork process" << endl;
    exit(1);
  }

  if (pid == 0) {      // Child process
    close(pipe_fd[1]); // Close the write end of the pipe
    dup2(pipe_fd[0], STDIN_FILENO); // Replace standard input with the read end of the pipe
    
    execl("./p3", "p3.cpp", (char *)NULL); // Execute the next layer (layer2_receive)
    
    cout << "Error: execl failed" << endl;
    exit(1);
  } 
  else 
  {             // Parent process
    close(pipe_fd[0]); // Close the read end of the pipe

     vector<pthread_t> threads;
     
    // Create a thread for each input-weight combination
    for (const float &v : received_data) {
    for (const float &d : data) {
        pthread_t thread;
        ThreadData *data = new ThreadData;
        data->pipe_fd = pipe_fd[1];
        data->input = v;
        data->weight = d;
        pthread_create(&thread, NULL, send_value, (void *)data);
        threads.push_back(thread);
    }
   }
    
    sleep(5);
  char exe_name[1024];
    ssize_t count = readlink("/proc/self/exe", exe_name, sizeof(exe_name));
    if (count == -1 || static_cast<size_t>(count) == sizeof(exe_name)) {
        std::cerr << "Error: Unable to get the name of the current executable file." << std::endl;
        return 1;
    }
    exe_name[count] = '\0';

    // Check if the current executable file is "p3"
    if (std::strcmp(exe_name, "./p3") == 0) {
        // Execute "p1"
        execl("./p1", "p1.cpp", nullptr);
        std::cerr << "Error: execl failed" << std::endl;
        return 1;
    }

   // if(a==0)
  //  {
  
  //  print_backpropagated_values(argc, argv);
//}
    close(pipe_fd[1]); // Close the write end of the pipe
    wait(NULL);        // Wait for the child process to finish
    pthread_mutex_destroy(&mtx);
    exit(0);
  }

  return 0;
}
