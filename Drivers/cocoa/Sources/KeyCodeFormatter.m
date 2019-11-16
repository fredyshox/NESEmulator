//
//  KeyCodeFormatter.m
//  nescocoa
//
//  Created by Kacper Raczy on 14/11/2019.
//  Copyright © 2019 Kacper Raczy. All rights reserved.
//

#import "KeyCodeFormatter.h"

#define nsni(val) [NSNumber numberWithUnsignedInteger: val]

NSDictionary<NSNumber*, NSString*>* nonPritableKeys;

void initialize() {
    nonPritableKeys = @{
        nsni(kVK_Return): @"⮐",
        nsni(kVK_Tab): @"TAB",
        nsni(kVK_Space): @"SPACE",
        nsni(kVK_Escape): @"ESC",
        nsni(kVK_Shift): @"SHIFT",
        nsni(kVK_RightShift): @"R SHIFT",
        nsni(kVK_Option): @"OPTION",
        nsni(kVK_RightOption): @"R OPTION",
        nsni(kVK_Command): @"COMMAND",
        nsni(kVK_RightCommand): @"R COMMAND",
        nsni(kVK_Control): @"CONTROL",
        nsni(kVK_RightControl): @"R CONTROL",
        nsni(kVK_Function): @"FN",
        nsni(kVK_Delete): @"DELETE",
        nsni(kVK_CapsLock): @"CAPSLOCK",
        nsni(kVK_Help): @"HELP",
        nsni(kVK_Home): @"HOME",
        nsni(kVK_PageUp): @"PAGEUP",
        nsni(kVK_PageDown): @"PAGEDOWN",
        nsni(kVK_End): @"END",
        nsni(kVK_RightArrow): @"→",
        nsni(kVK_LeftArrow): @"←",
        nsni(kVK_UpArrow): @"↑",
        nsni(kVK_DownArrow): @"↓",
        nsni(kVK_F1): @"F1",
        nsni(kVK_F2): @"F2",
        nsni(kVK_F3): @"F3",
        nsni(kVK_F4): @"F4",
        nsni(kVK_F5): @"F5",
        nsni(kVK_F6): @"F6",
        nsni(kVK_F7): @"F7",
        nsni(kVK_F8): @"F8",
        nsni(kVK_F9): @"F9",
        nsni(kVK_F10): @"F10",
        nsni(kVK_F11): @"F11",
        nsni(kVK_F12): @"F12",
        nsni(kVK_F13): @"F13",
        nsni(kVK_F14): @"F14",
        nsni(kVK_F15): @"F15",
        nsni(kVK_F16): @"F16",
        nsni(kVK_F17): @"F17",
        nsni(kVK_F18): @"F18",
        nsni(kVK_F19): @"F19",
        nsni(kVK_F20): @"F20"
    };
}

NSString* stringForPrintable(uint16_t keyCode) {
    TISInputSourceRef currentKeyboard = TISCopyCurrentKeyboardInputSource();
    CFDataRef layoutData =
        TISGetInputSourceProperty(currentKeyboard,
                                  kTISPropertyUnicodeKeyLayoutData);
    const UCKeyboardLayout *keyboardLayout =
        (const UCKeyboardLayout *)CFDataGetBytePtr(layoutData);
    
    UInt32 keysDown = 0;
    UniChar chars[4];
    UniCharCount realLength;
    
    OSStatus status =
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
    if (status == noErr && realLength != 0) {
        return [NSString stringWithCharacters: chars length: realLength];
    }
    
    return nil;
}

NSString* __nullable stringFromKeyCode(uint16_t keyCode) {
    NSString* str = stringForPrintable(keyCode);
    
    if (str == nil || [str length] == 0) {
        NSLog(@"keyCode: %u", keyCode);
        str = [nonPritableKeys objectForKey: nsni(keyCode)];
    }
    
    return str;
}
