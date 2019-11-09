//
//  NESGame.m
//  nescocoa
//
//  Created by Kacper Raczy on 08/11/2019.
//  Copyright © 2019 Kacper Raczy. All rights reserved.
//

#import "NESGame.h"

@implementation NESGame

@synthesize title = _title;
@synthesize path = _path;

- (id)initWithTitle: (NSString*) title andPath: (NSURL*) path {
    if ([path.pathExtension isNotEqualTo: @"nes"]) {
        NSLog(@"ERROR: Invalid path - file is not .nes file");
        return nil;
    }
    
    if (self = [super init]) {
        _title = [[NSString alloc] initWithString: title];
        _path = path;
    }
    
    NSLog(@"%d %d", _title == nil, _path == nil);
    
    return self;
}

- (cartridge*)loadRomWithError:(NSError **)error {
    const char* path = [[_path path] cStringUsingEncoding: NSASCIIStringEncoding];
    cartridge* c = malloc(sizeof(cartridge));
    int res = cartridge_from_file(c, (char*) path);
    if (res != 0) {
        NSString* domain = @"com.raczy.nescocoa.CartridgeParseError";
        NSString* message = [NSString stringWithFormat: @"Unable to parse file: %@", [_path absoluteString]];
        NSDictionary* userInfo = @{
            NSLocalizedDescriptionKey: message
        };
        
        *error = [[NSError alloc] initWithDomain: domain code: res userInfo: userInfo];
        free(c);
        return NULL;
    }
    
    return c;
}

@end
