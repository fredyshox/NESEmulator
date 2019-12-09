//
//  NESSettingsViewController.m
//  nescocoa
//
//  Created by Kacper Raczy on 09/11/2019.
//  Copyright © 2019 Kacper Raczy. All rights reserved.
//

#import "NESKeyMapViewController.h"
#import "NSUserDefaults+NESKeyMap.h"
#import <nes/controller.h>

@implementation NESKeyMapViewController

- (id)initWithUserDefaultsKey:(NSString*) key {
    if (self = [super init]) {
        _userDefaultsKey = key;
        _userDefaults = [NSUserDefaults standardUserDefaults];
    }
    
    return self;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    
    _keyMap = [_userDefaults loadKeyMapUsingKey: _userDefaultsKey];
    if (_keyMap == nil) {
        _keyMap = [[NESKeyMap alloc] initWithSource: NESKeyMapSourceKeyboard];
    }
    
    [_tableView setAllowsColumnReordering: NO];
    [_tableView setAllowsColumnResizing: NO];
    [_tableView setAllowsTypeSelect: NO];
    [_tableView setColumnAutoresizingStyle: NSTableViewUniformColumnAutoresizingStyle];
    [_tableView setDataSource: self];
    [_tableView setDelegate: self];
    [_tableView setRefusesFirstResponder: YES];
    [_inputButton removeAllItems];
    [_inputButton addItemsWithTitles: @[@"Keyboard"]];
    [_inputButton selectItemAtIndex: [_keyMap source]];
}

- (void)viewDidAppear {
    [super viewDidAppear];
    [[[self view] window] makeFirstResponder: self];
}

- (void)viewWillDisappear {
    [super viewWillDisappear];
    [_keyMap setSource: [self selectedSource]];
    [_userDefaults storeKeyMap: _keyMap usingKey: _userDefaultsKey];
}

- (BOOL)acceptsFirstResponder {
    return YES;
}

- (NESKeyMapSource)selectedSource {
    return [_inputButton indexOfSelectedItem];
}

- (NSString*)stringForButton: (enum controller_button) button {
    switch (button) {
        case A_BUTTON:
            return @"A";
        case B_BUTTON:
            return @"B";
        case UP_BUTTON:
            return @"↑";
        case DOWN_BUTTON:
            return @"↓";
        case LEFT_BUTTON:
            return @"←";
        case RIGHT_BUTTON:
            return @"→";
        case SELECT_BUTTON:
            return @"SELECT";
        case START_BUTTON:
            return @"START";
    }
}

// MARK: Actions



// MARK: Keyboard

- (void)keyDown:(NSEvent *)event {
    NSLog(@"Key event");
    NSInteger row = [_tableView selectedRow];
    if (row == -1) {
        return;
    }
    
    uint16_t keyCode = [event keyCode];
    [_keyMap setKeyCode: keyCode forButton: (enum controller_button) row];
    if (_delegate != nil) {
        [_delegate keyCodeDidBecomeAssigned: keyCode forUserDefaultsKey: _userDefaultsKey];
    }
    [_tableView reloadData];
}

// MARK: NSTableViewDataSource & NSTableViewDelegate

- (NSInteger)numberOfRowsInTableView:(NSTableView *)tableView {
    return CONTROLLER_BUTTON_COUNT;
}

- (NSView *)tableView:(NSTableView *)tableView viewForTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row {
    NSString* cellIdentifier;
    NSString* text;
    BOOL editable;
    enum controller_button button = (enum controller_button) row;
    uint16_t keyCode;
    BOOL keyCodeFetchRes = [_keyMap keyCodeForButton: button keyCodePtr: &keyCode];
    if (tableColumn == [[tableView tableColumns] objectAtIndex: 0]) {
        editable = false;
        cellIdentifier = @"buttonCell";
        text = [self stringForButton: button];
    } else if (tableColumn == [[tableView tableColumns] objectAtIndex: 1]) {
        editable = false;
        cellIdentifier = @"keyCell";
        if (!keyCodeFetchRes) {
            text = @"";
        } else {
            text = stringFromKeyCode(keyCode);
        }
    } else {
        return nil;
    }
    
    NSTableCellView* cell = [tableView makeViewWithIdentifier: cellIdentifier owner: nil];
    if (cell != nil) {
        NSTextField* textField = [cell textField];
        [textField setSelectable: editable];
        [textField setEditable: editable];
        [textField setStringValue: text];
    }
    
    return cell;
}

- (BOOL)tableView:(NSTableView *)tableView shouldTypeSelectForEvent:(NSEvent *)event withCurrentSearchString:(NSString *)searchString {
    NSLog(@"sdfs");
    return NO;
}

// MARK: NESKeyMapViewControllerDelegate

- (void)keyCodeDidBecomeAssigned: (uint16_t) keyCode forUserDefaultsKey: (NSString*) key {
    if ([_userDefaultsKey isEqualToString: key]) {
        return;
    }
    
    [_keyMap clearButtonForKeyCode: keyCode];
    [_tableView reloadData];
}

@end
