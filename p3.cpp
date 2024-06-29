#include <fstream>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>
#include <sys/wait.h>
#include <thread>
#include <unistd.h>
#include <vector>
#include<cmath>
#include <cstring>

using namespace std;




float apply_formula(float v, float x) {
    return ((pow(v, 2) - x + 1)/2);
}
float apply_formula2(float v, float x) {
    return ((pow(v, 2) - x)/2);
}



//void backpropagate(int pipe_fd) {
    // Read error values from the parent process
  //  vector<float> errors;
    float error;
    //while (read(STDIN_FILENO, &error, sizeof(error)) > 0) {
    //    errors.push_back(error);
  //  }

    // Assuming received_data has forward pass results
  //  vector<float> backpropagated_values(received_data.size());

    // Apply formula on received_data instead of updating weights
   // for (size_t i = 0; i < errors.size(); i++) {
     //   for (size_t j = 0; j < received_data.size(); j++) {
      //      backpropagated_values[j] = apply_formula(received_data[j], errors[i]);
     //   }
  //  }

    // Print the backpropagating values
 //   cout << "Backpropagating values: " << endl;
  //  for (const float &backpropagated_value : backpropagated_values) {
      //  cout << backpropagated_value << endl;
   // }

    // Send backpropagated values to the parent process
   // for (const float &backpropagated_value : backpropagated_values) {
    //    write(pipe_fd, &backpropagated_value, sizeof(backpropagated_value));
   // }
//}


int main() {
  vector<float> received_data;
  float value;

  while (read(STDIN_FILENO, &value, sizeof(value)) > 0) {
    received_data.push_back(value);
  }

  cout << "after applying weights in p2.cpp we are in third layer now we will go to two and then to one:" << endl;

  int array_size = received_data.size();
  float *v_values = new float[array_size];

  for (int i = 0; i < array_size; ++i) {
    float v = received_data[i];
    cout << v << endl;
    v_values[i] = v;
  }
  int backpropagated_values[50];
   for (size_t i = 0; i < received_data.size(); i++) {
        backpropagated_values[i] = apply_formula( v_values[i], 1.0);
        backpropagated_values[i] = apply_formula2( v_values[i], 1.0);
    }
  int a;
  cout<<"we are in layer 1 now "<<endl;
  cout<<"back propgation ended"<<endl;
  cin>>a;
  if(a==1)
  {
      string data_string = "";
    for (size_t i = 0; i < 50; i++) {
        data_string += to_string(backpropagated_values[i]) + " ";
    }

    // Launch child process and pass data string as argument
     const char* args[] = {"./p2", data_string.c_str(), nullptr};
    execvp(args[0], const_cast<char* const*>(args));
  
  }
  
  return 0;
}
