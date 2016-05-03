#include "Device.h"
#include "Utilities.h"

using namespace mtca4u;

int main(){
  setDMapFilePath("dummies.dmap");

  Device device;
  device.open("PCIE2");

  auto accessor = device.getRegisterAccessor("WORD_STATUS","BOARD");

  std::cout <<" reading "<< accessor->read<int>() << std::endl;
  for(int i= 0; i < 100000; ++i){
      accessor->read<int>();
  }

  std::cout <<" writing " << std::endl;
  for(int i= 0; i < 100000; ++i){
      accessor->write<int>(i);
  }

  std::cout <<" reading raw " << std::endl;
  for(int i= 0; i < 100000; ++i){
    int buffer;
    accessor->readRaw(&buffer,sizeof(int));
  }

  std::cout <<" writing raw" << std::endl;
  for(int i= 0; i < 100000; ++i){
    accessor->writeRaw(&i, sizeof(int));
  }

  auto accessor2 = device.getRegisterAccessor("AREA_DMAABLE","ADC");
  for(int i= 0; i < 25; ++i){
      int buffer;
      accessor2->read<int>(&buffer, 1, i);
      std::cout << buffer << " ";
  }
  std::cout << std::endl;

  return 0;
}