#include"PeriodicActionTrigger.h"

extern bool POSTSuccess;

/// @brief Creates a periodic action trigger
/// @param Name The device name
/// @param configFile Name of the config file to use
PeriodicActionTrigger::PeriodicActionTrigger(String Name, String configFile) : Actor(Name) {
	config_path = "/settings/act/" + configFile;
}

/// @brief Starts a periodic trigger 
/// @return True on success
bool PeriodicActionTrigger::begin() {
	// Set description
	Description.actionQuantity = 1;
	Description.type = "trigger";
	Description.actions = {{"triggeraction", 0}};
	trigger.actions_config.Enabled = true;
	// Create settings directory if necessary
	if (!checkConfig(config_path)) {
		// Set defaults
		task_config.set_taskName(Description.name.c_str());
		task_config.taskPeriod = 1000;
		return setConfig(getConfig(), true);
	} else {
		// Load settings
		return setConfig(Storage::readFile(config_path), false);
	}
}

/// @brief Receives an action
/// @param action The action to process (only option is 0 for set output)
/// @param payload Any payload to be passed to the action
/// @return JSON response with OK
std::tuple<bool, String> PeriodicActionTrigger::receiveAction(int action, String payload) {
	if (action == 0) {
		if (triggerAction(payload)) {
			return { true, R"({"success": true})" };
		} else {
			return { true, R"({"success": false})" };
		}
	}	
	return { true, R"({"success": false})" };
}

/// @brief Gets the current config
/// @return A JSON string of the config
String PeriodicActionTrigger::getConfig() {
	// Allocate the JSON document
	JsonDocument doc;
	// Assign current values
	doc["Name"] = Description.name;
	doc["taskPeriod"] = task_config.taskPeriod;
	doc["taskEnabled"] = trigger_config.taskEnabled;

	// Add all actors/actions to dropdown
	doc["Action"]["current"] = trigger_config.action;
	std::map<String, std::map<int, String>> actions = trigger.listAllActions();
	if (actions.size() > 0) {
	int i = 0;
		for (std::map<String, std::map<int, String>>::iterator actor = actions.begin(); actor != actions.end(); actor++) {
			if (actor->first != Description.name) {
				for (const auto& action : actor->second) {
					doc["Action"]["options"][i] = actor->first + ":" + action.second;
					i++;
				}
			}
		}
	} else {
		doc["Action"]["options"][0] = "";
	}

	doc["Payload"] = trigger_config.payload;

	// Create string to hold output
	String output;
	// Serialize to string
	serializeJson(doc, output);
	return output;
}

/// @brief Sets the configuration for this device
/// @param config A JSON string of the configuration settings
/// @param save If the configuration should be saved to a file
/// @return True on success
bool PeriodicActionTrigger::setConfig(String config, bool save) {
	// Allocate the JSON document
  	JsonDocument doc;
	// Deserialize file contents
	DeserializationError error = deserializeJson(doc, config);
	// Test if parsing succeeds.
	if (error) {
		Logger.print(F("Deserialization failed: "));
		Logger.println(error.f_str());
		return false;
	}
	// Assign loaded values
	Description.name = doc["Name"].as<String>();
	task_config.set_taskName(Description.name.c_str());
	task_config.taskPeriod = doc["taskPeriod"].as<ulong>();
	trigger_config.payload = doc["Payload"].as<String>();
	trigger_config.taskEnabled = doc["taskEnabled"].as<bool>();

	// Parse actor and action
	trigger_config.action = doc["Action"]["current"].as<String>();
	int colon;
	if ((colon = trigger_config.action.indexOf(':')) != -1) {
		std::pair<String, String> chosen {trigger_config.action.substring(0, colon), trigger_config.action.substring(colon + 1)};
		action[0] = chosen.first;
		action[1] = chosen.second;
	}
	if (save) {
		if (!saveConfig(config_path, config)) {
			return false;
		}
	}
	return enableTask(trigger_config.taskEnabled);
}

/// @brief Runs the task to trigger the action
void PeriodicActionTrigger::runTask(ulong elapsed) {
	if (taskPeriodTriggered(elapsed)) {
		triggerAction(trigger_config.payload);
	}
}

/// @brief Triggers the set action
/// @param payload A payload to pass to the action
/// @return True on success
bool PeriodicActionTrigger::triggerAction(String payload) {
	if (POSTSuccess) {
		return trigger.triggerActions({{action[0], {{action[1], payload}}}});
	}
	return false;
}
