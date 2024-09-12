#include "type.h"
#include "controller.h"
#include "plugin.h"

void Type::registerFromDictionary(const json& dictionary) {
	shared_ptr<Type> type = make_shared<Type>();
	type->initData = dictionary["Data"];
	type->ID = (string)dictionary["PluginName"] + ".types." + (string)dictionary["ID"];
	if (dictionary.contains("Replace"))
		type->ID = dictionary["Replace"];
	vector<string> temp = split(type->ID, '.');
	if (!Plugin::plugins.contains(temp[0]))
		Plugin::plugins[temp[0]] = make_shared<Plugin>();
	Plugin::plugins[temp[0]]->types[temp[2]] = type;
	if( dictionary.contains("ADDTO") )
		type->ID = "ADDTO" + dictionary["ADDTO"];
}

void Type::finalize() {
	if( this->ID.find("ADDTO") == 0 ) {
		shared_ptr<Type> type = static_pointer_cast<Type>(Controller::componentNamed(this->ID.substr(5)));
		float f = 0;
		for( auto& data : initData ) {
			f += data["Probability"];
		}
		if (f > 1) {
			f = 1;
		}
		for (auto& data : type->initData) {
			data["Probability"] = data["Probability"] * (1.0f - f);
		}
		for (auto& data : initData) {
			type->initData.push_back(data);
		}
	}
}