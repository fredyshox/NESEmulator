//
//  NESGame.h
//  nescocoa
//
//  Created by Kacper Raczy on 08/11/2019.
//  Copyright © 2019 Kacper Raczy. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <nes/cartridge.h>

NS_ASSUME_NONNULL_BEGIN

@interface NESGame : NSObject

@property (strong, readonly) NSString* title;
@property (strong, readonly) NSURL* path;

- (id)initWithTitle: (NSString*) title andPath: (NSURL*) path;
- (cartridge*)loadRomWithError: (NSError**) error;

@end

NS_ASSUME_NONNULL_END
