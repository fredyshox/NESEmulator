//
//  NESGame.m
//  nescocoa
//
//  Created by Kacper Raczy on 08/11/2019.
//  Copyright © 2019 Kacper Raczy. All rights reserved.
//

#import "NESGame.h"

@implementation NESGame

@synthesize identifier = _identifier;
@synthesize title = _title;
@synthesize path = _path;

- (id)initWithId: (int) identifier andTitle: (NSString*) title andPath: (NSURL*) path {
    if ([path.pathExtension isNotEqualTo: @"nes"]) {
        NSLog(@"ERROR: Invalid path - file is not .nes file");
        return nil;
    }
    
    if (self = [super init]) {
        _identifier = identifier;
        _title = [[NSString alloc] initWithString: title];
        _path = path;
    }
        
    return self;
}

- (cartridge* _Nullable)loadRomWithError:(NSError **)error {
    const char* path = [[_path path] cStringUsingEncoding: NSASCIIStringEncoding];
    NSLog(@"%s", path);
    cartridge* c = malloc(sizeof(cartridge));
    int res = cartridge_from_file(c, (char*) path);
    if (res != 0) {
        NSString* domain = [[NSBundle mainBundle] bundleIdentifier];
        NSString* message = [NSString stringWithFormat: @"Unable to parse file: %@", [_path absoluteString]];
        NSDictionary* userInfo = @{
            NSLocalizedDescriptionKey: message
        };
        
        *error = [[NSError alloc] initWithDomain: domain code: res | 0x1000 userInfo: userInfo];
        free(c);
        return NULL;
    }
    
    return c;
}

@end
