//
//  AppDelegate.h
//  nescocoa
//
//  Created by Kacper Raczy on 13/04/2019.
//  Copyright © 2019 Kacper Raczy. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <ppu/renderer.h>
#import "NESLibraryViewController.h"

@interface AppDelegate : NSObject <NSApplicationDelegate>

@property (strong) NESLibraryViewController* libraryViewController;

@end

