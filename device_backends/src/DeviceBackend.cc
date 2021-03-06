/*
 * DeviceBackend.cc
 */

#include "DeviceBackend.h"

namespace ChimeraTK {

  DeviceBackend::~DeviceBackend() {
  }

  void DeviceBackend::readDMA(uint8_t, uint32_t, int32_t*,  size_t) {
    std::cerr << "**********************************************************************************" << std::endl;
    std::cerr << " The function DeviceBackend::readDMA() was removed after deprecation." << std::endl;
    std::cerr << "**********************************************************************************" << std::endl;
    exit(1);
  }

  void DeviceBackend::writeDMA(uint8_t, uint32_t, int32_t const*,  size_t) {
    std::cerr << "**********************************************************************************" << std::endl;
    std::cerr << " The function DeviceBackend::readDMA() was removed after deprecation." << std::endl;
    std::cerr << "**********************************************************************************" << std::endl;
    exit(1);
  }

} /* namespace ChimeraTK */
