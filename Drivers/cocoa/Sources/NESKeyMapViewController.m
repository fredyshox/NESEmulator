//
//  NESSettingsViewController.m
//  nescocoa
//
//  Created by Kacper Raczy on 09/11/2019.
//  Copyright © 2019 Kacper Raczy. All rights reserved.
//

#import "NESKeyMapViewController.h"
#import <nes/controller.h>

@implementation NESKeyMapViewController

- (id)initWithUserDefaultsKey:(NSString*) key {
    if (self = [super init]) {
        _userDefaultsKey = key;
    }
    
    return self;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    
    _keyMap = [self loadKeyMapUsingKey: _userDefaultsKey];
    [_tableView setAllowsColumnReordering: NO];
    [_tableView setAllowsColumnResizing: NO];
    [_tableView setColumnAutoresizingStyle: NSTableViewUniformColumnAutoresizingStyle];
    [_tableView setDataSource: self];
    [_tableView setDelegate: self];
    [_inputButton removeAllItems];
    [_inputButton addItemsWithTitles: @[@"Keyboard"]];
}

- (void)viewWillDisappear {
    [super viewWillDisappear];
    [self storeKeyMap: _keyMap usingKey: _userDefaultsKey];
}

- (BOOL)acceptsFirstResponder {
    return YES;
}

- (NESKeyMap*) loadKeyMapUsingKey: (NSString*) key {
    NSUserDefaults* defaults = [NSUserDefaults standardUserDefaults];
    NESKeyMap* keyMap;
    NSData* data = [defaults objectForKey: key];
    if (data == nil) {
        NSLog(@"Created new keymap");
        return [[NESKeyMap alloc] init];
    }
    
    NSError* error;
    keyMap = [NSKeyedUnarchiver unarchivedObjectOfClass: [NESKeyMap class] fromData: data error: &error];
    if (error != nil) {
        NSLog(@"Decoding Error: %@", [error localizedDescription]);
        NSLog(@"Created new keymap");
        return [[NESKeyMap alloc] init];
    }
    
    return keyMap;
}

- (void) storeKeyMap: (NESKeyMap*) keyMap usingKey: (NSString*) key {
    NSUserDefaults* defaults = [NSUserDefaults standardUserDefaults];
    NSError* error;
    NSData* data = [NSKeyedArchiver archivedDataWithRootObject: keyMap requiringSecureCoding: YES error: &error];
    if (error != nil) {
        NSLog(@"Error while saving: %@", [error localizedDescription]);
        return;
    }
    
    [defaults setObject: data forKey: key];
}

- (NSString*) stringForButton: (enum controller_button) button {
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

- (NSString*) stringForKeyCode: (uint16_t) keyCode {
    TISInputSourceRef currentKeyboard = TISCopyCurrentKeyboardInputSource();
    CFDataRef layoutData =
    TISGetInputSourceProperty(currentKeyboard,
                              kTISPropertyUnicodeKeyLayoutData);
    const UCKeyboardLayout *keyboardLayout =
    (const UCKeyboardLayout *)CFDataGetBytePtr(layoutData);

    UInt32 keysDown = 0;
    UniChar chars[4];
    UniCharCount realLength;

    UCKeyTranslate(keyboardLayout,
                   keyCode,
                   kUCKeyActionDisplay,
                   0,
                   LMGetKbdType(),
                   kUCKeyTranslateNoDeadKeysBit,
                   &keysDown,
                   sizeof(chars) / sizeof(chars[0]),
                   &realLength,
                   chars);
    CFRelease(currentKeyboard);

    CFStringRef cfString = CFStringCreateWithCharacters(kCFAllocatorDefault, chars, 1);
    return (__bridge NSString*) cfString;
}

// MARK: Keyboard

- (void)keyDown:(NSEvent *)event {
    NSInteger row = [_tableView selectedRow];
    uint16_t keyCode = [event keyCode];
    [_keyMap setKeyCode: keyCode forButton: (enum controller_button) row];
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
    uint16_t keyCode = [_keyMap keyCodeForButton: button];
    if (tableColumn == [[tableView tableColumns] objectAtIndex: 0]) {
        editable = false;
        cellIdentifier = @"buttonCell";
        text = [self stringForButton: button];
    } else if (tableColumn == [[tableView tableColumns] objectAtIndex: 1]) {
        editable = false;
        cellIdentifier = @"keyCell";
        text = (keyCode == NESKeyMapKeyCodeNone) ? @"" : [self stringForKeyCode: keyCode];
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
    return NO;
}

@end
