//
//  GameCollectionViewItem.m
//  nescocoa
//
//  Created by Kacper Raczy on 08/11/2019.
//  Copyright © 2019 Kacper Raczy. All rights reserved.
//

#import "NESGameCollectionViewItem.h"

@implementation NESGameCollectionViewItem

@synthesize delegate = _delegate;

- (void)prepareForReuse {
    [super prepareForReuse];
    _delegate = nil;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    [[self view] setWantsLayer: YES];
    CALayer* viewLayer = [[self view] layer];
    [viewLayer setBorderWidth: 0.0];
    [viewLayer setCornerRadius: 12.0];
    [viewLayer setBorderColor: NSColor.lightGrayColor.CGColor];
    [[self textField] setBezeled: NO];
    [[self textField] setDrawsBackground: NO];
    [[self textField] setEditable: NO];
    [[self textField] setSelectable: NO];
}

- (void)setSelected:(BOOL)selected {
    [super setSelected: selected];
    [[[self view] layer] setBorderWidth: selected ? 3.0 : 0.0];
}

- (void)mouseUp:(NSEvent *)event {
    if ([event clickCount] == 2) {
        [_delegate gcvItemDidReceivePlayGameRequest: self];
    }
    
    [super mouseUp: event];
}

- (IBAction)playGame:(NSMenuItem *)sender {
    [_delegate gcvItemDidReceivePlayGameRequest: self];
}

- (IBAction)showInFinder:(NSMenuItem *)sender {
    [_delegate gcvItemDidReceiveShowInFinderRequest: self];
}

- (IBAction)deleteGame:(NSMenuItem *)sender {
    [_delegate gcvItemDidReceiveDeleteGameRequest: self];
}

@end
