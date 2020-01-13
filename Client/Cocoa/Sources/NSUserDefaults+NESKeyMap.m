//
//  NSUserDefaults+NSUserDefaults_NESKeyMap.m
//  nescocoa
//
//  Created by Kacper Raczy on 09/12/2019.
//  Copyright © 2019 Kacper Raczy. All rights reserved.
//

#import "NSUserDefaults+NESKeyMap.h"

@implementation NSUserDefaults (NESKeyMapSupport)

- (NESKeyMap* _Nullable)loadKeyMapUsingKey: (NSString*) key {
    NESKeyMap* keyMap;
    NSData* data = [self objectForKey: key];
    if (data == nil) {
        NSLog(@"Created new keymap");
        return nil;
    }
    
    NSError* error;
    keyMap = [NSKeyedUnarchiver unarchivedObjectOfClass: [NESKeyMap class] fromData: data error: &error];
    if (error != nil) {
        NSLog(@"Decoding Error: %@", [error localizedDescription]);
        NSLog(@"Created new keymap");
        return nil;
    }
    
    return keyMap;
}

- (void)storeKeyMap: (NESKeyMap* _Nonnull) keyMap usingKey: (NSString*) key {
    NSError* error;
    NSData* data = [NSKeyedArchiver archivedDataWithRootObject: keyMap requiringSecureCoding: YES error: &error];
    if (error != nil) {
        NSLog(@"Error while saving: %@", [error localizedDescription]);
        return;
    }
    
    [self setObject: data forKey: key];
}

@end
