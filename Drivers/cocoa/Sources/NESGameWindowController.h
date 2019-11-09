//
//  NESGameWindowController.h
//  nescocoa
//
//  Created by Kacper Raczy on 09/11/2019.
//  Copyright © 2019 Kacper Raczy. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "NESGameViewController.h"
#import "NESGame.h"

NS_ASSUME_NONNULL_BEGIN

@interface NESGameWindowController : NSWindowController <NSWindowDelegate>

@property (strong, nullable) NESGameViewController* gameViewController;
@property (strong, nullable) NESGame* game;

@end

NS_ASSUME_NONNULL_END
