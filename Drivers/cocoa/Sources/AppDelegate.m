//
//  AppDelegate.m
//  nescocoa
//
//  Created by Kacper Raczy on 13/04/2019.
//  Copyright © 2019 Kacper Raczy. All rights reserved.
//

#import "AppDelegate.h"

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

@end
