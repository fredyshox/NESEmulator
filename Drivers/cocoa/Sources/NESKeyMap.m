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

-(id) init {
    if (self = [super init]) {
        for (int i = 0; i<CONTROLLER_BUTTON_COUNT; i++) {
            keyArray[i] = -1;
            keyCodeDict = [NSMutableDictionary new];
        }
    }
    
    return self;
}

// MARK: NSCoding

- (id)initWithCoder:(NSCoder *)aDecoder {
    if (self = [super init]) {
        NSUInteger length;
        const uint8_t* bytes = [aDecoder decodeBytesForKey: @"keyArray" returnedLength: &length];
        if (bytes != NULL && length == (sizeof(uint16_t) * 8)) {
            memcpy(keyArray, bytes, sizeof(uint16_t) * 8);
        }
        keyCodeDict = [self createKeyCodeDict];
    }
    
    return self;
}

- (void)encodeWithCoder:(NSCoder *)coder {
    [coder encodeBytes: (uint8_t *) keyArray length: sizeof(uint16_t) * 8 forKey: @"keyArray"];
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

- (void)setKeyCode:(uint16_t)keyCode forButton:(enum controller_button)button {
    keyArray[button] = keyCode;
    NSNumber* buttonNumber = [NSNumber numberWithInt: button];
    NSNumber* keyCodeNumber = [NSNumber numberWithUnsignedShort: keyCode];
    [keyCodeDict setObject: buttonNumber forKey: keyCodeNumber];
}

@end
