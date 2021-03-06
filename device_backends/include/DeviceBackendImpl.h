#ifndef CHIMERA_TK_DEVICE_BACKEND_IMPL_H
#define CHIMERA_TK_DEVICE_BACKEND_IMPL_H

#include <list>

#include "DeviceBackend.h"
#include "DeviceException.h"
#include "DataModifierPlugin.h"

namespace ChimeraTK {

  /**
   *  DeviceBackendImpl implements some basic functionality which should be available for all backends. This is
   *  required to allow proper decorator patterns which should not have this functionality in the decorator itself.
   */
  class DeviceBackendImpl: public DeviceBackend
  {

  public:

      DeviceBackendImpl();
      virtual ~DeviceBackendImpl();

      virtual bool isOpen(){ return _opened; }

      virtual bool isConnected(){ return _connected; }

      virtual const RegisterCatalogue& getRegisterCatalogue() const {
        return _catalogue;
      }

      virtual void read(uint8_t /*bar*/, uint32_t /*address*/, int32_t* /*data*/,  size_t /*sizeInBytes*/) {
        throw DeviceException("The depcrecated DeviceBackend::read() function is not implemented by this backend. "
            "Use the Device frontend instead!",DeviceException::NOT_IMPLEMENTED);
      }

      virtual void write(uint8_t /*bar*/, uint32_t /*address*/, int32_t const* /*data*/,  size_t /*sizeInBytes*/) {
        throw DeviceException("The depcrecated DeviceBackend::write() function is not implemented by this backend. "
            "Use the Device frontend instead!",DeviceException::NOT_IMPLEMENTED);
      }

      virtual void read(const std::string &, const std::string &, int32_t *, size_t = 0, uint32_t = 0) {
        throw DeviceException("The depcrecated DeviceBackend::read() function is not implemented by this backend. "
            "Use the Device frontend instead!",DeviceException::NOT_IMPLEMENTED);
      }

      virtual void write(const std::string &, const std::string &, int32_t const *, size_t = 0, uint32_t = 0) {
        throw DeviceException("The depcrecated DeviceBackend::write() function is not implemented by this backend. "
            "Use the Device frontend instead!",DeviceException::NOT_IMPLEMENTED);
      }

      virtual boost::shared_ptr<const RegisterInfoMap> getRegisterMap() const  {
        // implementing this read function is not mandatory, so we throw a not-implemented exception by default
        throw DeviceException("Obtaining a register map is not supported by this backend.",DeviceException::NOT_IMPLEMENTED);
      }

  protected:
      
      /** the register catalogue containing describing the registers known by this backend */
      RegisterCatalogue _catalogue;

      /** flag if device is opened */
      bool        _opened;
      
      /** flag if device is connected. */
      bool        _connected;

      /** Add plugin-provided decorators to a NDRegisterAccessor */
      template<typename UserType>
      boost::shared_ptr< NDRegisterAccessor<UserType> > decorateRegisterAccessor(
          const RegisterPath &registerPathName, boost::shared_ptr< NDRegisterAccessor<UserType> > accessor) const {
        if(!_catalogue.hasRegister(registerPathName)) return accessor;
        auto info = _catalogue.getRegister(registerPathName);
        for(auto i = info->plugins_begin(); i != info->plugins_end(); ++i) {
          boost::shared_ptr<DataModifierPlugin> plugin = boost::dynamic_pointer_cast<DataModifierPlugin>(i.getPointer());
          if(!plugin) continue;
          accessor = plugin->decorateRegisterAccessor<UserType>(accessor);
        }
        return accessor;
      }

  };

}//namespace ChimeraTK

#endif /*CHIMERA_TK_DEVICE_BACKEND_IMPL_H*/
