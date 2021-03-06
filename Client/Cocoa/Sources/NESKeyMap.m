//
//  NESKeyMap.m
//  nescocoa
//
//  Created by Kacper Raczy on 04/11/2019.
//  Copyright © 2019 Kacper Raczy. All rights reserved.
//

#import "NESKeyMap.h"
#import <Carbon/Carbon.h>

@implementation NESKeyMap {
    uint16_t keyArray[CONTROLLER_BUTTON_COUNT];
    NSMutableDictionary<NSNumber*, NSNumber*>* keyCodeDict;
}

@synthesize source;

+ (NESKeyMap *)defaultKeyMap {
    uint16_t buttons[8] = {/* A */ kVK_ANSI_A, /* B */ kVK_ANSI_S,
                           /* SELECT */ kVK_ANSI_Q, /*START*/ kVK_ANSI_W,
                           /* UP */ kVK_UpArrow, /* DOWN */ kVK_DownArrow,
                           /* LEFT */ kVK_LeftArrow, /* RIGHT */ kVK_RightArrow};
    NESKeyMap* km = [[NESKeyMap alloc] initWithSource: NESKeyMapSourceKeyboard
                                          AndKeyArray: buttons
                                             keyCount: 8];
    return km;
}

- (id)initWithSource: (NESKeyMapSource) keyMapSource AndKeyArray: (uint16_t*) arr keyCount: (int) count {
    if (self = [self initWithSource: keyMapSource]) {
        for (int i = 0; i < count && i < 8; i++) {
            keyArray[i] = arr[i];
            NSNumber* button = [NSNumber numberWithInt: i];
            NSNumber* keyCode = [NSNumber numberWithUnsignedShort: arr[i]];
            [keyCodeDict removeObjectForKey: keyCode];
            [keyCodeDict setObject: button forKey: keyCode];
        }
    }
    
    return self;
}

- (id)initWithSource: (NESKeyMapSource) keyMapSource {
    if (self = [super init]) {
        source = keyMapSource;
        for (int i = 0; i<CONTROLLER_BUTTON_COUNT; i++) {
            keyArray[i] = NESKeyMapKeyCodeNone;
            keyCodeDict = [NSMutableDictionary new];
        }
    }
    
    return self;
}

// MARK: NSSecureCoding

- (id)initWithCoder:(NSCoder *)aDecoder {
    if (self = [super init]) {
        NSUInteger length;
        const uint8_t* bytes = [aDecoder decodeBytesForKey: @"keyArray" returnedLength: &length];
        if (bytes != NULL && length == (sizeof(uint16_t) * 8)) {
            memcpy(keyArray, bytes, sizeof(uint16_t) * 8);
        }
        keyCodeDict = [self createKeyCodeDict];
        source = [aDecoder decodeIntegerForKey: @"source"];
    }
    
    return self;
}

- (void)encodeWithCoder:(NSCoder *)coder {
    [coder encodeBytes: (uint8_t *) keyArray length: sizeof(uint16_t) * 8 forKey: @"keyArray"];
    [coder encodeInteger: source forKey: @"source"];
}

+ (BOOL)supportsSecureCoding {
    return YES;
}

// MARK: Utilities

- (NSMutableDictionary<NSNumber*, NSNumber*>*) createKeyCodeDict {
    NSMutableDictionary<NSNumber*, NSNumber*>* dict = [NSMutableDictionary new];
    for (int i = 0; i < CONTROLLER_BUTTON_COUNT; i++) {
        NSNumber* keyCode = [NSNumber numberWithUnsignedShort: keyArray[i]];
        NSNumber* button = [NSNumber numberWithInt: i];
        [dict setObject: button forKey: keyCode];
    }
    
    return dict;
}

// MARK: Public methods

- (BOOL)buttonForKeyCode: (uint16_t) keyCode buttonPtr: (enum controller_button*) buttonPtr {
    NSNumber* button = [keyCodeDict objectForKey: [NSNumber numberWithUnsignedShort: keyCode]];
    if (button != nil) {
        *buttonPtr = (enum controller_button) button.intValue;
        return YES;
    }
    
    return NO;
}

- (BOOL)keyCodeForButton: (enum controller_button) button keyCodePtr: (uint16_t*) keyCodePtr; {
    uint16_t keyCode = keyArray[button];
    if (keyCode != NESKeyMapKeyCodeNone) {
        *keyCodePtr = keyCode;
        return YES;
    }
    
    return NO;
}

- (void)setKeyCode:(uint16_t)keyCode forButton:(enum controller_button)button {
    keyArray[button] = keyCode;
    [self clearButtonForKeyCode: keyCode];
    
    NSNumber* keyCodeNumber = [NSNumber numberWithUnsignedShort: keyCode];
    NSNumber* buttonNumber = [NSNumber numberWithInt: button];
    [keyCodeDict setObject: buttonNumber forKey: keyCodeNumber];
}

- (void)clearKeyCodeForButton: (enum controller_button) button {
    keyArray[button] = NESKeyMapKeyCodeNone;
    [keyCodeDict removeObjectForKey: [NSNumber numberWithInt: button]];
}

- (void)clearButtonForKeyCode: (uint16_t) keyCode {
    NSNumber* keyCodeNumber = [NSNumber numberWithUnsignedShort: keyCode];
    NSNumber* buttonNumber = [keyCodeDict objectForKey: keyCodeNumber];
    if (buttonNumber != nil) {
        keyArray[buttonNumber.intValue] = NESKeyMapKeyCodeNone;
        [keyCodeDict removeObjectForKey: keyCodeNumber];
    }
}

- (void)printKeyMap {
    for (int i = 0; i < 8; i++) {
        NSLog(@"Button %d: %u", i, keyArray[i]);
    }
}

@end
