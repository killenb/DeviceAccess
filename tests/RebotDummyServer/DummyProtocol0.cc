#include "DummyProtocol0.h"
#include "RebotDummyServer.h"

namespace ChimeraTK{
  
  DummyProtocol0::DummyProtocol0(RebotDummyServer & parent)
    : _parent(parent) {
  }

  void DummyProtocol0::multiWordRead(std::vector<uint32_t>& buffer){
    uint32_t numberOfWordsToRead = buffer.at(2);
    
    if (numberOfWordsToRead > 361) { // not supported
      _parent.sendSingleWord(RebotDummyServer::TOO_MUCH_DATA_REQUESTED);
    } else {
      _parent.readRegisterAndSendData(buffer);
    }
  }

}//namespace ChimeraTK
