/*
 * LNMBackendBufferingChannelAccessor.h
 *
 *  Created on: Feb 15, 2016
 *      Author: Martin Hierholzer
 */

#ifndef MTCA4U_LNM_BACKEND_BUFFERING_CHANNEL_ACCESSOR_H
#define MTCA4U_LNM_BACKEND_BUFFERING_CHANNEL_ACCESSOR_H

#include <algorithm>

#include "LogicalNameMappingBackend.h"
#include "TwoDRegisterAccessor.h"
#include "TwoDRegisterAccessorImpl.h"
#include "FixedPointConverter.h"
#include "Device.h"

namespace mtca4u {

  class DeviceBackend;

  /*********************************************************************************************************************/

  template<typename T>
  class LNMBackendBufferingChannelAccessor : public BufferingRegisterAccessorImpl<T> {
    public:

      LNMBackendBufferingChannelAccessor(boost::shared_ptr<DeviceBackend> dev, const std::string &module,
          const std::string &registerName)
      : _registerName(registerName),_moduleName(module)
      {
        _dev = boost::dynamic_pointer_cast<LogicalNameMappingBackend>(dev);
        std::string name = ( module.length() > 0 ? module + "." + registerName : registerName );
        _info = _dev->_map.getRegisterInfo(name);
        if( _info.targetType != LogicalNameMap::TargetType::CHANNEL ) {
          throw DeviceException("LNMBackendBufferingChannelAccessor used for wrong register type.",
              DeviceException::EX_WRONG_PARAMETER);
        }
        _targetDevice = _dev->_devices[_info.deviceName];
        _accessor = _targetDevice->getTwoDRegisterAccessor<T>("",_info.registerName);
      }

      virtual ~LNMBackendBufferingChannelAccessor() {};

      virtual void read() {
        _accessor.read();
      }

      virtual void write() {
        _accessor.write();
      }

      virtual T& operator[](unsigned int index) {
        return _accessor[_info.channel][index];
      }

      virtual unsigned int getNumberOfElements() {
        return _accessor[_info.channel].size();
      }

      typedef typename BufferingRegisterAccessorImpl<T>::iterator iterator;
      typedef typename BufferingRegisterAccessorImpl<T>::const_iterator const_iterator;
      typedef typename BufferingRegisterAccessorImpl<T>::reverse_iterator reverse_iterator;
      typedef typename BufferingRegisterAccessorImpl<T>::const_reverse_iterator const_reverse_iterator;
      virtual iterator begin() { return _accessor[_info.channel].begin(); }
      virtual const_iterator begin() const { return _accessor[_info.channel].begin(); }
      virtual iterator end() { return _accessor[_info.channel].begin(); }
      virtual const_iterator end() const { return _accessor[_info.channel].begin(); }
      virtual reverse_iterator rbegin() { return _accessor[_info.channel].rend(); }
      virtual const_reverse_iterator rbegin() const { return _accessor[_info.channel].rend(); }
      virtual reverse_iterator rend() { return _accessor[_info.channel].rend(); }
      virtual const_reverse_iterator rend() const { return _accessor[_info.channel].rend(); }

      virtual void swap(std::vector<T> &x) {
        _accessor[_info.channel].swap(x);
      }

      virtual bool isSameRegister(const boost::shared_ptr<TransferElement const> &other) const {
        auto rhsCasted = boost::dynamic_pointer_cast< const LNMBackendBufferingChannelAccessor<T> >(other);
        if(!rhsCasted) return false;
        if(_registerName != rhsCasted->_registerName) return false;
        if(_moduleName != rhsCasted->_moduleName) return false;
        if(_dev != rhsCasted->_dev) return false;
        return true;
      }

    protected:

      /// pointer to underlying accessor
      TwoDRegisterAccessor<T> _accessor;

      /// register and module name
      std::string _registerName, _moduleName;

      /// backend device
      boost::shared_ptr<LogicalNameMappingBackend> _dev;

      /// register information
      LogicalNameMap::RegisterInfo _info;

      /// target device
      boost::shared_ptr<Device> _targetDevice;

      virtual std::vector< boost::shared_ptr<TransferElement> > getHardwareAccessingElements() {
        return _accessor.getHardwareAccessingElements();
      }

      virtual void replaceTransferElement(boost::shared_ptr<TransferElement> newElement) {
        _accessor.replaceTransferElement(newElement);
      }

  };

}    // namespace mtca4u

#endif /* MTCA4U_LNM_BACKEND_BUFFERING_CHANNEL_ACCESSOR_H */
