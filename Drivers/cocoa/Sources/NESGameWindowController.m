//
//  NESGameWindowController.m
//  nescocoa
//
//  Created by Kacper Raczy on 09/11/2019.
//  Copyright © 2019 Kacper Raczy. All rights reserved.
//

#import "NESGameWindowController.h"
#import <ppu/renderer.h>

@implementation NESGameWindowController

@synthesize game = _game;
@synthesize gameViewController = _gameViewController;

- (void)windowDidLoad {
    [super windowDidLoad];
    [[self window] setDelegate: self];
    [[self window] setContentAspectRatio: NSMakeSize(HORIZONTAL_RES, VERTICAL_RES)];
    [[self window] setContentMinSize: NSMakeSize(HORIZONTAL_RES, VERTICAL_RES)];
    [[self window] setContentSize: NSMakeSize(HORIZONTAL_RES * 2, VERTICAL_RES * 2)];
    if (_game == nil) {
        [NSException raise: @"GameNotSet" format: @"NESGame is set to nil"];
        return;
    }
    
    _gameViewController = [[NESGameViewController alloc] initWithGame: _game];
    [[_gameViewController view] setFrame: [[self window] frame]];
    [[self window] setTitle: [_game title]];
    [[self window] makeFirstResponder: _gameViewController];
    [self setContentViewController: _gameViewController];
}

- (void)windowWillClose:(NSNotification *)notification {
    NSLog(@"Game window will close");
    [self setContentViewController: nil];
    _gameViewController = nil;
    _game = nil;
}

@end
