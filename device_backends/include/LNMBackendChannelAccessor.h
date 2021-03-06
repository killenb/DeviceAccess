/*
 * LNMBackendBufferingChannelAccessor.h
 *
 *  Created on: Feb 15, 2016
 *      Author: Martin Hierholzer
 */

#ifndef CHIMERA_TK_LNM_BACKEND_BUFFERING_CHANNEL_ACCESSOR_H
#define CHIMERA_TK_LNM_BACKEND_BUFFERING_CHANNEL_ACCESSOR_H

#include <algorithm>

#include "SyncNDRegisterAccessor.h"
#include "LogicalNameMappingBackend.h"
#include "TwoDRegisterAccessor.h"
#include "FixedPointConverter.h"
#include "Device.h"

namespace ChimeraTK {

  /*********************************************************************************************************************/

  template<typename UserType>
  class LNMBackendChannelAccessor : public SyncNDRegisterAccessor<UserType> {
    public:

      LNMBackendChannelAccessor(boost::shared_ptr<DeviceBackend> dev, const RegisterPath &registerPathName,
          size_t numberOfWords, size_t wordOffsetInRegister, AccessModeFlags flags)
      : SyncNDRegisterAccessor<UserType>(registerPathName),
        _registerPathName(registerPathName)
      {
        try {
          // check for unknown flags
          flags.checkForUnknownFlags({AccessMode::raw});
          // check for illegal parameter combinations
          if(flags.has(AccessMode::raw)) {
            throw DeviceException("LNMBackendChannelAccessor: raw access not yet supported!", DeviceException::NOT_IMPLEMENTED);
          }
          _dev = boost::dynamic_pointer_cast<LogicalNameMappingBackend>(dev);
          // copy the register info and create the internal accessors, if needed
          _info = *( boost::static_pointer_cast<LNMBackendRegisterInfo>(
              _dev->getRegisterCatalogue().getRegister(_registerPathName)) );
          _info.createInternalAccessors(dev);
          // check for incorrect usage of this accessor
          if( _info.targetType != LNMBackendRegisterInfo::TargetType::CHANNEL ) {
            throw DeviceException("LNMBackendChannelAccessor used for wrong register type.",
                DeviceException::WRONG_PARAMETER); // LCOV_EXCL_LINE (impossible to test...)
          }
          // get target device and accessor
          std::string devName = _info.deviceName;
          boost::shared_ptr<DeviceBackend> targetDevice;
          if(devName != "this") {
            targetDevice = _dev->_devices[devName];
          }
          else {
            targetDevice = dev;
          }
          _accessor = targetDevice->getRegisterAccessor<UserType>(RegisterPath(_info.registerName), numberOfWords,wordOffsetInRegister, false);
          // allocate the buffer
          NDRegisterAccessor<UserType>::buffer_2D.resize(1);
          NDRegisterAccessor<UserType>::buffer_2D[0].resize(_accessor->getNumberOfSamples());
        }
        catch(...) {
          this->shutdown();
          throw;
        }
      }

      virtual ~LNMBackendChannelAccessor() { this->shutdown(); };

      void doReadTransfer() override {
        _accessor->doReadTransfer();
      }

      bool doWriteTransfer(ChimeraTK::VersionNumber /*versionNumber*/={}) override {
        throw DeviceException("Writing to channel-type registers of logical name mapping devices is not supported.",
            DeviceException::REGISTER_IS_READ_ONLY);
      }

      bool doReadTransferNonBlocking() override {
        doReadTransfer();
        return true;
      }

      bool doReadTransferLatest() override {
        doReadTransfer();
        return true;
      }

      void doPreRead() override {
        _accessor->preRead();
      }

      void doPostRead() override {
        _accessor->postRead();
        _accessor->accessChannel(_info.channel).swap(NDRegisterAccessor<UserType>::buffer_2D[0]);
      };

      bool mayReplaceOther(const boost::shared_ptr<TransferElement const> &other) const override {
        auto rhsCasted = boost::dynamic_pointer_cast< const LNMBackendChannelAccessor<UserType> >(other);
        if(!rhsCasted) return false;
        if(_registerPathName != rhsCasted->_registerPathName) return false;
        if(_dev != rhsCasted->_dev) return false;
        return true;
      }

      bool isReadOnly() const override {
        return true;
      }

      bool isReadable() const override {
        return true;
      }

      bool isWriteable() const override {
        return false;
      }

      FixedPointConverter getFixedPointConverter() const override {
        throw DeviceException("FixedPointConverterse are not available in Logical Name Mapping",
                              DeviceException::NOT_AVAILABLE);
      }

    protected:

      /// pointer to underlying accessor
      boost::shared_ptr< NDRegisterAccessor<UserType> > _accessor;

      /// register and module name
      RegisterPath _registerPathName;

      /// backend device
      boost::shared_ptr<LogicalNameMappingBackend> _dev;

      /// register information. We hold a copy of the RegisterInfo, since it might contain register accessors
      /// which may not be owned by the backend
      LNMBackendRegisterInfo _info;

      std::vector< boost::shared_ptr<TransferElement> > getHardwareAccessingElements() override {
        return _accessor->getHardwareAccessingElements();
      }

      std::list< boost::shared_ptr<TransferElement> > getInternalElements() override {
        auto result = _accessor->getInternalElements();
        result.push_front(_accessor);
        return result;
      }

      void replaceTransferElement(boost::shared_ptr<TransferElement> newElement) override {
        auto casted = boost::dynamic_pointer_cast< NDRegisterAccessor<UserType> >(newElement);
        if(casted && _accessor->mayReplaceOther(newElement)) {
          _accessor = casted;
        }
        else {
          _accessor->replaceTransferElement(newElement);
        }
      }

  };

  DECLARE_TEMPLATE_FOR_CHIMERATK_USER_TYPES(LNMBackendChannelAccessor);

}    // namespace ChimeraTK

#endif /* CHIMERA_TK_LNM_BACKEND_BUFFERING_CHANNEL_ACCESSOR_H */
