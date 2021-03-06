/*
 * SupportedUserTypes.h - Define boost::fusion::maps of the user data types supported by the CHIMERA_TK library.
 *
 *  Created on: Feb 29, 2016
 *      Author: Martin Hierholzer
 */

#ifndef CHIMERA_TK_SUPPORTED_USER_TYPES_H
#define CHIMERA_TK_SUPPORTED_USER_TYPES_H

#include <boost/fusion/container/map.hpp>

namespace ChimeraTK {

  /** Map of UserType to value of the UserType. Used e.g. by the FixedPointConverter to store coefficients etc. in
   *  dependence of the UserType. */
  typedef boost::fusion::map<
      boost::fusion::pair<int8_t,int8_t>,
      boost::fusion::pair<uint8_t,uint8_t>,
      boost::fusion::pair<int16_t,int16_t>,
      boost::fusion::pair<uint16_t,uint16_t>,
      boost::fusion::pair<int32_t,int32_t>,
      boost::fusion::pair<uint32_t,uint32_t>,
      boost::fusion::pair<int64_t,int64_t>,
      boost::fusion::pair<uint64_t,uint64_t>,
      boost::fusion::pair<float,float>,
      boost::fusion::pair<double,double>,
      boost::fusion::pair<std::string,std::string>
  > userTypeMap;

  /** Map of UserType to a class template with the UserType as template argument. Used e.g. by the
   *  VirtualFunctionTemplate macros to implement the vtable. */
  template< template<typename> class TemplateClass >
  class TemplateUserTypeMap {
    public:
      boost::fusion::map<
          boost::fusion::pair<int8_t, TemplateClass<int8_t> >,
          boost::fusion::pair<uint8_t, TemplateClass<uint8_t> >,
          boost::fusion::pair<int16_t, TemplateClass<int16_t> >,
          boost::fusion::pair<uint16_t, TemplateClass<uint16_t> >,
          boost::fusion::pair<int32_t, TemplateClass<int32_t> >,
          boost::fusion::pair<uint32_t, TemplateClass<uint32_t> >,
          boost::fusion::pair<int64_t, TemplateClass<int64_t> >,
          boost::fusion::pair<uint64_t, TemplateClass<uint64_t> >,
          boost::fusion::pair<float, TemplateClass<float> >,
          boost::fusion::pair<double, TemplateClass<double> >,
          boost::fusion::pair<std::string, TemplateClass<std::string> >
       > table;
  };

  /** Map of UserType to a single type */
  template<typename T>
  using SingleTypeUserTypeMap = boost::fusion::map<
                                    boost::fusion::pair<int8_t,T>,
                                    boost::fusion::pair<uint8_t,T>,
                                    boost::fusion::pair<int16_t,T>,
                                    boost::fusion::pair<uint16_t,T>,
                                    boost::fusion::pair<int32_t,T>,
                                    boost::fusion::pair<uint32_t,T>,
                                    boost::fusion::pair<int64_t,T>,
                                    boost::fusion::pair<uint64_t,T>,
                                    boost::fusion::pair<float,T>,
                                    boost::fusion::pair<double,T>,
                                    boost::fusion::pair<std::string,T>
                                > ;

#define DECLARE_TEMPLATE_FOR_CHIMERATK_USER_TYPES( TemplateClass ) \
  extern template class TemplateClass<int8_t>;  \
  extern template class TemplateClass<uint8_t>;  \
  extern template class TemplateClass<int16_t>; \
  extern template class TemplateClass<uint16_t>; \
  extern template class TemplateClass<int32_t>; \
  extern template class TemplateClass<uint32_t>; \
  extern template class TemplateClass<int64_t>; \
  extern template class TemplateClass<uint64_t>; \
  extern template class TemplateClass<float>;   \
  extern template class TemplateClass<double>;  \
  extern template class TemplateClass<std::string>// the last semicolon is added by the user

#define INSTANTIATE_TEMPLATE_FOR_CHIMERATK_USER_TYPES( TemplateClass )      \
  template class TemplateClass<int8_t>;  \
  template class TemplateClass<uint8_t>;  \
  template class TemplateClass<int16_t>; \
  template class TemplateClass<uint16_t>; \
  template class TemplateClass<int32_t>; \
  template class TemplateClass<uint32_t>; \
  template class TemplateClass<int64_t>; \
  template class TemplateClass<uint64_t>; \
  template class TemplateClass<float>;   \
  template class TemplateClass<double>;  \
  template class TemplateClass<std::string>// the last semicolon is added by the user

} /* namespace ChimeraTK */

#endif /* CHIMERA_TK_SUPPORTED_USER_TYPES_H */
