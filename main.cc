#include <atomic>
#include <iostream>
#include <thread>
#include <vector>
#include <chrono>

std::atomic<int> threadPos;
std::atomic<int> readerDone;
int *dataArray = nullptr;
int numReader = 0;
int numWork = 0;
std::vector<std::thread> allthreads;

void writing(){ 
  int tid = threadPos.fetch_sub(1);
  if (tid < 0) {
    return;
  }
  while(threadPos >= 0) {
    std::this_thread::sleep_for (std::chrono::milliseconds(1));
  }

  std::cout << "thread " << tid << " start." << std::endl;

  int idx = 0;
  while(idx < numWork) {
    dataArray[tid]++;
    std::this_thread::sleep_for (std::chrono::milliseconds(1));
    idx++;
  }
  std::cout << "thread " << tid << " done." << std::endl;
  readerDone.fetch_add(1);
}

void reading(){  
  while(readerDone < numReader) {
    std::this_thread::sleep_for (std::chrono::milliseconds(1));
    int idx = 0;
    int sum = 0;
    while(idx < numReader) {
        sum += dataArray[idx];
        // std::cout << "idx=" << idx << " val=" << dataArray[idx] << std::endl;
        idx++;
    }
    std::cout << "sum = " << sum << std::endl;
  }
}

int main(int argc, char *argv[]){
  std::cout << argc << std::endl;
  if (argc != 3) {
    std::cerr << "Invalid parameter." << std::endl;
    return -1;
  } 

  numReader = atoi(argv[1]);
  numWork = atoi(argv[2]);
  std::cout << "total reader " << numReader << std::endl;
  dataArray = new int[numReader-1];
  threadPos = numReader-1;
  readerDone = 0;

  int idx = numReader;
  while(idx-- > 0) {
    allthreads.emplace_back(std::thread(writing));
  }

  allthreads.emplace_back(std::thread(reading));

  for (auto &t : allthreads) {
    t.join();
  }
}