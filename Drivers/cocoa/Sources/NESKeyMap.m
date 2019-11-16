//
//  NESKeyMap.m
//  nescocoa
//
//  Created by Kacper Raczy on 04/11/2019.
//  Copyright © 2019 Kacper Raczy. All rights reserved.
//

#import "NESKeyMap.h"

@implementation NESKeyMap {
    uint16_t keyArray[CONTROLLER_BUTTON_COUNT];
    NSMutableDictionary<NSNumber*, NSNumber*>* keyCodeDict;
}

@synthesize source;

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

- (enum controller_button)buttonForKeyCode:(uint16_t)keyCode {
    NSNumber* button = [keyCodeDict objectForKey: [NSNumber numberWithUnsignedShort: keyCode]];
    if (button != nil) {
        return (enum controller_button) button.intValue;
    }
    
    return -1;
}

- (uint16_t) keyCodeForButton: (enum controller_button) button {
    return keyArray[button];
}

- (void)setKeyCode:(uint16_t)keyCode forButton:(enum controller_button)button {
    keyArray[button] = keyCode;
    [self clearButtonForKeyCode: keyCode];
    
    NSNumber* keyCodeNumber = [NSNumber numberWithUnsignedShort: keyCode];
    NSNumber* buttonNumber = [NSNumber numberWithInt: button];
    [keyCodeDict setObject: buttonNumber forKey: keyCodeNumber];
    //[self printKeyMap];
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
