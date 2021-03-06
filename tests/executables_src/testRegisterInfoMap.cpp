///@todo FIXME My dynamic init header is a hack. Change the test to use BOOST_AUTO_TEST_CASE!
#include "boost_dynamic_init_test.h"
using namespace boost::unit_test_framework;

#include "helperFunctions.h"
#include "MapException.h"
namespace mtca4u{
  using namespace ChimeraTK;
}

class MapFileTest {
  public:
    void testInsertElement();
    void testInsertMetadata();
    void testGetRegisterInfo();
    void testGetMetaData();
    void testCheckRegistersOfSameName();
    void testCheckRegisterAddressOverlap();
    void testMetadataCoutStreamOperator();
    void testRegisterInfoCoutStreamOperator();
    void testErrElemTypeCoutStreamOperator();
    void testErrorElemCoutStreamOperator();
    void testErrorListCoutStreamOperator();
    void testMapFileCoutStreamOperator();
    void testGetRegistersInModule();
    static void testRegisterInfo();
};

class mapFileTestSuite : public test_suite {
  public:
    mapFileTestSuite(): test_suite("RegisterInfoMap class test suite"){
      boost::shared_ptr<MapFileTest>
      MapFileTestPtr(new MapFileTest);

      test_case* testInsertElement =
          BOOST_CLASS_TEST_CASE(&MapFileTest::testInsertElement,
              MapFileTestPtr);
      test_case* teestInsertMetadata =
          BOOST_CLASS_TEST_CASE(&MapFileTest::testInsertMetadata,
              MapFileTestPtr);
      test_case* testGetRegisterInfo =
          BOOST_CLASS_TEST_CASE(&MapFileTest::testGetRegisterInfo,
              MapFileTestPtr);
      test_case* testGetMetaData =
          BOOST_CLASS_TEST_CASE(&MapFileTest::testGetMetaData,
              MapFileTestPtr);
      test_case* testCheckRegistersOfSameName =
          BOOST_CLASS_TEST_CASE(&MapFileTest::testCheckRegistersOfSameName,
              MapFileTestPtr);
      test_case* testCheckRegisterAddressOverlap =
          BOOST_CLASS_TEST_CASE(&MapFileTest::testCheckRegisterAddressOverlap,
              MapFileTestPtr);
      test_case* testMetadataCoutStreamOperator =
          BOOST_CLASS_TEST_CASE(&MapFileTest::testMetadataCoutStreamOperator,
              MapFileTestPtr);
      test_case* testRegisterInfoCoutStreamOperator =
          BOOST_CLASS_TEST_CASE(&MapFileTest::testRegisterInfoCoutStreamOperator,
              MapFileTestPtr);
      test_case* testErrElemTypeCoutStreamOperator =
          BOOST_CLASS_TEST_CASE(&MapFileTest::testErrElemTypeCoutStreamOperator,
              MapFileTestPtr);
      test_case* testErrorElemCoutStreamOperator =
          BOOST_CLASS_TEST_CASE(&MapFileTest::testErrorElemCoutStreamOperator,
              MapFileTestPtr);
      test_case* testErrorListCoutStreamOperator =
          BOOST_CLASS_TEST_CASE(&MapFileTest::testErrorListCoutStreamOperator,
              MapFileTestPtr);
      test_case* testMapFileCoutStreamOperator =
          BOOST_CLASS_TEST_CASE(&MapFileTest::testMapFileCoutStreamOperator,
              MapFileTestPtr);
      add(testInsertElement);
      add(teestInsertMetadata);
      add(testGetRegisterInfo);
      add(testGetMetaData);
      add(testCheckRegistersOfSameName);
      add(testCheckRegisterAddressOverlap);
      add(testMetadataCoutStreamOperator);
      add(testRegisterInfoCoutStreamOperator);
      add(testErrElemTypeCoutStreamOperator);
      add(testErrorElemCoutStreamOperator);
      add(testErrorListCoutStreamOperator);
      add(testMapFileCoutStreamOperator);

      add(BOOST_TEST_CASE(MapFileTest::testRegisterInfo));
      add(BOOST_CLASS_TEST_CASE(&MapFileTest::testGetRegistersInModule, MapFileTestPtr));
    }
};

bool init_unit_test(){
  framework::master_test_suite().p_name.value =
      "RegisterInfoMap class test suite";
  framework::master_test_suite().add(new mapFileTestSuite());

  return true;
}

void MapFileTest::testInsertElement(){
  mtca4u::RegisterInfoMap dummyMapFile("dummy.map");
  mtca4u::RegisterInfoMap::RegisterInfo RegisterInfoent1("TEST_REGISTER_NAME_1", 2);
  mtca4u::RegisterInfoMap::RegisterInfo RegisterInfoent2("TEST_REGISTER_NAME_2", 1);
  mtca4u::RegisterInfoMap::RegisterInfo RegisterInfoent3("TEST_REGISTER_NAME_3", 4);
  mtca4u::RegisterInfoMap::RegisterInfo RegisterInfoentModule1("COMMON_REGISTER_NAME", 2, 8, 8, 1, 32, 0, true, 1, "Module1");
  mtca4u::RegisterInfoMap::RegisterInfo RegisterInfoentModule2("COMMON_REGISTER_NAME", 2, 16, 8, 1, 32, 0, true, 2, "Module2");

  dummyMapFile.insert(RegisterInfoent1);
  dummyMapFile.insert(RegisterInfoent2);
  dummyMapFile.insert(RegisterInfoent3);
  dummyMapFile.insert(RegisterInfoentModule1);
  dummyMapFile.insert(RegisterInfoentModule2);

  mtca4u::RegisterInfoMap const & constDummyMapFile = dummyMapFile;

  mtca4u::RegisterInfoMap::RegisterInfo* ptrList[5];
  ptrList[0] = &RegisterInfoent1;
  ptrList[1] = &RegisterInfoent2;
  ptrList[2] = &RegisterInfoent3;
  ptrList[3] = &RegisterInfoentModule1;
  ptrList[4] = &RegisterInfoentModule2;

  int index;
  mtca4u::RegisterInfoMap::iterator it;
  mtca4u::RegisterInfoMap::const_iterator const_it;
  for( it = dummyMapFile.begin(), index = 0;
      (it != dummyMapFile.end()) && (index < 3);
      ++it, ++index){
    BOOST_CHECK((compareRegisterInfoents(*ptrList[index], *it)) == true);
  }
  for( const_it = constDummyMapFile.begin(), index = 0;
      (const_it != constDummyMapFile.end()) && (index < 3);
      ++const_it, ++index){
    BOOST_CHECK((compareRegisterInfoents(*ptrList[index], *const_it)) == true);
  }
  BOOST_CHECK(dummyMapFile.getMapFileSize() == 5);
}

void MapFileTest::testInsertMetadata(){
  mtca4u::RegisterInfoMap dummyMapFile("dummy.map");

  mtca4u::RegisterInfoMap::MetaData metaData1("HW_VERSION", "1.6");
  mtca4u::RegisterInfoMap::MetaData metaData2("FW_VERSION", "2.5");
  mtca4u::RegisterInfoMap::MetaData metaData3("TEST", "Some additional information");

  dummyMapFile.insert(metaData1);
  dummyMapFile.insert(metaData2);
  dummyMapFile.insert(metaData3);

  std::string metaDataNameToRetrive;
  std::string retrivedValue;

  metaDataNameToRetrive = "HW_VERSION";
  dummyMapFile.getMetaData(metaDataNameToRetrive, retrivedValue);
  BOOST_CHECK(retrivedValue == "1.6");

  metaDataNameToRetrive = "FW_VERSION";
  dummyMapFile.getMetaData(metaDataNameToRetrive, retrivedValue);
  BOOST_CHECK(retrivedValue == "2.5");

  metaDataNameToRetrive = "TEST";
  dummyMapFile.getMetaData(metaDataNameToRetrive, retrivedValue);
  BOOST_CHECK(retrivedValue == "Some additional information");
}

void MapFileTest::testGetRegisterInfo () {
  mtca4u::RegisterInfoMap dummyMapFile("dummy.map");
  mtca4u::RegisterInfoMap::RegisterInfo RegisterInfoent1("TEST_REGISTER_NAME_1", 2, 0, 8);
  mtca4u::RegisterInfoMap::RegisterInfo RegisterInfoentModule1("COMMON_REGISTER_NAME", 2, 8, 8, 0, 32, 0, true, 1, "Module1");
  mtca4u::RegisterInfoMap::RegisterInfo RegisterInfoentModule2("COMMON_REGISTER_NAME", 2, 16, 8, 0, 32, 0, true, 2, "Module2");
  mtca4u::RegisterInfoMap::RegisterInfo retrievedDeviceInfo;

  dummyMapFile.insert(RegisterInfoent1);
  dummyMapFile.insert(RegisterInfoentModule1);
  dummyMapFile.insert(RegisterInfoentModule2);

  dummyMapFile.getRegisterInfo("TEST_REGISTER_NAME_1", retrievedDeviceInfo);
  BOOST_CHECK(compareRegisterInfoents(RegisterInfoent1, retrievedDeviceInfo) == true);

  dummyMapFile.getRegisterInfo("COMMON_REGISTER_NAME", retrievedDeviceInfo, "Module1");
  BOOST_CHECK(compareRegisterInfoents(RegisterInfoentModule1, retrievedDeviceInfo) == true);

  dummyMapFile.getRegisterInfo("COMMON_REGISTER_NAME", retrievedDeviceInfo, "Module2");
  BOOST_CHECK(compareRegisterInfoents(RegisterInfoentModule2, retrievedDeviceInfo) == true);

  BOOST_CHECK_THROW(
      dummyMapFile.getRegisterInfo("some_name", retrievedDeviceInfo),
      mtca4u::MapFileException);
  try{
    dummyMapFile.getRegisterInfo("some_name", retrievedDeviceInfo);
  } catch(mtca4u::MapFileException& mapFileException){
    BOOST_CHECK(mapFileException.getID() ==
        mtca4u::LibMapException::EX_NO_REGISTER_IN_MAP_FILE);
  }

  dummyMapFile.getRegisterInfo(0, retrievedDeviceInfo);
  BOOST_CHECK(compareRegisterInfoents(RegisterInfoent1, retrievedDeviceInfo) == true);
  BOOST_CHECK_THROW(
      dummyMapFile.getRegisterInfo(3, retrievedDeviceInfo),
      mtca4u::MapFileException);
  try{
    dummyMapFile.getRegisterInfo(3, retrievedDeviceInfo);
  } catch(mtca4u::MapFileException& mapFileException){
    BOOST_CHECK(mapFileException.getID() ==
        mtca4u::LibMapException::EX_NO_REGISTER_IN_MAP_FILE);
  }

}

void MapFileTest::testGetMetaData () {
  mtca4u::RegisterInfoMap dummyMapFile("dummy.map");
  mtca4u::RegisterInfoMap::MetaData metaData1("HW_VERSION", "1.6");
  dummyMapFile.insert(metaData1);

  std::string metaDataNameToRetrive;
  std::string retrivedValue;

  metaDataNameToRetrive = "HW_VERSION";
  dummyMapFile.getMetaData(metaDataNameToRetrive, retrivedValue);
  BOOST_CHECK(retrivedValue == "1.6");

  metaDataNameToRetrive = "some_name";
  BOOST_CHECK_THROW(
      dummyMapFile.getMetaData(metaDataNameToRetrive, retrivedValue),
      mtca4u::MapFileException);
  try{
    dummyMapFile.getMetaData(metaDataNameToRetrive, retrivedValue);
  } catch (mtca4u::MapFileException& mapFileException){
    BOOST_CHECK(mapFileException.getID() ==
        mtca4u::LibMapException::EX_NO_METADATA_IN_MAP_FILE);
  }
}

void MapFileTest::testCheckRegistersOfSameName(){
  mtca4u::RegisterInfoMap dummyMapFile("dummy.map");

  mtca4u::RegisterInfoMap::RegisterInfo RegisterInfoent1("TEST_REGISTER_NAME_1", 1, 0, 4, 0);
  mtca4u::RegisterInfoMap::RegisterInfo RegisterInfoent2("TEST_REGISTER_NAME_1", 1, 4, 4, 1);
  mtca4u::RegisterInfoMap::RegisterInfo RegisterInfoent3("TEST_REGISTER_NAME_1", 1, 8, 4, 0);
  mtca4u::RegisterInfoMap::RegisterInfo RegisterInfoent4("TEST_REGISTER_NAME_2", 1, 8, 4, 2);
  mtca4u::RegisterInfoMap::RegisterInfo RegisterInfoentModule1("COMMON_REGISTER_NAME", 2, 8, 8, 3, 32, 0, true, 1, "Module1");
  mtca4u::RegisterInfoMap::RegisterInfo RegisterInfoentModule2("COMMON_REGISTER_NAME", 2, 16, 8, 3, 32, 0, true, 2, "Module2");

  mtca4u::RegisterInfoMap::ErrorList errorList;
  dummyMapFile.insert(RegisterInfoent1);
  // check after the first element to cover the specific branch (special case)
  dummyMapFile.check(errorList, mtca4u::RegisterInfoMap::ErrorList::ErrorElem::WARNING);
  BOOST_CHECK(errorList.errors.empty());

  dummyMapFile.insert(RegisterInfoentModule1);
  dummyMapFile.insert(RegisterInfoentModule2);
  dummyMapFile.check(errorList, mtca4u::RegisterInfoMap::ErrorList::ErrorElem::WARNING);
  BOOST_CHECK(errorList.errors.empty());

  dummyMapFile.insert(RegisterInfoent2);
  dummyMapFile.insert(RegisterInfoent3);
  dummyMapFile.insert(RegisterInfoent4);
  dummyMapFile.check(errorList, mtca4u::RegisterInfoMap::ErrorList::ErrorElem::WARNING);
  BOOST_CHECK(errorList.errors.size() == 2);

  std::list<mtca4u::RegisterInfoMap::ErrorList::ErrorElem>::iterator errorIterator;
  for(errorIterator = errorList.errors.begin();
      errorIterator != errorList.errors.end();
      ++errorIterator){

    BOOST_CHECK(errorIterator->_errorType ==
        mtca4u::RegisterInfoMap::ErrorList::ErrorElem::NONUNIQUE_REGISTER_NAME);
    BOOST_CHECK(errorIterator->_type ==
        mtca4u::RegisterInfoMap::ErrorList::ErrorElem::ERROR);
  }

  // duplicate identical entries is an error
  dummyMapFile.insert(RegisterInfoent4);
  // only get the errors. There also is an overlap warning now.
  dummyMapFile.check(errorList, mtca4u::RegisterInfoMap::ErrorList::ErrorElem::ERROR);
  BOOST_CHECK(errorList.errors.size() == 3);
}

void MapFileTest::testCheckRegisterAddressOverlap(){
  mtca4u::RegisterInfoMap dummyMapFile("dummy.map");

  mtca4u::RegisterInfoMap::RegisterInfo RegisterInfoent1("TEST_REGISTER_NAME_1", 1, 0, 4, 0);
  mtca4u::RegisterInfoMap::RegisterInfo RegisterInfoent2("TEST_REGISTER_NAME_2", 1, 11, 4, 0);
  mtca4u::RegisterInfoMap::RegisterInfo RegisterInfoent3("TEST_REGISTER_NAME_3", 1, 10, 4, 0);
  // 4 overlaps with 1, but is not next to it in the list
  mtca4u::RegisterInfoMap::RegisterInfo RegisterInfoent4("TEST_REGISTER_NAME_4", 1, 3, 4, 0);
  mtca4u::RegisterInfoMap::RegisterInfo RegisterInfoent5("THE_WHOLE_MODULE", 2, 16, 8, 0);
  mtca4u::RegisterInfoMap::RegisterInfo RegisterInfoent6("REGISTER_1", 1, 16, 4, 0, 32, 0, true, 0, "THE_MODULE" );
  mtca4u::RegisterInfoMap::RegisterInfo RegisterInfoent7("REGISTER_2", 1, 20, 4, 0, 32, 0, true, 0, "THE_MODULE" );

  dummyMapFile.insert(RegisterInfoent1);
  dummyMapFile.insert(RegisterInfoent2);
  dummyMapFile.insert(RegisterInfoent3);
  dummyMapFile.insert(RegisterInfoent4);
  dummyMapFile.insert(RegisterInfoent5);
  dummyMapFile.insert(RegisterInfoent6);
  dummyMapFile.insert(RegisterInfoent7);

  mtca4u::RegisterInfoMap::ErrorList errorList;
  dummyMapFile.check(errorList,
      mtca4u::RegisterInfoMap::ErrorList::ErrorElem::ERROR);
  BOOST_CHECK(errorList.errors.empty());
  dummyMapFile.check(errorList,
      mtca4u::RegisterInfoMap::ErrorList::ErrorElem::WARNING);
  BOOST_CHECK(errorList.errors.size() == 2);
  std::list<mtca4u::RegisterInfoMap::ErrorList::ErrorElem>::iterator errorIterator;

  errorIterator = errorList.errors.begin();
  BOOST_CHECK(errorIterator->_errorRegister1.name == "TEST_REGISTER_NAME_3");
  BOOST_CHECK(errorIterator->_errorRegister2.name == "TEST_REGISTER_NAME_2");
  BOOST_CHECK(errorIterator->_errorType ==
      mtca4u::RegisterInfoMap::ErrorList::ErrorElem::WRONG_REGISTER_ADDRESSES);
  BOOST_CHECK(errorIterator->_type ==
      mtca4u::RegisterInfoMap::ErrorList::ErrorElem::WARNING);

  ++errorIterator;
  BOOST_CHECK(errorIterator->_errorRegister1.name == "TEST_REGISTER_NAME_4");
  BOOST_CHECK(errorIterator->_errorRegister2.name == "TEST_REGISTER_NAME_1");
  BOOST_CHECK(errorIterator->_errorType ==
      mtca4u::RegisterInfoMap::ErrorList::ErrorElem::WRONG_REGISTER_ADDRESSES);
  BOOST_CHECK(errorIterator->_type ==
      mtca4u::RegisterInfoMap::ErrorList::ErrorElem::WARNING);
}

void MapFileTest::testMetadataCoutStreamOperator(){
  mtca4u::RegisterInfoMap::MetaData meta_data("metadata_name", "metadata_value");
  std::stringstream expected_stream;
  expected_stream << "METADATA-> NAME: \"" <<
      "metadata_name" << "\" VALUE: " << "metadata_value" << std::endl;

  std::stringstream actual_stream;
  actual_stream << meta_data;

  BOOST_CHECK(expected_stream.str() == actual_stream.str());
}

void MapFileTest::testRegisterInfoCoutStreamOperator(){
  mtca4u::RegisterInfoMap::RegisterInfo RegisterInfoent1("Some_Register");
  mtca4u::RegisterInfoMap::RegisterInfo RegisterInfoent2("TEST_REGISTER_NAME_2", 2, 4, 8, 1,
      18, 3, false, 0, "SomeModule");

  std::stringstream expected_stream;
  expected_stream << "Some_Register" << " 0x"
      << std::hex << 0 << " 0x" << 0 << " 0x" << 0 << " 0x"<< 0 << std::dec
      << " 32 0 true";
  expected_stream << "TEST_REGISTER_NAME_2" << " 0x"
      << std::hex << 2 << " 0x" << 4 << " 0x" << 8 << " 0x" << 1 << std::dec
      << " 18 3 false SomeModule";
  std::stringstream actual_stream;
  actual_stream << RegisterInfoent1;
  actual_stream << RegisterInfoent2;

  BOOST_CHECK(expected_stream.str() == actual_stream.str());
}

void MapFileTest::testErrElemTypeCoutStreamOperator(){
  std::stringstream stream1;
  stream1 << mtca4u::RegisterInfoMap::ErrorList::ErrorElem::ERROR;
  BOOST_CHECK(stream1.str() == "ERROR");

  std::stringstream stream2;
  stream2 << mtca4u::RegisterInfoMap::ErrorList::ErrorElem::WARNING;
  BOOST_CHECK(stream2.str() == "WARNING");

  std::stringstream stream3;
  stream3 << mtca4u::RegisterInfoMap::ErrorList::ErrorElem::TYPE(4);
  BOOST_CHECK(stream3.str() == "UNKNOWN");
}

void MapFileTest::testErrorElemCoutStreamOperator(){
  mtca4u::RegisterInfoMap dummyMapFile("dummy.map");

  mtca4u::RegisterInfoMap::RegisterInfo RegisterInfoent1("TEST_REGISTER_NAME_1", 1, 0, 4, 0);
  mtca4u::RegisterInfoMap::RegisterInfo RegisterInfoent2("TEST_REGISTER_NAME_2", 1, 3, 4, 0);

  dummyMapFile.insert(RegisterInfoent1);
  dummyMapFile.insert(RegisterInfoent2);

  mtca4u::RegisterInfoMap::ErrorList errorList;
  dummyMapFile.check(errorList,
      mtca4u::RegisterInfoMap::ErrorList::ErrorElem::WARNING);
  std::stringstream expected_stream;
  expected_stream << mtca4u::RegisterInfoMap::ErrorList::ErrorElem::WARNING <<
      ": Found two registers with overlapping addresses: \"" <<
      "TEST_REGISTER_NAME_2" << "\" and \"" << "TEST_REGISTER_NAME_1" <<
      "\" in file " << "dummy.map" << " in lines "
      << 0 << " and " << 0;
  std::list<mtca4u::RegisterInfoMap::ErrorList::ErrorElem>::iterator errorIterator;
  errorIterator = errorList.errors.begin();

  std::stringstream actual_stream;
  actual_stream << *errorIterator;
  BOOST_CHECK(expected_stream.str() == actual_stream.str());

  mtca4u::RegisterInfoMap dummyMapFile1("dummy.map");
  mtca4u::RegisterInfoMap::RegisterInfo RegisterInfoent3("TEST_REGISTER_NAME_1", 1, 0, 4, 0);
  mtca4u::RegisterInfoMap::RegisterInfo RegisterInfoent4("TEST_REGISTER_NAME_1", 1, 4, 4, 1);
  dummyMapFile1.insert(RegisterInfoent3);
  dummyMapFile1.insert(RegisterInfoent4);
  mtca4u::RegisterInfoMap::ErrorList errorList1;

  dummyMapFile1.check(errorList1,
      mtca4u::RegisterInfoMap::ErrorList::ErrorElem::WARNING);
  std::list<mtca4u::RegisterInfoMap::ErrorList::ErrorElem>::iterator errorIterator1;
  errorIterator1 = errorList1.errors.begin();
  std::stringstream actual_stream1;
  actual_stream1 << *errorIterator1;

  std::stringstream expected_stream1;
  expected_stream1 << mtca4u::RegisterInfoMap::ErrorList::ErrorElem::ERROR <<
      ": Found two registers with the same name: \"" <<
      "TEST_REGISTER_NAME_1" <<
      "\" in file " << "dummy.map" <<\
      " in lines " << 0 << " and " << 0;

  BOOST_CHECK(expected_stream1.str() == actual_stream1.str());
}

inline void
MapFileTest::testErrorListCoutStreamOperator () {
  mtca4u::RegisterInfoMap dummyMapFile("dummy.map");

  mtca4u::RegisterInfoMap::RegisterInfo RegisterInfoent1("TEST_REGISTER_NAME_1", 1, 0, 4, 0);
  mtca4u::RegisterInfoMap::RegisterInfo RegisterInfoent2("TEST_REGISTER_NAME_2", 1, 4, 4, 0);
  mtca4u::RegisterInfoMap::RegisterInfo RegisterInfoent3("TEST_REGISTER_NAME_1", 1, 10, 4, 0);
  mtca4u::RegisterInfoMap::RegisterInfo RegisterInfoent4("TEST_REGISTER_NAME_3", 1, 12, 4, 0);

  dummyMapFile.insert(RegisterInfoent1);
  dummyMapFile.insert(RegisterInfoent2);
  dummyMapFile.insert(RegisterInfoent3);
  dummyMapFile.insert(RegisterInfoent4);

  mtca4u::RegisterInfoMap::ErrorList errorList;
  dummyMapFile.check(errorList,
      mtca4u::RegisterInfoMap::ErrorList::ErrorElem::WARNING);

  std::stringstream expected_stream;
  expected_stream << mtca4u::RegisterInfoMap::ErrorList::ErrorElem::ERROR <<
      ": Found two registers with the same name: \"" <<
      "TEST_REGISTER_NAME_1" <<
      "\" in file " << "dummy.map" <<\
      " in lines " << 0 << " and " << 0 <<std::endl;
  expected_stream << mtca4u::RegisterInfoMap::ErrorList::ErrorElem::WARNING <<
      ": Found two registers with overlapping addresses: \"" <<
      "TEST_REGISTER_NAME_3" << "\" and \"" << "TEST_REGISTER_NAME_1" <<
      "\" in file " << "dummy.map" << " in lines "
      << 0 << " and " << 0 << std::endl;

  std::stringstream actual_stream;
  actual_stream << errorList;
  BOOST_CHECK(expected_stream.str() == actual_stream.str());

}

void MapFileTest::testMapFileCoutStreamOperator(){
  mtca4u::RegisterInfoMap dummyMapFile("dummy.map");
  mtca4u::RegisterInfoMap::RegisterInfo RegisterInfoent1("TEST_REGISTER_NAME_1");
  mtca4u::RegisterInfoMap::RegisterInfo RegisterInfoent2("TEST_REGISTER_NAME_2", 2, 4, 8, 1,
      18, 3, false, 0, "TEST_MODULE");
  mtca4u::RegisterInfoMap::MetaData metaData1("HW_VERSION", "1.6");

  dummyMapFile.insert(metaData1);
  dummyMapFile.insert(RegisterInfoent1);
  dummyMapFile.insert(RegisterInfoent2);

  std::stringstream expected_stream;
  expected_stream << "=======================================" << std::endl;
  expected_stream << "MAP FILE NAME: " << "dummy.map" << std::endl;
  expected_stream << "---------------------------------------" << std::endl;
  expected_stream << "METADATA-> NAME: \"" <<
      "HW_VERSION" << "\" VALUE: " << "1.6" << std::endl;
  expected_stream << "---------------------------------------" << std::endl;
  expected_stream << "TEST_REGISTER_NAME_1" << " 0x"
      << std::hex << 0 << " 0x" << 0 << " 0x" << 0 << " 0x" << 0 << std::dec
      << " 32 0 true" << std::endl;
  expected_stream << "TEST_REGISTER_NAME_2" << " 0x"
      << std::hex << 2 << " 0x" << 4 << " 0x" << 8 << " 0x" << 1 << std::dec
      << " 18 3 false TEST_MODULE" << std::endl;
  expected_stream << "=======================================";

  std::stringstream actual_stream;
  actual_stream << dummyMapFile;

  BOOST_CHECK(actual_stream.str() == expected_stream.str());
}

void MapFileTest::testRegisterInfo(){
  // just test the constructor. Default and all arguments
  mtca4u::RegisterInfoMap::RegisterInfo defaultRegisterInfo;
  BOOST_CHECK( defaultRegisterInfo.name.empty() );
  BOOST_CHECK( defaultRegisterInfo.nElements == 0 );
  BOOST_CHECK( defaultRegisterInfo.address == 0 );
  BOOST_CHECK( defaultRegisterInfo.nBytes == 0 );
  BOOST_CHECK( defaultRegisterInfo.bar == 0 );
  BOOST_CHECK( defaultRegisterInfo.width == 32 );
  BOOST_CHECK( defaultRegisterInfo.nFractionalBits == 0 );
  BOOST_CHECK( defaultRegisterInfo.signedFlag == true );
  BOOST_CHECK( defaultRegisterInfo.lineNumber == 0 );
  BOOST_CHECK( defaultRegisterInfo.module.empty() );

  // Set values which are all different from the default
  mtca4u::RegisterInfoMap::RegisterInfo myRegisterInfo( "MY_NAME",
      4, // nElements
      0x42, //address
      16, // size
      3, // bar
      18, // width
      5, // frac_bits
      false, // signed
      123, //lineNumber
      "MY_MODULE");
  BOOST_CHECK( myRegisterInfo.name == "MY_NAME" );
  BOOST_CHECK( myRegisterInfo.nElements == 4 );
  BOOST_CHECK( myRegisterInfo.address == 0x42 );
  BOOST_CHECK( myRegisterInfo.nBytes == 16 );
  BOOST_CHECK( myRegisterInfo.bar == 3 );
  BOOST_CHECK( myRegisterInfo.width == 18 );
  BOOST_CHECK( myRegisterInfo.nFractionalBits == 5 );
  BOOST_CHECK( myRegisterInfo.signedFlag == false );
  BOOST_CHECK( myRegisterInfo.lineNumber == 123 );
  BOOST_CHECK( myRegisterInfo.module == "MY_MODULE" );
  
  BOOST_CHECK( myRegisterInfo.getDataDescriptor().fundamentalType() == mtca4u::RegisterInfo::FundamentalType::numeric );
  BOOST_CHECK( myRegisterInfo.getDataDescriptor().isIntegral() == false );
  BOOST_CHECK( myRegisterInfo.getDataDescriptor().isSigned() == false );
  BOOST_CHECK( myRegisterInfo.getDataDescriptor().nFractionalDigits() == 2 );   // 2^5 = 32 -> 2 digits
  BOOST_CHECK( myRegisterInfo.getDataDescriptor().nDigits() == 7 );             // 2^(18-5) = 8192 -> 4 digits + 1 for the dot + 2 fractional

  // test a signed integer
  mtca4u::RegisterInfoMap::RegisterInfo myRegisterInfo2( "ANOTHER_NAME",
      1, // nElements
      0xDEADBEEF, //address
      4, // size
      0, // bar
      23, // width
      0, // frac_bits
      true, // signed
      234, //lineNumber
      "XYZ");
  BOOST_CHECK( myRegisterInfo2.name == "ANOTHER_NAME" );
  BOOST_CHECK( myRegisterInfo2.nElements == 1 );
  BOOST_CHECK( myRegisterInfo2.address == 0xDEADBEEF );
  BOOST_CHECK( myRegisterInfo2.nBytes == 4 );
  BOOST_CHECK( myRegisterInfo2.bar == 0 );
  BOOST_CHECK( myRegisterInfo2.width == 23 );
  BOOST_CHECK( myRegisterInfo2.nFractionalBits == 0 );
  BOOST_CHECK( myRegisterInfo2.signedFlag == true );
  BOOST_CHECK( myRegisterInfo2.lineNumber == 234 );
  BOOST_CHECK( myRegisterInfo2.module == "XYZ" );
  
  BOOST_CHECK( myRegisterInfo2.getDataDescriptor().fundamentalType() == mtca4u::RegisterInfo::FundamentalType::numeric );
  BOOST_CHECK( myRegisterInfo2.getDataDescriptor().isIntegral() == true );
  BOOST_CHECK( myRegisterInfo2.getDataDescriptor().isSigned() == true );
  BOOST_CHECK( myRegisterInfo2.getDataDescriptor().nDigits() == 8 );             // 2^23 = 8388608 -> 7 digits + 1 for the sign

  // ... and an unsigned integer (otherwise identical)
  mtca4u::RegisterInfoMap::RegisterInfo myRegisterInfo3( "ANOTHER_NAME",
      1, // nElements
      0xDEADBEEF, //address
      4, // size
      0, // bar
      23, // width
      0, // frac_bits
      false, // signed
      234, //lineNumber
      "XYZ");
  BOOST_CHECK( myRegisterInfo3.name == "ANOTHER_NAME" );
  BOOST_CHECK( myRegisterInfo3.nElements == 1 );
  BOOST_CHECK( myRegisterInfo3.address == 0xDEADBEEF );
  BOOST_CHECK( myRegisterInfo3.nBytes == 4 );
  BOOST_CHECK( myRegisterInfo3.bar == 0 );
  BOOST_CHECK( myRegisterInfo3.width == 23 );
  BOOST_CHECK( myRegisterInfo3.nFractionalBits == 0 );
  BOOST_CHECK( myRegisterInfo3.signedFlag == false );
  BOOST_CHECK( myRegisterInfo3.lineNumber == 234 );
  BOOST_CHECK( myRegisterInfo3.module == "XYZ" );
  
  BOOST_CHECK( myRegisterInfo3.getDataDescriptor().fundamentalType() == mtca4u::RegisterInfo::FundamentalType::numeric );
  BOOST_CHECK( myRegisterInfo3.getDataDescriptor().isIntegral() == true );
  BOOST_CHECK( myRegisterInfo3.getDataDescriptor().isSigned() == false );
  BOOST_CHECK( myRegisterInfo3.getDataDescriptor().nDigits() == 7 );             // 2^23 = 8388608 -> 7 digits

  // a boolean
  mtca4u::RegisterInfoMap::RegisterInfo myRegisterInfo4( "SOME_BOOLEAN",
      1, // nElements
      0x46, //address
      4, // size
      0, // bar
      1, // width
      0, // frac_bits
      false, // signed
      235, //lineNumber
      "TEST");
  BOOST_CHECK( myRegisterInfo4.name == "SOME_BOOLEAN" );
  BOOST_CHECK( myRegisterInfo4.nElements == 1 );
  BOOST_CHECK( myRegisterInfo4.address == 0x46 );
  BOOST_CHECK( myRegisterInfo4.nBytes == 4 );
  BOOST_CHECK( myRegisterInfo4.bar == 0 );
  BOOST_CHECK( myRegisterInfo4.width == 1 );
  BOOST_CHECK( myRegisterInfo4.nFractionalBits == 0 );
  BOOST_CHECK( myRegisterInfo4.signedFlag == false );
  BOOST_CHECK( myRegisterInfo4.lineNumber == 235 );
  BOOST_CHECK( myRegisterInfo4.module == "TEST" );
  
  BOOST_CHECK( myRegisterInfo4.getDataDescriptor().fundamentalType() == mtca4u::RegisterInfo::FundamentalType::boolean );

  // a boolean
  mtca4u::RegisterInfoMap::RegisterInfo myRegisterInfo5( "SOME_VOID",
      1, // nElements
      0x46, //address
      4, // size
      0, // bar
      0, // width
      0, // frac_bits
      false, // signed
      236, //lineNumber
      "TEST");
  BOOST_CHECK( myRegisterInfo5.name == "SOME_VOID" );
  BOOST_CHECK( myRegisterInfo5.nElements == 1 );
  BOOST_CHECK( myRegisterInfo5.address == 0x46 );
  BOOST_CHECK( myRegisterInfo5.nBytes == 4 );
  BOOST_CHECK( myRegisterInfo5.bar == 0 );
  BOOST_CHECK( myRegisterInfo5.width == 0 );
  BOOST_CHECK( myRegisterInfo5.nFractionalBits == 0 );
  BOOST_CHECK( myRegisterInfo5.signedFlag == false );
  BOOST_CHECK( myRegisterInfo5.lineNumber == 236 );
  BOOST_CHECK( myRegisterInfo5.module == "TEST" );
  
  BOOST_CHECK( myRegisterInfo5.getDataDescriptor().fundamentalType() == mtca4u::RegisterInfo::FundamentalType::nodata );
  
}

void MapFileTest::testGetRegistersInModule(){
  mtca4u::RegisterInfoMap someMapFile("some.map");
  mtca4u::RegisterInfoMap::RegisterInfo module0_register1("REGISTER_1", 1, 0x0, 4, 0, 32, 0, true, 0, "MODULE_BAR0");
  mtca4u::RegisterInfoMap::RegisterInfo module1_register1("REGISTER_1", 1, 0x0, 4, 1, 32, 0, true, 0, "MODULE_BAR1");
  mtca4u::RegisterInfoMap::RegisterInfo module0_aregister2("A_REGISTER_2", 1, 0x4, 4, 0, 32, 0, true, 0, "MODULE_BAR0");
  mtca4u::RegisterInfoMap::RegisterInfo module1_aregister2("A_REGISTER_2", 1, 0x4, 4, 1, 32, 0, true, 0, "MODULE_BAR1");
  mtca4u::RegisterInfoMap::RegisterInfo module0_register3("REGISTER_3", 1, 0x8, 4, 0, 32, 0, true, 0, "MODULE_BAR0");
  mtca4u::RegisterInfoMap::RegisterInfo module1_register3("REGISTER_3", 1, 0x8, 4, 1, 32, 0, true, 0, "MODULE_BAR1");
  mtca4u::RegisterInfoMap::RegisterInfo module0_register4("REGISTER_4", 1, 0xC, 4, 0, 32, 0, true, 0, "MODULE_BAR0");
  mtca4u::RegisterInfoMap::RegisterInfo module1_register4("REGISTER_4", 1, 0xC, 4, 1, 32, 0, true, 0, "MODULE_BAR1");

  // add stuff from two different modules, interleaved. We need all registers back in
  // alphabetical order.
  someMapFile.insert( module0_register1 );
  someMapFile.insert( module1_register1 );
  someMapFile.insert( module0_aregister2 );
  someMapFile.insert( module1_aregister2 );
  someMapFile.insert( module0_register3 );
  someMapFile.insert( module1_register3 );
  someMapFile.insert( module0_register4 );
  someMapFile.insert( module1_register4 );

  std::list< mtca4u::RegisterInfoMap::RegisterInfo > resultList = someMapFile.getRegistersInModule( "MODULE_BAR1" );
  BOOST_CHECK( resultList.size() == 4 );

  // create a reference list to iterate
  std::list< mtca4u::RegisterInfoMap::RegisterInfo > referenceList;
  referenceList.push_back(  module1_aregister2 );
  referenceList.push_back(  module1_register1 );
  referenceList.push_back(  module1_register3 );
  referenceList.push_back(  module1_register4 );

  std::list< mtca4u::RegisterInfoMap::RegisterInfo >::const_iterator resultIter;
  std::list< mtca4u::RegisterInfoMap::RegisterInfo >::const_iterator referenceIter;
  for (resultIter = resultList.begin(), referenceIter = referenceList.begin();
      (resultIter != resultList.end()) && (referenceIter != referenceList.end());
      ++resultIter, ++referenceIter){
    std::stringstream message;
    message << "Failed comparison on Register '" << referenceIter->name
        << "', module '" << referenceIter->module << "'";
    BOOST_CHECK( compareRegisterInfoents( *resultIter, *referenceIter ) == true );
  }

  std::list< mtca4u::RegisterInfoMap::RegisterInfo > shouldBeEmptyList = someMapFile.getRegistersInModule( "MODULE_BAR5" );
  BOOST_CHECK( shouldBeEmptyList.empty() );
}
