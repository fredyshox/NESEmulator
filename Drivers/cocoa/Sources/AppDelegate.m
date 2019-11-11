//
//  AppDelegate.m
//  nescocoa
//
//  Created by Kacper Raczy on 13/04/2019.
//  Copyright © 2019 Kacper Raczy. All rights reserved.
//

#import "AppDelegate.h"

NSString* _Nonnull kUserDefaultsController1Key = @"NESController1";
NSString* _Nonnull kUserDefaultsController2Key = @"NESController2";

@interface AppDelegate ()
@property (weak) IBOutlet NSWindow *window;
@end

@implementation AppDelegate

@synthesize libraryViewController = _libraryViewController;

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    [_window setContentSize: NSMakeSize(600, 400)];
    _libraryViewController = [[NESLibraryViewController alloc] init];
    [[_libraryViewController view] setFrame: _window.frame];
    [_window setContentViewController: _libraryViewController];
    NSLog(@"Window frame %f %f", _window.frame.size.width, _window.frame.size.height);
    
    [_window makeKeyWindow];
}

- (IBAction)showPreferencesModal:(id)sender {
    NSWindow* window = [[NSWindow alloc] initWithContentRect: NSZeroRect
                                                   styleMask: NSWindowStyleMaskTitled | NSWindowStyleMaskClosable
                                                     backing: NSBackingStoreBuffered
                                                       defer: false];
    [window setTitle: @"Controls"];
    NSTabViewController* tabVC = [[NSTabViewController alloc] init];
    [tabVC setTabStyle: NSTabViewControllerTabStyleUnspecified];
    [[tabVC tabView] setTabPosition: NSTabPositionTop];
    NESKeyMapViewController* controller1VC = [[NESKeyMapViewController alloc] initWithUserDefaultsKey: kUserDefaultsController1Key];
    [controller1VC setTitle: @"Controller 1"];
    NESKeyMapViewController* controller2VC = [[NESKeyMapViewController alloc] initWithUserDefaultsKey: kUserDefaultsController2Key];
    [controller2VC setTitle: @"Controller 2"];
    [tabVC setTabViewItems: @[
        [NSTabViewItem tabViewItemWithViewController: controller1VC],
        [NSTabViewItem tabViewItemWithViewController: controller2VC]
    ]];
    [[tabVC tabView] setTabPosition: NSTabPositionTop];
    [window setContentViewController: tabVC];
    [window setContentSize: NSMakeSize(400, 400)];
    [[tabVC view] setFrame: NSMakeRect(20.0, 20.0, 360.0, 360.0)];
    [window setDelegate: self];
    [window center];
    [window setReleasedWhenClosed: NO];
    [window makeFirstResponder: tabVC];
    [window makeKeyAndOrderFront: nil];
    _preferencesWindow = window;
}

// MARK: NSWindowDelegate

- (void)windowWillClose:(NSNotification *)notification {
    NSWindow* window = [notification object];
    if (window == _preferencesWindow) {
        [_preferencesWindow setDelegate: nil];
        _preferencesWindow = nil;
    }
}

@end
