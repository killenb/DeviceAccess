#ifndef _MTCA4U_SEQUENCE_DE_MULTIPLEXER_H_
#define _MTCA4U_SEQUENCE_DE_MULTIPLEXER_H_

#include "RegisterInfoMap.h"
#include "FixedPointConverter.h"
#include "DeviceBackend.h"
#include "MultiplexedDataAccessorException.h"
#include "MapException.h"
#include "NotImplementedException.h"
#include <sstream>
#include <boost/shared_ptr.hpp>

namespace mtca4u{

template<class UserType, class SequenceWordType>
class SequenceDeMultiplexerTest;

template<class UserType>
class MixedTypeTest;

typedef RegisterInfoMap::RegisterInfo SequenceInfo;


static const std::string MULTIPLEXED_SEQUENCE_PREFIX="AREA_MULTIPLEXED_SEQUENCE_";
static const std::string SEQUENCE_PREFIX="SEQUENCE_";

/** Base class which does not depend on the SequenceWordType.
 */
template<class UserType>
class MultiplexedDataAccessor{
public:
	/** Constructor to intialise the members.
	 */
	MultiplexedDataAccessor( boost::shared_ptr< DeviceBackend > const & ioDevice,
			std::vector< FixedPointConverter > const & converters );

	/** Operator to access individual sequences.
	 */
	std::vector<UserType> & operator[](size_t sequenceIndex);

	/** Read the data from the device, de-multiplex the hardware IO buffer and
	 *  fill the sequence buffers using the fixed point converters. The read
	 *  method will handle reads into the DMA regions as well
	 */
	virtual void read() = 0;

	/** Multiplex the data from the sequence buffer into the hardware IO buffer,
	 * using the fixed point converters, and write it to the device. Can be used
	 * to write to DMA memory Areas, but this functionality has not been
	 * implemented yet
	 */
	virtual void write() = 0;

	/**
	 * Return the number of sequences that have been Multiplexed
	 */
	virtual size_t getNumberOfDataSequences() = 0;

	/** A factory function which parses the register mapping and determines the
	 *  correct type of SequenceDeMultiplexer.
	 */
	static boost::shared_ptr< MultiplexedDataAccessor<UserType> > createInstance(
			std::string const & multiplexedSequenceName,
			std::string const & moduleName,
			boost::shared_ptr< DeviceBackend > const & ioDevice,
			boost::shared_ptr< RegisterInfoMap > const & registerMapping );

	/**
	 * Default destructor
	 */
	virtual ~MultiplexedDataAccessor(){};
protected:
	/** The converted data for the user space. */
	std::vector< std::vector< UserType > > _sequences;

	/** One fixed point converter for each sequence. */
	std::vector< FixedPointConverter > _converters;

	/** The device from (/to) which to perform the DMA transfer */
	boost::shared_ptr<DeviceBackend> _ioDevice; // This should not be a reference.
	// Reason: there is a chance that this
	// can reference a shared pointer object
	// with use_count==1. This risks the
	// reference pointing to an invalid
	// object if this shared pointer goes
	// out of scope.

	size_t _nBlocks;
};

template<class UserType>
std::vector<UserType> & MultiplexedDataAccessor<UserType>::operator[](
		size_t sequenceIndex){
	return _sequences[sequenceIndex];
}

template<class UserType, class SequenceWordType>
class FixedTypeMuxedDataAccessor : public MultiplexedDataAccessor<UserType>{
public:
	/** Convenience constructor which extracts the required information
	 *  to create the converters etc. from the register mapping.
	 */
	//SequenceDeMultiplexer( std::string const & sequenceName,
	//			 boost::shared_ptr< DeviceBackend > & ioDevice,
	//			 boost::shared_ptr< RegisterInfoMap > const & registerMapping );

	/** Contructor which needs pre-defined sequence infos and fixed point
	 *  converters. Mainly used for testing.
	 *  FIXME: Could be private because the tests are friends?
	 */
	FixedTypeMuxedDataAccessor(boost::shared_ptr<DeviceBackend>const& ioDevice,
			SequenceInfo const& areaInfo,
			std::list<SequenceInfo> const& sequenceInfos);

	/** The simplest possible constructor for testing.
	 */
	FixedTypeMuxedDataAccessor( boost::shared_ptr< DeviceBackend > const & ioDevice,
			SequenceInfo const & areaInfo,
			std::vector< FixedPointConverter > const & converters );

	void read();

	void write();

	size_t getNumberOfDataSequences();

private:
	/** Fill the sequeces by de-multiplexing the sequence read from the device
	 *  and calling the correct fixed point converter for each word.
	 */
	void fillSequences();

	/** Multiplex the sequence buffers into the ioBuffer.
	 */
	void fillIO_Buffer();

	/** The buffer which is used by the ioDevice,
	 *  already formatted in the correct word size.
	 */
	std::vector< SequenceWordType > _ioBuffer;

	/** The main sequence info. Address needed to access the hardware */
	SequenceInfo _areaInfo;

	/**
	 * Are we accessing DMA fetched Memory
	 */
	bool isDMAMemoryArea();

	friend class SequenceDeMultiplexerTest<UserType, SequenceWordType>;
};

template <class UserType>
class MixedTypeMuxedDataAccessor : public MultiplexedDataAccessor<UserType>{
public:
	/**Class Constructor with minimum information to create the converters
	 */
	MixedTypeMuxedDataAccessor(boost::shared_ptr< DeviceBackend > const & ioDevice,
			SequenceInfo const & areaInfo,
			std::vector<SequenceInfo> const& sequenceInfos,
			std::vector< FixedPointConverter > const & converters );
	void read();

	void write();

	size_t getNumberOfDataSequences();

	uint32_t getSizeOneBlock(){return _sizeOneBlock;}

private:

	void fillSequences();

	void fillIO_Buffer();

	std::vector<int32_t> _ioBuffer;

	SequenceInfo _areaInfo;

	std::vector<SequenceInfo> _sequenceInfos;

	uint32_t _sizeOneBlock;

	bool isDMAMemoryArea();

	friend class MixedTypeTest<UserType>;
};

template<class UserType>
class MixedTypeTest{
public:
	MixedTypeTest(MixedTypeMuxedDataAccessor < UserType > *mixedTypeInstance = NULL) : _mixedTypeInstance(mixedTypeInstance){};
	uint32_t getSizeOneBlock(){
		return _mixedTypeInstance->_sizeOneBlock;
	}
	size_t getNBlock(){
		return _mixedTypeInstance->_nBlocks;
	}
	size_t getConvertersSize(){
		return (_mixedTypeInstance->_converters).size();
	}
	int32_t getIOBUffer(uint index){
		return _mixedTypeInstance->_ioBuffer[index];
	}

private:
	MixedTypeMuxedDataAccessor<UserType> *_mixedTypeInstance;
};

// nBlocks can only be set by the derrived classes. There is no way to know the
// number of bytes per block at this point, so it cannot be calculates, even if the
// SequenceInfo of the area was known. The only way would be passing all Sequence infos
// which would duplicate the code to analyse it because it is also needed in the
// derrived class.
template<class UserType>
MultiplexedDataAccessor<UserType>::MultiplexedDataAccessor(
		boost::shared_ptr< DeviceBackend > const & ioDevice,
		std::vector< FixedPointConverter > const & converters ) :
		_sequences(converters.size()),
		_converters(converters),
		_ioDevice(ioDevice),
		_nBlocks(0){
}

template<class UserType, class SequenceWordType>
FixedTypeMuxedDataAccessor<UserType, SequenceWordType>::FixedTypeMuxedDataAccessor(
		boost::shared_ptr< DeviceBackend >const & ioDevice,
		SequenceInfo const & areaInfo,
		std::vector< FixedPointConverter > const & converters ) :
		MultiplexedDataAccessor<UserType>(ioDevice, converters),
		_ioBuffer(areaInfo.reg_size),
		_areaInfo(areaInfo){

	MultiplexedDataAccessor<UserType>::_nBlocks = areaInfo.reg_size / sizeof(SequenceWordType) / converters.size();

	for (size_t i=0; i<MultiplexedDataAccessor<UserType>::_sequences.size(); ++i){
		MultiplexedDataAccessor<UserType>::_sequences[i].resize(MultiplexedDataAccessor<UserType>::_nBlocks);
	}
}

template <class UserType, class SequenceWordType>
void FixedTypeMuxedDataAccessor<UserType, SequenceWordType>::read() {
	// For now use an if check.  A read function should be returned by
	// some factory later. The factory should go through the map file to
	// understand what kind of area is being accessed by the accessor (through the
	// register/area name given to it). Then provide the suitable callback to a
	// method that reads/writes this area

	// if this is not a dma area -> use the readArea from the interface
	// else the read DMA
	if (isDMAMemoryArea()) {
		MultiplexedDataAccessor<UserType>::_ioDevice->readDMA(_areaInfo.reg_bar,
				_areaInfo.reg_address, reinterpret_cast<int32_t*>(&(_ioBuffer[0])),
				_areaInfo.reg_size);
	} else {
		MultiplexedDataAccessor<UserType>::_ioDevice->read(_areaInfo.reg_bar,
				_areaInfo.reg_address, reinterpret_cast<int32_t*>(&(_ioBuffer[0])),
				//_areaInfo.reg_size, _areaInfo.reg_bar);
				_areaInfo.reg_size);
	}

	fillSequences();
}

template<class UserType, class SequenceWordType>
void FixedTypeMuxedDataAccessor<UserType, SequenceWordType>::fillSequences(){
	size_t globalIndex=0;
	for(size_t blockIndex=0; blockIndex < MultiplexedDataAccessor<UserType>::_nBlocks;
			++blockIndex){
		for(size_t sequenceIndex=0;
				sequenceIndex < MultiplexedDataAccessor<UserType>::_converters.size();
				++sequenceIndex, ++globalIndex){
			MultiplexedDataAccessor<UserType>::_sequences[sequenceIndex][blockIndex] =
					MultiplexedDataAccessor<UserType>::_converters[sequenceIndex].template toCooked<UserType>(_ioBuffer[globalIndex]);
		}
	}
}

template <class UserType, class SequenceWordType>
void FixedTypeMuxedDataAccessor<UserType, SequenceWordType>::write() {
	fillIO_Buffer();

	if (isDMAMemoryArea()) {
		throw NotImplementedException("writeViaDMA is not implemented yet");
	} else {
		MultiplexedDataAccessor<UserType>::_ioDevice->write(_areaInfo.reg_bar,
				_areaInfo.reg_address, reinterpret_cast<int32_t*>(&(_ioBuffer[0])),
				_areaInfo.reg_size);
	}
}

template<class UserType, class SequenceWordType>
void FixedTypeMuxedDataAccessor<UserType, SequenceWordType>::fillIO_Buffer(){
	size_t globalIndex=0;
	for(size_t blockIndex=0; blockIndex < MultiplexedDataAccessor<UserType>::_nBlocks;
			++blockIndex){
		for(size_t sequenceIndex=0;
				sequenceIndex < MultiplexedDataAccessor<UserType>::_converters.size();
				++sequenceIndex, ++globalIndex){
			_ioBuffer[globalIndex] = MultiplexedDataAccessor<UserType>::_converters[sequenceIndex].toRaw(
					MultiplexedDataAccessor<UserType>::_sequences[sequenceIndex][blockIndex] );
		}
	}
}

template <class UserType>
MixedTypeMuxedDataAccessor<UserType>::MixedTypeMuxedDataAccessor(boost::shared_ptr< DeviceBackend > const & ioDevice,
		SequenceInfo const & areaInfo,
		std::vector<SequenceInfo> const& sequenceInfos,
		std::vector< FixedPointConverter > const & converters ) :
		MultiplexedDataAccessor<UserType>(ioDevice, converters),
		_ioBuffer(areaInfo.reg_size),
		_areaInfo(areaInfo),
		_sequenceInfos(sequenceInfos){
	size_t indexTemp = 0;
	size_t wordSize = 0;

	_sizeOneBlock = 0;
	//to change. retrieve actual size from the map file
	while (indexTemp<converters.size()){
		wordSize += _sequenceInfos[indexTemp].reg_size;
		if (wordSize > 4){
			_sizeOneBlock++;
			wordSize = _sequenceInfos[indexTemp].reg_size;
		}
		indexTemp++;
		if (indexTemp == converters.size()){
			_sizeOneBlock++;
		}
	}

	MultiplexedDataAccessor<UserType>::_nBlocks = areaInfo.reg_size / 4 / _sizeOneBlock ;

	for (size_t i=0; i<MultiplexedDataAccessor<UserType>::_sequences.size(); ++i){
		MultiplexedDataAccessor<UserType>::_sequences[i].resize(MultiplexedDataAccessor<UserType>::_nBlocks);
	}
}

template <class UserType>
void MixedTypeMuxedDataAccessor<UserType>::read() {

	// if this is not a dma area -> use the readArea from the interface
	// else the read DMA
	if (isDMAMemoryArea()) {
		MultiplexedDataAccessor<UserType>::_ioDevice->readDMA(
				_areaInfo.reg_bar,
				_areaInfo.reg_address, &(_ioBuffer[0]),
				_areaInfo.reg_size);
	} else {
		MultiplexedDataAccessor<UserType>::_ioDevice->read(
				_areaInfo.reg_bar,
				_areaInfo.reg_address, &(_ioBuffer[0]),
				_areaInfo.reg_size);
	}

	fillSequences();
}

template <class UserType>
void MixedTypeMuxedDataAccessor<UserType>::fillSequences(){
	uint8_t *standOfMyioBuffer = reinterpret_cast<uint8_t*>(&_ioBuffer[0]);
	for(size_t blockIndex=0;
			blockIndex < MultiplexedDataAccessor<UserType>::_nBlocks;
			++blockIndex){
		for(size_t sequenceIndex=0;
				sequenceIndex < MultiplexedDataAccessor<UserType>::_converters.size();
				++sequenceIndex){
			switch(_sequenceInfos[sequenceIndex].reg_size){
			case 1: //8 bit variables
				MultiplexedDataAccessor<UserType>::_sequences[sequenceIndex][blockIndex] =
						MultiplexedDataAccessor<UserType>::_converters[sequenceIndex].template toCooked<UserType>(*(standOfMyioBuffer));
				standOfMyioBuffer++;
				break;
			case 2: //16 bit words
				MultiplexedDataAccessor<UserType>::_sequences[sequenceIndex][blockIndex] =
						MultiplexedDataAccessor<UserType>::_converters[sequenceIndex].template toCooked<UserType>(*(reinterpret_cast<uint16_t*>(standOfMyioBuffer)));
				standOfMyioBuffer = standOfMyioBuffer + 2;
				break;
			case 4: //32 bit words
				MultiplexedDataAccessor<UserType>::_sequences[sequenceIndex][blockIndex] =
						MultiplexedDataAccessor<UserType>::_converters[sequenceIndex].template toCooked<UserType>(*(reinterpret_cast<uint32_t*>(standOfMyioBuffer)));
				standOfMyioBuffer = standOfMyioBuffer + 4;
				break;
			}
		}
	}
}

template <class UserType>
void MixedTypeMuxedDataAccessor<UserType>::write() {
	fillIO_Buffer();

	if (isDMAMemoryArea()) {
		throw NotImplementedException("writeViaDMA is not implemented yet");
	} else {
		MultiplexedDataAccessor<UserType>::_ioDevice->write(
				_areaInfo.reg_bar,
				_areaInfo.reg_address, &(_ioBuffer[0]),
				_areaInfo.reg_size);
	}
}

template<class UserType>
void MixedTypeMuxedDataAccessor<UserType>::fillIO_Buffer(){
	uint8_t *standOfMyioBuffer = reinterpret_cast<uint8_t*>(&_ioBuffer[0]);
	for(size_t blockIndex=0;
			blockIndex < MultiplexedDataAccessor<UserType>::_nBlocks;
			++blockIndex){
		for(size_t sequenceIndex=0;
				sequenceIndex < MultiplexedDataAccessor<UserType>::_converters.size();
				++sequenceIndex){
			switch(_sequenceInfos[sequenceIndex].reg_size){
			case 1: //8 bit variables
				*(standOfMyioBuffer) = MultiplexedDataAccessor<UserType>::_converters[sequenceIndex].toRaw(
						MultiplexedDataAccessor<UserType>::_sequences[sequenceIndex][blockIndex] );
				standOfMyioBuffer++;
				break;
			case 2: //16 bit variables
				*(reinterpret_cast<uint16_t*>(standOfMyioBuffer)) = MultiplexedDataAccessor<UserType>::_converters[sequenceIndex].toRaw(
						MultiplexedDataAccessor<UserType>::_sequences[sequenceIndex][blockIndex] );
				standOfMyioBuffer = standOfMyioBuffer + 2;
				break;
			case 4: //32 bit variables
				*(reinterpret_cast<uint32_t*>(standOfMyioBuffer)) = MultiplexedDataAccessor<UserType>::_converters[sequenceIndex].toRaw(
						MultiplexedDataAccessor<UserType>::_sequences[sequenceIndex][blockIndex] );
				standOfMyioBuffer = standOfMyioBuffer + 4;
				break;
			}
		}
	}
}

template<class UserType>
boost::shared_ptr< MultiplexedDataAccessor<UserType> >
MultiplexedDataAccessor<UserType>::createInstance(
		std::string const & multiplexedSequenceName,
		std::string const & moduleName,
		boost::shared_ptr< DeviceBackend > const & ioDevice,
		boost::shared_ptr< RegisterInfoMap > const & registerMapping ){

	std::string areaName = MULTIPLEXED_SEQUENCE_PREFIX+multiplexedSequenceName;

	SequenceInfo multiplexedSequenceInfo;
	registerMapping->getRegisterInfo( areaName, multiplexedSequenceInfo, moduleName);
	std::vector< FixedPointConverter > converters;
	std::vector<SequenceInfo> sequencesInfo;
	size_t i = 0;
	size_t sequenceWordSize = 0; // initializing as 0 to avoid clang warning
	// "'sequenceWordSize' may be used
	// uninitialized in this function". Looking at
	// the logic below we are actually putting the  reg_size inside
	// sequenceWordSize, before using it in the if conditional
	// check
	bool useFixedType=true;
	while(true){
		SequenceInfo sequenceInfo;
		std::stringstream sequenceNameStream;
		sequenceNameStream << SEQUENCE_PREFIX << multiplexedSequenceName << "_" << i++;
		try{
			registerMapping->getRegisterInfo( sequenceNameStream.str(), sequenceInfo,
					moduleName );
		}catch(MapFileException & ){
			break;
		}

		if( sequenceInfo.reg_elem_nr != 1 ){
			throw MultiplexedDataAccessorException( "Sequence words must have exactly one element",
					MultiplexedDataAccessorException::INVALID_N_ELEMENTS );
		}
		sequencesInfo.push_back( sequenceInfo );
		converters.push_back( FixedPointConverter( sequenceInfo.reg_width,
				sequenceInfo.reg_frac_bits,
				sequenceInfo.reg_signed ) );
		if(converters.size()==1){
			sequenceWordSize=sequenceInfo.reg_size;
		}else{
			if(sequenceWordSize != sequenceInfo.reg_size){
				useFixedType=false;
			}
		}
	}

	if( converters.empty() ){
		throw MultiplexedDataAccessorException( "No sequenes found for name \""+
				multiplexedSequenceName+"\".",
				MultiplexedDataAccessorException::EMPTY_AREA );
	}

	if( !useFixedType ){
		//throw NotImplementedException("mixed word sizes for the sequences are not supported yet.");
		return boost::shared_ptr< mtca4u::MultiplexedDataAccessor<UserType> >(
				new MixedTypeMuxedDataAccessor<UserType>( ioDevice,
						multiplexedSequenceInfo,
						sequencesInfo,
						converters) );
	}
	switch( sequenceWordSize ){
	case 1:
		return boost::shared_ptr< mtca4u::MultiplexedDataAccessor<UserType> >(
				new FixedTypeMuxedDataAccessor<UserType, int8_t>( ioDevice,
						multiplexedSequenceInfo,
						converters ) );
	case 2:
		return boost::shared_ptr< mtca4u::MultiplexedDataAccessor<UserType> >(
				new FixedTypeMuxedDataAccessor<UserType, int16_t>( ioDevice,
						multiplexedSequenceInfo,
						converters ) );
	case 4:
		return boost::shared_ptr< mtca4u::MultiplexedDataAccessor<UserType> >(
				new FixedTypeMuxedDataAccessor<UserType, int32_t>( ioDevice,
						multiplexedSequenceInfo,
						converters ) );
	default:
		throw MultiplexedDataAccessorException( "Sequence word size must correspond to a primitive type",  MultiplexedDataAccessorException::INVALID_WORD_SIZE );
	}
}

template <class UserType, class SequenceWordType>
size_t FixedTypeMuxedDataAccessor<UserType, SequenceWordType>::getNumberOfDataSequences() {
	return (MultiplexedDataAccessor<UserType>::_sequences.size());
}

template <class UserType>
size_t MixedTypeMuxedDataAccessor<UserType>::getNumberOfDataSequences() {
	return (MultiplexedDataAccessor<UserType>::_sequences.size());
}

}  //namespace mtca4u

template <class UserType, class SequenceWordType>
bool mtca4u::FixedTypeMuxedDataAccessor<UserType,
SequenceWordType>::isDMAMemoryArea() {
	// 0xD as the  register bar is used to indicate DMA regions in the current
	// implementation;
	// This is a detail which would probably change in the future
	return (_areaInfo.reg_bar == 0xD);
}

template <class UserType>
bool mtca4u::MixedTypeMuxedDataAccessor<UserType>::isDMAMemoryArea() {
	// 0xD as the  register bar is used to indicate DMA regions in the current
	// implementation;
	// This is a detail which would probably change in the future
	return (_areaInfo.reg_bar == 0xD);
}

#endif // _MTCA4U_SEQUENCE_DE_MULTIPLEXER_H_