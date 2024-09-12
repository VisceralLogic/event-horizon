#include "mission.h"
#include "controller.h"
#include "plugin.h"
#include "solarsystem.h"
#include "government.h"

string dateToString(chrono::system_clock::time_point date);

void Mission::registerFromDictionary(const json& dictionary) {
	shared_ptr<Mission> mission = make_shared<Mission>();
	json empty;
	mission->ID = (string)dictionary["PluginName"] + ".missions." + (string)dictionary["ID"];
	mission->availType = dictionary.contains("AvailType") ? (string)dictionary["AvailType"] : "";
	mission->avail = dictionary.contains("Avail") ? (string)dictionary["Avail"] : "";
	mission->availFlags = dictionary.contains("AvailFlags") ? (string)dictionary["AvailFlags"] : "";
	mission->destType = dictionary.contains("DestType") ? (string)dictionary["DestType"] : "";
	mission->dest = dictionary.contains("Dest") ? (string)dictionary["Dest"] : "";
	mission->startFlags = dictionary.contains("StartFlags") ? (string)dictionary["StartFlags"] : "";
	mission->setFlags = dictionary.contains("SetFlags") ? (string)dictionary["SetFlags"] : "";
	mission->size = dictionary.contains("Size") ? (int)dictionary["Size"] : 1;
	mission->payment = dictionary.contains("Payment") ? (int)dictionary["Payment"] : 1000;
	mission->title = dictionary.contains("Title") ? (string)dictionary["Title"] : mission->ID;
	mission->description = dictionary.contains("Description") ? (string)dictionary["Description"] : mission->ID;
	mission->freq = dictionary.contains("Freq") ? (float)dictionary["Freq"] : 1.0f;
	mission->endText = dictionary.contains("EndText") ? (string)dictionary["EndText"] : "";
	mission->failText = dictionary.contains("FailText") ? (string)dictionary["FailText"] : "";
	mission->endTime = dictionary.contains("EndTime") ? (int)dictionary["EndTime"] * 86400 : 0;
	if (dictionary.contains("Replace"))
		mission->ID = dictionary["Replace"];
	vector<string> temp = split(mission->ID, '.');
	if (!Plugin::plugins.contains(temp[0]))
		Plugin::plugins[temp[0]] = make_shared<Plugin>();
	Plugin::plugins[temp[0]]->missions[temp[2]] = mission;
}

void Mission::finalize() {

}

shared_ptr<Mission> Mission::newInstanceIn(shared_ptr<Solarsystem> system, shared_ptr<Planet> planet) {
	shared_ptr<Mission> mission = make_shared<Mission>();
	mission->ID = ID;
	mission->startSystem = system;
	mission->startPlanet = planet;
	mission->size = size;
	mission->payment = payment;
	mission->description = description;
	if (destType == "government") {
		vector<shared_ptr<EHObject>> systems = Controller::objectsOfType("system");
		for (int i = 0; i < systems.size(); i++) {
			shared_ptr<Solarsystem> sys = static_pointer_cast<Solarsystem>(systems[i]);
			if (sys->gov.get() == nullptr || (dest != "any" && sys->gov->ID != dest)) {
				systems.erase(systems.begin() + i);
				i--;
			}
		}
		mission->endSystem = static_pointer_cast<Solarsystem>(systems[rand()*systems.size()]);
		mission->endPlanet = mission->endSystem->planets[rand()*mission->endSystem->planets.size()];
	} else if (destType == "system") {
		mission->endSystem = static_pointer_cast<Solarsystem>(Controller::componentNamed(dest));
		mission->endPlanet = mission->endSystem->planets[rand() * mission->endSystem->planets.size()];
	} else if( destType == "planet" ) {
		mission->endPlanet = static_pointer_cast<Planet>(Controller::componentNamed(dest));
		mission->endSystem = mission->endPlanet->system;
	}
	if( endTime != 0 ){
		mission->expires = sys->date + chrono::hours(24*endTime);
	}
	mission->title = replace(title, "<PLANET>", mission->endPlanet->name);
	mission->title = replace(mission->title, "<SYSTEM>", mission->endSystem->name);
	mission->title = replace(mission->title, "<DATE>", dateToString(mission->expires));
	mission->description = replace(description, "<PLANET>", mission->endPlanet->name);
	mission->description = replace(mission->description, "<SYSTEM>", mission->endSystem->name);
	mission->description = replace(mission->description, "<DATE>", dateToString(mission->expires));
	mission->endText = replace(endText, "<PLANET>", mission->endPlanet->name);
	mission->endText = replace(mission->endText, "<SYSTEM>", mission->endSystem->name);
	mission->endText = replace(mission->endText, "<DATE>", dateToString(mission->expires));
	mission->failText = replace(failText, "<PLANET>", mission->endPlanet->name);
	mission->failText = replace(mission->failText, "<SYSTEM>", mission->endSystem->name);
	mission->failText = replace(mission->failText, "<DATE>", dateToString(mission->expires));
	mission->startFlags = startFlags;
	mission->setFlags = setFlags;
	mission->endTime = endTime;

	return mission;
}

string dateToString(chrono::system_clock::time_point date) {
	time_t time = chrono::system_clock::to_time_t(date);
	char dateChars[11];
	strftime(dateChars, 10, "%F", localtime(&time));
	dateChars[10] = 0;
	return string(dateChars);
}