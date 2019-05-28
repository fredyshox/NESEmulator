//
//  AppDelegate.h
//  nescocoa
//
//  Created by Kacper Raczy on 13/04/2019.
//  Copyright © 2019 Kacper Raczy. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <ppu/renderer.h>
#import "GameViewController.h"

@interface AppDelegate : NSObject <NSApplicationDelegate>

@property (strong) GameViewController* gameViewController;


@end

