#include <memory>
#include <vector>

#include "plugin.h"
#include "government.h"
#include "controller.h"

void Government::registerFromDictionary(const json& dictionary) {
	shared_ptr<Government> gov = make_shared<Government>();
	json empty;
	gov->name = dictionary.contains("Name") ? dictionary["Name"] : dictionary["ID"];
	gov->ID = (string)(dictionary["PluginName"]) + ".governments." + (string)(dictionary["ID"]);
	if (dictionary.contains("Color") && dictionary["Color"].is_array()) {
		gov->red = dictionary["Color"][0];
		gov->green = dictionary["Color"][1];
		gov->blue = dictionary["Color"][2];
	} else {
		gov->red = 1;
		gov->green = 1;
		gov->blue = 1;
	}
	gov->shootPenalty = dictionary.contains("ShootPenalty") ? (int)dictionary["ShootPenalty"] : 20;
	gov->disablePenalty = dictionary.contains("DisablePenalty") ? (int)dictionary["DisablePenalty"] : 50;
	gov->boardPenalty = dictionary.contains("BoardPenalty") ? (int)dictionary["BoardPenalty"] : 50;
	gov->destroyPenalty = dictionary.contains("DestroyPenalty") ? (int)dictionary["DestroyPenalty"] : 75;
	gov->initData["InitialRecord"] = dictionary.contains("InitialRecord") ? (int)dictionary["InitialRecord"] : 0;
	gov->initData["Enemies"] = dictionary.contains("Enemies") ? dictionary["Enemies"] : empty;
	gov->initData["Allies"] = dictionary.contains("Allies") ? dictionary["Allies"] : empty;
	if (dictionary.contains("Replace"))
		gov->ID = dictionary["Replace"];
	vector<string> temp = split(gov->ID, '.');
	if( !Plugin::plugins.contains(temp[0]) )
		Plugin::plugins[temp[0]] = make_shared<Plugin>();
	Plugin::plugins[temp[0]]->governments[temp[2]] = gov;
}

void Government::finalize() {
// assign enemies and allies
}