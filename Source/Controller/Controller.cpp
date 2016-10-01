/*
 * Controller.cpp
 *
 *  Created on: 16 Apr 2016
 *      Author: jeremy
 */

#include "Controller.h"

namespace Gui
{

	Controller::Controller(Glib::RefPtr<Gtk::Builder>& builder, std::string const& mainFolder)
	: mainFolder(mainFolder), builder(builder)
	{

		// Get all widgets
		{
			// Buttons
			builder->get_widget("AboutButton", aboutButton);
			builder->get_widget("PlayButton", playButton);
			builder->get_widget("DeleteDrumKitButton", deleteKitButton);
			builder->get_widget("EnableClickButton", enableClickButton);
			builder->get_widget("RhythmCoachPrefButton", rhythmCoachPrefButton);
			builder->get_widget("MetronomeConfigSave", metronomeConfigSave);


			// Lists
			builder->get_widget("KitsList", kitsList);
			builder->get_widget("MetronomeSoundType", clickTypes);

			// Faders' box
			builder->get_widget("FadersSaveButton", saveFaders);
			builder->get_widget("FadersList", fadersList);

			// Scales
			builder->get_widget("ClickVolumeScale", clickVolumeScale);

			// Dialogs
			builder->get_widget("eXaDrumsAboutDialog", aboutDialog);
			builder->get_widget("DeleteKitDialog", deleteKitDialog);

			// Windows
			builder->get_widget("MetronomeConfig", metronomeWindow);

		}

		// Start drum kit
		const std::string moduleLocation(mainFolder+"/../Data/");
		drumKit = std::unique_ptr<eXaDrums>(new eXaDrums(moduleLocation.c_str()));

		// Configure Metronome Window
		std::vector<std::string> clicks = RetrieveClickTypes();

		for(const std::string& clickType : clicks)
		{
			clickTypes->append(clickType);
		}
		clickTypes->set_active(0);

		// Populate Kits list
		CreateKitsList();

		// Add faders
		UpdateFaders();
		saveFaders->set_sensitive(false);


		// Load current kit
		drumKit->SelectKit(GetCurrentKitId());

		// Connect all signals
		{
			// Buttons
			aboutButton->signal_clicked().connect(sigc::mem_fun(this, &Controller::ShowAboutDialog));
			playButton->signal_clicked().connect(sigc::mem_fun(this, &Controller::PlayDrums));
			deleteKitButton->signal_clicked().connect(sigc::mem_fun(this, &Controller::DeleteKitDialog));
			saveFaders->signal_clicked().connect(sigc::mem_fun(this, &Controller::SaveFaders));
			enableClickButton->signal_clicked().connect(sigc::mem_fun(this, &Controller::EnableClick));
			rhythmCoachPrefButton->signal_clicked().connect(sigc::mem_fun(this, &Controller::ShowMetronomePrefs));
			metronomeConfigSave->signal_clicked().connect(sigc::mem_fun(this, &Controller::SaveMetronomeConfig));

			// Scales
			clickVolumeScale->signal_value_changed().connect(sigc::mem_fun(this, &Controller::ChangeTempo));

			// Kits list
			kitsList->signal_changed().connect(sigc::mem_fun(this, &Controller::ChangeKit));

			// Dialog
			aboutDialog->signal_response().connect(std::bind(sigc::mem_fun(this, &Controller::HideAboutDialog), 0));
		}

		return;
	}

	Controller::~Controller()
	{

		// Delete all pointers (dialogs and windows)
		delete deleteKitDialog;
		delete aboutDialog;
		delete metronomeWindow;

		// Stop drum kit
		if(drumKit->IsStarted())
		{
			drumKit->Stop();
		}

		return;
	}

	// PRIVATE METHODS

	void Controller::SetInstrumentVolume(FaderPtr& fader) const
	{

		saveFaders->set_sensitive(true);
		drumKit->SetInstrumentVolume(fader->GetInstrumentId(), fader->GetValue());

		return;
	}

	void Controller::SaveFaders() const
	{

		SaveKitConfig();

		// Disable button
		saveFaders->set_sensitive(false);

		return;
	}

	void Controller::UpdateFaders()
	{

		// Remove existing faders
		std::for_each(faders.begin(), faders.end(), [](FaderPtr& fader) { fader.reset(); });
		faders.clear();

		std::vector<std::string> instNames = RetrieveInstrumentsNames();
		for(std::size_t i = 0; i < instNames.size(); i++)
		{
			std::string name(instNames[i]);
			int volume = GetInstrumentVolume(i);

			FaderPtr fader(new Fader(name, i, volume));

			faders.push_back(fader);
		}

		// Add all faders to GUI
		std::for_each(faders.cbegin(), faders.cend(), [this](FaderPtr const& f){ this->fadersList->add(*f); });

		// Connect faders signals
		for(FaderPtr& fader : faders)
		{
			fader->GetScale().signal_value_changed().connect(std::bind(sigc::mem_fun(this, &Controller::SetInstrumentVolume), std::ref(fader)));
		}

		return;
	}

	std::vector<std::string> Controller::RetrieveKitsNames() const
	{

		int numKits = drumKit->GetNumKits();

		// Get kit names
		std::vector<std::string> kitsNames(numKits);
		{
			for(int i = 0; i < numKits; i++)
			{

				// Create local array to store string given by libeXaDrums
				char kitName[128];
				int nameLength;

				// Get characters and string's length
				drumKit->GetKitNameById(i, kitName, nameLength);

				// Convert to string
				std::string name(kitName, nameLength);

				kitsNames[i] = name;
			}
		}

		return kitsNames;
	}

	void Controller::CreateKitsList()
	{

		// Retrieve kits names
		std::vector<std::string> kitsNames = RetrieveKitsNames();

		// Add kits to the list
		for(std::size_t i = 0; i < kitsNames.size(); i++)
		{
			kitsList->insert(i, kitsNames[i]);
		}
		// Set default kit
		kitsList->set_active(0);

		return;
	}

	void Controller::DeleteKit(const int& id)
	{

		int numKits = drumKit->GetNumKits();

		// Can't delete if only one kit
		if(numKits > 1)
		{
			// Stop module if started
			if(drumKit->IsStarted())
			{
				PlayDrums();
			}

			// Deselect kit
			int activeKit;

			if(id == 0)
			{
				activeKit = 1;
			}
			else
			{
				activeKit = id - 1;
			}


			kitsList->set_active(activeKit);

			// Delete kit
			drumKit->DeleteKit(id);

			// Remove kit from list
			kitsList->remove_text(id);

			// Set new kit
			ChangeKit();

		}


		return;
	}

	std::vector<std::string> Controller::RetrieveClickTypes() const
	{

		int numClickTypes = drumKit->GetNumClickTypes();

		std::vector<std::string> clickTypes(numClickTypes);
		{

			for(int i = 0; i < numClickTypes; i++)
			{

				// Create local array to store string given by libeXaDrums
				char clickName[128];
				int nameLength;

				// Get characters and string's length
				drumKit->GetClickTypeById(i, clickName, nameLength);

				// Convert to string
				std::string name(clickName, nameLength);

				clickTypes[i] = name;
			}

		}

		return clickTypes;
	}

	void Controller::ShowMetronomePrefs()
	{

		metronomeWindow->show();

		return;
	}

	void Controller::SaveMetronomeConfig()
	{

		metronomeWindow->hide();

		return;
	}

	std::vector<std::string> Controller::RetrieveInstrumentsNames() const
	{

		int numInstruments = drumKit->GetNumInstruments();

		// Get instruments names
		std::vector<std::string> instNames(numInstruments);
		{
			for(int i = 0; i < numInstruments; i++)
			{

				// Create local array to store string given by libeXaDrums
				char instName[128];
				int nameLength;

				// Get characters and string's length
				drumKit->GetInstrumentName(i, instName, nameLength);

				// Convert to string
				std::string name(instName, nameLength);

				instNames[i] = name;
			}
		}

		return instNames;
	}


	int Controller::GetCurrentKitId() const
	{

		int currentKitId = kitsList->get_active_row_number();

		return currentKitId;
	}


	void Controller::ShowAboutDialog()
	{
		aboutDialog->show();
		return;
	}

	void Controller::HideAboutDialog(int responseId)
	{
		aboutDialog->hide();
		return;
	}

	void Controller::EnableClick() const
	{

		bool enable =  enableClickButton->get_active();

		drumKit->EnableMetronome(enable);

		return;
	}

	void Controller::ChangeTempo() const
	{

		int tempo = (int) clickVolumeScale->get_value();
		drumKit->ChangeTempo(tempo);

		return;
	}

	void Controller::PlayDrums()
	{

		if(drumKit->IsStarted())
		{
			this->playButton->set_property("label", Gtk::StockID("gtk-media-play"));
			drumKit->Stop();
		}
		else
		{
			this->playButton->set_property("label", Gtk::StockID("gtk-media-stop"));
			drumKit->Start();
		}

		return;
	}

	void Controller::ChangeKit()
	{

		bool started = drumKit->IsStarted();

		// Stop module if started
		if(started)
		{
			drumKit->Stop();
		}


		// Load new kit
		drumKit->SelectKit(GetCurrentKitId());

		// Update faders
		this->UpdateFaders();
		saveFaders->set_sensitive(false);

		// Restart module if needed
		if(started)
		{
			drumKit->Start();
		}

		return;
	}

	void Controller::DeleteKitDialog()
	{

		// Get answer
		int answer = deleteKitDialog->run();

		// Check answer
		switch (answer)
		{
			case Gtk::RESPONSE_CANCEL: break;
			case Gtk::RESPONSE_OK: this->DeleteKit(GetCurrentKitId()); break;
			default: break;
		}

		// Close dialog
		deleteKitDialog->hide();

		return;
	}

} /* namespace Gui */
