#ifndef KEYENTRY_H
#define KEYENTRY_H
#include <string>

enum class KeyModifiers {
	Shift = 1 << 0, 
	Ctrl  = 1 << 1, 
	Menu  = 1 << 2
};

class KeyEntry {
public:
	KeyEntry(const std::string& fromString);
};

bool operator == (const KeyEntry& lhs, const KeyEntry& rhs);

#endif