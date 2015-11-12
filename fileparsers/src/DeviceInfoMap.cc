#include "predicates.h"
#include "MapException.h"
#include "DeviceInfoMap.h"
#include <algorithm>

namespace mtca4u{

DeviceInfoMap::DeviceInfoMap(const std::string &file_name)
: dmap_file_name(file_name) {
}

size_t DeviceInfoMap::getdmapFileSize() {
	return dmap_file_elems.size();
}

std::ostream& operator<<(std::ostream &os, const DeviceInfoMap& me) {
	size_t size;
	os << "=======================================" << std::endl;
	os << "MAP FILE NAME: " << me.dmap_file_name << std::endl;
	os << "---------------------------------------" << std::endl;
	for (size = 0; size < me.dmap_file_elems.size(); size++) {
		os << me.dmap_file_elems[size] << std::endl;
	}
	os << "=======================================";
	return os;
}

void DeviceInfoMap::insert(const DeviceInfo &elem) {
	dmap_file_elems.push_back(elem);
}

void DeviceInfoMap::getDeviceInfo(const std::string& dev_name, DeviceInfo &value) {
	std::vector<DeviceInfo>::iterator iter;
	iter = find_if(dmap_file_elems.begin(), dmap_file_elems.end(), findDevByName_pred(dev_name));
	if (iter == dmap_file_elems.end()) {
		throw DMapFileException("Cannot find device \"" + dev_name + "\"in DMAP file", LibMapException::EX_NO_DEVICE_IN_DMAP_FILE);
	}
	value = *iter;
}

DeviceInfoMap::DeviceInfo::DeviceInfo() : dmap_file_line_nr(0)
{
}        

std::pair<std::string, std::string> DeviceInfoMap::DeviceInfo::getDeviceFileAndMapFileName() const
{
	return std::pair<std::string, std::string>(dev_file, map_file_name);
}        

std::ostream& operator<<(std::ostream &os, const DeviceInfoMap::DeviceInfo& de) {
	os << "(" << de.dmap_file_name << ") NAME: " << de.dev_name << " DEV : " << de.dev_file << " MAP : " << de.map_file_name;
	return os;
}

//fixme: why is level not used?
bool DeviceInfoMap::check(DeviceInfoMap::ErrorList &err, DeviceInfoMap::ErrorList::ErrorElem::TYPE /*level*/) {

	std::vector<DeviceInfoMap::DeviceInfo> dmaps = dmap_file_elems;
	std::vector<DeviceInfoMap::DeviceInfo>::iterator iter_p, iter_n;
	bool ret = true;

	err.clear();
	if (dmaps.size() < 2) {
		return true;
	}

	std::sort(dmaps.begin(), dmaps.end(), copmaredRegisterInfosByName2_functor());
	iter_p = dmaps.begin();
	iter_n = iter_p + 1;
	while (1) {
		if ((*iter_p).dev_name == (*iter_n).dev_name) {
			if ((*iter_p).dev_file != (*iter_n).dev_file || (*iter_p).map_file_name != (*iter_n).map_file_name) {
				err.insert(ErrorList::ErrorElem(ErrorList::ErrorElem::ERROR, ErrorList::ErrorElem::NONUNIQUE_DEVICE_NAME, (*iter_p), (*iter_n)));
				ret = false;
			}
		}
		iter_p = iter_n;
		iter_n = ++iter_n;
		if (iter_n == dmaps.end()) {
			break;
		}
	}
	return ret;
}

std::ostream& operator<<(std::ostream &os, const DeviceInfoMap::ErrorList::ErrorElem::TYPE& me) {
	switch (me) {
	case DeviceInfoMap::ErrorList::ErrorElem::ERROR:
		os << "ERROR";
		break;
	case DeviceInfoMap::ErrorList::ErrorElem::WARNING:
		os << "WARNING";
		break;
	default:
		os << "UNKNOWN";
		break;
	}
	return os;
}

DeviceInfoMap::ErrorList::ErrorElem::ErrorElem(DeviceInfoMap::ErrorList::ErrorElem::TYPE info_type, DeviceInfoMap::ErrorList::ErrorElem::DMAP_FILE_ERR e_type, const DeviceInfoMap::DeviceInfo &dev_1, const DeviceInfoMap::DeviceInfo &dev_2) {
	err_type = e_type;
	err_dev_1 = dev_1;
	err_dev_2 = dev_2;
	type = info_type;
}

std::ostream& operator<<(std::ostream &os, const DeviceInfoMap::ErrorList::ErrorElem& me) {
	switch (me.err_type) {
	case DeviceInfoMap::ErrorList::ErrorElem::NONUNIQUE_DEVICE_NAME:
		os << me.type << ": Found two devices with the same name but different properties: \"" << me.err_dev_1.dev_name << "\" in file \"" << me.err_dev_1.dmap_file_name << "\" in line " << me.err_dev_1.dmap_file_line_nr << " and \"" << me.err_dev_2.dmap_file_name << "\" in line " << me.err_dev_2.dmap_file_line_nr;
		break;
	}
	return os;
}

void DeviceInfoMap::ErrorList::clear() {
	errors.clear();
}

void DeviceInfoMap::ErrorList::insert(const ErrorElem& elem) {
	errors.push_back(elem);
}

std::ostream& operator<<(std::ostream &os, const DeviceInfoMap::ErrorList& me) {
	std::list<DeviceInfoMap::ErrorList::ErrorElem>::const_iterator iter;
	for (iter = me.errors.begin(); iter != me.errors.end(); ++iter) {
		os << (*iter) << std::endl;
	}
	return os;
}

DeviceInfoMap::iterator DeviceInfoMap::begin() {
	return dmap_file_elems.begin();
}

DeviceInfoMap::iterator DeviceInfoMap::end() {
	return dmap_file_elems.end();
}

}//namespace mtca4u
