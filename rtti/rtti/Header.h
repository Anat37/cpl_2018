#pragma once
#include<string>
#include<iostream>
#include<vector>
#include<map>

std::map<std::string, int> idmap;
std::map<int, std::string> namemap;
std::map<int, int> sizesmap;
std::map<int, std::vector<int> > basesmap;
std::map<int, std::vector<int> > derivativesmap;

bool isBaseOf(int base, int derived) {
	if (base == derived) {
		return true;
	}
	std::vector<int> totry({ derived });
	while (totry.size()) {
		auto tmp = basesmap.find(totry.back());
		totry.pop_back();
		if (tmp != basesmap.end()) {
			for (int cl : tmp->second) {
				if (cl == base) {
					return true;
				}
				totry.push_back(cl);
			}
		}
	}
	return false;
}

void* shiftToStart(int base, int derivative, void* ptr) {
	auto list = basesmap[derivative];
	int shift = 0;
	for (int i = 0; i < list.size(); ++i) {
		if (list[i] == base) {
			break;
		} else {
			shift += sizesmap[list[i]];
		}
	}
	return (void*)((char*)ptr - shift);
}

bool checkClass(void* ptr, int id) {
	int shift = 0;
	for (int cl : basesmap[id]) {
		shift += sizesmap[cl];
	}
	return id == *(int*)((char*)ptr + shift);
}

int detectClass(void* ptr) {
	int id = -1;
	int msize = 0;
	for (auto cl : idmap) {
		if (checkClass(ptr, cl.second) && sizesmap[cl.second] > msize) {
			msize = sizesmap[cl.second];
			id = cl.second;
		}
	}
	return id;
}

int findTop(void* &ptr) {
	int id = detectClass(ptr);
	bool flag = true;
	while (flag) {
		flag = false;
		auto list = derivativesmap[id];
		for (int cl : list) {
			void* tmp = shiftToStart(id, cl, ptr);
			if (checkClass(tmp, cl)) {
				id = cl;
				ptr = tmp;
				flag = true;
				break;
			}
		}
	}
	return id;
}


void* shiftToBase(int base, int derivative, void* ptr) {
	auto list = basesmap[derivative];
	int shift = 0;
	for (int i = 0; i < list.size(); ++i) {
		if (list[i] == base) {
			break;
		} else {
			shift += sizesmap[list[i]];
		}
	}
	return (void*)((char*)ptr + shift);
}

void* upCast(int base, int derived, void* ptr) {
	std::vector<std::pair<int, std::vector<int> > > way({ std::make_pair(derived, std::vector<int>{}) });
	if (base == derived) {
		return ptr;
	}
	while (way.size()) {
		auto tmpCl = way.back();
		auto tmp = basesmap.find(tmpCl.first);
		way.pop_back();
		tmpCl.second.push_back(tmpCl.first);
		if (tmp != basesmap.end()) {
			for (int cl : tmp->second) {
				if (cl == base) {
					auto list = tmpCl.second;
					list.push_back(cl);
					for (int i = 1; i < list.size(); ++i) {
						ptr = shiftToBase(list[i], derived, ptr);
						derived = list[i];
						return ptr;
					}
				}
				auto ntmp = tmpCl;
				ntmp.first = cl;
				way.push_back(ntmp);
			}
		}
	}
	return nullptr;
}

void* dynamicCast(void* pointer, std::string target) {
	int id = findTop(pointer);
	if (target == "void") {
		return pointer;
	}
	int targetId = idmap[target];
	if (targetId == id) {
		return pointer;
	}
	if (isBaseOf(targetId, id)) {
		return upCast(targetId, id, pointer);
	}
}

struct MyTypeId {
	int id;
	std::string name;
	MyTypeId(int mid, std::string mname) {
		id = mid;
		name = mname;
	}

	bool operator ==(const MyTypeId a) {
		return a.id == id;
	}
};

MyTypeId getMyTypeId(void* ptr) {
	int id = findTop(ptr);
	return MyTypeId(id, namemap[id]);
}


#define RTTI_INIT(class_name) if (!isInited) { \
								idmap.emplace(#class_name, mytypeid); \
								sizesmap.emplace(mytypeid, sizeof(class_name)); \
								namemap.emplace(mytypeid, #class_name);\
							}

#define RTTI_INIT_ADD_BASE(class_name, base) if (!isInited) { \
												basesmap[idmap[#class_name]].push_back(idmap[#base]);\
												derivativesmap[idmap[#base]].push_back(idmap[#class_name]);\
											}

#define RTTI_END_INIT isInited = true;

#define ENABLE_RTTI public:\
					const int mytypeid = __COUNTER__;\
					static bool isInited;\
					private:
					
#define END_ENABLE_RTTI(class_name) bool class_name::isInited = false;

#define DYNAMIC_CAST(pointer, target) (target*) dynamicCast(pointer, #target)

#define MYTYPEID(pointer) getMyTypeId(pointer)





