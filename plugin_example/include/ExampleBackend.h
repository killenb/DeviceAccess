/*
 * ExampleBackend.h
 *
 *  Created on: Jul 31, 2015
 *      Author: nshehzad
 */

#ifndef SOURCE_DIRECTORY__EXAMPLES_EXAMPLEBACKEND_H_
#define SOURCE_DIRECTORY__EXAMPLES_EXAMPLEBACKEND_H_

#include <list>
#include <iostream>
#include <boost/shared_ptr.hpp>

#include <mtca4u/DeviceBackendImpl.h>
#include <mtca4u/BackendFactory.h>
#include <mtca4u/DeviceAccessVersion.h>

/** An Example to show how to write a backend device class and add it to the factory.
 *
 */
// LCOV_EXCL_START dont include this in the coverage report
class ExampleBackend : public mtca4u::DeviceBackendImpl {
  public:
    ExampleBackend();
    virtual ~ExampleBackend();
    virtual void open();
    virtual void close();
    static boost::shared_ptr<mtca4u::DeviceBackend> createInstance(std::string host, std::string instance,
        std::list<std::string> parameters, std::string mapFileName);

    virtual std::string readDeviceInfo() {return std::string("Example_Device");}

  protected:

    /** Implement the virtual function template to obtain the buffering register accessor */
    template<typename UserType>
    boost::shared_ptr< mtca4u::NDRegisterAccessor<UserType> > getRegisterAccessor_impl(
        const mtca4u::RegisterPath &registerPathName, size_t numberOfWords, size_t wordOffsetInRegister, mtca4u::AccessModeFlags flags);
    DEFINE_VIRTUAL_FUNCTION_TEMPLATE_VTABLE_FILLER( ExampleBackend, getRegisterAccessor_impl, 4 );

  public:

    /** The registerer is announcing the new type to the registerer in its constructor.
     *  We have one static instance of this registerer in the backend. This causes the
     *  constructor to be executed when the library is loaded, end the backend is known by the
     *  factory afterwards.
     */
    class BackendRegisterer{
    public:
      BackendRegisterer(){
        mtca4u::BackendFactory::getInstance().registerBackendType("exx","",&ExampleBackend::createInstance, CHIMERATK_DEVICEACCESS_VERSION);
      }
    };
    /** The one static instance of the registerer. Currently we keep it public so there is an object which
     *  can be used in the client code. This trick is needed to force the library to be loaded
     *  as long as the loading mechanism is not implemented into the dmap file
     *  (see DeviceClient.cpp how to do it).
     */
    static BackendRegisterer backendRegisterer;
};
//LCOV_EXCL_STOP
#endif /* SOURCE_DIRECTORY__EXAMPLES_EXAMPLEBACKEND_H_ */
