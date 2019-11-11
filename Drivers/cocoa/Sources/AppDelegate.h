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
#import "NESKeyMapViewController.h"

extern const NSString* _Nonnull kUserDefaultsController1Key;
extern const NSString* _Nonnull kUserDefaultsController2Key;

@interface AppDelegate : NSObject <NSApplicationDelegate, NSWindowDelegate>

@property (strong, nonnull) NESLibraryViewController* libraryViewController;
@property (strong, nullable) NSWindow* preferencesWindow;

- (IBAction)showPreferencesModal:(id _Nullable)sender;

@end

