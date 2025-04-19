/*
* This file and associated .cpp file are licensed under the GPLv3 License Copyright (c) 2025 Sam Groveman
* 
* ArduinoJSON: https://arduinojson.org/
*
* Contributors: Sam Groveman
*/
#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>
#include <array>
#include <Actor.h>
#include <PeriodicTask.h>
#include <ActionTrigger.h>

/// @brief Class providing action triggers at regular intervals
class PeriodicActionTrigger : public Actor, public PeriodicTask {
	protected:
		/// @brief Output configuration
		struct {
			/// @brief Name of the actor/action to use
			String action = "";

			/// @brief The payload to deliver to the action
			String payload = "";

			/// @brief Enables the task
			bool taskEnabled = false;
		} trigger_config;

		/// @brief Path to configuration file
		String config_path;

		/// @brief Trigger to access actions
		ActionTrigger trigger;

		/// @brief Stores the action that's to be triggered
		std::array<String, 2> action;

		bool configureOutput();
		void runTask(ulong elapsed);
		virtual bool triggerAction(String payload);

	public:
		PeriodicActionTrigger(String Name, String configFile = "PeriodicActionTrigger.json");
		bool begin();
		std::tuple<bool, String> receiveAction(int action, String payload = "");
		String getConfig();
		bool setConfig(String config, bool save);
};