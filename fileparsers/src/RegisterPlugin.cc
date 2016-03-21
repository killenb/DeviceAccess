/*
 * RegisterPlugin.cc
 */

#include <boost/smart_ptr.hpp>

#include "RegisterPlugin.h"
#include "BufferingRegisterAccessorImpl.h"

namespace mtca4u {

  RegisterPlugin::~RegisterPlugin() {
    FILL_VIRTUAL_FUNCTION_TEMPLATE_VTABLE(decorateBufferingRegisterAccessor_impl);
  }

  /********************************************************************************************************************/

  template<typename UserType>
  boost::shared_ptr< BufferingRegisterAccessorImpl<UserType> > RegisterPlugin::decorateBufferingRegisterAccessor_impl(
      boost::shared_ptr< BufferingRegisterAccessorImpl<UserType> > accessor) const {
    return accessor;
  }
 
  /********************************************************************************************************************/

  boost::shared_ptr<RegisterAccessor> RegisterPlugin::decorateRegisterAccessor(boost::shared_ptr<RegisterAccessor> accessor) const {
    return accessor;
  }

}