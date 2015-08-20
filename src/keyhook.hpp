#ifndef KEYHOOK_HPP
#define KEYHOOK_HPP
#include <string>
#include <functional>

typedef std::function<bool()> KeyPressedCallback;


bool isKeyRegistered(const std::string& key);

bool registerKey(const std::string& key, KeyPressedCallback callback);

bool unregisterKey(const std::string& key);

bool initializeKeyhook();

bool destroyKeyhook();

void setKeyboardHookedCallback(std::function<void(bool)> callback);
void unsetKeyboardHookedCallback();

#endif