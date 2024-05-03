#include "../keycode.h"

#include <CoreFoundation/CoreFoundation.h>
#include <Carbon/Carbon.h> /* For kVK_ constants, and TIS functions. */

/* Returns string representation of key, if it is printable.
 * Ownership follows the Create Rule; that is, it is the caller's
 * responsibility to release the returned object. */
CFStringRef createStringForKey(CGKeyCode keyCode);

MMKeyCode keyCodeForChar(const char c)
{
	static CFMutableDictionaryRef charToCodeDict = NULL;
	static dispatch_once_t onceToken;
	CGKeyCode code = UINT16_MAX; // Use UINT16_MAX as the error code.
	UniChar character = c;
	CFStringRef charStr = NULL;

	// Thread-safe initialization of the dictionary.
	dispatch_once(&onceToken, ^{
			charToCodeDict = CFDictionaryCreateMutable(kCFAllocatorDefault,
																									128,
																									&kCFCopyStringDictionaryKeyCallBacks,
																									NULL);
			if (charToCodeDict != NULL) {
					// Populate the dictionary with keycode-character mappings.
					for (size_t i = 0; i < 128; ++i) {
							CFStringRef string = createStringForKey((CGKeyCode)i);
							if (string != NULL) {
									CFDictionaryAddValue(charToCodeDict, string, (const void *)i);
									CFRelease(string);
							}
					}
			}
	});

	// Convert the character to a CFString.
	charStr = CFStringCreateWithCharacters(kCFAllocatorDefault, &character, 1);
	if (charStr != NULL) {
			// Look up the keycode for the character.
			if (!CFDictionaryGetValueIfPresent(charToCodeDict, charStr, (const void **)&code)) {
					code = UINT16_MAX; // Not found, return error code.
			}
			CFRelease(charStr);
	}

	return (MMKeyCode)code;
}

CFStringRef createStringForKey(CGKeyCode keyCode)
{
	TISInputSourceRef currentKeyboard = TISCopyCurrentASCIICapableKeyboardInputSource();
	CFDataRef layoutData =
		(CFDataRef)TISGetInputSourceProperty(currentKeyboard,
								  kTISPropertyUnicodeKeyLayoutData);
	const UCKeyboardLayout *keyboardLayout =
		(const UCKeyboardLayout *)CFDataGetBytePtr(layoutData);

	UInt32 keysDown = 0;
	UniChar chars[4];
	UniCharCount realLength;

	UCKeyTranslate(keyboardLayout,
				   keyCode,
				   kUCKeyActionDisplay,
				   0,
				   LMGetKbdType(),
				   kUCKeyTranslateNoDeadKeysBit,
				   &keysDown,
				   sizeof(chars) / sizeof(chars[0]),
				   &realLength,
				   chars);
	CFRelease(currentKeyboard);

	return CFStringCreateWithCharacters(kCFAllocatorDefault, chars, 1);
}
