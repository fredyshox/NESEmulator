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

- (void)awakeFromNib {
    if (_window != nil) {
        [_window setAspectRatio: NSMakeSize(HORIZONTAL_RES, VERTICAL_RES)];
    }
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    _gameViewController = [[GameViewController alloc] init];
    [_gameViewController.view setFrame: _window.frame];
    NSLog(@"Window frame %f %f", _window.frame.size.width, _window.frame.size.height);
    [_window setContentViewController: _gameViewController];
    [_window makeFirstResponder: _gameViewController];
    [_window makeKeyWindow];
}

@end
