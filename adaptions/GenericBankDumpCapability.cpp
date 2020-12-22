/*
   Copyright (c) 2020 Christof Ruch. All rights reserved.

   Dual licensed: Distributed under Affero GPL license by default, an MIT license is available for purchase
*/

#include "GenericBankDumpCapability.h"

#include "Sysex.h"

#include <pybind11/embed.h>
#include <pybind11/stl.h>

#include <boost/format.hpp>

namespace py = pybind11;

namespace knobkraft {

	std::vector<juce::MidiMessage> GenericBankDumpCapability::requestBankDump(MidiBankNumber bankNo) const
	{
		try {
			int c = me_->channel().toZeroBasedInt();
			int bank = bankNo.toZeroBased();
			py::object result = me_->callMethod("createBankDumpRequest", c, bank);
			std::vector<uint8> byteData = GenericAdaptation::intVectorToByteVector(result.cast<std::vector<int>>());
			return Sysex::vectorToMessages(byteData);
		}
		catch (std::exception &ex) {
			SimpleLogger::instance()->postMessage((boost::format("Adaptation: Error calling createBankDumpRequest: %s") % ex.what()).str());
			return {};
		}
	}

	bool GenericBankDumpCapability::isBankDump(const MidiMessage& message) const
	{
		try {
			auto vector = me_->messageToVector(message);
			py::object result = me_->callMethod("isPartOfBankDump", vector);
			return result.cast<bool>();
		}
		catch (std::exception &ex) {
			SimpleLogger::instance()->postMessage((boost::format("Adaptation: Error calling isPartOfBankDump: %s") % ex.what()).str());
			return false;
		}
	}

	bool GenericBankDumpCapability::isBankDumpFinished(std::vector<MidiMessage> const &bankDump) const
	{
		try {
			std::vector<std::vector<int>> vector;
			for (auto message : bankDump) {
				vector.push_back(me_->messageToVector(message));
			}
			py::object result = me_->callMethod("isBankDumpFinished", vector);
			return result.cast<bool>();
		}
		catch (std::exception &ex) {
			SimpleLogger::instance()->postMessage((boost::format("Adaptation: Error calling isBankDumpFinished: %s") % ex.what()).str());
			return false;
		}
	}

	midikraft::TPatchVector GenericBankDumpCapability::patchesFromSysexBank(const MidiMessage& message) const
	{
		try {
			std::vector<int> vector = me_->messageToVector(message);
			py::object result = me_->callMethod("extractPatchesFromBank", vector);
			return {};
		}
		catch (std::exception &ex) {
			SimpleLogger::instance()->postMessage((boost::format("Adaptation: Error calling extractPatchesFromBank: %s") % ex.what()).str());
			return {};
		}
	}

}


