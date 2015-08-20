var keyhook = require("./build/Release/keyhook");

console.log("Keyboard hook is started...");
keyhook.hookKeyboard(function(success) {
	console.log("Keyboard hooked: " + success);
});

// Remove the keyhook after 5 seconds
setTimeout(function() {
	console.log(keyhook.unhookKeyboard());
}, 5000);
