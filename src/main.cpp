#include "keyhook.hpp"
#include <node.h>
#include <string>
#include <condition_variable>
#include <memory>

#define V8_EXCEPTION(str) v8::ThrowException(v8::Exception::Error(v8::String::New(str)));
#define V8_FROM_STRING(obj) *v8::String::Utf8Value(obj->ToString());

struct hookKeyboardData {
	bool success = false;
	bool done = false;
	std::mutex lock;
	std::condition_variable condition;
	v8::Persistent<v8::Function> callback;
};

void hookKeyboardInitializer(bool s, hookKeyboardData *data) {
	std::unique_lock<std::mutex> locker(data->lock);
	data->success = s;
	data->done = true;
	data->condition.notify_all();
}

void hookKeyboardWorker(uv_work_t *req) {
	auto data = (hookKeyboardData *)req->data;

	setKeyboardHookedCallback(std::bind(hookKeyboardInitializer, std::placeholders::_1, data));
	if (!initializeKeyhook()) {
		data->success = false;
		return;
	}

	std::unique_lock<std::mutex> locker(data->lock);
	while (!data->done) {
		data->condition.wait(locker);
	}

	unsetKeyboardHookedCallback();
}

void hookKeyboardAfter(uv_work_t *req, int) {
	v8::HandleScope scope;

	hookKeyboardData *data = (hookKeyboardData *)req->data;
	
	v8::Handle<v8::Value> arguments[] = {
		v8::Boolean::New(data->success)
	};

	v8::TryCatch tryCatch;
	data->callback->Call(v8::Context::GetCurrent()->Global(), 1, arguments);
	if (tryCatch.HasCaught()) {
		// Ignore exceptions in the callback
	}

	data->callback.Dispose();

	delete data;
	delete req;
}

namespace NodeFunctions {
	v8::Handle<v8::Value> hookKeyboard(const v8::Arguments& args) {
		v8::HandleScope scope;

		if (args.Length() < 1)
			V8_EXCEPTION("First argument must be a callback!");

		if (!args[0]->IsFunction())
			V8_EXCEPTION("First argument must be a callback!");


		auto callback = v8::Local<v8::Function>::Cast(args[0]);

		hookKeyboardData *data = new hookKeyboardData;
		uv_work_t *worker = new uv_work_t;

		data->callback = v8::Persistent<v8::Function>::New(callback);
		worker->data = data;
		
		uv_queue_work(uv_default_loop(), worker, hookKeyboardWorker, hookKeyboardAfter);
		
		return scope.Close(v8::Undefined());
	}

	v8::Handle<v8::Value> unhookKeyboard(const v8::Arguments& args) {
		v8::HandleScope scope;

		return scope.Close(v8::Boolean::New(destroyKeyhook()));
	}

	v8::Handle<v8::Value> isRegistered(const v8::Arguments& args) {
		v8::HandleScope scope;

		if (args.Length() != 1)
			V8_EXCEPTION("Invalid parameter count!");

		if (!args[0]->IsString())
			V8_EXCEPTION("The first argument must be a string!");

		
		std::string key = V8_FROM_STRING(args[0]);

		return scope.Close(v8::Boolean::New(false));
	}
}
void RegisterModule(v8::Handle<v8::Object> target) {
	NODE_SET_METHOD(target, "hookKeyboard", NodeFunctions::hookKeyboard);
	NODE_SET_METHOD(target, "unhookKeyboard", NodeFunctions::unhookKeyboard);
	NODE_SET_METHOD(target, "isRegistered", NodeFunctions::isRegistered);
}

NODE_MODULE(keyhook, RegisterModule);