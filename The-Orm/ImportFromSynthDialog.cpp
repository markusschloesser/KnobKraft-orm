/*
   Copyright (c) 2020 Christof Ruch. All rights reserved.

   Dual licensed: Distributed under Affero GPL license by default, an MIT license is available for purchase
*/

#include "ImportFromSynthDialog.h"

ImportFromSynthThread::ImportFromSynthThread() : ThreadWithProgressWindow("Importing...", true, true)
{
}

void ImportFromSynthThread::run()
{
	while (!threadShouldExit()) {
		Thread::sleep(100);
	}
}

bool ImportFromSynthThread::shouldAbort() const
{
	return threadShouldExit();
}

void ImportFromSynthThread::setProgressPercentage(double zeroToOne)
{
	setProgress(zeroToOne);
}

void ImportFromSynthThread::onSuccess()
{
}

void ImportFromSynthThread::onCancel()
{
	signalThreadShouldExit();
}

ImportFromSynthDialog::ImportFromSynthDialog(midikraft::Synth *synth, TSuccessHandler onOk) : onOk_(onOk)
{
	addAndMakeVisible(propertyPanel_);
	addAndMakeVisible(cancel_);
	addAndMakeVisible(ok_);
	ok_.setButtonText("OK");
	ok_.addListener(this);
	cancel_.setButtonText("Cancel");
	cancel_.addListener(this);

	// Populate the bank selector
	numBanks_ = synth->numberOfBanks();
	StringArray choices;
	Array<var> choiceValues;
	for (int i = 0; i < numBanks_; i++) {
		choices.add(synth->friendlyBankName(MidiBankNumber::fromZeroBase(i)));
		choiceValues.add(i);
	}
	choices.add("All"); choiceValues.add(numBanks_);
	bankValue_ = Array<var>(0); // Only the first entry is selected by default
	banks_ = new MultiChoicePropertyComponent(bankValue_, "Banks", choices, choiceValues);
	banks_->setExpanded(true);
	propertyPanel_.addProperties({ banks_});

	setBounds(0, 0, 400, 400);
}

void ImportFromSynthDialog::resized()
{
	Rectangle<int> area(getLocalBounds());
	auto bottom = area.removeFromBottom(40).reduced(8);
	ok_.setBounds(bottom.removeFromLeft(bottom.getWidth() / 2));
	cancel_.setBounds(bottom);
	propertyPanel_.setBounds(area.reduced(8));
}

void ImportFromSynthDialog::buttonClicked(Button *button)
{
	if (button == &ok_) {
		// Close Window
		if (DialogWindow* dw = findParentComponentOfClass<DialogWindow>()) {
			dw->exitModalState(1);
		}
		std::vector<MidiBankNumber> result;
		var selected = bankValue_.getValue();
		for (auto bank : *selected.getArray()) {
			if ((int)bank < numBanks_) {
				result.push_back(MidiBankNumber::fromZeroBase((int)bank));
			}
			else {
				// All selected, just add all banks into the array
				jassertfalse;
			}
		}

		onOk_(result);
	}
	else if (button == &cancel_) {
		if (DialogWindow* dw = findParentComponentOfClass<DialogWindow>()) {
			dw->exitModalState(-1);
		}
	}
}

