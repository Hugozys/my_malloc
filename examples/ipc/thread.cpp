#include<iostream>
#include<thread>
class DisplayThread{
public:
  DisplayThread(){}
  void operator()(){
    for(int i = 0; i < 10000; ++i){
      std::cout<< "Display Thread Executing" <<std::endl;
    }
  }
};

int main(void){
  // DisplayThread temp();
  std::thread threadObj((DisplayThread()));
  for (int i = 0; i < 10000; i++){
    std::cout<<"Display From Main Thread "<<std::endl;
  }
  std::cout<<"Waiting For Thread to complete"<<std::endl;
  threadObj.join();
  std::cout<<"Exiting From Main Thread"<<std::endl;
  return 0;
}
  
