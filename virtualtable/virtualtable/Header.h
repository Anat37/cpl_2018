#pragma once
#include<iostream>
#include<map>
#include<vector>
#include<functional>


#define VIRTUAL_CLASS(class_name)	struct class_name{ \
										static std::map<std::string, std::function<void(void*)> > staticVirtualTable; \
										std::map<std::string, std::function<void(void*)> > virtualTable; \
										class_name() { \
											virtualTable = staticVirtualTable;\
										}

#define END(class_name) };  \
					std::map<std::string, std::function<void(void*)> > class_name :: staticVirtualTable; \
					class_name baseClassObj;

#define DECLARE_METHOD(class_name, method_name, ...) class_name :: staticVirtualTable.emplace(#method_name, [](void* object) {\
																		class_name* obj = reinterpret_cast<class_name*>(object);\
																		std::cout << #class_name << ' ' << #method_name << std::endl;\
																		__VA_ARGS__\
																	});

#define VIRTUAL_CLASS_DERIVED(derived_name, base_name) struct derived_name : public base_name { \
															static std::map<std::string, std::function<void(void*)> > staticVirtualTable; \
															derived_name() { \
																virtualTable = staticVirtualTable;\
															}

#define VIRTUAL_CALL(object, method_name) {auto it = object->virtualTable.find(#method_name); \
											if (it != object->virtualTable.end()) { \
												(*it).second(object); \
											} else { \
												auto it = baseClassObj.staticVirtualTable.find(#method_name); \
												(*it).second(object); \
											}}

#define END_DERIVE(class_name, base_name) };\
							std::map<std::string, std::function<void(void*)> > class_name :: staticVirtualTable;