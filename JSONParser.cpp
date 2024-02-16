#include "JSONParser.h"

JSONParser::JSONParser(std::string filePath)
{
	this->myJSON = nullptr;
	std::ifstream file(filePath);
	if (!file.is_open()) {
		std::cout << "ERROR: File is not open";
		return;
	}
	parser(lexer(file));
	file.close();
}

JSONParser::~JSONParser()
{
	delete myJSON;
}

bool JSONParser::checkDeliminator(char character)
{
	return (character == '}') || (character == '{') || (character == ']') || (character == '[') || (character == ',') || (character == ':');
}

std::vector<std::string> JSONParser::lexer(std::ifstream& file)
{
	std::vector<std::string> tokens;
	std::string token;
	char c;
	int flag = 2;
	while (file.get(c)) {
		if (c == '"')
			flag--;
		if (checkDeliminator(c) && (flag == 2 || flag == 0)) {
			if (!token.empty()) {
				tokens.push_back(token);
				token.clear();
			}
			tokens.push_back(std::string(1, c));
			flag = 2;
		}
		else if (!std::isspace(c) || (std::isspace(c) && flag == 1)) {
			token += c;
		}
	}
	return tokens;
}

bool JSONParser::verifyJSON(std::vector<std::string> input)
{
	std::string exIt;
	for (auto it : input) {
		if (it.size() == 1 && checkDeliminator(it[0])) {
			if (it[0] == '{' || it[0] == '[') {
				if (checkStack.size() < 1 && it[0] == '[')
					return false;
				if (checkStack.size() > 0) {
					if (checkStack.top()[0] == ',') {
						if (it[0] == '[' && exIt[0] != ']') {
							return false;
						}
						else if (it[0] == '{' && !(exIt[0] == '"' || std::isalpha(exIt[0]) || std::isdigit(exIt[0]) || exIt[0] == '}')) {
							return false;
						}
						else {
							checkStack.pop();
						}
					}
				}
				else if (it[0] == '[') {
					if (checkStack.top()[0] != ':')
						return false;
					else
						checkStack.pop();
				}
				checkStack.push(it);
			}
			else if (it[0] == ']' || it[0] == '}') {
				if (checkStack.top()[0] == ':') {
					checkStack.pop();
				}
				if (checkStack.empty()) {
					return false;
				}
				if (checkStack.top()[0] == ',') {
					return false;
				}
				if ((checkStack.top()[0] == '[' && it[0] == '}') || (checkStack.top()[0] == '{' && it[0] == ']'))
					return false;
				checkStack.pop();
				if (!checkStack.empty() && checkStack.top()[0] == ':') {
					checkStack.pop();
				}
			}
			else if (it[0] == ',') {
				if (exIt[0] == '{' || exIt[0] == '[' || exIt[0] == ':')
					return false;
				if (checkStack.size() < 1)
					return false;
				if (checkStack.top()[0] == ',' || checkStack.top()[0] == ':') {
					return false;
				}
				checkStack.push(it);
			}
			else { // it[0] == ':'
				if (exIt[0] != '"' or checkStack.size() < 1)
					return false;
				checkStack.push(it);
			}
		}
		else if (it[0] == '"' || std::isalpha(it[0]) || std::isdigit(it[0])) {
			if (checkStack.top()[0] == ',') {
				checkStack.pop();
				if (checkStack.top()[0] != '[')
					checkStack.push(",");
			}
			if (exIt[0] == '{' && it[0] != '"')
				return false;
			if (exIt[0] == '"' && !(checkStack.top()[0] == ':' || checkStack.top()[0] == ','))
				return false;
			if (checkStack.size() < 1)
				return false;
			if (it[0] == '"' && checkStack.top()[0] != ':') { // type name
				if (it[it.size() - 1] != '"')
					return false;
				if (checkStack.top()[0] == ',')
					checkStack.pop();
			}
			else if (it[0] == '"' && checkStack.top()[0] == ':') { //  type value string
				if (it[it.size() - 1] != '"')
					return false;
				checkStack.pop();
			}
			else if (checkStack.top()[0] == ':') {
				checkStack.pop();
			}
			else {
				// numbers or boolean values
				if (std::islower(it[0]) && it[0] != 't' && it[0] != 'f')
					return false;
				if (!std::islower(it[0])) {
					try {
						int numberValue = std::stoi(it);
					}
					catch (...) {
						return 0;
					}
				}

			}
		}
		else {
			return false;
		}
		if (it[0] != ',' && it[0] != ':') {
			exIt = it;
		}
	}
	return true;
}

void JSONParser::parser(std::vector<std::string> input)
{
	if (!verifyJSON(input)) {
		std::cout << "ERROR : Invalid JSON";
		return;
	}
	myJSON = createJObject(splitJSON(input));
	if (myJSON)
		std::cout << "The file was successfully parsed" << '\n';
}

std::vector<std::vector<std::string>> JSONParser::splitJSON(std::vector<std::string> input)
{
	std::vector<std::vector<std::string>> result;
	int index = 1;
	while (index < input.size() - 1) {
		std::vector<std::string> component;
		component.push_back(input[index]); // add tag
		index++;
		component.push_back(input[index]); // add ":"
		index++;
		//value
			// object or list object
		if (input[index][0] == '{' || input[index][0] == '[') {
			component.push_back(input[index]);
			checkStack.push(input[index]);
			index++;
			while (!checkStack.empty()) {
				if (input[index][0] == '{' || input[index][0] == '[')
					checkStack.push(input[index]);
				else if (input[index][0] == '}' || input[index][0] == ']')
					checkStack.pop();
				component.push_back(input[index]);
				index++;
			}
		}
		// string, number or boolean value
		else {
			component.push_back(input[index]);
			index++;
		}
		if (index < input.size())
			if (input[index][0] == ',')
				index++;
		result.push_back(component);
	}
	return result;
}

bool JSONParser::stackIsFree()
{
	return checkStack.empty();
}

const jobject* JSONParser::getMyHSON()
{
	return myJSON;
}

jobject* JSONParser::createJObject(std::vector<std::vector<std::string>> components)
{
	jobject* result = new jobject();
	result->type = object;
	for (auto it : components) {
		std::string tag = it[0];
		tag.erase(0, 1);
		tag.pop_back();
		json_value* value = new json_value();
		if (it[2][0] == '{') {
			std::vector<std::string> newObject = it;
			newObject.erase(newObject.begin());
			newObject.erase(newObject.begin());
			jobject* newObjectValue = new jobject();
			newObjectValue = createJObject(splitJSON(newObject));
			newObjectValue->type = object;
			*value = newObjectValue;
		//	delete newObjectValue;
			//result->map_value.insert(std::make_pair(tag, *value));
		}
		else if (it[2][0] == '[') {
			jlist* newList = new jlist();
			std::vector<std::string> newListString = it;
			newListString.erase(newListString.begin());
			newListString.erase(newListString.begin());
			newList = createJList(newListString);
			newList->type = list;
			*value = newList;
			//delete newList;
			//result->map_value.insert(std::make_pair(tag, *value));
		}
		else {
			if (it[2][0] == '"') {
				std::string stringValue = it[2];
				stringValue.erase(0, 1);
				stringValue.pop_back();
				std::string* test = new std::string(stringValue);
				*value = stringValue;
				//delete test;
				//result->map_value.insert(std::make_pair(tag, *value));
			}
			else if (it[2][0] == 't' or it[2][0] == 'f') {
				bool flag = (it[2][0] == 't') ? true : false;
				*value = flag;
				//result->map_value.insert(std::make_pair(tag, *value));
			}
			else {
				std::string number = it[2];
				if (number.find(".") != std::string::npos) {
					float numberValue = (float)std::stod(number);
					*value = numberValue;
				}
				else {
					int numberValue;
					try {
						numberValue = (int)std::stod(number);
					}
					catch (...) {
						std::cout << "ERROR : The value in the " << it[0] << " field is wrong !!!" << '\n';
						numberValue = -1;
					}
					//int numberValue = (int)std::stod(number);
					*value = numberValue;
				}
				//result->map_value.insert(std::make_pair(tag, *value));
			}
		}
		result->map_value.insert(std::make_pair(tag, value));
	}
	return result;
}

jlist* JSONParser::createJList(std::vector<std::string> input)
{
	jlist* result = new jlist();
	result->type = list;
	result->type = list;
	int index = 1;
	while (index < input.size() - 1) {
		json_value* value = new json_value();
		if (input[index][0] == '{') {
			std::vector<std::string> newObject;
			newObject.push_back(input[index]);
			checkStack.push(input[index]);
			index++;
			while (!checkStack.empty()) {
				if (input[index][0] == '{' || input[index][0] == '[')
					checkStack.push(input[index]);
				else if (input[index][0] == '}' || input[index][0] == ']')
					checkStack.pop();
				newObject.push_back(input[index]);
				index++;
			}
			*value = createJObject(splitJSON(newObject));
		}
		else if (input[index][0] == '[') {
			std::vector<std::string> newList;
			newList.push_back(input[index]);
			checkStack.push(input[index]);
			index++;
			while (!checkStack.empty()) {
				if (input[index][0] == '{' || input[index][0] == '[')
					checkStack.push(input[index]);
				else if (input[index][0] == '}' || input[index][0] == ']')
					checkStack.pop();
				newList.push_back(input[index]);
				index++;
			}
			*value = createJList(newList);
		}
		else if (input[index][0] == '"') {
			std::string valueString = input[index];
			valueString.erase(0, 1);
			valueString.pop_back();
			*value = valueString;
		}
		else if (input[index][0] == 't' or input[index][0] == 'f') {
			bool flag = (input[index][0] == 't') ? true : false;
			*value = flag;
		}
		else {
			std::string number = input[index];
			if (number.find(".") != std::string::npos) {
				float numberValue = (float)std::stod(number);
				*value = numberValue;
			}
			else {
				int numberValue;
				try {
					numberValue = (int)std::stod(number);
				}
				catch (...) {
					std::cout << "ERROR : The value in the " << "LIST OBJECT" << " field is wrong !!!" << '\n';
					numberValue = -1;
				}
				*value = numberValue;
			}
		}
		result->vector_value.push_back(value);
		index++;
		if (index < input.size())
			if (input[index][0] == ',')
				index++;
	}
	return result;
}

void JSONParser::toString()
{
	if (!myJSON) {
		std::cout << "ERROR : NO OBJECT JSON EXISTS" << '\n';
		std::cout << "{None}";
		return;
	}
	printObject(myJSON, 0);
}

void JSONParser::writeSpace(int number)
{
	number = number * 4;
	for (int i = 0; i < number; i++)
		std::cout << " ";
}

void JSONParser::printObject(jobject* value, int number)
{
	writeSpace(number);
	std::cout <<'\n' << "{" << '\n';
	for (auto& it : value->map_value) {
		writeSpace(number);
		std::cout << it.first << " = ";
		if (std::holds_alternative<jobject*>(*it.second)) {
			printObject(std::get<jobject*>(*it.second), number + 1);
		}
		else if (std::holds_alternative<jlist*>(*it.second)) {
			printList(std::get<jlist*>(*it.second), number);
		}
		else {
			std::cout << getValue(it.second);
		}
		std::cout << '\n';
	}
	writeSpace(number);
	std::cout << "}" << '\n';
}

void JSONParser::printList(jlist* list, int number)
{
	std::cout << "[ " << '\n';
	writeSpace(number);
	for (auto element : list->vector_value) {
		writeSpace(number);
		if (std::holds_alternative<jobject*>(*element)) {
			printObject(std::get<jobject*>(*element), number);
		}
		else if (std::holds_alternative<jlist*>(*element)) {
			printList(std::get<jlist*>(*element), number);
		}
		else {
			std::cout << getValue(element);
		}
		std::cout << '\n';
	}
	writeSpace(number);
	std::cout << "]" << '\n';
}

std::string JSONParser::getValue(json_value* value)
{

	if (std::holds_alternative<std::string>(*value)) {
		std::string result = std::get<std::string>(*value);
		return result;
	}
	if (std::holds_alternative<bool>(*value)) {
		std::string result = (std::get<bool>(*value)) ? "true" : "false";
		return result;
	}
	if (std::holds_alternative<int>(*value)) {
		std::string result = std::to_string(std::get<int>(*value));
		return result;
	}
	if (std::holds_alternative<float>(*value)) {
		std::string result = std::to_string(std::get<float>(*value));
		return result;
	}
	return "None";
}